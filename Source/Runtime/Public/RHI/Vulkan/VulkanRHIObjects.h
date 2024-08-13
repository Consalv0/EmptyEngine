
namespace EE
{
    class VulkanRHIPhysicalDevice;
    class VulkanRHIInstance;
    class VulkanRHIDevice;
    class VulkanRHIPresentContext;
    class VulkanRHISemaphore;
    class VulkanRHIFence;
    class VulkanRHIQueue;
    class VulkanRHIBuffer;
    class VulkanRHITexture;
    class VulkanRHISwapChain;
    class VulkanRHISurface;
    class VulkanRHICommandPool;
    class VulkanRHICommandBuffer;
    class VulkanRHIShaderStage;

    class VulkanRHIPhysicalDevice
    {
    private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        uint32 extensionCount;
        TArray<VkExtensionProperties> extensions;
        uint32 queueFamilyCount;
        TArray<VkQueueFamilyProperties> queueFamilies;

        TMap<VkSurfaceKHR, VulkanSurfaceSupportDetails> surfaceSupportDetails;

    public:
        VulkanRHIPhysicalDevice( VkInstance instance, VkPhysicalDevice physicalDevice );

        ~VulkanRHIPhysicalDevice();

        bool CheckDeviceExtensionSupport() const;

        QueueFamilyIndices GetQueueFamilies() const;

        bool IsDeviceSuitable( VulkanRHISurface* surface ) const;

        int RateDeviceSuitabilityForSurface( VulkanRHISurface* surface ) const;

        void AddSurfaceSupportDetails( VkSurfaceKHR surface, uint32 queueFamilyIndex );

        FORCEINLINE const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; };

