
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
#define STBI_ASSERT(x) EE_ASSERT( x )
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr size_t kBufferBlockSize = 1u << 18u;

bool EE::PNGImporter::LoadImage( ImageImporter::ImageResult& result, const ImageImporter::Options& options )
{
	result.Clear();
    Timestamp timer;

    timer.Begin();
    FileMap file( options.file );
    if ( file.GetError() != 0 )
    {
        EE_LOG_ERROR( L"Error reading file '{}', returned code {}", options.file.GetPath(), file.GetError() );
        return false;
    }

    const NString filename = Text::WideToNarrow( options.file.GetPath() );

	int32 width, height, comp;

	uint64 bytesRead = 0;
	void* fileData = malloc( kBufferBlockSize );

	if ( file.ReadBlock( 0, kBufferBlockSize, fileData ) != 0 )
	{
		EE_LOG_ERROR( L"Error reading block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
		return false;
	}

	if ( file.WaitForResult( &bytesRead ) != 0 )
	{
		EE_LOG_ERROR( L"Error reading result of block of file '{}', returned code {}", options.file.GetPath(), file.GetError() );
		return false;
	}

	void* data = NULL;
	if ( GPixelFormatInfo[ options.format ].format )
		data = stbi_loadf_from_memory( static_cast<const stbi_uc*>( fileData ), (int)bytesRead, &width, &height, &comp, GPixelFormatInfo[options.format].channels );
	else
		data = stbi_load_from_memory( static_cast<const stbi_uc*>(fileData), (int)bytesRead, &width, &height, &comp, GPixelFormatInfo[options.format].channels );

	free( fileData );
	if ( data == NULL )
	{
		EE_LOG_ERROR( L"Image '{0}' couldn't be loaded", options.file.GetFileName().c_str() );
		return false;
	}

	result.Populate( { (uint32)width, (uint32)height, 1u }, options.format, data );
	stbi_image_free( data );
	timer.Stop();

    return true;
}
