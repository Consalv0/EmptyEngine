
#include <CoreMinimal.h>

#include "Files/FileMap.h"

#include <Core/Name.h>
#include "Engine/Ticker.h"
#include "Utils/TextFormatting.h"
#include "Math/CoreMath.h"

#include "Resources/PNGImporter.h"

#ifdef EE_PLATFORM_WINDOWS
#define STBI_WINDOWS_UTF8
#endif // EE_PLATFORM_WINDOWS

#define STBI_ONLY_PNG
#define STBI_
#define STBI_ASSERT(x) EE_ASSERT( x )
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr size_t kBufferBlockSize = 1u << 16u;

bool EE::PNGImporter::LoadImage( ImageImporter::ImageResult& result, const ImageImporter::Options& options )
{
	result.Clear();

	if ( GPixelFormatInfo[ options.format ].supported == false )
	{
		EE_LOG_ERROR( "Error importing image, format not supported '{}'", (int32)options.format );
		return false;
	}

    Timestamp timer;

    timer.Begin();
    FileMap file( options.file );
    if ( file.GetError() != 0 )
    {
        EE_LOG_ERROR( "Error reading file '{}', returned code {}", options.file.GetPath(), file.GetError() );
        return false;
    }

    const U8String filename = options.file.GetPath();

	int32 width, height, comp;

	uint64 bytesRead = 0;
	uint64 blockSize = ((file.GetSize() / kBufferBlockSize) + 1) * kBufferBlockSize;
	void* fileData = malloc( blockSize );

	if ( file.ReadBlock( 0, blockSize, fileData ) != 0 )
	{
		EE_LOG_ERROR( "Error reading block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
		return false;
	}

	if ( file.WaitForResult( &bytesRead ) != 0 )
	{
		EE_LOG_ERROR( "Error reading result of block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
		return false;
	}

	void* data = NULL;
	if ( GPixelFormatInfo[ options.format ].size <= 1 )
		data = stbi_load_from_memory( static_cast<const stbi_uc*>( fileData ), (int)bytesRead, &width, &height, &comp, GPixelFormatInfo[options.format].channels );
	else
		data = stbi_loadf_from_memory( static_cast<const stbi_uc*>( fileData ), (int)bytesRead, &width, &height, &comp, GPixelFormatInfo[options.format].channels );

	free( fileData );
	if ( data == NULL )
	{
		EE_LOG_ERROR( "Image '{}' couldn't be loaded: {}", options.file.GetFileName().c_str(), stbi_failure_reason() );
		return false;
	}

	result.Populate( { (uint32)width, (uint32)height, 1u }, options.format, data );
	stbi_image_free( data );
	timer.Stop();

    return true;
}
