#pragma once

#include "Defines.h"

#include "Core/Memory.h"
#include "Math/Math.h"

#include "Resource/Resource.h"
#include "Mesh.h"

namespace yoyo
{
   class YAPI Model
    {
    public:
        Model() = default;
        ~Model() = default;

        static Ref<Model> LoadFromAsset(const char* asset_path);

        static Ref<Model> Create(const std::string& name = "");
        const ResourceId& ID() const { return m_id; }

        std::vector<Ref<Mesh>> meshes;
        std::vector<Mat4x4> model_matrices;
    protected:
        ResourceId m_id;
    };

}