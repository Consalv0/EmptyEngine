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

        class ImageResult
        {
        public:
            ImageResult();

            ImageResult( const ImageResult& other ) = delete;

            ~ImageResult();

            void Transfer( ImageResult& other );

            void Clear();

            void Populate( const UIntVector3& extents, EPixelFormat format, const void* data );

            constexpr const PixelMap& GetPixelMap() const { return _pixelMap; }

            constexpr const bool& IsValid() const { return _isValid; }

        private:
            PixelMap _pixelMap;
            //* The image has been succesfully loaded
            bool _isValid;
        };

    private:
        typedef std::function<void( ImageResult& )> FinishTaskFunction;
        typedef std::function<void( ImageResult&, const Options&, std::future<bool>& )> FutureTask;

        static bool sTaskRunning;

        class Task
        {
        public:
            Task( const Task& other ) = delete;
            Task( const Options& options, FinishTaskFunction finishTaskFunction, FutureTask futureTask );

        public:
            void Start( std::future<bool>& task );
            void Finish();

        private:
            Options _options;
            ImageResult _result;
            FinishTaskFunction _finishTaskFunction;
            FutureTask _futureTask;
        };

        static bool RecognizeFileExtensionAndLoad( ImageResult& result, const Options& options );

        static void FinishCurrentAsyncTask();

        //* Mesh Loading Threads
        static std::queue<Task*> sPendingTasks;
        static std::future<bool> sCurrentFutureTask;

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