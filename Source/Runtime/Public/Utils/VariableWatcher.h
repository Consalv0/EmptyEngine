
namespace EE
{
    class DebugVariableWatcher
    {
    public:
        class VariableWatcher
        {
            EE_CLASSNOCOPY( VariableWatcher )

        public:
            VariableWatcher( const size_t& line, const char* name, const NChar* file, const NChar* function, const uint64& lastChangeTime )
                : line( line )
                , name( name )
                , file( file )
                , function( function )
                , lastChangeTime( lastChangeTime )
            {
            }

        public:
            size_t line;
            const NChar* name;
            const NChar* file;
            const NChar* function;
            uint64 lastChangeTime;
        };

        enum VariableType
        {
            VariableType_Float,
            VariableType_Double,
        };

        class Variable
        {
            EE_CLASSNOCOPY( Variable )
        public:
            VariableType GetType() const;
            void* GetVariable() const;
            const NChar* GetName() const;
            const size_t& GetLine() const;
            const NChar* GeFile() const;
            const NChar* GetFunction() const;
            const uint64& GetLastTimeChanged() const;

            const VariableWatcher& GetWatcher() const;
            VariableWatcher& GetWatcher();

            Variable( VariableType type, VariableWatcher* watcher );
            Variable( );
            ~Variable();

        private:
            VariableType type_;
            VariableWatcher* variableWatcher_;
        };

    public:
        static void WatchFloat( const float& x, const char* name, size_t line, const char* file, const char* function );
        static void WatchDouble( const double& x, const char* name, size_t line, const char* file, const char* function );

        static void GetAllVariables( size_t* size, const Variable** variables );
    };
}

#define EE_WATCH_FLOAT( X ) DebugVariableWatcher::WatchFloat( X, #X, __LINE__, __FILE__, __func__ );
#define EE_WATCH_DOUBLE( X ) DebugVariableWatcher::WatchDouble( X, #X, __LINE__, __FILE__, __func__ );