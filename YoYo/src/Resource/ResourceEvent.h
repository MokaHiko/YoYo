#pragma once

#include "Events/Event.h"

namespace yoyo
{
    class Mesh;
    class MeshCreatedEvent : public Event
    {
    public:
        MeshCreatedEvent(Ref<Mesh> new_mesh);
        virtual ~MeshCreatedEvent() = default;

        Ref<Mesh> mesh;

        EVENT_TYPE(MeshCreatedEvent)
    };

    class Texture;
    class TextureCreatedEvent : public Event
    {
    public:
        TextureCreatedEvent(Ref<Texture> new_texture);
        virtual ~TextureCreatedEvent() = default;

        Ref<Texture> texture;

        EVENT_TYPE(TextureCreatedEvent)
    };

    class Shader;
    class ShaderCreatedEvent : public Event
    {
    public:
        ShaderCreatedEvent(Ref<Shader> new_shader);
        virtual ~ShaderCreatedEvent() = default;

        Ref<Shader> shader;
        EVENT_TYPE(ShaderCreatedEvent)
    };

    class Material;
    class MaterialCreatedEvent : public Event
    {
    public:
        MaterialCreatedEvent(Ref<Material> new_material);
        virtual ~MaterialCreatedEvent() = default;

        Ref<Material> material;

        EVENT_TYPE(MaterialCreatedEvent)
    };
}