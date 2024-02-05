#pragma once


#include "Defines.h"
#include "Core/Memory.h"

namespace yoyo
{
    // Components for working with meshes
    class Mesh;
    class Material;
    struct YAPI MeshRenderer
    {
    public:
        MeshRenderer();
        ~MeshRenderer();

        Ref<Mesh> mesh;
        Ref<Material> material;
    };
}