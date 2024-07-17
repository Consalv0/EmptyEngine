
#include "RHI/RHI.h"

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
        RHISwapChain* CreateRHISwapChain( const RHISwapChainCreateDescription& description, class Window* window ) const;
        RHICommandBuffer* CreateRHICommandBuffer( const RHICommandBufferCreateDescription& description ) const;
        RHIBuffer* CreateRHIBuffer( const RHIBufferCreateDescription& description ) const;
        RHITexture* CreateRHITexture( const RHITextureCreateDescription& description ) const;
        RHISampler* CreateRHISampler( const RHISamplerCreateDescription& description ) const;
        RHIShaderStage* CreateRHIShaderStage( EShaderStage stage, const void* pShaderBytecode, size_t bytecodeLength ) const;

        const RHIDevice* GetRHIDevice() const override;

        void SetName( RHIResource* pResource, const NChar* name );

        void WaitForDevice() const;

        EShaderFormat GetShaderFormat() const { return ShaderFormat_GLSL; }

        WString const& GetName() const;
    };

    void GetVulkanInstanceExtensions( uint32* count, const NChar* const*& extensions );
}