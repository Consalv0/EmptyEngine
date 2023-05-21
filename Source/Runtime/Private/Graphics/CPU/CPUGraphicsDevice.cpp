
#include "CoreMinimal.h"

#include "Core/Window.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"

#include "SDL3/SDL.h"

// --- Make discrete GPU by default.
extern "C" {
	// --- developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	// --- developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}

namespace EEngine
{
	struct CPUSwapChain
	{
		SDL_Renderer* renderer;
		SDL_Texture* backBuffer;
		
		~CPUSwapChain()
		{
			SDL_DestroyRenderer( renderer );
			SDL_DestroyTexture( backBuffer );
		}
	};

	CPUSwapChain* ToInternal( const SwapChain& swapChain )
	{
		return static_cast<CPUSwapChain*>(swapChain.internalState.get());
	}

	bool CPUGraphicsDevice::CreateSwapChain( const SwapChainDescription& description, void* window, SwapChain& outSwapChain ) const 
	{
		auto internalState = std::make_shared<CPUSwapChain>( CPUSwapChain() );

		internalState->renderer = SDL_CreateRenderer( (SDL_Window*)window, NULL, 0 );
		if ( internalState->renderer == NULL )
		{
			LOG_CORE_ERROR( L"Error creating SDL renderer" );
			return false;
		}

		outSwapChain.internalState = internalState;
		outSwapChain.description = description;
		outSwapChain.type = ResourceType_Texture;

		return true;
	}

	bool CPUGraphicsDevice::CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const { return false; }

	bool CPUGraphicsDevice::CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const { return false; }

	bool CPUGraphicsDevice::CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const { return false; }

	bool CPUGraphicsDevice::CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const { return false; }

	void CPUGraphicsDevice::SetName( GraphicsDeviceResource* pResource, const NChar* name ) { }

	EEngine::CommandList CPUGraphicsDevice::BeginCommandList( ECommandQueueType queue ) { return CommandList(); }

	void CPUGraphicsDevice::SubmitCommandLists() { }

	void CPUGraphicsDevice::WaitForDevice() const { }

	Texture CPUGraphicsDevice::GetBackBuffer( const SwapChain* swapchain ) const { return Texture(); }

	void CPUGraphicsDevice::RenderPassBegin( const SwapChain& swapchain, CommandList cmd ) 
	{
		auto internalSwapChain = ToInternal( swapchain );
		Vector4 clearColor = swapchain.description.clearcolor * 255.F;
		
		SDL_SetRenderDrawColor( internalSwapChain->renderer,
			(Uint8)clearColor.r, (Uint8)clearColor.g, (Uint8)clearColor.b, (Uint8)clearColor.a );

		SDL_RenderClear( internalSwapChain->renderer );
	}

	void CPUGraphicsDevice::RenderPassEnd( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
		SDL_RenderPresent( internalSwapChain->renderer );
	}

	void CPUGraphicsDevice::BindScissorRects( uint32_t numRects, const Box2D* rects, CommandList cmd ) { }

	void CPUGraphicsDevice::BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32_t slot, CommandList cmd, int subresource ) { }

	void CPUGraphicsDevice::BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd ) { }

	void CPUGraphicsDevice::BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32_t slot, CommandList cmd, int subresource ) { }

	void CPUGraphicsDevice::BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd ) { }

	void CPUGraphicsDevice::UnbindResources( uint32_t slot, uint32_t num, CommandList cmd ) { }

	void CPUGraphicsDevice::UnbindUAVs( uint32_t slot, uint32_t num, CommandList cmd ) { }

	void CPUGraphicsDevice::BindSampler( EShaderStage stage, const Sampler* sampler, uint32_t slot, CommandList cmd ) { }

	void CPUGraphicsDevice::BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32_t slot, CommandList cmd ) { }

	void CPUGraphicsDevice::BindVertexBuffers( const Buffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets, CommandList cmd ) { }

	void CPUGraphicsDevice::BindIndexBuffer( const Buffer* indexBuffer, uint32_t offset, CommandList cmd ) { }

	void CPUGraphicsDevice::BindStencilRef( uint32_t value, CommandList cmd ) { }

	void CPUGraphicsDevice::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }

	void CPUGraphicsDevice::BindComputeShader( const Shader* cs, CommandList cmd ) { }

	void CPUGraphicsDevice::Draw( uint32_t vertexCount, uint32_t startVertexLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexed( uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawInstanced( uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexedInstanced( uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawInstancedIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd ) { }

	void CPUGraphicsDevice::DrawIndexedInstancedIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd ) { }
	
	void CPUGraphicsDevice::Dispatch( uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd ) { }

	void CPUGraphicsDevice::DispatchIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd ) { }

	void CPUGraphicsDevice::CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd ) { }

	void CPUGraphicsDevice::UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int dataSize ) { }

	GraphicsDevice::DeviceAllocation CPUGraphicsDevice::AllocateToDevice( size_t dataSize, CommandList cmd ) { return DeviceAllocation(); }

	void CPUGraphicsDevice::EventBegin( const char* name, CommandList cmd ) { }

	void CPUGraphicsDevice::EventEnd( CommandList cmd ) { }

	void CPUGraphicsDevice::SetMarker( const char* name, CommandList cmd ) { }
}