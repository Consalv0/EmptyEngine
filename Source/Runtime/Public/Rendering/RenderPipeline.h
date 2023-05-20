#pragma once

#include "CoreTypes.h"
#include "Math/Transform.h"
#include "Rendering/RenderTarget.h"

namespace EEngine
{
	class RenderPipeline
	{
	public:
		bool needResize;

		virtual void Initialize() = 0;

		virtual void ContextInterval( int interval ) = 0;

		virtual void Begin() = 0;

		virtual void End() = 0;

		virtual void SetCamera( const Transform& eyeTransform, const Matrix4x4& projection, uint8_t renderingMask ) = 0;

		virtual IntVector2 GetRenderSize() const = 0;

		virtual void SetRenderScale( float scale ) = 0;

		virtual TexturePtr GetFramebuffer() const = 0;

		template <typename T>
		bool CreateStage( const IName& stageName );

		virtual void RemoveStage( const IName& stageName ) = 0;

		virtual class RenderStage* GetStage( const IName& stageName ) const = 0;

		virtual void BeginFrame() = 0;

		virtual void EndOfFrame() = 0;

	protected:
		TDictionary<size_t, class RenderStage*> _renderStages;

		RenderTargetPtr _mainScreenTarget;

		TexturePtr _frameBuffer;

		// Render Scale Target
		float _renderScale;
	};

	template<typename T>
	bool RenderPipeline::CreateStage( const IName& stageName )
	{
		if ( _renderStages.find( stageName.GetID() ) == _renderStages.end() )
		{
			_renderStages.insert( std::pair<size_t, RenderStage*>( stageName.GetID(), new T( stageName, this ) ) );
			return true;
		}
		return false;
	}
}