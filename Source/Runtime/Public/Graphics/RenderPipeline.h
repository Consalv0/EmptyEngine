#pragma once

#include "CoreTypes.h"
#include "Math/Transform.h"
#include "Graphics/Graphics.h"

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

		virtual Texture* GetFramebuffer() const = 0;

		template <typename T>
		bool CreateStage( const IName& stageName );

		virtual void RemoveStage( const IName& stageName ) = 0;

		virtual class RenderStage* GetStage( const IName& stageName ) const = 0;

		virtual void BeginFrame() = 0;

		virtual void EndOfFrame() = 0;

	protected:
		TDictionary<size_t, class RenderStage*> renderStages_;

		Texture* mainScreenTarget_;

		Texture* frameBuffer_;

		// Render Scale Target
		float renderScale_;
	};

	template<typename T>
	bool RenderPipeline::CreateStage( const IName& stageName )
	{
		if ( renderStages_.find( stageName.GetID() ) == renderStages_.end() )
		{
			renderStages_.insert( std::pair<size_t, RenderStage*>( stageName.GetID(), new T( stageName, this ) ) );
			return true;
		}
		return false;
	}
}