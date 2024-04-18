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
        float time_alive = 0;
        float life_span = 0.5f;

        Vec3 position{ 0.0f, 0.0f, 0.0f };
        Vec3 rotation{ 0.0f, 0.0f, 0.0f };
        Vec3 scale{ 1.0f, 1.0f, 1.0f };

        Vec3 linear_velocity{ 0.0f, 0.0f, 0.0f };
        Vec3 angular_velocity{ 0.0f, 0.0f, 0.0f };

        Vec3 start_scale{ 1.0f, 1.0f, 1.0f };
        Vec3 end_scale{ 1.0f, 1.0f, 1.0f };

        Vec4 color{ 1.0f, 1.0f, 1.0f , 1.0f };
        Vec4 start_color{ 1.0f, 1.0f, 1.0f, 1.0f };
        Vec4 end_color{ 0.0f, 0.0f, 0.0f, 0.0f };
    };

    class MeshPassObject;
    class YAPI ParticleSystem : public Resource
    {
    public:
        RESOURCE_TYPE(ParticleSystem)

        ParticleSystem();
        virtual ~ParticleSystem();

        std::pair<float, float> life_span_range = { 0.0f, 3.0f };
        std::pair<float, float> scale_range = {1.0f, 2.0f};

        std::pair<Vec3, Vec3> position_offset_range = { {0.0f, 0.0f, 0.0}, {0.0f, 0.0f, 0.0f} };
        std::pair<Vec3, Vec3> linear_velocity_range = { Vec3{0.5f, 0.5f, 0.5} * -20.0f, Vec3{0.5f, 0.5f, 0.5f} * 20.0f };
        std::pair<Vec3, Vec3> angular_velocity_range = { Vec3{0.0f, 0.0f, 1.0f} * -Y_PI, Vec3{0.0f, 0.0f, 1.0f} * Y_PI };

        static Ref<ParticleSystem> Create(const std::string& name);

        void Update(float dt);

        const std::vector<Particle>& GetParticles() const { return m_particles; }

        const uint32_t GetMaxParticles() const { return m_max_particles; }
        void SetMaxParticles(uint32_t max_particles);

        const uint32_t GetParticlesAlive() const { return m_particles_alive; }

        const yoyo::Vec3& GetGravityScale() const { return m_gravity_scale; }
        void SetGravityScale(const yoyo::Vec3& gravity_scale) { m_gravity_scale = gravity_scale; }

        const float GetEmissionRate() const { return m_emission_rate; }
        void SetEmissionRate(float emission_rate) { m_emission_rate = emission_rate; }

        const ParticleSystemType GetType() const { return m_type; }
        void SetType(ParticleSystemType type) { m_type = type; }

        const ParticleSystemSpace GetSimulationSpace() const { return m_space; }
        void SetSimulationSpace(ParticleSystemSpace simulation_space) { m_space = simulation_space; }

        void SetGlobalTransform(const Mat4x4& global_transform) { m_global_transform = global_transform; }
    protected:
        // Particle behavior
        float m_emission_rate = 32.0f;
        Vec3 m_gravity_scale = { 0.0f, -9.8f, 0.0f };

        ParticleSystemType m_type = ParticleSystemType::Cpu;
        ParticleSystemSpace m_space = ParticleSystemSpace::Local;

        // For local space particles
        Mat4x4 m_global_transform = { 1.0f };

        uint32_t m_max_particles = 0;
        uint32_t m_particles_alive = 0;
        float m_time_alive = 0.0f;

        std::vector<Particle> m_particles = {};
    };
}