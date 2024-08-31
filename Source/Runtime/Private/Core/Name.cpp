
#include "CoreMinimal.h"

#include "Core/Name.h"

#include "Utils/Hasher.h"
#include "Utils/TextFormatting.h"
#include "Core/Collections.h"

namespace EE
{
    static TMap<size_t, WString> GWideNamesTable = TMap<size_t, WString>();
    static TMap<size_t, NString> GNarrowNamesTable = TMap<size_t, NString>();
    static TMap<size_t, size_t> GNameCountTable = TMap<size_t, size_t>();
    
    Name GEmptyName = { L"", 0 };

    Name::Name( const WString & text ) 
        : id( WStringToHash( text ) )
        , number()
    {
        if ( GWideNamesTable.try_emplace(id, text).second )
        {
			GNarrowNamesTable.emplace( id, Text::WideToNarrow( text ) );
            GNameCountTable.emplace( id, 0 );
            number = 0;
        }
        else 
        {
            number = ++GNameCountTable[ id ];
        }
    }

	Name::Name( const WChar * text )
		: id( WStringToHash( text ) )
		, number()
	{
		if ( GWideNamesTable.try_emplace(id, text).second )
		{
			GNarrowNamesTable.emplace( id, Text::WideToNarrow( text ) );
			GNameCountTable.emplace( id, 0 );
			number = 0;
		}
		else
		{
			number = ++GNameCountTable[id];
		}
	}

	Name::Name( const NString& text )
		: Name( Text::NarrowToWide( text ) )
	{
	}

	Name::Name( const NChar* text )
		: Name( Text::NarrowToWide( text ) )
	{
	}

	Name::Name( size_t id )
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

	Name::Name( const WString& text, size_t number )
		: id ( WStringToHash( text ) )
		, number( number )
	{
	}

	Name::~Name() { }

	const WString& Name::GetName() const
	{
		return GWideNamesTable[ id ];
	}

	const NString& Name::GetNarrowName() const
	{
		return GNarrowNamesTable[ id ];
	}

	WString Name::GetInstanceName() const
	{
		return GWideNamesTable[ id ] + L"_" + Text::ToWide(  number);
	}

	NString Name::GetNarrowInstanceName() const
	{
		return GNarrowNamesTable[ id ] + "_" + Text::ToNarrow( number );
	}

	const size_t& Name::GetNumber() const
	{
		return number;
	}

	size_t Name::GetInstanceID() const
	{
		size_t ret = 0;
		EE::HashCombine( ret, id, number );
		return ret;
	}

	const size_t& Name::GetID() const
	{
		return id;
	}

	bool Name::operator<(const Name & other) const
	{
		uint32 i = 0;
		const WString& entryName = GetName();
		const WString& otherName = other.GetName();
		const size_t& length = entryName.length();
		const size_t& otherLength = otherName.length();
		
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
		return id != other.id || number != number;
	}

	bool Name::operator==(const Name & other) const
	{
		return id == other.id && number == number;
	}

}
