#pragma once

#include "Resources/MeshParser.h"

namespace EE
{
    class OBJImporter
    {
    public:
        /** Load mesh data from file extension Wavefront, it will return the models separated by objects, optionaly
          * there's a way to optimize the vertices. */
        static bool LoadModel( ModelParser::ModelDataInfo& info, const ModelParser::ParsingOptions& options );
    };
}