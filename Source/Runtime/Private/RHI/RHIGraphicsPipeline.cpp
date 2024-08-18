
#include "CoreMinimal.h"

#include <RHI/RHI.h>

namespace EE
{
    RHIRenderSubpassDescription::RHIRenderSubpassDescription()
        : colorAttachmentReferences()
        , inputAttachmentReferences()
        , depthAttachment()
        , usingDepth( false )
    {
    }

    RHIRenderSubpassDescription::~RHIRenderSubpassDescription()
    {
    }
    
    void RHIRenderSubpassDescription::AddInputAttachment( uint32 atachmentIndex )
    {
        inputAttachmentReferences.emplace_back( atachmentIndex, TextureLayout_Undefined );
    }

    void RHIRenderSubpassDescription::AddColorAttachment( uint32 atachmentIndex, ETextureLayout layout )
    {
        colorAttachmentReferences.emplace_back( atachmentIndex, layout );
    }

    void RHIRenderSubpassDescription::AddDepthAttachment( uint32 atachmentIndex, ETextureLayout layout )
    {
        depthAttachment = RHIAttachmentReference( atachmentIndex, layout );
        usingDepth = true;
    }

    RHIRenderPassCreateInfo::RHIRenderPassCreateInfo()
        : subpasses()
        , attachments()
    {
    }

    RHIRenderPassCreateInfo::~RHIRenderPassCreateInfo()
    {
    }

    void RHIRenderPassCreateInfo::AddRenderSubpass( const RHIRenderSubpassDescription& subpass )
    {
        subpasses.emplace_back( subpass );
    }

    void RHIRenderPassCreateInfo::AddColorAttachment( const RHIColorAttachmentDescription& attachment )
    {
        attachments.emplace_back( attachment );
    }

    RHIGraphicsPipelineCreateInfo::RHIGraphicsPipelineCreateInfo()
        : vertexShader()
        , fragmentShader()
        , geometryShader()
        , domainShader()
        , hullShader()
        , primitiveState()
        , colorAttachments()
        , renderpass( NULL )
        , subpassIndex( 0 )
    {
    }

    RHIGraphicsPipelineCreateInfo::~RHIGraphicsPipelineCreateInfo()
    {
    }

    void RHIGraphicsPipelineCreateInfo::AddShaderStage( const RHIShaderStage* shaderStage )
    {
        switch ( shaderStage->GetStage() )
        {
        case ShaderStage_Vertex:      vertexShader.shaderStage = shaderStage; break;
        case ShaderStage_Fragment:  fragmentShader.shaderStage = shaderStage; break;
        case ShaderStage_Geometry:  geometryShader.shaderStage = shaderStage; break;
        default:
            break;
        }
    }

    void RHIGraphicsPipelineCreateInfo::AddColorAttachment( const RHIColorAttachmentState& state )
    {
        colorAttachments.emplace_back( state );
    }
}