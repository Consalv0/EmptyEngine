
#include "CoreMinimal.h"

#include <RHI/RHI.h>

namespace EE
{
    RHIGraphicsPipelineCreateInfo::RHIGraphicsPipelineCreateInfo()
        : vertexShader()
        , fragmentShader()
        , geometryShader()
        , domainShader()
        , hullShader()
    {
    }

    RHIGraphicsPipelineCreateInfo::~RHIGraphicsPipelineCreateInfo()
    {
    }

    void RHIGraphicsPipelineCreateInfo::AddShaderStage( const RHIShaderStage* shaderStage )
    {
        switch ( shaderStage->GetStage() )
        {
        case ShaderStage_Vertex:
            vertexShader.shaderStage = shaderStage;
            break;
        case ShaderStage_Fragment:
            fragmentShader.shaderStage = shaderStage;
            break;
        case ShaderStage_Geometry:
            geometryShader.shaderStage = shaderStage;
            break;
        default:
            break;
        }
    }
}