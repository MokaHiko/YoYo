#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"

#include "Texture.h"
#include "Shader.h"

#include "Resource/Resource.h"

namespace yoyo
{
    enum class MaterialTextureType : uint8_t
    {
        MainTexture,
        SpecularMap,
        NormalMap,
    };

    enum class MaterialPropertyType
    {
        Int32,
        Float32,
        Vec4,
        Texture,
    };

    struct MaterialProperty
    {
        uint64_t size;
        uint64_t offset;
        MaterialPropertyType type;
    };

    enum class MaterialDirtyFlags
    {
        Clean = 0,
        PropertyChange = 1,
        TextureChange = 1 << 1,
        RenderMode = 1 << 2,
    };

    inline MaterialDirtyFlags operator~ (MaterialDirtyFlags a) { return (MaterialDirtyFlags)~(int)a; }
    inline MaterialDirtyFlags operator| (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a | (int)b); }
    inline MaterialDirtyFlags operator& (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a & (int)b); }
    inline MaterialDirtyFlags operator^ (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a ^ (int)b); }
    inline MaterialDirtyFlags& operator|= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a |= (int)b); }
    inline MaterialDirtyFlags& operator&= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a &= (int)b); }
    inline MaterialDirtyFlags& operator^= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a ^= (int)b); }

    class YAPI Material
    {
    public:
        Ref<Shader> shader; // The shader used by the material.

        Vec4 color;     // The main color of the Material.
        bool receive_shadows;
        bool instanced; // Sets whether material uses instancing.

        const Ref<Texture> MainTexture() const { return textures.empty() ? nullptr : textures.front(); }// Main texture of material (textures index 0).
        const Ref<Texture> GetTexture(int index) const { return textures[index]; }

        void SetTexture(int index, Ref<Texture> texture);
        void SetTexture(MaterialTextureType type, Ref<Texture> texture);

        void SetFloat(const std::string& name, float value);
        void SetVec4(const std::string& name, const Vec4& value);

        virtual void Bind(void* render_context, MeshPassType mesh_pass_type) = 0;
        virtual void Unbind() {};

        static Ref<Material> Create(Ref<Shader> shader, const std::string& name = "");
        const ResourceId& ID() const { return m_id; }
        const bool Dirty() const { return m_dirty != MaterialDirtyFlags::Clean; }
    protected:
        Material();
        virtual ~Material();

        MaterialDirtyFlags& DirtyFlags() {return m_dirty;}

        void SetProperty(const std::string& name, void* data);
        void AddProperty(const std::string& name, const MaterialProperty& property);

        const void* PropertyData() const { return m_property_data; } // Raw ptr to material property buffer
        const uint64_t PropertyDataSize() const { return m_property_size; } // Size of PropertyData in bytes
    protected:
        // Properties
        uint64_t m_property_size;
        void* m_property_data;
        std::unordered_map<std::string, MaterialProperty> m_properties;

        // Textures
        std::vector<Ref<Texture>> textures; 

        MaterialDirtyFlags m_dirty; 
        uint64_t renderpass; // The id of this material's render pass
        ResourceId m_id; // Id of this resource
    };
}
