#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <queue>
#include <core/Types.h>

#if WIN32

#include <dxgi.h>
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#else if APPLE


#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#endif

struct SDL_Window;

enum class BindFlags {
	BIND_VERTEX_BUFFER,
	BIND_INDEX_BUFFER,
	BIND_CONSTANT_BUFFER
};

enum class ColorFormat {
	RGBA8_SRGB
};

struct PipelineCreateDesc {
	std::string vertexShader;
	std::string pixelShader;
};

struct TextureCreateDesc {
	std::string name;
	glm::vec2 size;
	ColorFormat format;
	std::vector<char> data;
};

struct BufferCreateDesc {
	BindFlags bindFlags;
	int byteWidth;
	void *data;
};

#ifdef WIN32

struct Pipeline {
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
};

struct Texture {
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> srv;
};

struct Buffer {
	ComPtr<ID3D11Buffer> buf;
};

struct DrawData {
	ComPtr<ID3D11DeviceContext> cmdList;

	spt::ref<Pipeline> pipeline;
	spt::ref<Buffer> vertexBuffer;
	spt::ref<Buffer> uniformBuffer;
	
	// todo: allow multiple texture binds
	spt::ref<Texture> texture;

	// todo: indexed rendering 
	// spt::ref<Buffer> indexBuffer;
	// uint32_t indexCount;

	uint32_t vertexCount;
	uint32_t vertexStride;
	uint32_t vertexOffset;

};

inline D3D11_BIND_FLAG dx11_map_bind_flag(BindFlags bf)
{
	switch (bf)
	{
	case BindFlags::BIND_VERTEX_BUFFER:
		return D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		break;
	case BindFlags::BIND_INDEX_BUFFER:
		return D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		break;
	case BindFlags::BIND_CONSTANT_BUFFER:
		return D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		break;
	}
}

inline DXGI_FORMAT dx11_map_color_format(ColorFormat cf)
{
	switch (cf)
	{
	case ColorFormat::RGBA8_SRGB:
		return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		break;
	}
}


#else if APPLE

struct Pipeline {
    MTL::Library* library;
    MTL::Function* vertexShader;
    MTL::Function* pixelShader;

    MTL::RenderPipelineState* pso;
};

struct Texture {
    MTL::Texture* texture;
};

struct Buffer {
    MTL::Buffer* buf;
};

struct DrawData {
    spt::ref<Pipeline> pipeline;
    spt::ref<Buffer> vertexBuffer;
    spt::ref<Buffer> uniformBuffer;

    // todo: allow multiple texture binds
    spt::ref<Texture> texture;

    // todo: indexed rendering
    // spt::ref<Buffer> indexBuffer;
    // uint32_t indexCount;

    uint32_t vertexCount;
    uint32_t vertexStride;
    uint32_t vertexOffset;

};

#endif

class Renderer {
public:
	Renderer(SDL_Window* winRef);
	~Renderer();

	spt::ref<Pipeline> create_pipeline(PipelineCreateDesc pcd);
	spt::ref<Buffer> create_buffer(BufferCreateDesc bcd);
	spt::ref<Texture> create_texture(TextureCreateDesc tcd);

	void update_buffer(spt::ref<Buffer> buf, void* data, int size);

	void submit_draw(DrawData dat);
	void clear();
	void commit();
	void swap();

private:
	glm::mat4 m_mvp;

	// windows stuff
#if WIN32
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_ctx;

	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	
	ComPtr<ID3D11SamplerState> m_samplerState;

	ID3D11DepthStencilState* pDepthStencilState = NULL;
	ID3D11DepthStencilView* pDepthStencilView = NULL;
	ID3D11Texture2D* pDepthStencilBuffer = NULL;

	std::queue<ComPtr<ID3D11CommandList>> m_cmdLists;

	D3D11_VIEWPORT viewport;
#else if APPLE
    // https://developer.apple.com/library/archive/documentation/3DDrawing/Conceptual/MTLBestPracticesGuide/TripleBuffering.html#//apple_ref/doc/uid/TP40016642-CH5-SW1
    dispatch_semaphore_t m_frameSemaphore;

	CA::MetalLayer* m_swapchain;
	CA::MetalDrawable* m_drawable;

	MTL::Device* m_device;
	MTL::CommandQueue* m_queue;
	MTL::CommandBuffer* m_cmdBuffer;

	MTL::RenderCommandEncoder* m_encoder;
#endif
};