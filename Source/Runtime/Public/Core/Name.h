#pragma once

namespace EE
{
	class EName {
	public:
		EName(const WString & Text);

		EName(const WChar * Text);

		EName(size_t number);

		EName(const WString & Text, size_t number);

		~EName();

		WString GetDisplayName() const;

		NString GetNarrowDisplayName() const;

		WString GetInstanceName() const;

		NString GetNarrowInstanceName() const;
		
		size_t GetNumber() const;

		size_t GetInstanceID() const;
		
		size_t GetID() const;

		bool operator<(const EName& Other) const;

		bool operator==(const EName& Other) const;

		bool operator!=(const EName& Other) const;

	private:
		size_t number;
		WString entryName;
		size_t id;

		static TMap<size_t, WString> GNamesTable;
		static TMap<size_t, size_t> GNameCountTable;
	};

    extern EE::EName GEmptyName;
}