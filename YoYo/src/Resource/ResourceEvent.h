#pragma once

#include "Events/Event.h" 
#include "Renderer/Mesh.h"

namespace yoyo
{
    template<typename MeshType>
    class MeshCreatedEvent : public Event
    {
    public:
        EVENT_TYPE(MeshCreatedEvent)
        MeshCreatedEvent(Ref<MeshType> new_mesh)
		    :mesh(new_mesh){}

        virtual ~MeshCreatedEvent() = default;
        Ref<MeshType> mesh;
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

    class SkeletalHierarchy;
    class SkeletalHierarchyCreatedEvent : public Event
    {
    public:
        EVENT_TYPE(SkeletalHierarchyCreatedEvent)
        SkeletalHierarchyCreatedEvent(Ref<SkeletalHierarchy> new_skeletal_hierarchy)
		    :skeletal_hierarchy(new_skeletal_hierarchy){}
        virtual ~SkeletalHierarchyCreatedEvent() = default;

        Ref<SkeletalHierarchy> skeletal_hierarchy;
    };

    class Animation;
    class AnimationCreatedEvent : public Event
    {
    public:
        EVENT_TYPE(AnimationCreatedEvent)
        AnimationCreatedEvent(Ref<Animation> new_animation)
		    :animation(new_animation){}
        virtual ~AnimationCreatedEvent() = default;

        Ref<Animation> animation;
    };
}