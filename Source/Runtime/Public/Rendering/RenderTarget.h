#pragma once

#include "Rendering/Texture.h"
#include "Math/MathUtils.h"
#include "Math/IntVector2.h"
#include "Math/Box2D.h"

namespace EEngine
{
	typedef std::shared_ptr<class RenderTarget> RenderTargetPtr;

	class RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;

		//* Get the dimension of the texture
		virtual inline ETextureDimension GetDimension() const = 0;

		//* All future functions will modify this texture
		virtual void BindDepthTexture2D( Texture2D* texture, EPixelFormat format, const IntVector2& size, int lod = 0 ) = 0;

		//* All future functions will modify this texture
		virtual void BindTexture2D( Texture2D* texture, const IntVector2& size, int lod = 0, int textureAttachment = 0 ) = 0;

		//* All future functions will modify this textures
		virtual void BindTextures2D( Texture2D** textures, const IntVector2& size, int* lods, int* textureAttachments, uint32_t count ) = 0;

		//* All future functions will modify this texture
		virtual void BindCubemapFace( Cubemap* texture, const int& size, ECubemapFace face, int lod = 0, int textureAttachment = 0 ) = 0;

		//* Creates a depth buffer for depth testing
		virtual void CreateRenderDepthBuffer2D( EPixelFormat format, const IntVector2& size ) = 0;

		//* Copy the depth buffer to another render, if Null it will use the default render
		virtual void TransferBitsTo( RenderTarget* target, bool color, bool stencil, bool depth, const EFilterMode& filterMode, const IntBox2D& from, const IntBox2D& to ) = 0;

		//* Returns empty if no texture
		virtual Texture* GetBindedTexture( int index ) const = 0;

		//* Get the renderbuffer object
		virtual void* GetNativeObject() const = 0;

		//* Get size of render
		virtual IntVector3 GetSize() const = 0;

		//* Get the viewport for this renderer
		virtual IntBox2D GetViewport() const = 0;

		//* Checks the framebuffer status
		virtual bool CheckStatus() const = 0;

		//* Bind the render target
		virtual void Bind() const = 0;

		//* Unbind the render target
		virtual void Unbind() const = 0;

		//* Clears the texture binded
		virtual void Clear() const = 0;

		//* Release the texture binded
		virtual void ReleaseTextures() = 0;

		//* Check if render target is valid
		virtual bool IsValid() const = 0;

		static RenderTargetPtr Create();
	};

}