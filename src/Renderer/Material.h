#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"

#include "Resource/Resource.h"
#include "RenderTypes.h"

namespace yoyo
{
    enum class MaterialTextureType : uint8_t
    {
        MainTexture = 0,
        SpecularMap,
        NormalMap,

        Ambient,
        Displacement,
        Height,

        Max
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
        Property = 1,
        Texture = 1 << 1,
        RenderMode = 1 << 2,
    };

    enum class MaterialRenderMode
    {
        Uknown = 0,
        Opaque,
        Transparent,
    };

    inline MaterialDirtyFlags operator~ (MaterialDirtyFlags a) { return (MaterialDirtyFlags)~(int)a; }
    inline MaterialDirtyFlags operator| (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a | (int)b); }
    inline MaterialDirtyFlags operator& (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a & (int)b); }
    inline MaterialDirtyFlags operator^ (MaterialDirtyFlags a, MaterialDirtyFlags b) { return (MaterialDirtyFlags)((int)a ^ (int)b); }
    inline MaterialDirtyFlags& operator|= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a |= (int)b); }
    inline MaterialDirtyFlags& operator&= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a &= (int)b); }
    inline MaterialDirtyFlags& operator^= (MaterialDirtyFlags& a, MaterialDirtyFlags b) { return (MaterialDirtyFlags&)((int&)a ^= (int)b); }

    class Texture;
    class Shader;
    class YAPI Material : public Resource
    {
    public:
        RESOURCE_TYPE(Material)
        Material();
        virtual ~Material();

        Ref<Shader> shader = nullptr; // The shader used by the material.

        const Vec4& GetColor() const {return m_color;}
        void SetColor(const Vec4& color) {m_color = color;}

        const MaterialRenderMode& GetRenderMode() const {return m_render_mode;}
        void SetRenderMode(MaterialRenderMode render_mode) {m_render_mode = render_mode;}

        const bool IsCastingShadows() const {return m_cast_shadows;}
        const bool IsReceivingShadows() const {return m_receive_shadows;}
        const bool IsInstanced() const {return m_instanced;}

        void ToggleInstanced(bool instanced) { m_instanced = instanced;}
        void ToggleCastShadows(bool cast_shadows) {m_cast_shadows = cast_shadows;}
        void ToggleReceiveShadows(bool receive_shadows) {m_receive_shadows = receive_shadows;}

        const Ref<Texture> MainTexture() const { return textures.empty() ? nullptr : textures.front(); }// Main texture of material (textures index 0).
        const Ref<Texture> GetTexture(int index) const;

        void SetTexture(int index, Ref<Texture> texture);
        void SetTexture(MaterialTextureType type, Ref<Texture> texture);

        void SetFloat(const std::string& name, float value);
        void SetVec4(const std::string& name, const Vec4& value);

        virtual void Bind(void* render_context, MeshPassType mesh_pass_type) = 0;
        virtual void Unbind() {};

        static Ref<Material> Create(Ref<Shader> shader, const std::string& name = "");

        // Create material from existing material
        static Ref<Material> Create(Ref<Material> base, const std::string& name);
        const bool Dirty() const { return m_dirty != MaterialDirtyFlags::Clean; }
    public:
        bool operator==(const Material& other) const
        {
            return (shader == other.shader && m_instanced == other.m_instanced && MainTexture() == other.MainTexture());
        };

        MaterialDirtyFlags& DirtyFlags() { return m_dirty; }

        void SetProperty(const std::string& name, const void* data);
        void GetProperty(const std::string& name, void* data) const;
        void AddProperty(const std::string& name, const MaterialProperty& property);

        const void* PropertyData() const { return m_property_data; } // Raw ptr to material property buffer
        const uint64_t PropertyDataSize() const { return m_property_size; } // Size of PropertyData in bytes

        const std::unordered_map<std::string, MaterialProperty>& GetProperties() const {return m_properties;}
    protected:
        // Properties
        uint64_t m_property_size;
        void* m_property_data;
        std::unordered_map<std::string, MaterialProperty> m_properties;

        // Textures
        std::vector<Ref<Texture>> textures;

        MaterialDirtyFlags m_dirty;
        uint64_t renderpass; // The id of this material's render pass
    private:
        Vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f}; // The main color of the Material.
        bool m_cast_shadows = true; // Sets whether material receives shadows
        bool m_receive_shadows = true; // Sets whether material receives shadows
        bool m_instanced = false; // Sets whether material uses instancing.
        MaterialRenderMode m_render_mode = MaterialRenderMode::Uknown; // Render mode of material
    };
}

template<>
struct std::hash<yoyo::Material>
{
    std::size_t operator()(const yoyo::Material& material) const noexcept
    {
        // Compute individual hash values for first, second and third
        // http://stackoverflow.com/a/1646913/126995

        std::size_t res = 17;
        res = res * 31 + hash<string>()(material.name);
        res = res * 31 + hash<bool>()(material.IsInstanced());

        //if(const auto& main_texture = material.MainTexture())
        //{
        //    res = res * 31 + hash<string>()(main_texture->name);
        //}

        return res;
    }
};
