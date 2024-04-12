#pragma once

#include <Renderer/Particles/ParticleSystem.h>
#include <Math/Math.h>

#include "ECS/Components/RenderableComponents.h"
#include "ECS/System.h"

namespace yoyo
{
    class RendererLayer;
    class ParticleSystem;
}

struct ParticleSystemComponent
{
    ParticleSystemComponent();
    ~ParticleSystemComponent();

    const std::vector<yoyo::Particle>& GetParticles() const;
    std::vector<yoyo::Particle>& GetParticles();

    const uint32_t GetMaxParticles() const;

    void SetMaxParticles(uint32_t size);

    const yoyo::Vec3& GetGravityScale() const;
    void SetGravityScale(const yoyo::Vec3& gravity_scale);

    const float GetEmissionRate() const;
    void SetEmissionRate(float emission_rate);

    const yoyo::ParticleSystemType GetType() const;
    void SetType(yoyo::ParticleSystemType type);

    const yoyo::ParticleSystemSpace GetSimulationSpace() const;
    void SetSimulationSpace(yoyo::ParticleSystemSpace simulation_space);

    const std::vector<Ref<yoyo::Material>> GetMaterials() const {return m_materials;}
    
    // Adds a material to the particle system
    void AddMaterial(Ref<yoyo::Material> material);
private:
    friend class ParticleSystemManager;
    Ref<yoyo::ParticleSystem> m_particle_system;

    // Instance rendering
    std::vector<Ref<yoyo::MeshPassObject>> m_particle_renderable_objects = {};

    // Materials
    std::vector<Ref<yoyo::Material>> m_materials;
private:
        // Batch Rendering
        //Ref<MeshPassObject> particle_batch_mesh = nullptr;
};

// Updates transforms and relative transform components of the scene hierarchy
class ParticleSystemManager : public System<ParticleSystemComponent>
{
public:
    ParticleSystemManager(Scene* scene, yoyo::RendererLayer* renderer_layer)
        :System<ParticleSystemComponent>(scene), m_renderer_layer(renderer_layer) {}

    virtual ~ParticleSystemManager() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update(float dt) override;

    virtual void OnComponentCreated(Entity e, ParticleSystemComponent& transform) override;
    virtual void OnComponentDestroyed(Entity e, ParticleSystemComponent& transform) override;
private:
    yoyo::RendererLayer* m_renderer_layer = nullptr;
    std::vector<Ref<yoyo::ParticleSystem>> m_particle_systems;
};
