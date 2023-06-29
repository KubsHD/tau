#ifdef APPLE

#include "../../Device.h"

#include "SDL_metal.h"
#include <fstream>
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

Device::Device(SDL_Window* winRef)
{
	m_device = MTL::CreateSystemDefaultDevice();
	auto metal_view = SDL_Metal_CreateView(winRef);

	m_swapchain = (CA::MetalLayer*)SDL_Metal_GetLayer(metal_view);
	
	auto layer = (CAMetalLayer*)SDL_Metal_GetLayer(metal_view);
	layer.device = (__bridge id<MTLDevice>)m_device;

	m_queue = m_device->newCommandQueue();
	

	// build shaders

}

Device::~Device()
{
    m_device->release();
}

spt::ref<Pipeline> Device::create_pipeline(PipelineCreateDesc pcd)
{
	auto pipeline = spt::create_ref<Pipeline>();

    MTL::CompileOptions* opts = MTL::CompileOptions::alloc()->init();
    NS::Error* err;

    std::string line,text;
    std::ifstream in(Asset::get_real_path("shaders/sprite.msl"));
    while(std::getline(in, line))
    {
        text += line + "\n";
    }
    const char* data = text.c_str();


    pipeline->library = m_device->newLibrary(NS::String::string(data, NS::StringEncoding::ASCIIStringEncoding), opts, &err);

    if (!pipeline->library)
    {
        printf("%s", err->localizedDescription()->utf8String());
    }

    pipeline->vertexShader = pipeline->library->newFunction(NS::String::string("VSMain", NS::StringEncoding::ASCIIStringEncoding));
    pipeline->pixelShader = pipeline->library->newFunction(NS::String::string("PSMain", NS::StringEncoding::ASCIIStringEncoding));

    MTL::RenderPipelineDescriptor *pipelineStateDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineStateDescriptor->setLabel(NS::String::string("pipeline", NS::StringEncoding::ASCIIStringEncoding));
    pipelineStateDescriptor->setVertexFunction(pipeline->vertexShader);
    pipelineStateDescriptor->setFragmentFunction(pipeline->pixelShader);
    pipelineStateDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    pipeline->pso = m_device->newRenderPipelineState(pipelineStateDescriptor, &err);

    pipelineStateDescriptor->release();
    opts->release();

    return pipeline;
}

spt::ref<Buffer> Device::create_buffer(BufferCreateDesc bcd)
{
	auto buf = spt::create_ref<Buffer>();

    buf->buf = m_device->newBuffer(bcd.data, NS::UInteger(bcd.byteWidth), MTL::ResourceStorageModeManaged);

	return buf;
}

spt::ref<Texture> Device::create_texture(TextureCreateDesc tcd)
{
	auto tex = spt::create_ref<Texture>();

    MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
    pTextureDesc->setWidth( tcd.size.x );
    pTextureDesc->setHeight( tcd.size.y );
    pTextureDesc->setPixelFormat( MTL::PixelFormatRGBA8Unorm );
    pTextureDesc->setTextureType( MTL::TextureType2D );
    pTextureDesc->setStorageMode( MTL::StorageModeManaged );
    pTextureDesc->setUsage( MTL::ResourceUsageSample | MTL::ResourceUsageRead );

    tex->texture = m_device->newTexture( pTextureDesc );

    tex->texture->replaceRegion(MTL::Region( 0, 0, 0, tcd.size.x, tcd.size.y, 1 ),
                                0, tcd.data.data(), tcd.size.y * 4);

    pTextureDesc->release();

	return tex;
}

void Device::clear()
{
	m_drawable = next_drawable(m_swapchain);

	m_cmdBuffer = m_queue->commandBuffer();



}

void Device::draw_texture(spt::ref<Pipeline> pip, spt::ref<Buffer> vertexBuffer, spt::ref<Texture> texture, glm::vec2 pos, glm::vec2 size)
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();


    MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    MTL::RenderPassColorAttachmentDescriptor* cd = renderPassDescriptor->colorAttachments()->object(0);

    cd->setClearColor(MTL::ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
    cd->setLoadAction(MTL::LoadActionClear);
    cd->setTexture(m_drawable->texture());

    auto cmd_enc = m_cmdBuffer->renderCommandEncoder(renderPassDescriptor);


    cmd_enc->setRenderPipelineState(pip->pso);

    cmd_enc->setVertexBuffer(vertexBuffer->buf, 0,0);
    cmd_enc->setCullMode( MTL::CullModeNone );
    cmd_enc->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );
    cmd_enc->setFragmentTexture(texture->texture, 0);

    cmd_enc->drawPrimitives( MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(6) );

    cmd_enc->endEncoding();

    pPool->release();
}

void Device::swap()
{
	m_cmdBuffer->presentDrawable(m_drawable);
	m_cmdBuffer->commit();
}

#endif