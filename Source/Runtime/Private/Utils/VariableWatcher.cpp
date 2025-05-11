
#include <CoreMinimal.h>

#include "Engine/Ticker.h"

#include <Math/CoreMath.h>
#include <Core/Collections.h>
#include <Utils/VariableWatcher.h>
#include <Utils/Hasher.h>

namespace EE
{
    template<typename T>
    class VariableWatcher_T : public DebugVariableWatcher::VariableWatcher
    {
        EE_CLASSNOCOPY( VariableWatcher_T )

    public:
        VariableWatcher_T( const T& val, const U8Char* name, const uint32& line, const U8Char* file, const U8Char* function, const uint64& lastChangeTime )
            : value( val )
            , VariableWatcher( line, name, file, function, lastChangeTime )
        {
        }

        T value;
    };

    static TMap<uint64, DebugVariableWatcher::Variable> GVariables{};
}

namespace EE
{
    DebugVariableWatcher::EVariableType DebugVariableWatcher::Variable::GetType() const
    {
        return type_;
    }

    void* DebugVariableWatcher::Variable::GetVariable() const
    {
        switch ( type_ )
        {
          case DebugVariableWatcher::VariableType_String      : return &static_cast<VariableWatcher_T<    U8String>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Float       : return &static_cast<VariableWatcher_T<       float>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Double      : return &static_cast<VariableWatcher_T<      double>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Int8        : return &static_cast<VariableWatcher_T<        int8>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UInt8       : return &static_cast<VariableWatcher_T<       uint8>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Int16       : return &static_cast<VariableWatcher_T<       int16>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UInt16      : return &static_cast<VariableWatcher_T<      uint16>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Int32       : return &static_cast<VariableWatcher_T<       int32>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UInt32      : return &static_cast<VariableWatcher_T<      uint32>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Int64       : return &static_cast<VariableWatcher_T<       int64>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UInt64      : return &static_cast<VariableWatcher_T<      uint64>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector2f    : return &static_cast<VariableWatcher_T<    Vector2f>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector3f    : return &static_cast<VariableWatcher_T<    Vector3f>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector4f    : return &static_cast<VariableWatcher_T<    Vector4f>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector2     : return &static_cast<VariableWatcher_T<     Vector2>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector3     : return &static_cast<VariableWatcher_T<     Vector3>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_Vector4     : return &static_cast<VariableWatcher_T<     Vector4>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_IntVector2  : return &static_cast<VariableWatcher_T<  IntVector2>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_IntVector3  : return &static_cast<VariableWatcher_T<  IntVector3>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_IntVector4  : return &static_cast<VariableWatcher_T<  IntVector4>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UIntVector2 : return &static_cast<VariableWatcher_T< UIntVector2>*>(variableWatcher_)->value;
          case DebugVariableWatcher::VariableType_UIntVector3 : return &static_cast<VariableWatcher_T< UIntVector3>*>(variableWatcher_)->value;
        default:
            return NULL;
        }
    }

    const U8Char* DebugVariableWatcher::Variable::GetName() const
    {
        return GetWatcher().name;
    }

    const uint32& DebugVariableWatcher::Variable::GetLine() const
    {
        return GetWatcher().line;
    }

    const U8Char* DebugVariableWatcher::Variable::GeFile() const
    {
        return GetWatcher().file;
    }

    const U8Char* DebugVariableWatcher::Variable::GetFunction() const
    {
        return GetWatcher().function;
    }

    const uint64& DebugVariableWatcher::Variable::GetLastTimeChanged() const
    {
        return GetWatcher().lastChangeTime;
    }

    const DebugVariableWatcher::VariableWatcher& DebugVariableWatcher::Variable::GetWatcher() const
    {
        return *variableWatcher_;
    }

    DebugVariableWatcher::VariableWatcher& DebugVariableWatcher::Variable::GetWatcher()
    {
        return *variableWatcher_;
    }

    DebugVariableWatcher::Variable::Variable( EVariableType type, DebugVariableWatcher::VariableWatcher* watcher )
        : type_( type )
        , variableWatcher_( watcher )
    {
    }

    DebugVariableWatcher::Variable::Variable( )
        : type_()
        , variableWatcher_( NULL )
    {
    }

    DebugVariableWatcher::Variable::~Variable()
    {
        switch ( type_ )
        {
          case DebugVariableWatcher::VariableType_String      : delete static_cast<VariableWatcher_T<    U8String>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Float       : delete static_cast<VariableWatcher_T<       float>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Double      : delete static_cast<VariableWatcher_T<      double>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Int8        : delete static_cast<VariableWatcher_T<        int8>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UInt8       : delete static_cast<VariableWatcher_T<       uint8>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Int16       : delete static_cast<VariableWatcher_T<       int16>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UInt16      : delete static_cast<VariableWatcher_T<      uint16>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Int32       : delete static_cast<VariableWatcher_T<       int32>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UInt32      : delete static_cast<VariableWatcher_T<      uint32>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Int64       : delete static_cast<VariableWatcher_T<       int64>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UInt64      : delete static_cast<VariableWatcher_T<      uint64>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector2f    : delete static_cast<VariableWatcher_T<    Vector2f>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector3f    : delete static_cast<VariableWatcher_T<    Vector3f>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector4f    : delete static_cast<VariableWatcher_T<    Vector4f>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector2     : delete static_cast<VariableWatcher_T<     Vector2>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector3     : delete static_cast<VariableWatcher_T<     Vector3>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_Vector4     : delete static_cast<VariableWatcher_T<     Vector4>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_IntVector2  : delete static_cast<VariableWatcher_T<  IntVector2>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_IntVector3  : delete static_cast<VariableWatcher_T<  IntVector3>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_IntVector4  : delete static_cast<VariableWatcher_T<  IntVector4>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UIntVector2 : delete static_cast<VariableWatcher_T< UIntVector2>*>(variableWatcher_); break;
          case DebugVariableWatcher::VariableType_UIntVector3 : delete static_cast<VariableWatcher_T< UIntVector3>*>(variableWatcher_); break;
        default:
            break;
        }
    }

