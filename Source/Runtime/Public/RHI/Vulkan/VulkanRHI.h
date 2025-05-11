
#include "RHI/RHI.h"

// Enabling this makes the depth buffer fail??
// #define EE_CORE_VULKAN_VALIDATION_LAYER

namespace EE
{
    class VulkanRHI : public DynamicRHI
    {
    public:
        VulkanRHI();

    private:
        ~VulkanRHI();

        EDynamicRHI GetType() const override;

        RHIPresentContext* CreateRHIPresentContext( Window* window ) const override;
        RHISurface* CreateRHISurface( Window* window ) const override;
        RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateInfo& info ) const override;
        RHIBuffer* CreateRHIBuffer( const RHIBufferCreateInfo& info ) const override;
        RHITexture* CreateRHITexture( const RHITextureCreateInfo& info ) const override;
        RHITextureView* CreateRHITextureView( const RHITextureViewCreateInfo& info ) const override;
        RHISampler* CreateRHISampler( const RHISamplerCreateInfo& info ) const override;
        RHIShaderStage* CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const override;
        RHIBindGroup* CreateRHIBindGroup( const RHIBindGroupCreateInfo& info ) const override;
        RHIRenderPass* CreateRHIRenderPass( const RHIRenderPassCreateInfo& info ) const override;
        RHIGraphicsPipeline* CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const override;

        const RHIDevice* GetRHIDevice() const override;

        void SetName( RHIResource* pResource, const U8Char* name );

        void WaitForDevice() const;

        EShaderFormat GetShaderFormat() const override { return ShaderFormat_GLSL; }

        U8String const& GetName() const override;
    };

    void GetVulkanInstanceExtensions( uint32* count, const U8Char* const*& extensions );
    void* GetVulkanRHIInstance( );
}