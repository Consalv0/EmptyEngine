#pragma once

#include "Utils/TextFormatting.h"
#include "Utils/Hasher.h"

namespace EE
{
    class File
    {
    protected:
        WString path;
    
    public:
        File( const File& file );
        File( const WString& path );

        ~File();

        virtual bool IsValid() const;

        WString GetExtension() const;
        WString GetFileName() const;
        WString GetFileNameWithoutExtension() const;
        WString GetPath() const;
        WString GetShortPath() const;
    };

    template<typename StringType>
    class FileStream : public File
    {
        typedef File Super;
        using Char = StringType::value_type;
        using FStream = std::basic_fstream<Char, std::char_traits<Char>>;
        using IStream = std::basic_istream<Char, std::char_traits<Char>>;
        using OStream = std::basic_ostream<Char, std::char_traits<Char>>;
        using StringBuffer = std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>>;

    private:
        FStream* stream;
        size_t length;

    public:
        FileStream( const File& file );
        ~FileStream();

        bool ReadStream( StringType& output ) const;

        bool ReadStream( Char* output, size_t length ) const;

        StringType GetLine();

        bool IsValid() const override;

        template <typename T>
        inline const IStream& operator>>( T value )
        {
            return (std::move( *stream ) >> value);
        }

        template <typename T>
        inline const OStream& operator<<( T value )
        {
            return (std::move( *stream ) << value);
        }

        inline float GetProgress() const;

        inline size_t GetPosition() const;

        void LocaleToUTF8( const NChar* locale ) const;

        bool Open( int openFlags, bool utf8 );

        void Close();

        void Clean();

        void MoveCursor( size_t pos );

        void ReadToStringBuffer( StringBuffer& buffer ) const;
    };

    template<typename StringType>
    FileStream<StringType>::FileStream( const File& file ) : File( file )
    {
        stream = new FStream();
    }

    template<typename StringType>
    FileStream<StringType>::~FileStream()
    {
        delete stream;
    }

    template<typename StringType>
    inline float FileStream<StringType>::GetProgress() const
    {
        size_t progress = size_t( stream->tellg() );
        return progress / float( length );
    }

    template<typename StringType>
    inline size_t FileStream<StringType>::GetPosition() const
    {
        return stream->tellg();
    }

    template<typename StringType>
    bool FileStream<StringType>::ReadStream( Char* output, size_t length ) const
    {
        if ( IsValid() )
        {
            try
            {
                stream->read( output, length );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }
        else
        {
            EE_LOG_ERROR( L"File '{}' is not valid or do not exist", path );
            return false;
        }
    }

    template<typename StringType>
    bool FileStream<StringType>::ReadStream( StringType& output ) const
    {
        if ( IsValid() )
        {
            try
            {
                StringBuffer buffer;
                ReadToStringBuffer( buffer );
                output.append( buffer.str() );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }
        else
        {
            EE_LOG_ERROR( L"File '{}' is not valid or do not exist", path );
            return false;
        }
    }

    template<typename StringType>
    StringType FileStream<StringType>::GetLine()
    {
        WString string;
        std::getline( *stream, string );
        return string;
    }

    template<typename StringType>
    bool FileStream<StringType>::IsValid() const
    {
        return Super::IsValid() && stream != NULL && !stream->fail() && stream->good();
    }

    template<typename StringType>
    void FileStream<StringType>::LocaleToUTF8( const NChar* localeFormat ) const
    {
        if ( localeFormat == NULL )
            localeFormat = "en_US.UTF-8";
        static std::locale locale( localeFormat );
        stream->imbue( locale );
    }

    template<typename StringType>
    bool FileStream<StringType>::Open( int openFlags, bool utf8 )
    {
        if ( utf8 )
            LocaleToUTF8( NULL );

#ifdef EE_PLATFORM_WINDOWS
        stream->open( path, openFlags );
#else
        stream->open( Text::WideToNarrow( path ), openFlags );
#endif

        return stream->is_open();
    }

    template<typename StringType>
    void FileStream<StringType>::Clean()
    {
        if ( stream->is_open() )
            stream->close();
#ifdef EE_PLATFORM_WINDOWS
        stream->open( path, std::ios::in | std::ios::out | std::ios::trunc );
#else
        stream->open( Text::WideToNarrow( path ), std::ios::in | std::ios::out | std::ios::trunc );
#endif
    }

    template<typename StringType>
    void FileStream<StringType>::MoveCursor( size_t pos )
    {
        stream->seekg( pos );
    }

    template<typename StringType>
    void FileStream<StringType>::Close()
    {
        stream->close();
    }

    template<typename StringType>
    void FileStream<StringType>::ReadToStringBuffer( FileStream<StringType>::StringBuffer& buffer ) const
    {
        buffer << stream->rdbuf();
    }
}