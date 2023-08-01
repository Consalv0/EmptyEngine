#pragma once

#include "Math/CoreMath.h"
#include "Graphics/Graphics.h"

namespace EE
{
	typedef uint8_t CommandList;
	static const CommandList COMMANDLIST_COUNT = 32;
	static const CommandList INVALID_COMMAND_LIST = COMMANDLIST_COUNT;

	class GraphicsDevice
	{
	protected:
		static const uint32 bufferCount_ = 2;
		bool debugDevice_ = false;
		uint32 capabilities_ = 0;
		uint64_t timeStampFrecuency_ = 0;

	public:
		virtual ~GraphicsDevice() = default;

		virtual bool CreateSwapChain( const SwapChainDescription& description, class Window* window, SwapChain& outSwapChain ) const = 0;
		virtual bool CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const = 0;
		virtual bool CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const = 0;
		virtual bool CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const = 0;
		virtual bool CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const = 0;
		// virtual bool CreateQueryHeap( const GPUQueryHeapDesc* pDesc, GPUQueryHeap* pQueryHeap ) const = 0;
		// virtual bool CreatePipelineState( const PipelineStateDescription& description, PipelineState* outPipeline ) const = 0;
		// virtual bool CreateRenderPass( const RenderPassDesc* pDesc, RenderPass* renderpass ) const = 0;

		// virtual int CreateSubresource( Texture* texture, SUBRESOURCE_TYPE type, uint32 firstSlice, uint32 sliceCount, uint32 firstMip, uint32 mipCount ) const = 0;
		// virtual int CreateSubresource( Buffer* buffer, SUBRESOURCE_TYPE type, uint64_t offset, uint64_t size = ~0 ) const = 0;

		// virtual int GetDescriptorIndex( const GraphicsDeviceResource* resource, SUBRESOURCE_TYPE type, int subresource = -1 ) const { return -1; };
		// virtual int GetDescriptorIndex( const Sampler* sampler ) const { return -1; };

		// virtual void Map( const GPUResource* resource, Mapping* mapping ) const = 0;
		// virtual void Unmap( const GPUResource* resource ) const = 0;
		// virtual void QueryRead( const GPUQueryHeap* heap, uint32 index, uint32 count, uint64_t* results ) const = 0;

		// virtual void SetCommonSampler( const StaticSampler* sam ) = 0;

		virtual void SetName( GraphicsDeviceResource* pResource, const NChar* name ) = 0;

		// Begin a new command list for GPU command recording.
		//	This will be valid until SubmitCommandLists() is called.
		virtual CommandList BeginCommandList( ECommandQueueType queue = ComandQueue_Graphics ) = 0;
		// Submit all command list that were used with BeginCommandList before this call.
		//	This will make every command list to be in "available" state and restarts them
		virtual void SubmitCommandLists() = 0;

		virtual void WaitForDevice() const = 0;
		virtual void ClearPipelineStateCache() {};

		FORCEINLINE bool CheckCapability( EGraphicDeviceCapability capability ) const { return capabilities_ & capability; }

		static constexpr uint32 GetBufferCount() { return bufferCount_; }

		constexpr bool IsDebugDevice() const { return debugDevice_; }

		constexpr uint64_t GetTimestampFrequency() const { return timeStampFrecuency_; }

		virtual EShaderFormat GetShaderFormat() const { return ShaderFormat_None; }

		virtual Texture GetBackBuffer( const SwapChain& swapchain ) const = 0;

		///////////////Thread-sensitive////////////////////////

		virtual void WaitCommandList( CommandList cmd, CommandList wait_for ) {}
		virtual void RenderPassBegin( const SwapChain& swapchain, CommandList cmd ) = 0;
		// virtual void RenderPassBegin( const RenderPass* renderpass, CommandList cmd ) = 0;
		virtual void RenderPassEnd( const SwapChain& swapchain, CommandList cmd ) = 0;
		virtual void BindScissorRects( uint32 numRects, const Box2* rects, CommandList cmd ) = 0;
		// virtual void BindViewports( uint32 NumViewports, const Viewport* pViewports, CommandList cmd ) = 0;
		virtual void BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource = -1 ) = 0;
		virtual void BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) = 0;
		virtual void BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource = -1 ) = 0;
		virtual void BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) = 0;
		virtual void UnbindResources( uint32 slot, uint32 num, CommandList cmd ) = 0;
		virtual void UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) = 0;
		virtual void BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) = 0;
		virtual void BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) = 0;
		virtual void BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) = 0;
		virtual void BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) = 0;
		virtual void BindStencilRef( uint32 value, CommandList cmd ) = 0;
		virtual void BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) = 0;
		// virtual void BindShadingRate( SHADING_RATE rate, CommandList cmd ) {}
		// virtual void BindPipelineState( const PipelineState* pso, CommandList cmd ) = 0;
		virtual void BindComputeShader( const Shader* cs, CommandList cmd ) = 0;
		virtual void Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) = 0;
		virtual void DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) = 0;
		virtual void DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) = 0;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) = 0;
		virtual void DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
		virtual void DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
		virtual void Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) = 0;
		virtual void DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) = 0;
		virtual void DispatchMesh( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) {}
		virtual void DispatchMeshIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) {}
		virtual void CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd ) = 0;
		virtual void UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int dataSize = -1 ) = 0;
        virtual void UpdateTexture( const Texture& texture, const void* data ) = 0;
		// virtual void QueryBegin( const GPUQueryHeap* heap, uint32 index, CommandList cmd ) = 0;
		// virtual void QueryEnd( const GPUQueryHeap* heap, uint32 index, CommandList cmd ) = 0;
		// virtual void QueryResolve( const GPUQueryHeap* heap, uint32 index, uint32 count, CommandList cmd ) {}
		// virtual void Barrier( const GPUBarrier* barriers, uint32 numBarriers, CommandList cmd ) = 0;
		virtual void PushConstants( const void* data, uint32 size_, CommandList cmd ) {}

		struct DeviceAllocation
		{
			void* data = nullptr;				// application can write to this. Reads might be not supported or slow. The offset is already applied
			const Buffer* buffer = nullptr;	// application can bind it to the GPU
			uint32 offset = 0;					// allocation's offset from the GPUbuffer's beginning

			// Returns true if the allocation was successful
			inline bool IsValid() const { return data != nullptr && buffer != nullptr; }
		};
		// Allocates temporary memory that the CPU can write and GPU can read. 
		//	It is only alive for one frame and automatically invalidated after that.
		//	The CPU pointer gets invalidated as soon as there is a Draw() or Dispatch() event on the same thread
		//	This allocation can be used to provide temporary vertex buffer, index buffer or raw buffer data to shaders
		virtual DeviceAllocation AllocateToDevice( size_t dataSize, CommandList cmd ) = 0;

		virtual void EventBegin( const char* name, CommandList cmd ) = 0;
		virtual void EventEnd( CommandList cmd ) = 0;
		virtual void SetMarker( const char* name, CommandList cmd ) = 0;
	};
}