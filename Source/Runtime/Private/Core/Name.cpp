
#include "CoreMinimal.h"
#include "Utils/Hasher.h"
#include "Utils/TextFormatting.h"
#include "Core/Name.h"

namespace EE
{
	TDictionary<size_t, WString> EName::GNamesTable = TDictionary<size_t, WString>();
	TDictionary<size_t, size_t> EName::GNameCountTable = TDictionary<size_t, size_t>();
    
    EName GEmptyName = { L"", 0 };

	EName::EName(const WString & Text) : entryName(Text) {
		id = WStringToHash(Text);
		if (GNamesTable.try_emplace(id, Text).second) {
			GNameCountTable.emplace(id, 0);
			number = 0;
		}
		else {
			number = ++GNameCountTable[id];
		}
	}

	EName::EName(const WChar * Text) : entryName(Text) {
		id = WStringToHash(Text);
		if (GNamesTable.try_emplace(id, Text).second) {
			GNameCountTable.emplace(id, 0);
			number = 0;
		}
		else {
			number = ++GNameCountTable[id];
		}
	}

	EName::EName(size_t InNumber) {
		if (GNamesTable.find(InNumber) == GNamesTable.end()) {
			entryName = L"";
			number = 0;
		}
		else {
			entryName = GNamesTable[InNumber];
			number = ++GNameCountTable[InNumber];
		}
	}

	EName::EName(const WString & Text, size_t number) : entryName(Text), number(number) {
		id = WStringToHash(Text);
		GNamesTable.try_emplace(id, Text);
	}

	EName::~EName() { }

	WString EName::GetDisplayName() const {
		return entryName;
	}

	NString EName::GetNarrowDisplayName() const {
		return Text::WideToNarrow(entryName);
	}

	WString EName::GetInstanceName() const {
		return entryName + L"_" + std::to_wstring(number);
	}

	NString EName::GetNarrowInstanceName() const {
		return Text::WideToNarrow(entryName) + "_" + std::to_string(number);
	}

	size_t EName::GetNumber() const {
		return number;
	}

	size_t EName::GetInstanceID() const {
		return WStringToHash(entryName + L"_" + std::to_wstring(number));
	}

	size_t EName::GetID() const {
		return id;
	}

	bool EName::operator<(const EName & Other) const {
		uint32_t i = 0;
		while ((i < entryName.length()) && (i < Other.entryName.length())) {
			if (tolower(entryName[i]) < tolower(Other.entryName[i])) return true;
			else if (tolower(entryName[i]) > tolower(Other.entryName[i])) return false;
			++i;
		}
		return (entryName.length() + number < Other.entryName.length() + Other.number);
	}

	bool EName::operator!=(const EName & Other) const {
		return id != Other.id || number != number;
	}

	bool EName::operator==(const EName & Other) const {
		return id == Other.id && number == number;
	}

}
