#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"

#include "Texture.h"
#include "Shader.h"

namespace yoyo
{
    class YAPI Material
    {
    public:
        Material() = default;
        virtual ~Material() {};

        Ref<Shader> shader; // The shader used by the material.

        Vec3 color;     // The main color of the Material.
        bool instanced; // Sets whether material uses instancing.

        Ref<Texture> main_texture;          // Main texture of material (textures index 0).
        std::vector<Ref<Texture>> textures; // All textures of material.

        virtual void Bind(void* render_context, MeshPassType type) = 0;
        virtual void Unbind() {};

        static Ref<Material> Create();

        // TODO: Per Shader shader pass data
    protected:
        uint64_t m_renderpass; // The id of this material's render pass
        uint64_t m_id; // Id of this resource
    };
}
