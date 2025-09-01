
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
    bool ModelImporter::sTaskRunning;
    std::queue<ModelImporter::Task*> ModelImporter::sPendingTasks = std::queue<Task*>();
    std::future<bool> ModelImporter::sCurrentFutureTask;
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
        if ( !sTaskRunning )
        {
            FinishCurrentAsyncTask();
        }
        if ( !sPendingTasks.empty() && !sCurrentFutureTask.valid() && !sTaskRunning )
        {
            sPendingTasks.front()->Start( sCurrentFutureTask );
        }
    }

    void ModelImporter::FinishAsyncTasks()
    {
        do
        {
            FinishCurrentAsyncTask();
            UpdateStatus();
        } while ( !sPendingTasks.empty() );
    }

    void ModelImporter::FinishCurrentAsyncTask()
    {
        if ( !sPendingTasks.empty() && sCurrentFutureTask.valid() )
        {
            sCurrentFutureTask.get();
            sPendingTasks.front()->Finish();
            delete sPendingTasks.front();
            sPendingTasks.pop();
        }
    }

    size_t ModelImporter::GetAsyncTaskCount()
    {
        return sPendingTasks.size();
    }

    void ModelImporter::Exit()
    {
        if ( sCurrentFutureTask.valid() )
            sCurrentFutureTask.get();
    }

    bool ModelImporter::Load( ModelResult& info, const Options& options )
    {
        if ( options.file.IsValid() == false ) return false;

        if ( sTaskRunning )
        {
            FinishCurrentAsyncTask();
        }

        sTaskRunning = true;
        EE_LOG_INFO( "Reading File Model '{}'", options.file.GetShortPath() );
        RecognizeFileExtensionAndLoad( info, options );
        sTaskRunning = false;
        return info.isValid;
    }

    void ModelImporter::LoadAsync( const Options& options, FinishTaskFunction then )
    {
        if ( options.file.IsValid() == false ) return;

        sPendingTasks.push(
            new Task( options, then, []( ModelResult& data, const Options& options, std::future<bool>& futureTask )
            {
                futureTask = std::async( std::launch::async, Load, std::ref( data ),  std::ref( options ) );
            })
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
        _result(), _options( options ), _finishTaskFunction( finishTaskFunction ), _futureTask( futureTask )
    {
    }

    void ModelImporter::Task::Start( std::future<bool>& task )
    {
        _futureTask( _result, _options, task );
    }

    void ModelImporter::Task::Finish()
    {
        _finishTaskFunction( _result );
    }
}