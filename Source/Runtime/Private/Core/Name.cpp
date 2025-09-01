
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
        : _id( ConstU8StringToHash( text ) )
        , _number()
    {
        if ( GNamesTable.try_emplace( _id, text ).second )
        {
            GNamesTable.emplace( _id, text );
            GNameCountTable.emplace( _id, 0 );
            _number = 0;
        }
        else
        {
            _number = ++GNameCountTable[_id];
        }
    }

    Name::Name( const U8String& text )
        : Name( text.c_str() )
    {
    }

    Name::Name( uint64 id )
        : _id()
        , _number()
    {
        if ( GNameCountTable.find( id ) == GNameCountTable.end() )
        {
            /// Create empty <see cref=Name/>
            _id = GEmptyName.GetID();
            _number = ++GNameCountTable[ _id ];
        }
        else
        {
            _id = id;
            _number = ++GNameCountTable[ _id ];
        }
    }
    
    Name::Name( const U8Char* text, uint64 number )
        : _id( ConstU8StringToHash( text ) )
        , _number( number )
    {
    }

    Name::~Name() { }

    const U8String& Name::GetName() const
    {
        return GNamesTable[ _id ];
    }

    U8String Name::GetInstanceName() const
    {
        return GNamesTable[ _id ] + "_" + Text::ToUTF8( _number );
    }

    const uint64& Name::GetNumber() const
    {
        return _number;
    }

    uint64 Name::GetInstanceID() const
    {
        uint64 ret = 0;
        EE::HashCombine( &ret, _id, _number );
        return ret;
    }

    const uint64& Name::GetID() const
    {
        return _id;
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
        return (length + _number < otherLength + other._number);
    }

    bool Name::operator!=(const Name & other) const
    {
        return _id != other._id || _number != other._number;
    }

    bool Name::operator==(const Name & other) const
    {
        return _id == other._id && _number == other._number;
    }

}
