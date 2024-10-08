#pragma once

#include "Resources/ModelImporter.h"

namespace EE
{
    class OBJImporter
    {
    public:
        /** Load mesh data from file extension Wavefront, it will return the models separated by objects, optionaly
          * there's a way to optimize the vertices. */
        static bool LoadModel( ModelImporter::ModelResult& result, const ModelImporter::Options& options );
    };
}