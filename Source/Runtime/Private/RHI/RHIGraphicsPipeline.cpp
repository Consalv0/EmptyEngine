
#include "CoreMinimal.h"

#include <Core/Collections.h>
#include <Math/CoreMath.h>
#include <RHI/RHI.h>

namespace EE
{
    void RHIBindGroupCreateInfo::AddResourceBinding( const RHIResourceBinding& resourceBinding )
    {
        bindings.emplace_back( resourceBinding );
    }

    void RHIRenderSubpassDescription::AddInputAttachment( uint32 atachmentIndex )
    {
        inputAttachmentReferences.emplace_back( atachmentIndex, TextureLayout_Undefined );
    }

    void RHIRenderSubpassDescription::AddColorAttachment( uint32 atachmentIndex, ETextureLayout layout )
    {
        colorAttachmentReferences.emplace_back( atachmentIndex, layout );
    }

    void RHIRenderSubpassDescription::SetResolveAttachment( uint32 atachmentIndex, ETextureLayout layout )
    {
        resolveAttachmentReferences = RHIAttachmentReference( atachmentIndex, layout );
        hasResolve = true;
    }

    void RHIRenderSubpassDescription::SetDepthAttachment( uint32 atachmentIndex, ETextureLayout layout )
    {
        depthAttachment = RHIAttachmentReference( atachmentIndex, layout );
        usingDepth = true;
    }

    void RHIRenderPassCreateInfo::AddRenderSubpass( const RHIRenderSubpassDescription& subpass )
    {
        subpasses.emplace_back( subpass );
    }

    void RHIRenderPassCreateInfo::AddAttachment( const RHIAttachmentDescription& attachment )
    {
        attachments.emplace_back( attachment );
    }

    void RHIGraphicsPipelineCreateInfo::AddShaderStage( const RHIShaderStage* shaderStage )
    {
        switch ( shaderStage->GetStage() )
        {
        case ShaderStage_Vertex_Bit:      vertexShader.shaderStage = shaderStage; break;
        case ShaderStage_Fragment_Bit:  fragmentShader.shaderStage = shaderStage; break;
        case ShaderStage_Geometry_Bit:  geometryShader.shaderStage = shaderStage; break;
        default:
            break;
        }
    }

    void RHIGraphicsPipelineCreateInfo::AddColorAttachment( const RHIColorAttachmentState& state )
    {
        colorAttachments.emplace_back( state );
    }

    void RHIGraphicsPipelineCreateInfo::AddBindLayout( const RHIBindLayout& bindLayout )
    {
        bindLayouts.emplace_back( &bindLayout );
    }
}