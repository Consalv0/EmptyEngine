
#include <CoreMinimal.h>

#include "Engine/Ticker.h"

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
        VariableWatcher_T( const T& val, const NChar* name, const size_t& line, const NChar* file, const NChar* function, const uint64& lastChangeTime )
            : value( val )
            , VariableWatcher( line, name, file, function, lastChangeTime )
        {
        }

        T value;
    };

    class FloatWatcher : public VariableWatcher_T<float>
    {
        EE_CLASSNOCOPY( FloatWatcher )

    public:
        FloatWatcher( const float& val, const NChar* name, const size_t& line, const NChar* file, const NChar* function, const uint64& lastChangeTime )
            : VariableWatcher_T( val, name, line, file, function, lastChangeTime )
        {
        }
    };

    class DoubleWatcher : public VariableWatcher_T<double>
    {
        EE_CLASSNOCOPY( DoubleWatcher )

    public:
        DoubleWatcher( const double& val, const NChar* name, const size_t& line, const NChar* file, const NChar* function, const uint64& lastChangeTime )
            : VariableWatcher_T( val, name, line, file, function, lastChangeTime )
        {
        }
    };

    static TMap<size_t, DebugVariableWatcher::Variable> GVariables{};
}

EE_MAKE_HASHABLE( EE::FloatWatcher, t.line, t.file, t.function );
EE_MAKE_HASHABLE( EE::DoubleWatcher, t.line, t.file, t.function );

namespace EE
{
    DebugVariableWatcher::VariableType DebugVariableWatcher::Variable::GetType() const
    {
        return type_;
    }

    void* DebugVariableWatcher::Variable::GetVariable() const
    {
        switch ( type_ )
        {
        case DebugVariableWatcher::VariableType_Float:
            return &static_cast<FloatWatcher*>(variableWatcher_)->value;
        case DebugVariableWatcher::VariableType_Double:
            return &static_cast<DoubleWatcher*>(variableWatcher_)->value;
        default:
            return NULL;
        }
    }

    const NChar* DebugVariableWatcher::Variable::GetName() const
    {
        return GetWatcher().name;
    }

    const size_t& DebugVariableWatcher::Variable::GetLine() const
    {
        return GetWatcher().line;
    }

    const NChar* DebugVariableWatcher::Variable::GeFile() const
    {
        return GetWatcher().file;
    }

    const NChar* DebugVariableWatcher::Variable::GetFunction() const
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

    DebugVariableWatcher::Variable::Variable( VariableType type, DebugVariableWatcher::VariableWatcher* watcher )
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
        case DebugVariableWatcher::VariableType_Float:
            delete static_cast<FloatWatcher*>(variableWatcher_);
            break;
        default:
            break;
        }
    }

    void DebugVariableWatcher::WatchFloat( const float& x, const char* name, size_t line, const char* file, const char* function )
    {
        auto watcher = FloatWatcher( x, name, line, file, function, 0 );
        size_t hash = 0;
        EE::HashCombine( &hash, watcher );
        if ( GVariables.contains( hash ) )
        {
            float& f = *static_cast<float*>(GVariables[ hash ].GetVariable());
            if ( f != x )
            {
                f = x;
                GVariables[ hash ].GetWatcher().lastChangeTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
            }
        }
        else
        {
            GVariables.emplace( 
                std::piecewise_construct,
                std::forward_as_tuple( hash ),
                std::forward_as_tuple( VariableType_Float,
                    new FloatWatcher( watcher.value, watcher.name, watcher.line, watcher.file, watcher.function, Ticker::GetEpochTimeNow<Ticker::Nano>() ) )
            );
        }
    }

    void DebugVariableWatcher::WatchDouble( const double& x, const char* name, size_t line, const char* file, const char* function )
    {
        auto watcher = DoubleWatcher( x, name, line, file, function, 0 );
        size_t hash = 0;
        EE::HashCombine( &hash, watcher );
        if ( GVariables.contains( hash ) )
        {
            double& f = *static_cast<double*>(GVariables[ hash ].GetVariable());
            if ( f != x )
            {
                f = x;
                GVariables[ hash ].GetWatcher().lastChangeTime = Ticker::GetEpochTimeNow<Ticker::Nano>();
            }
        }
        else
        {
            GVariables.emplace( 
                std::piecewise_construct,
                std::forward_as_tuple( hash ),
                std::forward_as_tuple( VariableType_Double,
                    new DoubleWatcher( watcher.value, watcher.name, watcher.line, watcher.file, watcher.function, Ticker::GetEpochTimeNow<Ticker::Nano>() ) )
            );
        }
    }

    void DebugVariableWatcher::GetAllVariables( size_t* size, const DebugVariableWatcher::Variable** variables )
    {
        *size = GVariables.size();
        if ( variables == NULL )
        {
            return;
        }

        size_t index = 0;
        for ( TMap<size_t, DebugVariableWatcher::Variable>::const_iterator it = GVariables.begin(); it != GVariables.end(); it++ )
        {
            variables[ index++ ] = &it->second;
        }
    }
}