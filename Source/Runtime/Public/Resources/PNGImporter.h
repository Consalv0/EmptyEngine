#pragma once

#include "Resources/ImageImporter.h"

namespace EE
{
    class PNGImporter
    {
    public:
        /** Load mesh data from file extension Wavefront, it will return the models separated by objects, optionaly
          * there's a way to optimize the vertices. */
        static bool LoadImage( ImageImporter::ImageResult& result, const ImageImporter::Options& options );
    };
}