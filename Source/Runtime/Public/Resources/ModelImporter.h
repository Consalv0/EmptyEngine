#pragma once

#include <future>

#include "Rendering/Mesh.h"
#include "Files/FileManager.h"

namespace EE
{
    struct ModelNode
    {
        U8String name;
        Transform transform;
        bool hasMesh;
        size_t meshKey;
        ModelNode* parent;
        TArray<ModelNode*> children;

        ModelNode( const U8String& name ) : name( name ), transform(), hasMesh( false ), meshKey( 0 ), parent( NULL ), children() {};

        ModelNode& operator=( const ModelNode& other );

        ~ModelNode()
        {
            for ( auto& child : children )
            {
                delete child;
            }
        }

        inline ModelNode* AddChild( const U8String& name )
        {
            ModelNode* child = new ModelNode( name );
            child->parent = this;
            children.push_back( child );
            return child;
        }
    };

    class ModelImporter
    {
    public:
        struct Options
        {
            const File& file;
            bool optimize;
        };

        struct ModelResult
        {
            TArray<MeshData> meshes;
            ModelNode parentNode;

            //* The model data has been succesfully loaded
            bool isValid;
            bool hasAnimations;

            void Transfer( ModelResult& other );

            ModelResult();

            ModelResult( const ModelResult& other ) = delete;
        };

    private:
        typedef std::function<void( ModelResult& )> FinishTaskFunction;
        typedef std::function<void( ModelResult&, const Options&, std::future<bool>& )> FutureTask;

        static bool TaskRunning;

        struct Task
        {
            Options options;
            ModelResult info;
            FinishTaskFunction finishTaskFunction;
            FutureTask futureTask;

            Task( const Task& Other ) = delete;
            Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask );
        };

        static bool RecognizeFileExtensionAndLoad( ModelResult& data, const Options& options );

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

        static bool Load( ModelResult& info, const Options& options );

        static void LoadAsync( const Options& options, FinishTaskFunction onComplete );

    };
}