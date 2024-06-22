
namespace EE
{
    class VulkanRHIPhysicalDevice;
    class VulkanRHIInstance;
    class VulkanRHIDevice;
    class VulkanRHIPresentContext;
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

        const uint32& GetPhysicalDeviceCount() const { return physicalDeviceCount; }
        const VulkanRHIPhysicalDevice& GetPhysicalDevice( uint32& index ) const { return physicalDevices[ index ]; }
        VkInstance GetVulkanInstance() const { return instance; };
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

        VkQueue graphicsQueue;
        VkQueue presentQueue;

    public:
        FORCEINLINE const VkDevice GetVulkanDevice() const { return device; }
        
        FORCEINLINE const VulkanRHIPhysicalDevice* GetVulkanPhysicalDevice() const { return physicalDevice; }

        FORCEINLINE const VmaAllocator GetVulkanAllocator() const { return allocator; }

        FORCEINLINE uint32 GetGraphicsFamilyIndex() const { return graphicsQueueIndex; }

        FORCEINLINE uint32 GetPresentFamilyIndex() const { return presentQueueIndex; }

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

    public:
        FORCEINLINE VulkanRHISurface* GetRHISurface() { return surface; }
        FORCEINLINE const VulkanRHISurface* GetRHISurface() const { return surface; }
        FORCEINLINE VulkanRHISwapChain* GetRHISwapChain() { return swapChain; }
        FORCEINLINE const VulkanRHISwapChain* GetRHISwapChain() const { return swapChain; }
        FORCEINLINE const Window* GetWindow() const { return window; }

        ~VulkanRHIPresentContext();

        VulkanRHIPresentContext( Window* window, VulkanRHIInstance* instance );

        void CreateSurface();

        void CreateSwapChain();

        void CreateCommandBuffers();
        
        bool IsValid() const;
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

        VulkanRHIBuffer( RHIBufferCreateDescription& description, VulkanRHIDevice* device );
    };

    class VulkanRHITexture : public RHITexture
    {
    private:
        VulkanRHIDevice* device;
        VkSampler sampler;
        VkImage image;
        VkImageView imageView;
        VmaAllocation memory;
        VkFormat format;
        bool ownership;

    public:
        VulkanRHITexture( RHITextureCreateDescription& description, VulkanRHIDevice* device, VkImage image );

        VulkanRHITexture( RHITextureCreateDescription& description, VulkanRHIDevice* device );

        ~VulkanRHITexture();

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

    public:
        bool IsValid() const;

        VulkanRHISwapChain( const RHISwapChainCreateDescription& description, const VulkanRHIPresentContext* presentContext, VulkanRHIDevice* device );

        ~VulkanRHISwapChain();

        FORCEINLINE const uint32& GetImageCount() const { return imageCount; }
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

    class VulkanRHICommandPool
    {
    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandPool commandPool;

    public:
        ~VulkanRHICommandPool();

        VulkanRHICommandPool( VulkanRHIDevice* device, uint32 queueFamilyIndex );

        FORCEINLINE VkCommandPool GetVulkanCommandPool() const { return commandPool; }

    };

    class VulkanRHICommandBuffer : public RHICommandBuffer
    {
    private:
        VulkanRHIDevice* device;
        uint32 queueFamilyIndex;
        VkCommandBuffer commandBuffer;

    public:
        ~VulkanRHICommandBuffer();

        VulkanRHICommandBuffer( VulkanRHIDevice* device, uint32 queueFamilyIndex );

        bool IsValid() const;
    };

    class VulkanRHIShaderStage
    {
    private:
        VulkanRHIDevice* device;
        VkShaderModule shaderModule;

    public:
        ~VulkanRHIShaderStage();

        VulkanRHIShaderStage( VulkanRHIDevice* device, size_t codeLength, const uint32* code, const EShaderStage stage );
    };

    class VulkanRHIRasterPipeline : public RHIRasterPipeline
    {
    private:
        VkPipeline pipeline;

    public:
        VulkanRHIRasterPipeline();

        ~VulkanRHIRasterPipeline();

        VkPipeline GetVulkanPipeline() const { return pipeline; }
    };
}