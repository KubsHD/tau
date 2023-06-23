#include "Foundation/NSObject.hpp"
#include "Foundation/NSString.hpp"
#ifdef APPLE

#include "SDL_metal.h"
#include "Renderer.h"

#include <core/Asset.h>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#import <QuartzCore/QuartzCore.h>
#import <Metal/Metal.h>

// https://schneide.blog/2022/03/28/metal-in-c-with-sdl2/
CA::MetalDrawable* next_drawable(CA::MetalLayer* layer)
{
  CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
  id <CAMetalDrawable> metalDrawable = [metalLayer nextDrawable];
  CA::MetalDrawable* pMetalCppDrawable = ( __bridge CA::MetalDrawable*) metalDrawable;
  return pMetalCppDrawable;
} 

Renderer::Renderer(SDL_Window* winRef)
{
	m_device = MTL::CreateSystemDefaultDevice();
	auto metal_view = SDL_Metal_CreateView(winRef);

	m_swapchain = (CA::MetalLayer*)SDL_Metal_GetLayer(metal_view);
	
	auto layer = (CAMetalLayer*)SDL_Metal_GetLayer(metal_view);
	layer.device = (__bridge id<MTLDevice>)m_device;

	m_queue = m_device->newCommandQueue();
	

	// build shaders

	m_device->newLibrary(NS::String::string(Asset::get_real_path("shaders/sprite.msl"), NS::StringEncoding::ASCIIStringEncoding), nullptr);
}

Renderer::~Renderer()
{
    m_device->release();
}

spt::ref<Pipeline> Renderer::create_pipeline(PipelineCreateDesc pcd)
{
	auto pipeline = spt::create_ref<Pipeline>();
	
	return pipeline;
}

spt::ref<Buffer> Renderer::create_buffer(BufferCreateDesc bcd)
{
	auto buf = spt::create_ref<Buffer>();

	return buf;
}

spt::ref<Texture> Renderer::create_texture(TextureCreateDesc tcd)
{
	auto tex = spt::create_ref<Texture>();



	return tex;
}

void Renderer::clear()
{
	m_drawable = next_drawable(m_swapchain);

	m_cmdBuffer = m_queue->commandBuffer();

	MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);

	cd->setClearColor(MTL::ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
	cd->setLoadAction(MTL::LoadActionClear);
	cd->setTexture(m_drawable->texture());

	auto encoder = m_cmdBuffer->renderCommandEncoder(renderPassDescriptor);
	encoder->endEncoding();


}

void Renderer::draw_texture(spt::ref<Pipeline> pip, spt::ref<Buffer> vertexBuffer, spt::ref<Texture> texture, glm::vec2 pos, glm::vec2 size)
{

}

void Renderer::swap()
{
	m_cmdBuffer->presentDrawable(m_drawable);
	m_cmdBuffer->commit();
}

#endif