    template<typename T>
    void WatchImplementation( const T& x, const char* name, uint32 line, const char* file, const char* function )
    {
        auto watcher = VariableWatcher_T<T>( x, name, line, file, function, 0 );
        uint64 hash = 0;
        EE::HashCombine( &hash, watcher );
        if ( GVariables.contains( hash ) )
        {
            T& f = *static_cast<T*>(GVariables[ hash ].GetVariable());
            if ( f != x )
            {
                DebugVariableWatcher::VariableWatcher& variableWatcher = GVariables[ hash ].GetWatcher();
                variableWatcher.lastChangeTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
                f = x;
            }
        }
        else
        {
            GVariables.emplace(
                std::piecewise_construct,
                std::forward_as_tuple( hash ),
                std::forward_as_tuple( DebugVariableWatcher::VariableType<T>::type,
                    new VariableWatcher_T<T>( watcher.value, watcher.name, watcher.line, watcher.file, watcher.function, Ticker::GetEpochTimeNow<Ticker::Nano>()))
            );
        }
    }

    template<typename T>
    void WatchImplementation_Vector( const T& x, const char* name, uint32 line, const char* file, const char* function )
    {
        auto watcher = VariableWatcher_T<T>( x, name, line, file, function, 0 );
        uint64 hash = 0;
        EE::HashCombine( &hash, watcher );
        if ( GVariables.contains( hash ) )
        {
            T& f = *static_cast<T*>(GVariables[ hash ].GetVariable());
            if ( f != x )
            {
                DebugVariableWatcher::VariableWatcher& variableWatcher = GVariables[ hash ].GetWatcher();
                variableWatcher.lastChangeTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
                f = x;
            }
        }
        else
        {
            GVariables.emplace(
                std::piecewise_construct,
                std::forward_as_tuple( hash ),
                std::forward_as_tuple( DebugVariableWatcher::VariableType<T>::type,
                    new VariableWatcher_T<T>( watcher.value, watcher.name, watcher.line, watcher.file, watcher.function, Ticker::GetEpochTimeNow<Ticker::Nano>()))
            );
        }
    }

    void DebugVariableWatcher::GetAllVariables( uint64* size, const DebugVariableWatcher::Variable** variables )
    {
        *size = GVariables.size();
        if ( variables == NULL )
        {
            return;
        }

        uint64 index = 0;
        for ( TMap<uint64, DebugVariableWatcher::Variable>::const_iterator it = GVariables.begin(); it != GVariables.end(); it++ )
        {
            variables[ index++ ] = &it->second;
        }
    }
}

#define EE_IMPLEMENT_WATCHER( x )   EE_MAKE_HASHABLE( EE::VariableWatcher_T<x>, t.name, t.line, t.file, t.function ); \
                                    void EE::DebugVariableWatcher::Watch( const x& val, const char* name, uint32 line, const char* file, const char* function ) { \
                                        EE::WatchImplementation( val, name, line, file, function ); }

#define EE_IMPLEMENT_VECTOR_WATCHER( x )   EE_MAKE_HASHABLE( EE::VariableWatcher_T<x>, t.name, t.line, t.file, t.function ); \
                                    void EE::DebugVariableWatcher::Watch( const x& val, const char* name, uint32 line, const char* file, const char* function ) { \
                                        EE::WatchImplementation_Vector( val, name, line, file, function ); }

EE_IMPLEMENT_WATCHER( U8String )
EE_IMPLEMENT_WATCHER(    float )
EE_IMPLEMENT_WATCHER(   double )
EE_IMPLEMENT_WATCHER(     int8 )
EE_IMPLEMENT_WATCHER(    uint8 )
EE_IMPLEMENT_WATCHER(    int16 )
EE_IMPLEMENT_WATCHER(   uint16 )
EE_IMPLEMENT_WATCHER(    int32 )
EE_IMPLEMENT_WATCHER(   uint32 )
EE_IMPLEMENT_WATCHER(    int64 )
EE_IMPLEMENT_WATCHER(   uint64 )
EE_IMPLEMENT_VECTOR_WATCHER( EE::   Vector2f )
EE_IMPLEMENT_VECTOR_WATCHER( EE::   Vector3f )
EE_IMPLEMENT_VECTOR_WATCHER( EE::   Vector4f )
EE_IMPLEMENT_VECTOR_WATCHER( EE::    Vector2 )
EE_IMPLEMENT_VECTOR_WATCHER( EE::    Vector3 )
EE_IMPLEMENT_VECTOR_WATCHER( EE::    Vector4 )
EE_IMPLEMENT_VECTOR_WATCHER( EE:: IntVector2 )
EE_IMPLEMENT_VECTOR_WATCHER( EE:: IntVector3 )
EE_IMPLEMENT_VECTOR_WATCHER( EE:: IntVector4 )
EE_IMPLEMENT_VECTOR_WATCHER( EE::UIntVector2 )
EE_IMPLEMENT_VECTOR_WATCHER( EE::UIntVector3 )