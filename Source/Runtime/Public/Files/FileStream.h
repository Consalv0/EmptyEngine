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
            return (std::move( *mStream ) >> value);
        }

        template <typename T>
        inline const OStream& operator<<( T value )
        {
            return (std::move( *mStream ) << value);
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
        FStream* mStream;
        size_t mLength;
    };

    template<typename StringType>
    FileStream<StringType>::FileStream( const File& file ) : File( file )
    {
        mStream = new FStream();
    }

    template<typename StringType>
    FileStream<StringType>::~FileStream()
    {
        delete mStream;
    }

    template<typename StringType>
    inline float FileStream<StringType>::GetProgress() const
    {
        size_t progress = size_t( mStream->tellg() );
        return progress / float( mLength );
    }

    template<typename StringType>
    inline size_t FileStream<StringType>::GetPosition() const
    {
        return mStream->tellg();
    }

    template<typename StringType>
    bool FileStream<StringType>::ReadStream( Char* output, size_t length ) const
    {
        if ( IsValid() )
        {
            try
            {
                mStream->read( output, length );
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
        std::getline( *mStream, string );
        return string;
    }

    template<typename StringType>
    bool FileStream<StringType>::IsValid() const
    {
        return Super::IsValid() && mStream != NULL && !mStream->fail() && mStream->good();
    }

    template<typename StringType>
    void FileStream<StringType>::LocaleToUTF8( const U8Char* localeFormat ) const
    {
        if ( localeFormat == NULL )
            localeFormat = "en_US.UTF-8";
        static std::locale locale( localeFormat );
        mStream->imbue( locale );
    }

    template<typename StringType>
    bool FileStream<StringType>::Open( int openFlags, bool utf8 )
    {
        if ( utf8 )
            LocaleToUTF8( NULL );

#ifdef EE_PLATFORM_WINDOWS
        mStream->open( mPath, openFlags );
#else
        mStream->open( mPath, openFlags );
#endif

        return mStream->is_open();
    }

    template<typename StringType>
    void FileStream<StringType>::Clean()
    {
        if ( mStream->is_open() )
            mStream->close();
#ifdef EE_PLATFORM_WINDOWS
        mStream->open( mPath, std::ios::in | std::ios::out | std::ios::trunc );
#else
        mStream->open( mPath, std::ios::in | std::ios::out | std::ios::trunc );
#endif
    }

    template<typename StringType>
    void FileStream<StringType>::MoveCursor( size_t pos )
    {
        mStream->seekg( pos );
    }

    template<typename StringType>
    void FileStream<StringType>::Close()
    {
        mStream->close();
    }

    template<typename StringType>
    void FileStream<StringType>::ReadToStringBuffer( FileStream<StringType>::StringBuffer& buffer ) const
    {
        buffer << mStream->rdbuf();
    }
}