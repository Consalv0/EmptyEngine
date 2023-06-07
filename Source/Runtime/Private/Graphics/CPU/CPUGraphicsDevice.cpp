
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"

#include "SDL3/SDL.h"

// --- Make discrete GPU by default.
extern "C" {
	// --- developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	// --- developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}

namespace EE
{
    Uint32 ConvertPixelFormat( EPixelFormat format )
    {
        switch ( format )
        {
        case EE::PixelFormat_Unknown:
            break;
        case EE::PixelFormat_R8:
            break;
        case EE::PixelFormat_R32F:
            break;
        case EE::PixelFormat_RG8:
            return SDL_PIXELFORMAT_RGB888;
        case EE::PixelFormat_RG32F:
            break;
        case EE::PixelFormat_RG16F:
            break;
        case EE::PixelFormat_RGB8:
            break;
        case EE::PixelFormat_RGB32F:
            break;
        case EE::PixelFormat_RGB16F:
            break;
        case EE::PixelFormat_RGBA8:
            break;
        case EE::PixelFormat_RGBA16_UShort:
            break;
        case EE::PixelFormat_RGBA32F:
            return SDL_PIXELFORMAT_ARGB8888;
        case EE::PixelFormat_DepthComponent24:
            break;
        case EE::PixelFormat_DepthStencil:
            break;
        case EE::PixelFormat_ShadowDepth:
            break;
        case EE::Private_Num:
            break;
        default:
            break;
        }

        return 0;
    }

    struct CPUTexture
    {
        SDL_Texture* resource;

        ~CPUTexture()
        {
            SDL_DestroyTexture( resource );
        }
    };

	struct CPUSwapChain
	{
        TArray<Texture*> backBuffers;
        SDL_Renderer* renderer;
		
		~CPUSwapChain()
		{
            for ( int i = 0; i < backBuffers.size(); i++ )
            {
                delete backBuffers[ i ];
            }

            SDL_DestroyRenderer( renderer );
		}
	};

    CPUSwapChain* ToInternal( const SwapChain& swapChain )
    {
        return static_cast<CPUSwapChain*>(swapChain.internalState.get());
    }

    CPUTexture* ToInternal( const Texture& texture )
    {
        return static_cast<CPUTexture*>(texture.internalState.get());
    }

	bool CPUGraphicsDevice::CreateSwapChain( const SwapChainDescription& description, Window* window, SwapChain& outSwapChain ) const 
	{
		auto internalState = std::make_shared<CPUSwapChain>( CPUSwapChain() );

		internalState->renderer = SDL_CreateRenderer( (SDL_Window*)window->GetHandle(), NULL, 0);
		if ( internalState->renderer == NULL )
		{
			EE_LOG_CORE_ERROR( L"Error creating SDL renderer" );
			return false;
		}

        internalState->backBuffers = { new Texture() };

        TextureDescription textureDescription;
        textureDescription.layout = ImageLayout_RenderTarget;
        textureDescription.format = description.format;
        textureDescription.width = description.width;
        textureDescription.height = description.height;

        SubresourceData subresource = { internalState->renderer, 0, 0 };
        CreateTexture( textureDescription, &subresource, *internalState->backBuffers[ 0 ] );
        if ( internalState->backBuffers[ 0 ] == NULL )
        {
            EE_LOG_CORE_ERROR( L"Error creating SDL backbuffer" );
            return false;
        }

		outSwapChain.internalState = internalState;
		outSwapChain.description = description;
		outSwapChain.type = ResourceType_Texture;

		return true;
	}

	bool CPUGraphicsDevice::CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const { return false; }

	bool CPUGraphicsDevice::CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const 
    {
        SDL_Texture* texture = SDL_CreateTexture( (SDL_Renderer*)(pInitialData->memoryPointer), ConvertPixelFormat( description.format ), SDL_TEXTUREACCESS_STREAMING, description.width, description.height );

        auto internalState = std::make_shared<CPUTexture>( CPUTexture() );
        internalState->resource = texture;
        outTexture.description = description;
        outTexture.internalState = internalState;
        return false;
    }

	bool CPUGraphicsDevice::CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const { return false; }

	bool CPUGraphicsDevice::CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const { return false; }

	void CPUGraphicsDevice::SetName( GraphicsDeviceResource* pResource, const NChar* name ) { }

	EE::CommandList CPUGraphicsDevice::BeginCommandList( ECommandQueueType queue ) { return CommandList(); }

	void CPUGraphicsDevice::SubmitCommandLists() { }

	void CPUGraphicsDevice::WaitForDevice() const { }

	Texture CPUGraphicsDevice::GetBackBuffer( const SwapChain& swapchain ) const 
    {
        auto swapchainInternal = ToInternal( swapchain );
        return *swapchainInternal->backBuffers[ 0 ];
    }

	void CPUGraphicsDevice::RenderPassBegin( const SwapChain& swapchain, CommandList cmd ) 
	{
		auto internalSwapChain = ToInternal( swapchain );
		Vector4 clearColor = swapchain.description.clearcolor * 255.F;
		
		SDL_SetRenderDrawColor( internalSwapChain->renderer,
			(Uint8)clearColor.r, (Uint8)clearColor.g, (Uint8)clearColor.b, (Uint8)clearColor.a );

		SDL_RenderClear( internalSwapChain->renderer );
        SDL_RenderTexture( internalSwapChain->renderer, ToInternal( *internalSwapChain->backBuffers[ 0 ] )->resource, NULL, NULL);
	}

	void CPUGraphicsDevice::RenderPassEnd( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
		SDL_RenderPresent( internalSwapChain->renderer );
	}

	void CPUGraphicsDevice::BindScissorRects( uint32 numRects, const Box2D* rects, CommandList cmd ) { }

	void CPUGraphicsDevice::BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void CPUGraphicsDevice::BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void CPUGraphicsDevice::BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void CPUGraphicsDevice::BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void CPUGraphicsDevice::UnbindResources( uint32 slot, uint32 num, CommandList cmd ) { }

	void CPUGraphicsDevice::UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) { }

	void CPUGraphicsDevice::BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) { }

	void CPUGraphicsDevice::BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) { }

	void CPUGraphicsDevice::BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) { }

	void CPUGraphicsDevice::BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) { }

	void CPUGraphicsDevice::BindStencilRef( uint32 value, CommandList cmd ) { }

	void CPUGraphicsDevice::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }

	void CPUGraphicsDevice::BindComputeShader( const Shader* cs, CommandList cmd ) { }

	void CPUGraphicsDevice::Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }
	
	void CPUGraphicsDevice::Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) { }

	void CPUGraphicsDevice::DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void CPUGraphicsDevice::CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd ) { }

	void CPUGraphicsDevice::UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int32 dataSize ) { }

    void CPUGraphicsDevice::UpdateTexture( const Texture& texture, const void* data )
    {
        auto textureInternal = ToInternal( texture );

        SDL_UpdateTexture(
            textureInternal->resource,
            NULL, data, (int)(texture.description.width * sizeof( uint32 ))
        );
    }

	GraphicsDevice::DeviceAllocation CPUGraphicsDevice::AllocateToDevice( size_t dataSize, CommandList cmd ) { return DeviceAllocation(); }

	void CPUGraphicsDevice::EventBegin( const char* name, CommandList cmd ) { }

	void CPUGraphicsDevice::EventEnd( CommandList cmd ) { }

	void CPUGraphicsDevice::SetMarker( const char* name, CommandList cmd ) { }
}