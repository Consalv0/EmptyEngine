#pragma once

namespace EE
{
	class PixelMap
	{
	public:
		PixelMap();

		PixelMap( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat );

		PixelMap( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& data );

		PixelMap( const PixelMap& Other );

		void SetData( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& InData );

		//* Width in pixels. 
		inline uint32 GetWidth() const { return Width; };

		//* Height in pixels.
		inline uint32 GetHeight() const { return Height; };

		//* Depth in pixels.
		inline uint32 GetDepth() const { return Depth; };

		inline IntVector3 GetSize() const { return { (int32)Width, (int32)Height, (int32)Depth }; }

		inline bool IsEmpty() const { return data == NULL; };

		size_t GetMemorySize() const;

		inline EPixelFormat GetColorFormat() const { return pixelFormat; };

		PixelMap& operator=( const PixelMap& Other );

        constexpr const void* PointerToValue() const { return data; }

		~PixelMap();

	private:
		friend class PixelMapUtility;

		void* data;
		EPixelFormat pixelFormat;
		uint32 Width, Height, Depth;
	};

	class PixelMapUtility
	{
	public:
		static void CreateData( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& data );

		static void CreateData( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& Target, void* data );

		//* Flips the pixels vertically
		static void FlipVertically( PixelMap& Map );

		static unsigned char* GetCharPixelAt( PixelMap& Map, const uint32& X, const uint32& Y, const uint32& Z );

		static float* GetFloatPixelAt( PixelMap& Map, const uint32& X, const uint32& Y, const uint32& Z );

		static unsigned char* GetCharPixelAt( PixelMap& Map, const size_t& Index );

		static float* GetFloatPixelAt( PixelMap& Map, const size_t& Index );

		static void PerPixelOperator( PixelMap& Map, std::function<void( float*, const unsigned char& Channels )> const& Function );

		static void PerPixelOperator( PixelMap& Map, std::function<void( unsigned char*, const unsigned char& Channels )> const& Function );

		static bool FormatIsFloat( EPixelFormat format );

		static bool FormatIsShort( EPixelFormat format );

	private:
		template<typename T>
		static void _FlipVertically( uint32 Width, uint32 Height, uint32 Depth, void*& data );
	};

	template<typename T>
	inline void PixelMapUtility::_FlipVertically( uint32 Width, uint32 Height, uint32 Depth, void*& InData )
	{
		T* data = (T*)InData;
		T* TempRow = (T*)malloc( Width * sizeof( T ) );
		for ( uint32 DepthIndex = 0; DepthIndex < Depth; DepthIndex++ )
		{
			uint32 DepthOffset = Width * Height * DepthIndex;
			// Iterate only half the buffer to get a full flip
			uint32 Rows = Height / 2;

			for ( uint32 RowIndex = 0; RowIndex < Rows; RowIndex++ )
			{
				memcpy( TempRow, data + DepthOffset + RowIndex * Width, Width * sizeof( T ) );
				memcpy( data + DepthOffset + RowIndex * Width, data + DepthOffset + (Height - RowIndex - 1) * Width, Width * sizeof( T ) );
				memcpy( data + DepthOffset + (Height - RowIndex - 1) * Width, TempRow, Width * sizeof( T ) );
			}
		}
		free( TempRow );
	}

}