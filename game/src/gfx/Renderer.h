#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <core/Types.h>

#if WIN32

#include <dxgi.h>
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#else if APPLE


#endif

struct SDL_Window;


struct PipelineCreateDesc {
	std::string vertexShader;
	std::string pixelShader;
};

struct Pipeline {
	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
};

struct TextureCreateDesc {
	std::string name;
	glm::vec2 size;
	int format;
	std::vector<char> data;
};

struct Texture {
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> srv;
};

struct BufferCreateDesc {
	int bindFlags;
	int byteWidth;
	void *data;
};

struct Buffer {
	ComPtr<ID3D11Buffer> buf;
};


class Renderer {
public:
	Renderer(SDL_Window* winRef);
	~Renderer();

	spt::ref<Pipeline> create_pipeline(PipelineCreateDesc pcd);
	spt::ref<Buffer> create_buffer(BufferCreateDesc bcd);
	spt::ref<Texture> create_texture(TextureCreateDesc tcd);

	void clear();
	void draw_texture(spt::ref<Pipeline> pip, spt::ref<Buffer> vertexBuffer, spt::ref<Texture> texture, glm::vec2 pos, glm::vec2 size);
	void swap();

	// windows stuff
#if WIN32
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_ctx;

	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	
	ComPtr<ID3D11SamplerState> m_samplerState;

#endif
};