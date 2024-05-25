#pragma once

namespace EE
{
	class PixelMap
	{
	public:
		PixelMap();

		PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat );

		PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& data );

		PixelMap( const PixelMap& other );

		void SetData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& data );

		//* Width in pixels. 
		inline uint32 GetWidth() const { return width; };

		//* Height in pixels.
		inline uint32 GetHeight() const { return height; };

		//* Depth in pixels.
		inline uint32 GetDepth() const { return depth; };

		inline IntVector3 GetSize() const { return { (int32)width, (int32)height, (int32)depth }; }

		inline bool IsEmpty() const { return data == NULL; };

		size_t GetMemorySize() const;

		inline EPixelFormat GetColorFormat() const { return pixelFormat; };

		PixelMap& operator=( const PixelMap& other );

        constexpr const void* PointerToValue() const { return data; }

		~PixelMap();

	private:
		friend class PixelMapUtility;

		void* data;
		EPixelFormat pixelFormat;
		uint32 width, height, depth;
	};

	class PixelMapUtility
	{
	public:
		static void CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& data );

		static void CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& target, void* data );

		//* Flips the pixels vertically
		static void FlipVertically( PixelMap& map );

		static unsigned char* GetCharPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z );

		static float* GetFloatPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z );

		static unsigned char* GetCharPixelAt( PixelMap& map, const size_t& index );

		static float* GetFloatPixelAt( PixelMap& map, const size_t& index );

		static void PerPixelOperator( PixelMap& map, std::function<void( float*, const unsigned char& channels )> const& function );

		static void PerPixelOperator( PixelMap& map, std::function<void( unsigned char*, const unsigned char& channels )> const& function );

		static bool FormatIsFloat( EPixelFormat format );

		static bool FormatIsShort( EPixelFormat format );

	private:
		template<typename T>
		static void _FlipVertically( const uint32& width, const uint32& height, const uint32& depth, void*& data );
	};

	template<typename T>
	inline void PixelMapUtility::_FlipVertically( const uint32& width, const uint32& height, const uint32& depth, void*& inData )
	{
		T* data = (T*)inData;
		T* tempRow = (T*)malloc( width * sizeof( T ) );
		for ( uint32 depthIndex = 0; depthIndex < depth; depthIndex++ )
		{
			uint32 depthOffset = width * height * depthIndex;
			// Iterate only half the buffer to get a full flip
			uint32 rows = height / 2;

			for ( uint32 rowIndex = 0; rowIndex < rows; rowIndex++ )
			{
				memcpy( tempRow, data + depthOffset + rowIndex * width, width * sizeof( T ) );
				memcpy( data + depthOffset + rowIndex * width, data + depthOffset + (height - rowIndex - 1) * width, width * sizeof( T ) );
				memcpy( data + depthOffset + (height - rowIndex - 1) * width, tempRow, width * sizeof( T ) );
			}
		}
		free( tempRow );
	}

}