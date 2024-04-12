#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"

#include "Resource/Resource.h"

#include "Renderer/Mesh.h"

namespace yoyo
{
    enum class ParticleSystemType
    {
        Cpu,
        Gpu,
    };

    enum class ParticleSystemSpace
    {
        Local,
        World,

        Max,
    };

    struct Particle
    {
        Vec3 position{ 0.0f, 0.0f, 0.0f };
        Vec3 rotation{ 0.0f, 0.0f, 0.0f };
        Vec3 scale{ 0.0f, 0.0f, 0.0f };

        Vec3 linear_velocity{ 0.0f, 0.0f, 0.0f };
        Vec3 angular_velocity{ 0.0f, 0.0f, 0.0f };

        float time_alive = 0;
        float life_span = 0;
    };

    class MeshPassObject;
    class YAPI ParticleSystem : public Resource
    {
    public:
        RESOURCE_TYPE(ParticleSystem)

        ParticleSystem();
        ~ParticleSystem();

        static Ref<ParticleSystem> Create(const std::string& name);

        const std::vector<Particle>& GetParticles() const { return m_particles; }
        std::vector<Particle>& GetParticles() { return m_particles; }

        const uint32_t GetMaxParticles() const { return m_max_particles; }


        const yoyo::Vec3& GetGravityScale() const { return m_gravity_scale; }
        void SetGravityScale(const yoyo::Vec3& gravity_scale) { m_gravity_scale = gravity_scale; }

        const float GetEmissionRate() const { return m_emission_rate; }
        void SetEmissionRate(float emission_rate) { m_emission_rate = emission_rate; }

        const ParticleSystemType GetType() const { return m_type; }
        void SetType(ParticleSystemType type) { m_type = type; }

        const ParticleSystemSpace GetSimulationSpace() const { return m_space; }
        void SetSimulationSpace(ParticleSystemSpace simulation_space) { m_space = simulation_space; }

        void SetGlobalTransform(const Mat4x4& global_transform) { m_global_transform = global_transform; }
        void Update(float dt);
    private:
        // For local space particles
        Mat4x4 m_global_transform = { 1.0f };
    private:
        // Particle behavior
        float m_emission_rate = 0.0f;
        Vec3 m_gravity_scale = { 0.0f, -9.8f, 0.0f };

        ParticleSystemType m_type = ParticleSystemType::Cpu;
        ParticleSystemSpace m_space = ParticleSystemSpace::Local;

        uint32_t m_max_particles = 0;
        std::vector<Particle> m_particles = {};
    };
}