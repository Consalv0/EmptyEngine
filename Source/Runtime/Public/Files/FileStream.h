#pragma once

#include "FileManager.h"

namespace EE
{
    template<typename StringType>
    class FileStream : public File
    {
        typedef File Super;
        using Char = StringType::value_type;
        using FStream = std::basic_fstream<Char, std::char_traits<Char>>;
        using IStream = std::basic_istream<Char, std::char_traits<Char>>;
        using OStream = std::basic_ostream<Char, std::char_traits<Char>>;
        using StringBuffer = std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>>;

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
            return (std::move( *_stream ) >> value);
        }

        template <typename T>
        inline const OStream& operator<<( T value )
        {
            return (std::move( *_stream ) << value);
        }

        inline float GetProgress() const;

        inline size_t GetPosition() const;

        void LocaleToUTF8( const U8Char* locale ) const;

        bool Open( int openFlags, bool utf8 );

        void Close();

        void Clean();

        void MoveCursor( size_t pos );

        void ReadToStringBuffer( StringBuffer& buffer ) const;

    private:
        FStream* _stream;
        size_t _length;
    };

    template<typename StringType>
    FileStream<StringType>::FileStream( const File& file ) : File( file )
    {
        _stream = new FStream();
    }

    template<typename StringType>
    FileStream<StringType>::~FileStream()
    {
        delete _stream;
    }

    template<typename StringType>
    inline float FileStream<StringType>::GetProgress() const
    {
        size_t progress = size_t( _stream->tellg() );
        return progress / float( _length );
    }

    template<typename StringType>
    inline size_t FileStream<StringType>::GetPosition() const
    {
        return _stream->tellg();
    }

    template<typename StringType>
    bool FileStream<StringType>::ReadStream( Char* output, size_t length ) const
    {
        if ( IsValid() )
        {
            try
            {
                _stream->read( output, length );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }
        else
        {
            EE_LOG_ERROR( "File '{}' is not valid or do not exist", mPath );
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
            EE_LOG_ERROR( "File '{}' is not valid or do not exist", mPath );
            return false;
        }
    }

    template<typename StringType>
    StringType FileStream<StringType>::GetLine()
    {
        WString string;
        std::getline( *_stream, string );
        return string;
    }

    template<typename StringType>
    bool FileStream<StringType>::IsValid() const
    {
        return Super::IsValid() && _stream != NULL && !_stream->fail() && _stream->good();
    }

    template<typename StringType>
    void FileStream<StringType>::LocaleToUTF8( const U8Char* localeFormat ) const
    {
        if ( localeFormat == NULL )
            localeFormat = "en_US.UTF-8";
        static std::locale locale( localeFormat );
        _stream->imbue( locale );
    }

    template<typename StringType>
    bool FileStream<StringType>::Open( int openFlags, bool utf8 )
    {
        if ( utf8 )
            LocaleToUTF8( NULL );

#ifdef EE_PLATFORM_WINDOWS
        _stream->open( mPath, openFlags );
#else
        _stream->open( mPath, openFlags );
#endif

        return _stream->is_open();
    }

    template<typename StringType>
    void FileStream<StringType>::Clean()
    {
        if ( _stream->is_open() )
            _stream->close();
#ifdef EE_PLATFORM_WINDOWS
        _stream->open( mPath, std::ios::in | std::ios::out | std::ios::trunc );
#else
        _stream->open( mPath, std::ios::in | std::ios::out | std::ios::trunc );
#endif
    }

    template<typename StringType>
    void FileStream<StringType>::MoveCursor( size_t pos )
    {
        _stream->seekg( pos );
    }

    template<typename StringType>
    void FileStream<StringType>::Close()
    {
        _stream->close();
    }

    template<typename StringType>
    void FileStream<StringType>::ReadToStringBuffer( FileStream<StringType>::StringBuffer& buffer ) const
    {
        buffer << _stream->rdbuf();
    }
}