
#include "RHI/RHI.h"

namespace EE
{
    class WebGPURHI : public DynamicRHI
    {
    public:
        WebGPURHI();

    private:
        ~WebGPURHI();

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

    void* GetWebGPURHIInstance( );
}