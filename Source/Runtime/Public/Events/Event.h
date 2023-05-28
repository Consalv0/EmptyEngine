#pragma once

#include "CoreTypes.h"

#define EE_IMPLEMENT_EVENT_ENUMTYPE(EnumType, Type) FORCEINLINE static EnumType GetStaticType() { return EnumType::##Type; }\
                                                    FORCEINLINE virtual EnumType GetEventType() const override { return GetStaticType(); }\
                                                    FORCEINLINE virtual const WChar* GetName() const override { return L#EnumType ## "::" ## #Type; }

#define EE_IMPLEMENT_EVENT_CATEGORY(Category) FORCEINLINE virtual uint32 GetCategoryFlags() const override { return Category; }

#define EE_EVENT_DISPATCH( baseEvent, childEventType, call ) if ( baseEvent.GetEventType() == childEventType::GetStaticType() ) call( *(childEventType*)&baseEvent );

namespace EE
{
    class Event
    {
    public:
        virtual const WChar* GetName() const = 0;
        virtual uint32 GetCategoryFlags() const = 0;
    };

}