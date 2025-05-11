
#include "CoreMinimal.h"

#include "Utils/Hasher.h"
#include "Rendering/Mesh.h"
#include "Files/FileManager.h"
#include "Resources/ModelImporter.h"
#include "Resources/OBJImporter.h"

#include "Utils/TextFormatting.h"

#include <future>
#include <thread>

namespace EE
{
    bool ModelImporter::TaskRunning;
    std::queue<ModelImporter::Task*> ModelImporter::pendingTasks = std::queue<Task*>();
    std::future<bool> ModelImporter::currentFutureTask;
    std::mutex ModelImporterQueueLock;

    ModelNode& ModelNode::operator=( const ModelNode& other )
    {
        name = other.name;
        transform = other.transform;
        hasMesh = other.hasMesh;
        meshKey = other.meshKey;
        parent = NULL;
        children.clear();
        for ( auto& otherChild : other.children )
        {
            ModelNode* child = new ModelNode( name );
            *child = *otherChild;
            child->parent = this;
            children.push_back( child );
        }
        return *this;
    }

    bool FindNodeLevel( const U8String& name, const ModelNode* node, int32& level )
    {
        if ( node->name == name )
            return true;
        for ( auto& child : node->children )
        {
            bool find = FindNodeLevel( name, child, ++level );
            if ( find )
            {
                return true;
            }
        }
        level--;
        return false;
    }

    bool ModelImporter::RecognizeFileExtensionAndLoad( ModelResult& info, const Options& options )
    {
        const U8String extension = options.file.GetExtension();
        if ( Text::CompareIgnoreCase( extension, U8String( "OBJ" ) ) )
        {
            return OBJImporter::LoadModel( info, options );
        }
        return false;
    }

    bool ModelImporter::Initialize()
    {
        if ( std::thread::hardware_concurrency() <= 1 )
        {
            EE_LOG_WARN( "The aviable cores ({:d}) are insuficient for asyncronus loaders", std::thread::hardware_concurrency() );
            return false;
        }

        return true;
    }

    void ModelImporter::UpdateStatus()
    {
        if ( !pendingTasks.empty() && currentFutureTask.valid() && !TaskRunning )
        {
            currentFutureTask.get();
            pendingTasks.front()->finishTaskFunction( pendingTasks.front()->info );
            delete pendingTasks.front();
            pendingTasks.pop();
        }
        if ( !pendingTasks.empty() && !currentFutureTask.valid() && !TaskRunning )
        {
            pendingTasks.front()->futureTask( pendingTasks.front()->info, pendingTasks.front()->options, currentFutureTask );
        }
    }

    void ModelImporter::FinishAsyncTasks()
    {
        do
        {
            FinishCurrentAsyncTask();
            UpdateStatus();
        } while ( !pendingTasks.empty() );
    }

    void ModelImporter::FinishCurrentAsyncTask()
    {
        if ( !pendingTasks.empty() && currentFutureTask.valid() )
        {
            currentFutureTask.get();
            pendingTasks.front()->finishTaskFunction( pendingTasks.front()->info );
            delete pendingTasks.front();
            pendingTasks.pop();
        }
    }

    size_t ModelImporter::GetAsyncTaskCount()
    {
        return pendingTasks.size();
    }

    void ModelImporter::Exit()
    {
        if ( currentFutureTask.valid() )
            currentFutureTask.get();
    }

    bool ModelImporter::Load( ModelResult& info, const Options& options )
    {
        if ( options.file.IsValid() == false ) return false;

        if ( TaskRunning )
        {
            FinishCurrentAsyncTask();
        }

        TaskRunning = true;
        EE_LOG_INFO( "Reading File Model '{}'", options.file.GetShortPath() );
        RecognizeFileExtensionAndLoad( info, options );
        TaskRunning = false;
        return info.isValid;
    }

    void ModelImporter::LoadAsync( const Options& options, FinishTaskFunction then )
    {
        if ( options.file.IsValid() == false ) return;

        pendingTasks.push(
            new Task { options, then, []( ModelResult& data, const Options& options, std::future<bool>& futureTask )
            {
                futureTask = std::async( std::launch::async, Load, std::ref( data ),  std::ref( options ) );
            } }
        );
    }

    ModelImporter::ModelResult::ModelResult()
        : meshes(), parentNode( "ParentNode" ), isValid( false ), hasAnimations( false )
    {
    }

    void ModelImporter::ModelResult::Transfer( ModelResult& other )
    {
        meshes.clear();
        parentNode = other.parentNode;
        meshes.swap( other.meshes );
        isValid = other.isValid;
        hasAnimations = other.hasAnimations;
        other.isValid = false;
    }

    ModelImporter::Task::Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask ) :
        info(), options( options ), finishTaskFunction( finishTaskFunction ), futureTask( futureTask )
    {
    }
}