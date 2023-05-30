
#include "Graphics/GraphicsDevice.h"

namespace EE
{
	class CPUGraphicsDevice : public GraphicsDevice
	{
		bool CreateSwapChain( const SwapChainDescription& description, Window* window, SwapChain& outSwapChain ) const;
		bool CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const;
		bool CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const;
		bool CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const;
		bool CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const;

		void SetName( GraphicsDeviceResource* pResource, const NChar* name );

		CommandList BeginCommandList( ECommandQueueType queue = ComandQueue_Graphics );
		void SubmitCommandLists();

		void WaitForDevice() const;

		Texture GetBackBuffer( const SwapChain& swapchain ) const;

		void WaitCommandList( CommandList cmd, CommandList wait_for ) {}
		void RenderPassBegin( const SwapChain& swapchain, CommandList cmd );
		void RenderPassEnd( const SwapChain& swapchain, CommandList cmd );
		void BindScissorRects( uint32_t numRects, const Box2D* rects, CommandList cmd );
		void BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32_t slot, CommandList cmd, int subresource = -1 );
		void BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd );
		void BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32_t slot, CommandList cmd, int subresource = -1 );
		void BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32_t slot, uint32_t count, CommandList cmd );
		void UnbindResources( uint32_t slot, uint32_t num, CommandList cmd );
		void UnbindUAVs( uint32_t slot, uint32_t num, CommandList cmd );
		void BindSampler( EShaderStage stage, const Sampler* sampler, uint32_t slot, CommandList cmd );
		void BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32_t slot, CommandList cmd );
		void BindVertexBuffers( const Buffer* const* vertexBuffers, uint32_t slot, uint32_t count, const uint32_t* strides, const uint32_t* offsets, CommandList cmd );
		void BindIndexBuffer( const Buffer* indexBuffer, uint32_t offset, CommandList cmd );
		void BindStencilRef( uint32_t value, CommandList cmd );
		void BindBlendFactor( float r, float g, float b, float a, CommandList cmd );
		void BindComputeShader( const Shader* cs, CommandList cmd );
		void Draw( uint32_t vertexCount, uint32_t startVertexLocation, CommandList cmd );
		void DrawIndexed( uint32_t indexCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, CommandList cmd );
		void DrawInstanced( uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd );
		void DrawIndexedInstanced( uint32_t indexCount, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation, CommandList cmd );
		void DrawInstancedIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd );
		void DrawIndexedInstancedIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd );
		void Dispatch( uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd );
		void DispatchIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd );
		void DispatchMesh( uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ, CommandList cmd ) {}
		void DispatchMeshIndirect( const Buffer* args, uint32_t args_offset, CommandList cmd ) {}
		void CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd );
        void UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int dataSize = -1 );
        void UpdateTexture( const Texture& texture, const void* data );
		void PushConstants( const void* data, uint32_t size_, CommandList cmd ) {}

		DeviceAllocation AllocateToDevice( size_t dataSize, CommandList cmd );

		void EventBegin( const char* name, CommandList cmd );
		void EventEnd( CommandList cmd );
		void SetMarker( const char* name, CommandList cmd );
	};
}