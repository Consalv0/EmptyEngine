
#include "CoreMinimal.h"
#include "Rendering/RenderingDefinitions.h"
#include "Rendering/RenderingAPI.h"
#include "Rendering/Texture.h"

#include "Rendering/Rendering.h"

namespace EEngine
{
	Texture2D* Texture2D::Create(
		const IntVector2& Size, const EPixelFormat ColorFormat, const EFilterMode& FilterMode,
		const ESamplerAddressMode& AddressMode )
	{
#ifdef EE_OPEN_GL

#endif // EE_OPEN_GL

		switch ( Rendering::GetAPI() )
		{
		case RenderingAPI::API::OpenGL:
			return new OpenGLTexture2D( Size, ColorFormat, FilterMode, AddressMode );
		case RenderingAPI::API::None:
		default:
			EE_CORE_ASSERT( true, "Rendering API is not valid for this platform, can't create texture 2D!" );
			return NULL;
		}
	}

	Texture2D* Texture2D::Create(
		const IntVector2& Size, const EPixelFormat ColorFormat, const EFilterMode& FilterMode,
		const ESamplerAddressMode& AddressMode, const EPixelFormat InputFormat, const void* BufferData )
	{
		switch ( Rendering::GetAPI() )
		{
		case RenderingAPI::API::OpenGL:
			return new OpenGLTexture2D( Size, ColorFormat, FilterMode, AddressMode, InputFormat, BufferData );
		case RenderingAPI::API::None:
		default:
			EE_CORE_ASSERT( true, "Rendering API is not valid for this platform, can't create texture 2D!" );
			return NULL;
		}
	}

	Cubemap* Cubemap::Create(
		const uint32_t& Size, const EPixelFormat& Format, const EFilterMode& Filter, const ESamplerAddressMode& AddressMode )
	{
		switch ( Rendering::GetAPI() )
		{
		case RenderingAPI::API::OpenGL:
			return new OpenGLCubemap( Size, Format, Filter, AddressMode );
		case RenderingAPI::API::None:
		default:
			EE_CORE_ASSERT( true, "Rendering API is not valid for this platform, can't create texture 2D!" );
			return NULL;
		}
	}
}