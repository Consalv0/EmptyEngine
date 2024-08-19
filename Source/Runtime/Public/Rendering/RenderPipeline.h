#pragma once

#include "CoreTypes.h"
#include "Math/Transform.h"

#include "RHI/RHI.h"

namespace EE
{
	class RenderPipeline
	{
	public:
		bool needResize = false;

		virtual void Initialize() = 0;

		virtual void ContextInterval( int interval ) = 0;

		virtual void Begin() = 0;

		virtual void End() = 0;

		virtual void SetCamera( const Transform& eyeTransform, const Matrix4x4& projection, uint8 renderingMask ) = 0;

		virtual IntVector2 GetRenderSize() const = 0;

		virtual void SetRenderScale( float scale ) = 0;

		template <typename T>
		bool CreateStage( const Name& stageName );

		virtual void RemoveStage( const Name& stageName ) = 0;

		virtual class RenderStage* GetStage( const Name& stageName ) const = 0;

		virtual void BeginFrame() = 0;

		virtual void EndOfFrame() = 0;

	protected:
		TMap<size_t, class RenderStage*> renderStages;
	};

	template<typename T>
	bool RenderPipeline::CreateStage( const Name& stageName )
	{
		if ( renderStages.find( stageName.GetID() ) == renderStages.end() )
		{
			renderStages.insert( TMap<size_t, class RenderStage*>::value_type( stageName.GetID(), new T( stageName, this ) ) );
			return true;
		}
		return false;
	}
}