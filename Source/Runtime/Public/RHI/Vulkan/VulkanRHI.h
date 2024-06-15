
#include "RHI/RHI.h"

namespace EE
{
    class VulkanRHI : public DynamicRHI
    {
    private:
        class VulkanRHIDevice* device;
        class VulkanRHIInstance* instance;

    public:
        VulkanRHI();

    private:
        ~VulkanRHI();

        EDynamicRHI GetType() const;

        RHIPresentContext* CreateRHIPresentContext( Window* window ) const;
        RHISurface* CreateRHISurface( Window* window ) const;
        RHISwapChain* CreateRHISwapChain( const RHISwapChainCreateDescription& description, class Window* window ) const;
        RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateDescription& description ) const;
        RHIBuffer* CreateRHIBuffer( const RHIBufferCreateDescription& description ) const;
        RHITexture* CreateRHITexture( const RHITextureCreateDescription& description ) const;
        RHISampler* CreateRHISampler( const RHISamplerCreateDescription& description ) const;
        RHIShaderStage* CreateRHIShaderStage( EShaderStage stage, const void* pShaderBytecode, size_t bytecodeLength ) const;

        void SetName( RHIResource* pResource, const NChar* name );

        void WaitForDevice() const;

        RHITexture* GetBackTexure( const RHISwapChain& swapchain, uint32 index ) const;

        EShaderFormat GetShaderFormat() const { return ShaderFormat_GLSL; }

        WString const& GetName() const;

        const RHIDevice* GetDevice() const;
        const RHIInstance* GetInstance() const;
    };

    void GetVulkanInstanceExtensions( uint32* count, const NChar* const*& extensions );
}