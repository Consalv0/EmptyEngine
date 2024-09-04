
#include "CoreMinimal.h"

#include "Utils/Hasher.h"
#include "Rendering/Mesh.h"
#include "Files/FileManager.h"
#include "Resources/MeshParser.h"
#include "Resources/OBJImporter.h"

#include "Utils/TextFormatting.h"

#include <future>
#include <thread>

namespace EE
{
    bool ModelParser::TaskRunning;
    std::queue<ModelParser::Task*> ModelParser::pendingTasks = std::queue<Task*>();
    std::future<bool> ModelParser::currentFutureTask;
    std::mutex ModelParserQueueLock;

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

    bool FindNodeLevel( const NString& name, const ModelNode* node, int32& level )
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

    bool ModelParser::RecognizeFileExtensionAndLoad( ModelDataInfo& info, const ParsingOptions& options )
    {
        const WString extension = options.file.GetExtension();
        if ( Text::CompareIgnoreCase( extension, WString( L"OBJ" ) ) )
        {
            return OBJImporter::LoadModel( info, options );
        }
        return false;
    }

    bool ModelParser::Initialize()
    {
        if ( std::thread::hardware_concurrency() <= 1 )
        {
            EE_LOG_WARN( L"The aviable cores ({:d}) are insuficient for asyncronus loaders", std::thread::hardware_concurrency() );
            return false;
        }

        return true;
    }

    void ModelParser::UpdateStatus()
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

    void ModelParser::FinishAsyncTasks()
    {
        do
        {
            FinishCurrentAsyncTask();
            UpdateStatus();
        } while ( !pendingTasks.empty() );
    }

    void ModelParser::FinishCurrentAsyncTask()
    {
        if ( !pendingTasks.empty() && currentFutureTask.valid() )
        {
            currentFutureTask.get();
            pendingTasks.front()->finishTaskFunction( pendingTasks.front()->info );
            delete pendingTasks.front();
            pendingTasks.pop();
        }
    }

    size_t ModelParser::GetAsyncTaskCount()
    {
        return pendingTasks.size();
    }

    void ModelParser::Exit()
    {
        if ( currentFutureTask.valid() )
            currentFutureTask.get();
    }

    bool ModelParser::Load( ModelDataInfo& info, const ParsingOptions& options )
    {
        if ( options.file.IsValid() == false ) return false;

        if ( TaskRunning )
        {
            FinishCurrentAsyncTask();
        }

        TaskRunning = true;
        EE_LOG_INFO( L"Reading File Model '{}'", options.file.GetShortPath() );
        RecognizeFileExtensionAndLoad( info, options );
        TaskRunning = false;
        return info.isValid;
    }

    void ModelParser::LoadAsync( const ParsingOptions& options, FinishTaskFunction then )
    {
        if ( options.file.IsValid() == false ) return;

        pendingTasks.push(
            new Task { options, then, []( ModelDataInfo& data, const ParsingOptions& options, std::future<bool>& futureTask )
            {
                futureTask = std::async( std::launch::async, Load, std::ref( data ),  std::ref( options ) );
            } }
        );
    }

    ModelParser::ModelDataInfo::ModelDataInfo()
        : meshes(), parentNode( "ParentNode" ), isValid( false ), hasAnimations( false )
    {
    }

    void ModelParser::ModelDataInfo::Transfer( ModelDataInfo& other )
    {
        meshes.clear();
        parentNode = other.parentNode;
        meshes.swap( other.meshes );
        isValid = other.isValid;
        hasAnimations = other.hasAnimations;
        other.isValid = false;
    }

    ModelParser::Task::Task( const ParsingOptions& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask ) :
        info(), options( options ), finishTaskFunction( finishTaskFunction ), futureTask( futureTask )
    {
    }
}