
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Rendering/CPU/CPURenderingInterface.h"

#include <SDL3/SDL.h>

namespace EE
{
    static SDL_PixelFormatEnum ConvertPixelFormat( EPixelFormat vkFormat )
    {
        switch ( vkFormat )
        {
        case PixelFormat_R8G8B8A8_UNORM:
            return SDL_PIXELFORMAT_XRGB8888;
        case PixelFormat_A2R10G10B10_UNORM:
            return SDL_PIXELFORMAT_XRGB2101010;
        case PixelFormat_R16G16B16A16_SNORM:
            return SDL_PIXELFORMAT_RGBA64_FLOAT;
        default:
            return SDL_PIXELFORMAT_UNKNOWN;
        }
    }

    struct CPUWindowContext
    {
    };

    struct CPUTexture
    {
        SDL_Texture* resource;

        ~CPUTexture()
        {
            SDL_DestroyTexture( resource );
        }
    };

    struct CPUSurface
    {
        ~CPUSurface()
        { }
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

    ERenderInterface CPURenderInterface::GetType() const
    {
        return RenderInterface_CPU;
    }

    bool CPURenderInterface::Initialize()
    {
        return true;
    }

    bool CPURenderInterface::CreatePresentContext( Window* window, PresentContext& outContext )
    {
        outContext.internalState = std::make_shared<CPUWindowContext>();

        SurfaceDescription surfaceDescription;
        surfaceDescription.window = window;
        if ( CreateSurface( surfaceDescription, outContext.surface ) == false )
        {
            return false;
        }

        SwapChainDescription swapChainDesc{};
        swapChainDesc.width = window->GetWidth();
        swapChainDesc.height = window->GetHeight();
        swapChainDesc.fullscreen = window->GetWindowMode();
        swapChainDesc.format = PixelFormat_R8G8B8A8_UNORM;
        swapChainDesc.vsync = window->GetVSync();
        if ( CreateSwapChain( swapChainDesc, window, outContext.swapChain ) == false )
        {
            return false;
        }

        return true;
    }

    bool CPURenderInterface::CreateSurface( const SurfaceDescription& description, Surface& outSurface ) const
    {
        auto internalState = std::make_shared<CPUSurface>();

        outSurface.type = RenderingResourceType_Surface;
        outSurface.internalState = internalState;
        outSurface.description = description;
        return true;
    }

    bool CPURenderInterface::CreateSwapChain( const SwapChainDescription& description, Window* window, SwapChain& outSwapChain ) const
	{
		auto internalState = std::make_shared<CPUSwapChain>();

		internalState->renderer = SDL_CreateRenderer( (SDL_Window*)window->GetHandle(), NULL );
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

        if ( CreateTexture( textureDescription, *internalState->backBuffers[ 0 ] )== NULL )
        {
            EE_LOG_CORE_ERROR( L"Error creating SDL backbuffer" );
            return false;
        }

        outSwapChain.type = RenderingResourceType_Swapchain;
		outSwapChain.internalState = internalState;
		outSwapChain.description = description;

		return true;
	}

	bool CPURenderInterface::CreateBuffer( const BufferDescription& description, Buffer& outBuffer ) const { return false; }

	bool CPURenderInterface::CreateTexture( const TextureDescription& description, Texture& outTexture ) const
    {
        auto internalState = std::make_shared<CPUTexture>();
        SDL_Texture* texture = SDL_CreateTexture( 
            NULL,// (SDL_Renderer*)window->GetHandle(),
            ConvertPixelFormat( description.format ),
            SDL_TEXTUREACCESS_STREAMING,
            description.width,
            description.height 
        );
        internalState->resource = texture;
        outTexture.type = RenderingResourceType_Texture;
        outTexture.description = description;
        outTexture.internalState = internalState;
        return true;
    }

	bool CPURenderInterface::CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const { return false; }

