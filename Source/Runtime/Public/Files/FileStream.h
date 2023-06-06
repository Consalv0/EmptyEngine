#pragma once

#include "Utils/TextFormatting.h"
#include "Utils/Hasher.h"

namespace EE
{
    struct FileStream
    {
    private:
        std::wfstream* stream;
        long length;
        WString path;

    public:

        FileStream();
        FileStream( WString path );

        WString GetExtension() const;
        WString GetFileName() const;
        WString GetFileNameWithoutExtension() const;
        WString GetPath() const;
        WString GetShortPath() const;
        std::wstringstream ReadStream() const;
        bool ReadNarrowStream( NString* output ) const;
        WString GetLine();
        bool IsValid() const;

        template <typename T>
        inline const std::wistream& operator>>( T value );

        template <typename T>
        inline const std::wostream& operator<<( T value );

        inline float GetProgress() const;

        inline long GetPosition() const;

        void LocaleToUTF8( const NChar* locale ) const;

        long GetLenght();

        bool Open();

        void Close();

        void Clean();

        void Reset();
    };
}