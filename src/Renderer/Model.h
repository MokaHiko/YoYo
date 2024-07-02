#pragma once

#include "Defines.h"

#include "Core/Memory.h"
#include "Math/Math.h"

#include "Resource/Resource.h"

namespace yoyo
{
    class IMesh;
    class Material;
    class YAPI Model : public Resource
    {
    public:
        RESOURCE_TYPE(Model)
        Model() = default;
        ~Model() = default;

        static Ref<Model> LoadFromAsset(const char* asset_path, const std::string& name = "");
        static Ref<Model> Create(const std::string& name = "");

        std::vector<Ref<IMesh>> meshes;
        std::vector<Ref<Material>> materials;
        std::vector<uint32_t> material_indices;
        std::vector<Mat4x4> model_matrices;
    };

}