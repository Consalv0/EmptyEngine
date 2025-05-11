#pragma once

namespace EE
{
    class Name
    {
    private:
       uint64 number;
       uint64 id;

    private:
       Name() = delete;

    public:
       Name( const U8String& text );
       
       Name( const U8Char* text );
       
       Name( uint64 number );

       Name( const U8Char* text, uint64 number );
       
       ~Name();
       
       const U8String& GetName() const;
       
       U8String GetInstanceName() const;
       
       const uint64& GetNumber() const;
       
       uint64 GetInstanceID() const;
       
       const uint64& GetID() const;
       
       bool operator<( const Name& other ) const;
       
       bool operator==( const Name& other ) const;
       
       bool operator!=( const Name& other ) const;
    };

    extern EE::Name GEmptyName;
}