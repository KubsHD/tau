#ifdef WIN32
#include "Renderer.h"

#include <SDL.h>
#include <SDL_syswm.h>

#include <core/Asset.h>

#include <lib/stb_image.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")



inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch DirectX API errors
		throw std::exception();
	}
}

// https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
static HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
	{
		return S_FALSE;
	}

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (unsigned int i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();
	return hr;
}

Renderer::Renderer(SDL_Window* winRef)
{

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(winRef, &info);

	HWND hwnd = (HWND)info.info.win.window;

	// get window size from SDL_Window and store it in two ints
	int width, height;
	SDL_GetWindowSize(winRef, &width, &height);

	// init swapchain

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Windowed = true;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
		&swapChainDesc, m_swapChain.GetAddressOf(), m_device.GetAddressOf(), NULL, m_ctx.GetAddressOf()));
		
	ID3D11Texture2D *backBuffer;
	ThrowIfFailed(m_swapChain->GetBuffer(NULL, IID_PPV_ARGS(&backBuffer)));
	ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer, NULL, m_renderTargetView.GetAddressOf()));
	
	D3D11_RASTERIZER_DESC rasterizerDesc = {
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_NONE,
	};

	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	ThrowIfFailed(m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf()));


	D3D11_SAMPLER_DESC ImageSamplerDesc = {};

	ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ImageSamplerDesc.MipLODBias = 0.0f;
	ImageSamplerDesc.MaxAnisotropy = 1;
	ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ImageSamplerDesc.BorderColor[0] = 1.0f;
	ImageSamplerDesc.BorderColor[1] = 1.0f;
	ImageSamplerDesc.BorderColor[2] = 1.0f;
	ImageSamplerDesc.BorderColor[3] = 1.0f;
	ImageSamplerDesc.MinLOD = -FLT_MAX;
	ImageSamplerDesc.MaxLOD = FLT_MAX;

	
	ThrowIfFailed(m_device->CreateSamplerState(&ImageSamplerDesc, m_samplerState.GetAddressOf()));

	D3D11_VIEWPORT viewport = {};
	
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_ctx->RSSetViewports(1, &viewport);
}

Renderer::~Renderer()
{
}

static std::wstring convert_to_wstring(const char* str)
{
	auto string_path = std::string(str);
	return std::wstring(string_path.begin(), string_path.end());
}

spt::ref<Pipeline> Renderer::create_pipeline(PipelineCreateDesc pcd)
{
	auto pipeline = spt::create_ref<Pipeline>();
	
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ID3DBlob *blob;
	ID3DBlob *errblob;

	if (FAILED(D3DCompileFromFile(convert_to_wstring(Asset::get_real_path(pcd.vertexShader)).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &blob, &errblob)))
	{
		auto error = (char*)errblob->GetBufferPointer();
		throw std::exception();
	}
	
	m_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pipeline->vertexShader.GetAddressOf());
	
	ThrowIfFailed(CreateInputLayoutDescFromVertexShaderSignature(blob, m_device.Get(), pipeline->inputLayout.GetAddressOf()));


	if (FAILED(D3DCompileFromFile(convert_to_wstring(Asset::get_real_path(pcd.pixelShader)).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &blob, &errblob)))
	{
		auto error = (char*)errblob->GetBufferPointer();
		throw std::exception();
	}
	
	m_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pipeline->pixelShader.GetAddressOf());

	return pipeline;
}

spt::ref<Buffer> Renderer::create_buffer(BufferCreateDesc bcd)
{
	auto buf = spt::create_ref<Buffer>();

	// Vertex buffer desciption
	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = bcd.byteWidth;
	bufferDesc.BindFlags = bcd.bindFlags;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Create vertex buffer data
	D3D11_SUBRESOURCE_DATA bufferData{};
	bufferData.pSysMem = bcd.data;

	m_device->CreateBuffer(&bufferDesc, &bufferData, buf->buf.GetAddressOf());

	return buf;
}

spt::ref<Texture> Renderer::create_texture(TextureCreateDesc tcd)
{
	auto tex = spt::create_ref<Texture>();



	D3D11_TEXTURE2D_DESC desc{};
	
	desc.Width = tcd.size.x;
	desc.Height = tcd.size.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = (DXGI_FORMAT)tcd.format;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = tcd.data.data();
	data.SysMemPitch = tcd.size.x * 4;

	ThrowIfFailed(m_device->CreateTexture2D(&desc, &data, tex->texture.GetAddressOf()));
	ThrowIfFailed(m_device->CreateShaderResourceView(tex->texture.Get(), nullptr, tex->srv.GetAddressOf()));

	return tex;
}

void Renderer::clear()
{
	float backgroundColor[4] = { 0.0f, 0.2f, 0.25f, 1.0f };
	m_ctx->ClearRenderTargetView(m_renderTargetView.Get(), backgroundColor);
	m_ctx->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), 0);
}

void Renderer::draw_texture(spt::ref<Pipeline> pip, spt::ref<Buffer> vertexBuffer, spt::ref<Texture> texture, glm::vec2 pos, glm::vec2 size)
{
	UINT stride = sizeof(float) * 4;
	UINT offset = 0;

	m_ctx->RSSetState(m_rasterizerState.Get());

	m_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_ctx->IASetInputLayout(pip->inputLayout.Get());
	m_ctx->IASetVertexBuffers(0, 1, vertexBuffer->buf.GetAddressOf(), &stride, &offset);

	m_ctx->VSSetShader(pip->vertexShader.Get(), 0, 0);
	m_ctx->PSSetShader(pip->pixelShader.Get(), 0, 0);

	m_ctx->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());
	m_ctx->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	m_ctx->Draw(6, 0);
}

void Renderer::swap()
{
	m_swapChain->Present(1, 0);
}

#endif
