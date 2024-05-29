#pragma once

#include "CoreTypes.h"
#include "Math/Transform.h"
#include "Rendering/Common.h"
#include "Rendering/Objects.h"

namespace EE
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
		bool CreateStage( const EName& stageName );

		virtual void RemoveStage( const EName& stageName ) = 0;

		virtual class RenderStage* GetStage( const EName& stageName ) const = 0;

		virtual void BeginFrame() = 0;

		virtual void EndOfFrame() = 0;

	protected:
		TDictionary<size_t, class RenderStage*> renderStages;

		Texture* mainScreenTarget;

		Texture* frameBuffer;

		// Render Scale Target
		float renderScale;
	};

	template<typename T>
	bool RenderPipeline::CreateStage( const EName& stageName )
	{
		if ( renderStages.find( stageName.GetID() ) == renderStages.end() )
		{
			renderStages.insert( std::pair<size_t, RenderStage*>( stageName.GetID(), new T( stageName, this ) ) );
			return true;
		}
		return false;
	}
}