	bool CPURenderInterface::CreateShaderStage( EShaderStage stage, const void* code, size_t codeLength, ShaderStage& outShaderStage ) const { return false; }

	void CPURenderInterface::SetName( RenderResource* pResource, const NChar* name ) { }

	EE::CommandList CPURenderInterface::BeginCommandList( ECommandQueueType queue ) { return CommandList(); }

	void CPURenderInterface::SubmitCommandLists() { }

	void CPURenderInterface::WaitForDevice() const { }

	Texture* CPURenderInterface::GetBackBuffer( const SwapChain& swapchain ) const
    {
        auto swapchainInternal = ToInternal( swapchain );
        return swapchainInternal->backBuffers[ 0 ];
    }

	void CPURenderInterface::RenderPassBegin( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
		Vector4 clearColor = swapchain.description.clearcolor * 255;

        SDL_Texture* buffer = ToInternal( *internalSwapChain->backBuffers[ 0 ] )->resource;

        SDL_SetRenderTarget( internalSwapChain->renderer, buffer );

        const auto bm = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD,

            SDL_BLENDFACTOR_ONE,
            SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            SDL_BLENDOPERATION_ADD );
        SDL_SetTextureBlendMode( buffer, bm );

        SDL_SetRenderDrawBlendMode( internalSwapChain->renderer, SDL_BLENDMODE_NONE );
        SDL_SetRenderDrawColor( internalSwapChain->renderer,
        	(Uint8)clearColor.r, (Uint8)clearColor.g, (Uint8)clearColor.b, (Uint8)clearColor.a );
        SDL_RenderFillRect( internalSwapChain->renderer, NULL );

        SDL_RenderTexture( internalSwapChain->renderer, buffer, NULL, NULL );

		// SDL_RenderClear( internalSwapChain->renderer );
        // 
        // SDL_SetRenderDrawBlendMode( internalSwapChain->renderer, SDL_BLENDMODE_ADD );
        // SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_MOD );
        // SDL_RenderTexture( internalSwapChain->renderer, texture, NULL, NULL);
	}

	void CPURenderInterface::RenderPassEnd( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
		SDL_RenderPresent( internalSwapChain->renderer );
	}

	void CPURenderInterface::BindScissorRects( uint32 numRects, const Rect* rects, CommandList cmd ) { }

	void CPURenderInterface::BindResource( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void CPURenderInterface::BindResources( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void CPURenderInterface::BindUAV( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void CPURenderInterface::BindUAVs( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void CPURenderInterface::UnbindResources( uint32 slot, uint32 num, CommandList cmd ) { }

	void CPURenderInterface::UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) { }

	void CPURenderInterface::BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) { }

	void CPURenderInterface::BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) { }

	void CPURenderInterface::BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) { }

	void CPURenderInterface::BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) { }

	void CPURenderInterface::BindStencilRef( uint32 value, CommandList cmd ) { }

	void CPURenderInterface::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }

	void CPURenderInterface::Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) { }

	void CPURenderInterface::DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) { }

	void CPURenderInterface::DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void CPURenderInterface::DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void CPURenderInterface::DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void CPURenderInterface::DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }
	
	void CPURenderInterface::Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) { }

	void CPURenderInterface::DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void CPURenderInterface::CopyResource( const RenderResource* pDst, const RenderResource* pSrc, CommandList cmd ) { }

    void CPURenderInterface::UpdateTexture( const Texture& texture, const void* data )
    {
        auto textureInternal = ToInternal( texture );

        SDL_UpdateTexture(
            textureInternal->resource,
            NULL, data, (int)(texture.description.width * sizeof( uint32 ))
        );
    }

	void CPURenderInterface::EventBegin( const char* name, CommandList cmd ) { }

	void CPURenderInterface::EventEnd( CommandList cmd ) { }

	void CPURenderInterface::SetMarker( const char* name, CommandList cmd ) { }

    WString const& CPURenderInterface::GetName() const
    {
        static WString const deviceName = L"CPU";
        return deviceName;
    }
}