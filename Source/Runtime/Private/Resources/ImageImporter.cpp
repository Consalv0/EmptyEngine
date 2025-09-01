
#include <CoreMinimal.h>

#include <Resources/ImageImporter.h>
#include "Resources/PNGImporter.h"

#include "Utils/TextFormatting.h"

namespace EE
{
    bool ImageImporter::sTaskRunning;
    std::queue<ImageImporter::Task*> ImageImporter::sPendingTasks = std::queue<Task*>();
    std::future<bool> ImageImporter::sCurrentFutureTask;
    std::mutex sImageImporterQueueLock;

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
        if ( !sTaskRunning )
        {
            FinishCurrentAsyncTask();
        }
        if ( !sPendingTasks.empty() && !sCurrentFutureTask.valid() && !sTaskRunning )
        {
            sPendingTasks.front()->Start( sCurrentFutureTask );
        }
    }

    void ImageImporter::FinishAsyncTasks()
    {
        do
        {
            FinishCurrentAsyncTask();
            UpdateStatus();
        } while ( !sPendingTasks.empty() );
    }

    void ImageImporter::FinishCurrentAsyncTask()
    {
        if ( !sPendingTasks.empty() && sCurrentFutureTask.valid() )
        {
            sCurrentFutureTask.get();
            sPendingTasks.front()->Finish();
            delete sPendingTasks.front();
            sPendingTasks.pop();
        }
    }

    size_t ImageImporter::GetAsyncTaskCount()
    {
        return sPendingTasks.size();
    }

    void ImageImporter::Exit()
    {
        if ( sCurrentFutureTask.valid() )
            sCurrentFutureTask.get();
    }

    bool ImageImporter::Load( ImageResult& info, const Options& options )
    {
        if ( options.file.IsValid() == false ) return false;

        if ( sTaskRunning )
        {
            FinishCurrentAsyncTask();
        }

        sTaskRunning = true;
        EE_LOG_INFO( "Reading File Image '{}'", options.file.GetShortPath() );
        RecognizeFileExtensionAndLoad( info, options );
        sTaskRunning = false;
        return info.IsValid();
    }

    void ImageImporter::LoadAsync( const Options& options, FinishTaskFunction then )
    {
        if ( options.file.IsValid() == false ) return;

        sPendingTasks.push(
            new Task{ options, then, []( ImageResult& data, const Options& options, std::future<bool>& futureTask )
            {
                futureTask = std::async( std::launch::async, Load, std::ref( data ),  std::ref( options ) );
            } }
        );
    }

    ImageImporter::ImageResult::ImageResult()
        : _pixelMap(), _isValid( false )
    {
    }

    ImageImporter::ImageResult::~ImageResult()
    {
        Clear();
    }

    void ImageImporter::ImageResult::Transfer( ImageResult& other )
    {
        _pixelMap.Clear();
        _pixelMap.Swap( other._pixelMap );
        _isValid = other._isValid;
        other._isValid = false;
    }

    void ImageImporter::ImageResult::Clear()
    {
        _pixelMap.Clear();
        _isValid = false;
    }


    void ImageImporter::ImageResult::Populate( const UIntVector3& extents, EPixelFormat format, const void* data )
    {
        _pixelMap.SetData( extents.x, extents.y, extents.z, format, data );
        _isValid = true;
    }

    ImageImporter::Task::Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask ) :
        _result(), _options( options ), _finishTaskFunction( finishTaskFunction ), _futureTask( futureTask )
    {
    }

    void ImageImporter::Task::Start( std::future<bool>& task )
    {
        _futureTask( _result, _options, task );
    }

    void ImageImporter::Task::Finish()
    {
        _finishTaskFunction( _result );
    }
}