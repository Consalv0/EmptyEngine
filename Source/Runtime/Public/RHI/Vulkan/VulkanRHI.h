
#include "RHI/RHI.h"

// #define EE_CORE_VULKAN_VALIDATION_LAYER

namespace EE
{
    class VulkanRHI : public DynamicRHI
    {
    public:
        VulkanRHI();

    private:
        ~VulkanRHI();

        EDynamicRHI GetType() const;

        RHIPresentContext* CreateRHIPresentContext( Window* window ) const;
        RHISurface* CreateRHISurface( Window* window ) const;
        RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateInfo& info ) const;
        RHIBuffer* CreateRHIBuffer( const RHIBufferCreateInfo& info ) const;
        RHITexture* CreateRHITexture( const RHITextureCreateInfo& info ) const;
        RHISampler* CreateRHISampler( const RHISamplerCreateInfo& info ) const;
        RHIShaderStage* CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const;
        RHIBindGroup* CreateRHIBindGroup( const RHIBindGroupCreateInfo& info ) const;
        RHIRenderPass* CreateRHIRenderPass( const RHIRenderPassCreateInfo& info ) const;
        RHIGraphicsPipeline* CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const;

        const RHIDevice* GetRHIDevice() const override;

        void SetName( RHIResource* pResource, const NChar* name );

        void WaitForDevice() const;

        EShaderFormat GetShaderFormat() const { return ShaderFormat_GLSL; }

        WString const& GetName() const;
    };

    void GetVulkanInstanceExtensions( uint32* count, const NChar* const*& extensions );
}