
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

        enum EVariableType
        {
            VariableType_Uknown,
            VariableType_WString,
            VariableType_NString,
            VariableType_Float,
            VariableType_Double,
            VariableType_Int8,
            VariableType_UInt8,
            VariableType_Int16,
            VariableType_UInt16,
            VariableType_Int32,
            VariableType_UInt32,
            VariableType_Int64,
            VariableType_UInt64,
#ifdef EE_CORE_MATH
            VariableType_Vector2f,
            VariableType_Vector3f,
            VariableType_Vector4f,
            VariableType_Vector2,
            VariableType_Vector3,
            VariableType_Vector4,
            VariableType_IntVector2,
            VariableType_IntVector3,
            VariableType_IntVector4,
            VariableType_UIntVector2,
            VariableType_UIntVector3,
#endif
        };

#define CONDITIONAL_(b, A, B) b ? A : B

        template<typename T>
        struct VariableType
        {
            static constexpr EVariableType type =
#ifdef EE_CORE_MATH
                    CONDITIONAL_((std::is_same<T, NString>::value), VariableType_NString,
                    CONDITIONAL_((std::is_same<T, WString>::value), VariableType_WString,
                    CONDITIONAL_((std::is_same<T,   float>::value), VariableType_Float,
                    CONDITIONAL_((std::is_same<T,  double>::value), VariableType_Double,
                    CONDITIONAL_((std::is_same<T,    int8>::value), VariableType_Int8,
                    CONDITIONAL_((std::is_same<T,   uint8>::value), VariableType_UInt8,
                    CONDITIONAL_((std::is_same<T,   int16>::value), VariableType_Int16,
                    CONDITIONAL_((std::is_same<T,  uint16>::value), VariableType_UInt16,
                    CONDITIONAL_((std::is_same<T,   int32>::value), VariableType_Int32,
                    CONDITIONAL_((std::is_same<T,  uint32>::value), VariableType_UInt32,
                    CONDITIONAL_((std::is_same<T,   int64>::value), VariableType_Int64,
                    CONDITIONAL_((std::is_same<T,  uint64>::value), VariableType_UInt64,
                    CONDITIONAL_((std::is_same<T,     Vector2f>::value), VariableType_Vector2f,
                    CONDITIONAL_((std::is_same<T,     Vector3f>::value), VariableType_Vector3f,
                    CONDITIONAL_((std::is_same<T,     Vector4f>::value), VariableType_Vector4f,
                    CONDITIONAL_((std::is_same<T,      Vector2>::value), VariableType_Vector2,
                    CONDITIONAL_((std::is_same<T,      Vector3>::value), VariableType_Vector3,
                    CONDITIONAL_((std::is_same<T,      Vector4>::value), VariableType_Vector4,
                    CONDITIONAL_((std::is_same<T,   IntVector2>::value), VariableType_IntVector2,
                    CONDITIONAL_((std::is_same<T,   IntVector3>::value), VariableType_IntVector3,
                    CONDITIONAL_((std::is_same<T,   IntVector4>::value), VariableType_IntVector4,
                    CONDITIONAL_((std::is_same<T,  UIntVector2>::value), VariableType_UIntVector2,
                    CONDITIONAL_((std::is_same<T,  UIntVector3>::value), VariableType_UIntVector3,
                    VariableType_Uknown)))))))))))))))))))))));
#else
                    CONDITIONAL_((std::is_same<T, NString>::value), VariableType_NString,
                    CONDITIONAL_((std::is_same<T, WString>::value), VariableType_WString,
                    CONDITIONAL_((std::is_same<T,   float>::value), VariableType_Float,
                    CONDITIONAL_((std::is_same<T,  double>::value), VariableType_Double,
                    CONDITIONAL_((std::is_same<T,    int8>::value), VariableType_Int8,
                    CONDITIONAL_((std::is_same<T,   uint8>::value), VariableType_UInt8,
                    CONDITIONAL_((std::is_same<T,   int16>::value), VariableType_Int16,
                    CONDITIONAL_((std::is_same<T,  uint16>::value), VariableType_UInt16,
                    CONDITIONAL_((std::is_same<T,   int32>::value), VariableType_Int32,
                    CONDITIONAL_((std::is_same<T,  uint32>::value), VariableType_UInt32,
                    CONDITIONAL_((std::is_same<T,   int64>::value), VariableType_Int64,
                    CONDITIONAL_((std::is_same<T,  uint64>::value), VariableType_UInt64,
                    VariableType_Uknown))))))))))));
#endif
        };

        class Variable
        {
            EE_CLASSNOCOPY( Variable )
        public:
            EVariableType GetType() const;
            void* GetVariable() const;
            const NChar* GetName() const;
            const size_t& GetLine() const;
            const NChar* GeFile() const;
            const NChar* GetFunction() const;
            const uint64& GetLastTimeChanged() const;
            const bool& PositiveChange() const;

            const VariableWatcher& GetWatcher() const;
            VariableWatcher& GetWatcher();

            Variable( EVariableType type, VariableWatcher* watcher );
            Variable( );
            ~Variable();

        private:
            EVariableType type_;
            VariableWatcher* variableWatcher_;
        };

    public:
        static void Watch( const NString& x, const char* name, size_t line, const char* file, const char* function);
        static void Watch( const WString& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const   float& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  double& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const    int8& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const   uint8& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const   int16& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  uint16& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const   int32& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  uint32& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const   int64& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  uint64& x, const char* name, size_t line, const char* file, const char* function );
#ifdef EE_CORE_MATH
        static void Watch( const    Vector2f& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const    Vector3f& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const    Vector4f& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const     Vector2& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const     Vector3& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const     Vector4& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  IntVector2& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  IntVector3& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const  IntVector4& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const UIntVector2& x, const char* name, size_t line, const char* file, const char* function );
        static void Watch( const UIntVector3& x, const char* name, size_t line, const char* file, const char* function );
#endif

        static void GetAllVariables( size_t* size, const Variable** variables );
    };
}

#define EE_WATCH( X ) DebugVariableWatcher::Watch( X, #X, __LINE__, __FILE__, __func__ );