        FORCEINLINE VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; };

        const VulkanSurfaceSupportDetails* GetSurfaceSupportDetails( VkSurfaceKHR surface ) const;

        void UpdateSurfaceSupportDetails( VkSurfaceKHR surface );

        FORCEINLINE const bool HasSurfaceAnyFormat( VkSurfaceKHR surface ) const { return surfaceSupportDetails.at( surface ).formatCount > 0; }
        FORCEINLINE const VulkanSurfaceSupportDetails& GetSurfaceDetails( VkSurfaceKHR surface ) const { return surfaceSupportDetails.at( surface ); }
        FORCEINLINE const bool HasSurfaceAnyPresentMode( VkSurfaceKHR surface ) const { return surfaceSupportDetails.at( surface ).presentModeCount > 0; }
    };

    class VulkanRHIInstance : RHIInstance
    {
    private:
        VkInstance instance;

        uint32 physicalDeviceCount;
        TArray<VulkanRHIPhysicalDevice> physicalDevices;
        uint32 selectedDeviceIndex;

    public:
        VulkanRHIInstance( VkInstanceCreateInfo& createInfo );

        ~VulkanRHIInstance();

        int32 PickPhysicalDeviceForSurface( VulkanRHISurface* surface ) const;

        bool SelectSuitableDevice( VulkanRHISurface* surface );

        const VulkanRHIPhysicalDevice* GetSelectedPhysicalDevice() const;
        VulkanRHIPhysicalDevice* GetSelectedPhysicalDevice();

        bool AddSurfaceSupportDetails( VulkanRHISurface* surface );

        bool IsValid() const;

        FORCEINLINE const uint32& GetPhysicalDeviceCount() const { return physicalDeviceCount; }
        FORCEINLINE const VulkanRHIPhysicalDevice& GetPhysicalDevice( uint32& index ) const { return physicalDevices[ index ]; }
        FORCEINLINE const VkInstance& GetVulkanInstance() const { return instance; };
    };

    class VulkanRHIQueue : public RHIQueue
    {
    private:
        VkQueue queue;
        const VulkanRHIDevice* device;

    public:
        ~VulkanRHIQueue();

        VulkanRHIQueue( const VulkanRHIDevice* device, const uint32& familyIndex, const uint32& queueIndex );

        bool IsValid() const;

        FORCEINLINE const VkQueue& GetVulkanQueue() const { return queue; }

        void WaitIdle() const;

        void SubmitCommandBuffer( const RHICommandBuffer* commandBuffer, const RHIQueueSubmitInfo& info ) override;
    };

    class VulkanRHIDevice : public RHIDevice
    {
    private:
        uint32 physicalDeviceIndex;
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

    public:
        FORCEINLINE const VkDevice GetVulkanDevice() const { return device; }

        FORCEINLINE const VulkanRHIPhysicalDevice* GetVulkanPhysicalDevice() const { return physicalDevice; }

        FORCEINLINE VulkanRHIPhysicalDevice* GetVulkanPhysicalDevice() { return physicalDevice; }

        FORCEINLINE const VmaAllocator GetVulkanAllocator() const { return allocator; }

        FORCEINLINE uint32 GetGraphicsFamilyIndex() const { return graphicsQueueIndex; }

        FORCEINLINE VulkanRHIQueue* GetVulkanGraphicsQueue() const { return graphicsQueue; }

        RHIQueue* GetGraphicsQueue() const override { return GetVulkanGraphicsQueue(); }

        FORCEINLINE uint32 GetPresentFamilyIndex() const { return presentQueueIndex; }

        FORCEINLINE VulkanRHIQueue* GetVulkanPresentQueue() const { return presentQueue; }

        RHIQueue* GetPresentQueue() const override { return GetVulkanPresentQueue(); }

        FORCEINLINE const uint32* GetFamilyIndices() const { return queueFamilyIndices; }

        const VulkanRHICommandPool* GetCommandPool( uint32 familyIndex ) const;

        ~VulkanRHIDevice();

        VulkanRHIDevice( VulkanRHIInstance* instance );

        bool IsValid() const;

    private:

        bool CreateCommandBufferPools( VulkanRHIDevice* device );
    };

    class VulkanRHIPresentContext : public RHIPresentContext
    {
    private:
        VulkanRHIInstance* instance;
        //* The window this context is for
        Window* window;
        VulkanRHISurface* surface;
        VulkanRHISwapChain* swapChain;

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
        FORCEINLINE VulkanRHISurface* GetRHISurface() { return surface; }
        FORCEINLINE const VulkanRHISurface* GetRHISurface() const { return surface; }
        FORCEINLINE VulkanRHISwapChain* GetRHISwapChain() { return swapChain; }
        FORCEINLINE const VulkanRHISwapChain* GetRHISwapChain() const { return swapChain; }
        FORCEINLINE const Window* GetWindow() const { return window; }

        ~VulkanRHIPresentContext();

        VulkanRHIPresentContext( Window* window, VulkanRHIInstance* instance );

        void GetSurfaceColorFormat( bool hdr, EPixelFormat* outFormat, EColorSpace* outColorSpace ) const;

        const VulkanRHISemaphore& GetPresentSempahore( uint32 frameIndex ) const;

        const VulkanRHISemaphore& GetRenderSempahore( uint32 frameIndex ) const;

        const RHICommandBuffer* GetRenderCommandBuffer() const override;

        const RHIFence* GetRenderFence() const override;

        const RHITexture* GetBackbuffer() const override;

        const EPixelFormat& GetFormat() const override;

        void Present() override;

        bool SubmitPresentImage( VulkanRHIQueue* queue ) const;

        void SubmitRenderCommandBuffer( EPipelineStage stage ) const override;

        void AquireBackbuffer( uint64 timeout ) override;

        bool IsValid() const;

    protected:

        void RecreateSwapChain();
    };

    class VulkanRHIBuffer : public RHIBuffer
    {
    private:
        VulkanRHIDevice* device;
        VkBuffer buffer;
        VmaAllocation nativeAllocation;
        EBufferUsageFlags usages;

    public:
        ~VulkanRHIBuffer();

        VulkanRHIBuffer( RHIBufferCreateInfo& info, VulkanRHIDevice* device );
    };

    class VulkanRHITexture : public RHITexture
    {
    private:
        VulkanRHIDevice* device;
        VkSampler sampler;
        VkImage image;
        VkImageView imageView;
        VmaAllocation memory;
        UIntVector3 extent;
        VkFormat format;
        bool ownership;

        EPixelFormat pixelFormat;

    public:
        VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device, VkImage image );

        VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHITexture();

        void CleanImageView() const;

        VkImage GetVulkanImage() const { return image; }

        const UIntVector3& GetExtent() const override { return extent; };

        const EPixelFormat& GetFormat() const override { return pixelFormat; };

        bool IsValid() const;
    };

    class VulkanRHISwapChain : public RHISwapChain
    {
    private:
        VulkanRHIDevice* device;
        const VulkanRHIPresentContext* presentContext;
        VkSwapchainKHR swapChain;
        VkExtent2D size;
        uint32 imageCount;
        TArray<VkImage> images;
        TArray<VulkanRHITexture*> textures;
        uint32 backImageIndex;

    public:
        bool IsValid() const;

        VulkanRHISwapChain( const RHISwapChainCreateInfo& info, const VulkanRHIPresentContext* presentContext, VulkanRHIDevice* device );

        ~VulkanRHISwapChain();

        void Cleanup();

        void CreateSwapChain( const RHISwapChainCreateInfo& info );

        FORCEINLINE const uint32& GetImageCount() const { return imageCount; }

        FORCEINLINE const VkSwapchainKHR& GetVulkanSwapChain() const { return swapChain; }

        FORCEINLINE const uint32& BackImageIndex() const { return backImageIndex; }

        bool AquireNextImage( uint64 timeout, const VulkanRHISemaphore* semaphore, const VulkanRHIFence* fence );

        VkImage GetImage( uint32 index ) const { return images[ index ]; }

        const VulkanRHITexture* GetTexture( uint32 index ) const { return textures[ index ]; }
    };

    class VulkanRHISurface : public RHISurface
    {
    private:
        VulkanRHIInstance* instance;
        VkSurfaceKHR surface;

    public:
        ~VulkanRHISurface();

        VulkanRHISurface( Window* window, VulkanRHIInstance* instance );

    public:
        FORCEINLINE const VkSurfaceKHR GetVulkanSurface() const { return surface; }

        bool IsValid() const;
    };

    class VulkanRHIFence : public RHIFence
    {
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

        bool IsValid() const;
    };

    class VulkanRHISemaphore : public RHISemaphore
    {
    private:
        VulkanRHIDevice* device;
        VkSemaphore semaphore;

    public:
        VulkanRHISemaphore( VulkanRHIDevice* device );
        ~VulkanRHISemaphore() override;

        FORCEINLINE const VkSemaphore& GetVulkanSemaphore() const { return semaphore; }

        bool IsValid() const;
    };

    // TODO 
    // You can allocate as many VkCommandBuffer as you want from a given pool,
    // but you can only record commands from one thread at a time.
    // If you want multithreaded command recording, you need more VkCommandPool objects.
    class VulkanRHICommandPool
    {
    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandPool commandPool;

    public:
        ~VulkanRHICommandPool();

        VulkanRHICommandPool( VulkanRHIDevice* device, uint32 queueFamilyIndex );

        FORCEINLINE const VkCommandPool& GetVulkanCommandPool() const { return commandPool; }

    };

    class VulkanRHICommandBuffer : public RHICommandBuffer
    {
    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandBuffer commandBuffer;

    public:
        ~VulkanRHICommandBuffer();

        VulkanRHICommandBuffer( const RHICommandBufferCreateInfo& info, VulkanRHIDevice* device, uint32 queueFamilyIndex );

        bool IsValid() const;

        FORCEINLINE const VkCommandBuffer& GetVulkanCommandBuffer() const { return commandBuffer; }

        void Begin() const override;

        void End() const override;

        void TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout from, const ETextureLayout to ) const override;

        void ClearColor( Vector3f color, const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer ) const override;
    };

    class VulkanRHIShaderStage : public RHIShaderStage
    {
    private:
        VulkanRHIDevice* device;

        const NChar* entryPoint;

        VkShaderModule shaderModule;

    public:
        ~VulkanRHIShaderStage();

        VulkanRHIShaderStage( const RHIShaderStageCreateInfo& info, VulkanRHIDevice* device );

        bool IsValid() const;

        FORCEINLINE const VkShaderModule GetVulkanShaderModule() const { return shaderModule; }

        const NChar* GetEntryPoint() const override;
    };

    class VulkanRHIGraphicsPipeline : public RHIGraphicsPipeline
    {
    private:
        VkPipelineLayout pipelineLayout;

        VkPipeline pipeline;

        VulkanRHIDevice* device;

    public:
        VulkanRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info, VulkanRHIDevice* device );

        ~VulkanRHIGraphicsPipeline();

        bool IsValid() const;

        FORCEINLINE VkPipeline GetVulkanPipeline() const { return pipeline; }
    };
}