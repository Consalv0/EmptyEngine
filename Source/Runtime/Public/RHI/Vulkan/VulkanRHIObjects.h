
#include <optional>

namespace EE
{
    static bool CreateNativeVmaAllocator( VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator* outAllocator );

    class VulkanRHIPhysicalDevice;
    class VulkanRHIInstance;
    class VulkanRHIDevice;
    class VulkanRHIPresentContext;
    class VulkanRHISemaphore;
    class VulkanRHIFence;
    class VulkanRHIQueue;
    class VulkanRHIBuffer;
    class VulkanRHITexture;
    class VulkanRHITextureView;
    class VulkanRHISwapChain;
    class VulkanRHISurface;
    class VulkanRHICommandPool;
    class VulkanRHICommandBuffer;
    class VulkanRHIShaderStage;
    class VulkanRHIBindGroup;

    struct QueueFamilyIndices
    {
        std::optional<uint32> graphicsFamily;
        std::optional<uint32> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct VulkanSurfaceSupportDetails
    {
        VkBool32 supported = false;
        VkSurfaceCapabilitiesKHR capabilities = {};
        uint32 formatCount = 0;
        TArray<VkSurfaceFormatKHR> formats;
        uint32 presentModeCount = 0;
        TArray<VkPresentModeKHR> presentModes;
    };

    class VulkanRHIPhysicalDevice
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIPhysicalDevice )

    public:
        VulkanRHIPhysicalDevice( VkInstance instance, VkPhysicalDevice physicalDevice );

        ~VulkanRHIPhysicalDevice();

        bool CheckDeviceExtensionSupport() const;

        QueueFamilyIndices GetQueueFamilies() const;

        bool IsDeviceSuitable( VulkanRHISurface* surface ) const;

        int RateDeviceSuitabilityForSurface( VulkanRHISurface* surface ) const;

        void AddSurfaceSupportDetails( VkSurfaceKHR surface, uint32 queueFamilyIndex );

