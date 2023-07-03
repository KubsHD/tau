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
	void* data;
};

#ifdef WIN32

struct Pipeline {
	PipelineCreateDesc desc;

	ComPtr<ID3D11InputLayout> inputLayout;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
};

struct Texture {
	TextureCreateDesc desc;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> srv;
};

struct Buffer {
	BufferCreateDesc desc;

	ComPtr<ID3D11Buffer> buf;
};

struct DrawData {
	ComPtr<ID3D11DeviceContext> cmdList;

	spt::ref<Pipeline> pipeline;
	spt::ref<Buffer> vertexBuffer;

	spt::ref<Buffer> staticUniformBuffer;
	spt::ref<Buffer> dynamicUniformBuffer;

	void* dataToUpdateTheBufferWith = nullptr;

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

	PIpelineCreateDesc desc;


	MTL::Library* library;
	MTL::Function* vertexShader;
	MTL::Function* pixelShader;

	MTL::RenderPipelineState* pso;
};

struct Texture {
	TextureCreateDesc desc;

	MTL::Texture* texture;
};

struct Buffer {
	BufferCreateDesc desc;

	MTL::Buffer* buf;
};

#endif