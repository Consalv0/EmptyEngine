#include "CoreMinimal.h"
#include "Rendering/RenderingDefinitions.h"
// #include "Rendering/RenderingBuffers.h"
#include "Rendering/RenderTarget.h"
// #include "Rendering/RenderingAPI.h"
// #include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include "Rendering/Rendering.h"

namespace EEngine
{
	RenderTargetPtr RenderTarget::Create()
	{
		switch ( Rendering::GetAPI() )
		{
		case RenderingAPI::API::OpenGL:
			return std::make_shared<OpenGLRenderTarget>();
		case RenderingAPI::API::None:
		default:
			EE_CORE_ASSERT( true, "Rendering API is not valid for this platform, can't create render target!" );
			return NULL;
		}
	}
}