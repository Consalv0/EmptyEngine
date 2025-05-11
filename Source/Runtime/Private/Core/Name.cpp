
#include "CoreMinimal.h"

#include "Core/Name.h"

#include "Utils/Hasher.h"
#include "Utils/TextFormatting.h"
#include "Core/Collections.h"

namespace EE
{
    static TMap<uint64, U8String> GNamesTable = TMap<uint64, U8String>();
    static TMap<uint64, uint64> GNameCountTable = TMap<uint64, uint64>();
    
    Name GEmptyName = { "", 0 };

    Name::Name( const U8Char* text )
        : id( ConstU8StringToHash( text ) )
        , number()
    {
        if ( GNamesTable.try_emplace(id, text).second )
        {
            GNamesTable.emplace( id, text );
            GNameCountTable.emplace( id, 0 );
            number = 0;
        }
        else
        {
            number = ++GNameCountTable[id];
        }
    }

    Name::Name( const U8String& text )
        : Name( text.c_str() )
    {
    }

    Name::Name( uint64 id )
        : id()
        , number()
    {
        if ( GNameCountTable.find( id ) == GNameCountTable.end() ) {
            number = ++GNameCountTable[ GEmptyName.GetID() ];
            id = GEmptyName.GetID();
        }
        else {
            number = ++GNameCountTable[ id ];
        }
    }
    
    Name::Name( const U8Char* text, uint64 number )
        : id ( ConstU8StringToHash( text ) )
        , number( number )
    {
    }

    Name::~Name() { }

    const U8String& Name::GetName() const
    {
        return GNamesTable[ id ];
    }

    U8String Name::GetInstanceName() const
    {
        return GNamesTable[ id ] + "_" + Text::ToUTF8( number );
    }

    const uint64& Name::GetNumber() const
    {
        return number;
    }

    uint64 Name::GetInstanceID() const
    {
        uint64 ret = 0;
        EE::HashCombine( &ret, id, number );
        return ret;
    }

    const uint64& Name::GetID() const
    {
        return id;
    }

    bool Name::operator<(const Name & other) const
    {
        uint32 i = 0;
        const U8String& entryName = GetName();
        const U8String& otherName = other.GetName();
        const uint64& length = entryName.length();
        const uint64& otherLength = otherName.length();
        
        while ((i < length) && (i < otherLength))
        {
            if (entryName[i] < otherName[i]) return true;
            else if (entryName[i] > otherName[i]) return false;
            ++i;
        }
        return (length + number < otherLength + other.number);
    }

    bool Name::operator!=(const Name & other) const
    {
        return id != other.id || number != other.number;
    }

    bool Name::operator==(const Name & other) const
    {
        return id == other.id && number == other.number;
    }

}