        FORCEINLINE const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice_; }

        FORCEINLINE VkPhysicalDevice GetPhysicalDevice() { return physicalDevice_; }

        const VulkanSurfaceSupportDetails* GetSurfaceSupportDetails( VkSurfaceKHR surface ) const;

        void UpdateSurfaceSupportDetails( VkSurfaceKHR surface );

        FORCEINLINE const VkPhysicalDeviceProperties& GetProperties() const { return deviceProperties_; }

        FORCEINLINE const bool HasSurfaceAnyFormat( VkSurfaceKHR surface ) const { return surfaceSupportDetails_.at( surface ).formatCount > 0; }
        FORCEINLINE const VulkanSurfaceSupportDetails& GetSurfaceDetails( VkSurfaceKHR surface ) const { return surfaceSupportDetails_.at( surface ); }
        FORCEINLINE const bool HasSurfaceAnyPresentMode( VkSurfaceKHR surface ) const { return surfaceSupportDetails_.at( surface ).presentModeCount > 0; }

    private:
        VkInstance instance_;
        VkPhysicalDevice physicalDevice_;
        VkPhysicalDeviceProperties deviceProperties_;
        VkPhysicalDeviceFeatures deviceFeatures_;
        uint32 extensionCount_;
        TArray<VkExtensionProperties> extensions_;
        uint32 queueFamilyCount_;
        TArray<VkQueueFamilyProperties> queueFamilies_;

        TMap<VkSurfaceKHR, VulkanSurfaceSupportDetails> surfaceSupportDetails_;
    };

    class VulkanRHIInstance : RHIInstance
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIInstance )

    private:
        VkInstance instance;

        uint32 physicalDeviceCount;
        TArray<VulkanRHIPhysicalDevice*> physicalDevices;
        uint32 selectedDeviceIndex;

    public:
        VulkanRHIInstance( VkInstanceCreateInfo& createInfo );

        ~VulkanRHIInstance() override;

        int32 PickPhysicalDeviceForSurface( VulkanRHISurface* surface ) const;

        bool SelectSuitableDevice( VulkanRHISurface* surface );

        const VulkanRHIPhysicalDevice* GetSelectedPhysicalDevice() const;
        VulkanRHIPhysicalDevice* GetSelectedPhysicalDevice();

        bool AddSurfaceSupportDetails( VulkanRHISurface* surface );

        bool IsValid() const override;

        FORCEINLINE const uint32& GetPhysicalDeviceCount() const { return physicalDeviceCount; }
        FORCEINLINE const VulkanRHIPhysicalDevice& GetPhysicalDevice( uint32& index ) const { return *physicalDevices[ index ]; }
        FORCEINLINE const VkInstance& GetVulkanInstance() const { return instance; };
    };

    class VulkanRHIQueue : public RHIQueue
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIQueue )

    private:
        VkQueue queue;
        const VulkanRHIDevice* device;

    public:
        ~VulkanRHIQueue() override;

        VulkanRHIQueue( const VulkanRHIDevice* device, const uint32& familyIndex, const uint32& queueIndex );

        bool IsValid() const override;

        FORCEINLINE const VkQueue& GetVulkanQueue() const { return queue; }

        void WaitIdle() const override;

        void SubmitCommandBuffer( const RHICommandBuffer* commandBuffer, const RHIQueueSubmitInfo& info ) override;
    };

    class VulkanRHIDevice : public RHIDevice
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIDevice )

    private:
        VulkanRHIPhysicalDevice* physicalDevice;
        VkDevice device;
        VmaAllocator allocator;
        // map of command pools by queue family index
        TMap<uint32, VulkanRHICommandPool*> commandPools;

        union
        {
            uint32 graphicsQueueIndex;
            uint32 presentQueueIndex;
            uint32 queueFamilyIndices[ 2 ]{};
        };

        VulkanRHIQueue* graphicsQueue;
        VulkanRHIQueue* presentQueue;

        RHIDeviceLimits deviceLimits;

    public:
        VulkanRHIDevice( VulkanRHIInstance* instance );

        ~VulkanRHIDevice() override;

        FORCEINLINE const VkDevice GetVulkanDevice() const { return device; }

        FORCEINLINE const VulkanRHIPhysicalDevice* GetVulkanPhysicalDevice() const { return physicalDevice; }

        FORCEINLINE VulkanRHIPhysicalDevice* GetVulkanPhysicalDevice() { return physicalDevice; }

        FORCEINLINE const VmaAllocator GetVulkanAllocator() const { return allocator; }

        FORCEINLINE uint32 GetGraphicsFamilyIndex() const { return graphicsQueueIndex; }

        FORCEINLINE const RHIDeviceLimits& GetLimits() const override { return deviceLimits; }

        FORCEINLINE VulkanRHIQueue* GetVulkanGraphicsQueue() const { return graphicsQueue; }

        RHIQueue* GetGraphicsQueue() const override { return GetVulkanGraphicsQueue(); }

        FORCEINLINE uint32 GetPresentFamilyIndex() const { return presentQueueIndex; }

        FORCEINLINE VulkanRHIQueue* GetVulkanPresentQueue() const { return presentQueue; }

        RHIQueue* GetPresentQueue() const override { return GetVulkanPresentQueue(); }

        FORCEINLINE const uint32* GetFamilyIndices() const { return queueFamilyIndices; }

        const VulkanRHICommandPool* GetCommandPool( uint32 familyIndex ) const;

        bool IsValid() const override;

    private:

        bool CreateCommandBufferPools( VulkanRHIDevice* device );
    };

    class VulkanRHIPresentContext : public RHIPresentContext
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIPresentContext )

    private:
        VulkanRHIInstance* instance;
        //* The window this context is for
        Window* window;
        VulkanRHISurface* surface;
        VulkanRHISwapChain* swapChain;
        RHISurfaceFormat selectedSurfaceFormat_;
        bool isSwapChainDirty_;

        TList<VulkanRHICommandBuffer> commandBuffers;

        //* We'll need one semaphore to signal that an image has been acquired from the swapchain and is ready for rendering,
        TList<VulkanRHISemaphore> presentSemaphores;
        //* another one to signal that rendering has finished and presentation can happen,
        TList<VulkanRHISemaphore> renderSemaphores;
        //* and a fence to make sure only one frame is rendering at a time.
        TList<VulkanRHIFence> renderFences;

        void CreateSurface();

        void CreateSwapChain();

        void CreateCommandBuffers();

        void CreateSyncObjects();

    public:
        VulkanRHIPresentContext( Window* window, VulkanRHIInstance* instance );
        ~VulkanRHIPresentContext() override;

        FORCEINLINE VulkanRHISurface* GetRHISurface() { return surface; }
        FORCEINLINE const VulkanRHISurface* GetRHISurface() const { return surface; }
        FORCEINLINE VulkanRHISwapChain* GetRHISwapChain() { return swapChain; }
        FORCEINLINE const VulkanRHISwapChain* GetRHISwapChain() const { return swapChain; }
        FORCEINLINE const Window* GetWindow() const { return window; }

        const RHISurfaceFormat& GetSurfaceFormat() const override { return selectedSurfaceFormat_; }

        const VulkanRHISemaphore& GetPresentSempahore( uint32 frameIndex ) const;

        const VulkanRHISemaphore& GetRenderSempahore( uint32 frameIndex ) const;

        const RHICommandBuffer* GetRenderCommandBuffer() const override;

        const RHIFence* GetRenderFence() const override;

        const RHITextureView* GetBackbuffer() const override;

        void UpdateSelectedSurfaceFormat();

        void SetSwapChainDirty() override;

        void Present() override;

        bool SubmitPresentImage( VulkanRHIQueue* queue ) const;

        void SubmitRenderCommandBuffer( EPipelineStage stage ) const override;

        void AquireBackbuffer( uint64 timeout ) override;

        bool IsValid() const override;

    protected:
        void RecreateSwapChain();
    };

    class VulkanRHIBuffer : public RHIBuffer
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIBuffer )

    private:
        VulkanRHIDevice* device;
        VkBuffer buffer;
        VmaAllocation nativeAllocation;
        EBufferUsageFlags usage;
        ESharingMode sharing;
        uint64 size;
        uint64 aligment;
        uint64 offset;

    public:
        VulkanRHIBuffer( const RHIBufferCreateInfo& info, VulkanRHIDevice* device );
        ~VulkanRHIBuffer() override;

        bool IsValid() const override;

        uint64 GetSize() const override;

        uint64 GetOffset() const override;

        uint64 GetAligment() const override;

        void UploadData( const void* data, uint64 offset, uint64 size ) const override;

        void BindBuffer() const;

        const VkBuffer& GetVulkanBuffer() const { return buffer; };
    };

    class VulkanRHIVertexBuffer : public VulkanRHIBuffer
    {

    };

    class VulkanRHISampler final : public RHISampler
    {
    public:
        EE_CLASSNOCOPY( VulkanRHISampler )

    public:
        VulkanRHISampler( const RHISamplerCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHISampler() override;

        const EFilterMode& GetFilterMode() const override { return filter_; }

        const ESamplerMode& GetSamplerModeU() const override { return modeU_; }
        const ESamplerMode& GetSamplerModeV() const override { return modeV_; }
        const ESamplerMode& GetSamplerModeW() const override { return modeW_; }

        const ESamplerBorder& GetSamplerBorder() const override { return border_; }

        const VkSampler GetVulkanSampler() const { return sampler_; }

        bool IsValid() const override;

    private:
        VulkanRHIDevice* device_;
        VkSampler sampler_;
        EFilterMode filter_;
        ESamplerMode modeU_, modeV_, modeW_;
        ESamplerBorder border_;
    };

    class VulkanRHITextureView final : public RHITextureView
    {
    public:
        EE_CLASSNOCOPY( VulkanRHITextureView )

    public:
        VulkanRHITextureView( const RHITextureViewCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHITextureView() override;

        const RHITexture* GetTexture() const override { return (const RHITexture*)vulkanTexture_; }

        const VulkanRHITexture* GetVulkanTexture() const { return vulkanTexture_; }

        const VkImageView& GetVulkanImageView() const { return imageView_; }

        bool IsValid() const override;

    private:
        VulkanRHIDevice* device_;
        const VulkanRHITexture* vulkanTexture_;
        VkImageView imageView_;
        ETextureType type_;
    };

    class VulkanRHITexture final : public RHITexture
    {
    public:
        EE_CLASSNOCOPY( VulkanRHITexture )

    public:
        VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device, VkImage image );

        VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHITexture() override;

        const VkImage& GetVulkanImage() const { return image_; }

        const uint32& GetWidth() const override { return extents_.x; };

        const uint32& GetHeight() const override { return extents_.y; };

        const uint32& GetDepth() const override { return extents_.z; };

        const UIntVector3& GetExtents() const override { return extents_; };

        const EPixelFormat& GetFormat() const override { return pixelFormat_; };

        bool IsValid() const override;

    private:
        VulkanRHIDevice* device_;
        VkImage image_;
        VmaAllocation memory_;
        UIntVector3 extents_;
        VkFormat format_;
        bool ownership_;

        EPixelFormat pixelFormat_;
    };

    class VulkanRHISwapChain final : public RHISwapChain
    {
    public:
        EE_CLASSNOCOPY( VulkanRHISwapChain )

    private:
        VulkanRHIDevice* device;
        const VulkanRHIPresentContext* presentContext;
        VkSwapchainKHR swapChain;
        VkExtent2D size;
        uint32 imageCount;
        TArray<VkImage> images;
        TArray<VulkanRHITexture*> textures;
        TArray<VulkanRHITextureView*> textureViews;
        uint32 backImageIndex;

    public:
        VulkanRHISwapChain( const RHISwapChainCreateInfo& info, const VulkanRHIPresentContext* presentContext, VulkanRHIDevice* device );

        ~VulkanRHISwapChain() override;

        void Cleanup();

        void CreateSwapChain( const RHISwapChainCreateInfo& info );

        FORCEINLINE const uint32& GetImageCount() const { return imageCount; }

        FORCEINLINE const VkSwapchainKHR& GetVulkanSwapChain() const { return swapChain; }

        FORCEINLINE const uint32& BackImageIndex() const { return backImageIndex; }

        bool AquireNextImage( uint64 timeout, const VulkanRHISemaphore* semaphore, const VulkanRHIFence* fence );

        VkImage GetImage( uint32 index ) const { return images[ index ]; }

        const VulkanRHITextureView* GetTextureView( uint32 index ) const { return textureViews[ index ]; }
        
        bool IsValid() const override;
    };

    class VulkanRHISurface final : public RHISurface
    {
    public:
        EE_CLASSNOCOPY( VulkanRHISurface )

    private:
        VulkanRHIInstance* instance;
        VkSurfaceKHR surface;

    public:
        VulkanRHISurface( Window* window, VulkanRHIInstance* instance );
        ~VulkanRHISurface() override;

    public:
        FORCEINLINE const VkSurfaceKHR GetVulkanSurface() const { return surface; }

        bool IsValid() const override;
    };

    class VulkanRHIFence : public RHIFence
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIFence )

    private:
        VulkanRHIDevice* device;
        VkFence fence;

    public:
        VulkanRHIFence( VulkanRHIDevice* device, bool initSignaled );
        ~VulkanRHIFence() override;

        FORCEINLINE const VkFence& GetVulkanFence() const { return fence; }

        bool IsSignaled() const override;
        void Reset() const override;
        void Wait( uint64 timeout ) const override;

        bool IsValid() const override;
    };

    class VulkanRHISemaphore : public RHISemaphore
    {
    public:
        EE_CLASSNOCOPY( VulkanRHISemaphore )

    private:
        VulkanRHIDevice* device;
        VkSemaphore semaphore;

    public:
        VulkanRHISemaphore( VulkanRHIDevice* device );
        ~VulkanRHISemaphore() override;

        FORCEINLINE const VkSemaphore& GetVulkanSemaphore() const { return semaphore; }

        bool IsValid() const override;
    };

    // TODO 
    // You can allocate as many VkCommandBuffer as you want from a given pool,
    // but you can only record commands from one thread at a time.
    // If you want multithreaded command recording, you need more VkCommandPool objects.
    class VulkanRHICommandPool : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( VulkanRHICommandPool )

    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandPool commandPool;

    public:
        VulkanRHICommandPool( VulkanRHIDevice* device, uint32 queueFamilyIndex );
        ~VulkanRHICommandPool() override;

        bool IsValid() const override;

        FORCEINLINE const VkCommandPool& GetVulkanCommandPool() const { return commandPool; }

    };

    class VulkanRHICommandBuffer : public RHICommandBuffer
    {
    public:
        EE_CLASSNOCOPY( VulkanRHICommandBuffer )

    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandBuffer commandBuffer;

    public:
        ~VulkanRHICommandBuffer() override;

        VulkanRHICommandBuffer( const RHICommandBufferCreateInfo& info, VulkanRHIDevice* device, uint32 queueFamilyIndex );

        bool IsValid() const override;

        FORCEINLINE const VkCommandBuffer& GetVulkanCommandBuffer() const { return commandBuffer; }

        void Begin() const override;

        void End() const override;

        void BindGraphicsPipeline( const RHIGraphicsPipeline* pipeline ) const override;

        void BindBindingsGroup( const RHIGraphicsPipeline* pipeline, const class RHIBindGroup* bindGroup, const uint32& dynamicOffsetsCount, const uint32* dynamicOffsets ) const override;

        void BindVertexBuffer( const class RHIBuffer* buffer, uint32 bindIndex, uint64 offset ) const override;

        void BindIndexBuffer( const class RHIBuffer* buffer, uint64 offset ) const override;

        void SetCullMode( const ECullModeFlags& cull ) const override;

        void SetFrontFace( const EFaceWinding& frontFace ) const override;

        void SetViewport( Box2f viewport, float minDepth, float maxDepth ) const override;

        void SetScissor( IntBox2 scissor ) const override;

        void TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout& from, const ETextureLayout& to ) const override;

        void CopyBuffer( const RHIBuffer* fromBuffer, const RHIBuffer* toBuffer, uint32 regionCount, const RHIBufferCopyRegion* regions ) const override;

        void CopyBufferToTexture( const RHIBuffer* buffer, const RHITexture* texture, const ETextureLayout& layout, uint32 regionCount, const RHIBufferImageCopyRegion* regions ) const override;

        void ClearColor( Vector3f color, const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer ) const override;
        
        void DrawIndexed( uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance ) const override;

        void Draw( uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance ) const override;
    };

    class VulkanRHIShaderStage : public RHIShaderStage
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIShaderStage )

    private:
        VulkanRHIDevice* device;

        const U8Char* entryPoint;

        VkShaderModule shaderModule;

    public:
        ~VulkanRHIShaderStage() override;

        VulkanRHIShaderStage( const RHIShaderStageCreateInfo& info, VulkanRHIDevice* device );

        bool IsValid() const override;

        FORCEINLINE const VkShaderModule GetVulkanShaderModule() const { return shaderModule; }

        const U8Char* GetEntryPoint() const override;
    };

    class VulkanRHIRenderPass : public RHIRenderPass
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIRenderPass )

    private:
        VkRenderPass renderPass;

        VulkanRHIDevice* device;

        VkFramebuffer lastAttachment;

        VkRect2D renderArea;

        union
        {
            VkClearValue clearValues[ 2 ];
            struct
            {
                VkClearValue clearColorValue;
                VkClearValue clearDepthStencilValue;
            };
        };
        
        TMap<size_t, VkFramebuffer> framebufferAttachments;

    public:
        VulkanRHIRenderPass( const RHIRenderPassCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHIRenderPass() override;

        void SetAttachments( uint32 attachmentCount, const RHITextureView** textureViews ) override;

        void SetDrawArea( const IntBox2& extent ) override;

        void SetClearValues( const Vector4f& clearColor, const float& clearDepth, const uint32& clearStencil ) override;

        void BeginRenderPass( const RHICommandBuffer* cmd ) override;

        void EndRenderPass( const RHICommandBuffer* cmd ) override;

        bool IsValid() const override;

        FORCEINLINE VkRenderPass GetVulkanRenderPass() const { return renderPass; }
    };

    class VulkanRHIBindLayout : public RHIBindLayout
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIBindLayout )

    private:
        VulkanRHIDevice* device;

        VkDescriptorSetLayout descriptorSetLayout;

    public:
        VulkanRHIBindLayout( const RHIBindGroupCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHIBindLayout() override;

        bool IsValid() const override;

        FORCEINLINE const VkDescriptorSetLayout& GetVulkanDescriptorSetLayout() const { return descriptorSetLayout; }
    };

    class VulkanRHIBindGroup : public RHIBindGroup
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIBindGroup )

    private:
        const void CreateDescriptorPool( const RHIBindGroupCreateInfo& info );

    public:
        VulkanRHIBindGroup( const RHIBindGroupCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHIBindGroup() override;

        bool IsValid() const override;

        const RHIBindLayout& GetBindLayout() const override;

        FORCEINLINE const VkDescriptorSet& GetVulkanDescriptorSet() const { return descriptorSet_; }

    private:
        VulkanRHIDevice* device_;
        VulkanRHIBindLayout bindLayout_;
        VkDescriptorSet descriptorSet_;
        VkDescriptorPool pool_;
    };

    class VulkanRHIGraphicsPipeline : public RHIGraphicsPipeline
    {
    public:
        EE_CLASSNOCOPY( VulkanRHIGraphicsPipeline )

    private:
        TArray<VkDescriptorSetLayout> descriptorSetLayouts;

        VkPipelineLayout pipelineLayout;

        VkPipeline pipeline;

        VulkanRHIDevice* device;

    public:
        VulkanRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHIGraphicsPipeline() override;

        bool IsValid() const override;

        FORCEINLINE VkPipeline GetVulkanPipeline() const { return pipeline; }
        FORCEINLINE VkPipelineLayout GetVulkanPipelineLayout() const { return pipelineLayout; }
    };
}