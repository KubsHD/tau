#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <queue>
#include <core/Types.h>

#include "Resources.h"

#if WIN32

#include <dxgi.h>
#include <d3d11.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#else if APPLE


#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#endif


class Device {
public:
	Device(SDL_Window* winRef);
	~Device();

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