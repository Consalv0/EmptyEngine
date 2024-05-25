
#include "Graphics/GraphicsDevice.h"

namespace EE
{
	class VulkanGraphicsDevice : public GraphicsDevice
	{
    protected:
        void* VulkanInstance;
        void* VulkanPhysicalDevice;
        void* VulkanDevice;
        bool Initialized;

    private:
        ~VulkanGraphicsDevice();

        EWindowGraphicsAPI GetWindowGraphicsAPI() const;

        bool Initialize();
        bool StartDevices();

        bool CreateWindowContext( class Window* window, WindowContext& windowContext ) const;
        bool CreateWindowSurface( const WindowSurfaceDescription& description, WindowSurface& outWindowSurface ) const;
		bool CreateSwapChain( const SwapChainDescription& description, class Window* window, SwapChain& outSwapChain ) const;
		bool CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const;
		bool CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const;
		bool CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const;
		bool CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const;

		void SetName( GraphicsDeviceResource* pResource, const NChar* name );

		CommandList BeginCommandList( ECommandQueueType queue = ComandQueue_Graphics );
		void SubmitCommandLists();

		void WaitForDevice() const;

		Texture* GetBackBuffer( const SwapChain& swapchain ) const;

		void WaitCommandList( CommandList cmd, CommandList wait_for ) {}
		void RenderPassBegin( const SwapChain& swapchain, CommandList cmd );
		void RenderPassEnd( const SwapChain& swapchain, CommandList cmd );
		void BindScissorRects( uint32 numRects, const Rect* rects, CommandList cmd );
		void BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource = -1 );
		void BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd );
		void BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource = -1 );
		void BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd );
		void UnbindResources( uint32 slot, uint32 num, CommandList cmd );
		void UnbindUAVs( uint32 slot, uint32 num, CommandList cmd );
		void BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd );
		void BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd );
		void BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd );
		void BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd );
		void BindStencilRef( uint32 value, CommandList cmd );
		void BindBlendFactor( float r, float g, float b, float a, CommandList cmd );
		void BindComputeShader( const Shader* cs, CommandList cmd );
		void Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd );
		void DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd );
		void DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd );
		void DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd );
		void DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd );
		void DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd );
		void Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd );
		void DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd );
		void DispatchMesh( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) {}
		void DispatchMeshIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) {}
		void CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd );
        void UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int dataSize = -1 );
        void UpdateTexture( const Texture& texture, const void* data );
		void PushConstants( const void* data, uint32 size, CommandList cmd ) {}

		DeviceAllocation AllocateToDevice( size_t dataSize, CommandList cmd );

		void EventBegin( const char* name, CommandList cmd );
		void EventEnd( CommandList cmd );
		void SetMarker( const char* name, CommandList cmd );

        void* GetDeviceInstance() { return VulkanInstance; };
	};
}