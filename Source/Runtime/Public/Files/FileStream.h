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
            return (std::move( *stream_ ) >> value);
        }

        template <typename T>
        inline const OStream& operator<<( T value )
        {
            return (std::move( *stream_ ) << value);
        }

        inline float GetProgress() const;

        inline size_t GetPosition() const;

        void LocaleToUTF8( const NChar* locale ) const;

        bool Open( int openFlags, bool utf8 );

        void Close();

        void Clean();

        void MoveCursor( size_t pos );

        void ReadToStringBuffer( StringBuffer& buffer ) const;

    private:
        FStream* stream_;
        size_t length_;
    };

    template<typename StringType>
    FileStream<StringType>::FileStream( const File& file ) : File( file )
    {
        stream_ = new FStream();
    }

    template<typename StringType>
    FileStream<StringType>::~FileStream()
    {
        delete stream_;
    }

    template<typename StringType>
    inline float FileStream<StringType>::GetProgress() const
    {
        size_t progress = size_t( stream_->tellg() );
        return progress / float( length_ );
    }

    template<typename StringType>
    inline size_t FileStream<StringType>::GetPosition() const
    {
        return stream_->tellg();
    }

    template<typename StringType>
    bool FileStream<StringType>::ReadStream( Char* output, size_t length ) const
    {
        if ( IsValid() )
        {
            try
            {
                stream_->read( output, length );
                return true;
            }
            catch ( ... )
            {
                return false;
            }
        }
        else
        {
            EE_LOG_ERROR( L"File '{}' is not valid or do not exist", path_ );
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
            EE_LOG_ERROR( L"File '{}' is not valid or do not exist", path_ );
            return false;
        }
    }

    template<typename StringType>
    StringType FileStream<StringType>::GetLine()
    {
        WString string;
        std::getline( *stream_, string );
        return string;
    }

    template<typename StringType>
    bool FileStream<StringType>::IsValid() const
    {
        return Super::IsValid() && stream_ != NULL && !stream_->fail() && stream_->good();
    }

    template<typename StringType>
    void FileStream<StringType>::LocaleToUTF8( const NChar* localeFormat ) const
    {
        if ( localeFormat == NULL )
            localeFormat = "en_US.UTF-8";
        static std::locale locale( localeFormat );
        stream_->imbue( locale );
    }

    template<typename StringType>
    bool FileStream<StringType>::Open( int openFlags, bool utf8 )
    {
        if ( utf8 )
            LocaleToUTF8( NULL );

#ifdef EE_PLATFORM_WINDOWS
        stream_->open( path_, openFlags );
#else
        stream_->open( Text::WideToNarrow( path_ ), openFlags );
#endif

        return stream_->is_open();
    }

    template<typename StringType>
    void FileStream<StringType>::Clean()
    {
        if ( stream_->is_open() )
            stream_->close();
#ifdef EE_PLATFORM_WINDOWS
        stream_->open( path_, std::ios::in | std::ios::out | std::ios::trunc );
#else
        stream_->open( Text::WideToNarrow( path_ ), std::ios::in | std::ios::out | std::ios::trunc );
#endif
    }

    template<typename StringType>
    void FileStream<StringType>::MoveCursor( size_t pos )
    {
        stream_->seekg( pos );
    }

    template<typename StringType>
    void FileStream<StringType>::Close()
    {
        stream_->close();
    }

    template<typename StringType>
    void FileStream<StringType>::ReadToStringBuffer( FileStream<StringType>::StringBuffer& buffer ) const
    {
        buffer << stream_->rdbuf();
    }
}