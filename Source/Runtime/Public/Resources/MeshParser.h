#pragma once

#include <future>

#include "Graphics/Mesh.h"
#include "Files/FileStream.h"

namespace EE
{
    struct ModelNode
    {
        NString name;
        Transform transform;
        bool hasMesh;
        size_t meshKey;
        ModelNode* parent;
        TArray<ModelNode*> children;

        ModelNode( const NString& name ) : name( name ), transform(), hasMesh( false ), meshKey( 0 ), parent( NULL ), children() {};

        ModelNode& operator=( const ModelNode& other );

        ~ModelNode()
        {
            for ( auto& child : children )
            {
                delete child;
            }
        }

        inline ModelNode* AddChild( const NString& name )
        {
            ModelNode* child = new ModelNode( name );
            child->parent = this;
            children.push_back( child );
            return child;
        }
    };

    class ModelParser
    {
    public:
        struct ParsingOptions
        {
            const FileStream* file;
            bool optimize;
        };

        struct ModelDataInfo
        {
            TArray<MeshData> meshes;
            ModelNode parentNode;

            //* The model data has been succesfully loaded
            bool isValid;
            bool hasAnimations;

            void Transfer( ModelDataInfo& other );

            ModelDataInfo();

            ModelDataInfo( const ModelDataInfo& other ) = delete;
        };

    private:
        typedef std::function<void( ModelDataInfo& )> FinishTaskFunction;
        typedef std::function<class std::future<bool>( ModelDataInfo&, const ParsingOptions& )> FutureTask;

        static bool TaskRunning;

        struct Task
        {
            ParsingOptions options;
            ModelDataInfo info;
            FinishTaskFunction finishTaskFunction;
            FutureTask futureTask;

            Task( const Task& Other ) = delete;
            Task( const ParsingOptions& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask );
        };

        static bool RecognizeFileExtensionAndLoad( ModelDataInfo& data, const ParsingOptions& options );

        static void FinishCurrentAsyncTask();

        //* Mesh Loading Threads
        static std::queue<Task*> pendingTasks;
        static std::future<bool> currentFutureTask;

    public:
        static bool Initialize();

        static void UpdateStatus();

        static void FinishAsyncTasks();

        static size_t GetAsyncTaskCount();

        static void Exit();

        static bool Load( ModelDataInfo& info, const ParsingOptions& options );

        static void LoadAsync( const ParsingOptions& options, FinishTaskFunction onComplete );

    };
}