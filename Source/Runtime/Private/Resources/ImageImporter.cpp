
#include <CoreMinimal.h>

#include <Resources/ImageImporter.h>
#include "Resources/PNGImporter.h"

#include "Utils/TextFormatting.h"

namespace EE
{
    bool ImageImporter::TaskRunning;
    std::queue<ImageImporter::Task*> ImageImporter::pendingTasks = std::queue<Task*>();
    std::future<bool> ImageImporter::currentFutureTask;
    std::mutex ImageImporterQueueLock;

    bool ImageImporter::RecognizeFileExtensionAndLoad( ImageResult& info, const Options& options )
    {
        const U8String extension = options.file.GetExtension();
        if ( Text::CompareIgnoreCase( extension, U8String( "PNG" ) ) )
        {
            return PNGImporter::LoadImage( info, options );
        }
        return false;
    }

    bool ImageImporter::Initialize()
    {
        if ( std::thread::hardware_concurrency() <= 1 )
        {
            EE_LOG_WARN( "The aviable cores ({:d}) are insuficient for asyncronus loaders", std::thread::hardware_concurrency() );
            return false;
        }

        return true;
    }

    void ImageImporter::UpdateStatus()
    {
        if ( !pendingTasks.empty() && currentFutureTask.valid() && !TaskRunning )
        {
            currentFutureTask.get();
            pendingTasks.front()->finishTaskFunction( pendingTasks.front()->result );
            delete pendingTasks.front();
            pendingTasks.pop();
        }
        if ( !pendingTasks.empty() && !currentFutureTask.valid() && !TaskRunning )
        {
            pendingTasks.front()->futureTask( pendingTasks.front()->result, pendingTasks.front()->options, currentFutureTask );
        }
    }

    void ImageImporter::FinishAsyncTasks()
    {
        do
        {
            FinishCurrentAsyncTask();
            UpdateStatus();
        } while ( !pendingTasks.empty() );
    }

    void ImageImporter::FinishCurrentAsyncTask()
    {
        if ( !pendingTasks.empty() && currentFutureTask.valid() )
        {
            currentFutureTask.get();
            pendingTasks.front()->finishTaskFunction( pendingTasks.front()->result );
            delete pendingTasks.front();
            pendingTasks.pop();
        }
    }

    size_t ImageImporter::GetAsyncTaskCount()
    {
        return pendingTasks.size();
    }

    void ImageImporter::Exit()
    {
        if ( currentFutureTask.valid() )
            currentFutureTask.get();
    }

    bool ImageImporter::Load( ImageResult& info, const Options& options )
    {
        if ( options.file.IsValid() == false ) return false;

        if ( TaskRunning )
        {
            FinishCurrentAsyncTask();
        }

        TaskRunning = true;
        EE_LOG_INFO( "Reading File Image '{}'", options.file.GetShortPath() );
        RecognizeFileExtensionAndLoad( info, options );
        TaskRunning = false;
        return info.IsValid();
    }

    void ImageImporter::LoadAsync( const Options& options, FinishTaskFunction then )
    {
        if ( options.file.IsValid() == false ) return;

        pendingTasks.push(
            new Task{ options, then, []( ImageResult& data, const Options& options, std::future<bool>& futureTask )
            {
                futureTask = std::async( std::launch::async, Load, std::ref( data ),  std::ref( options ) );
            } }
        );
    }

    ImageImporter::ImageResult::ImageResult()
        : pixelMap_(), isValid_( false )
    {
    }

    ImageImporter::ImageResult::~ImageResult()
    {
        Clear();
    }

    void ImageImporter::ImageResult::Transfer( ImageResult& other )
    {
        pixelMap_.Clear();
        pixelMap_.Swap( other.pixelMap_ );
        isValid_ = other.isValid_;
        other.isValid_ = false;
    }

    void ImageImporter::ImageResult::Clear()
    {
        pixelMap_.Clear();
        isValid_ = false;
    }


    void ImageImporter::ImageResult::Populate( const UIntVector3& extents, EPixelFormat format, const void* data )
    {
        pixelMap_.SetData( extents.x, extents.y, extents.z, format, data );
        isValid_ = true;
    }

    ImageImporter::Task::Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask ) :
        result(), options( options ), finishTaskFunction( finishTaskFunction ), futureTask( futureTask )
    {
    }
}