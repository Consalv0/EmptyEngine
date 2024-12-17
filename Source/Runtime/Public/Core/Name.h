#pragma once

namespace EE
{
    class Name
    {
    private:
       size_t number;
       size_t id;

    private:
       Name() = delete;

    public:
       Name( const WString& text );
       
       Name( const WChar* text );
       
       Name( const NString& text );
       
       Name( const NChar* text );
       
       Name( size_t number );

       Name( const WString& text, size_t number );

       Name( const NChar* text, size_t number );
       
       ~Name();
       
       const WString& GetName() const;
       
       const NString& GetNarrowName() const;
       
       WString GetInstanceName() const;
       
       NString GetNarrowInstanceName() const;
       
       const size_t& GetNumber() const;
       
       size_t GetInstanceID() const;
       
       const size_t& GetID() const;
       
       bool operator<( const Name& other ) const;
       
       bool operator==( const Name& other ) const;
       
       bool operator!=( const Name& other ) const;
    };

    extern EE::Name GEmptyName;
}