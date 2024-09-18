#pragma once

#include <future>

#include "Rendering/PixelMap.h"
#include "Files/FileManager.h"

namespace EE
{
    class ImageImporter
    {
    public:
        struct Options
        {
            const File& file;
            EPixelFormat format;
        };

        struct ImageResult
        {
            PixelMap pixelMap;

            //* The image has been succesfully loaded
            bool isValid;

            void Transfer( ImageResult& other );

            ImageResult();

            ImageResult( const ImageResult& other ) = delete;
        };

    private:
        typedef std::function<void( ImageResult& )> FinishTaskFunction;
        typedef std::function<void( ImageResult&, const Options&, std::future<bool>& )> FutureTask;

        static bool TaskRunning;

        struct Task
        {
            Options options;
            ImageResult result;
            FinishTaskFunction finishTaskFunction;
            FutureTask futureTask;

            Task( const Task& Other ) = delete;
            Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask );
        };

        static bool RecognizeFileExtensionAndLoad( ImageResult& result, const Options& options );

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

        static bool Load( ImageResult& result, const Options& options );

        static void LoadAsync( const Options& options, FinishTaskFunction onComplete );

    };
}