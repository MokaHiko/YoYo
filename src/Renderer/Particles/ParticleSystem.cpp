#include "ParticleSystem.h"

#include "ParticleSystemEvents.h"

#include "Renderer/RenderPass.h"
#include "Math/Random.h"

#include <thread>

namespace yoyo
{
	static PRNGenerator<int> random_sign = { -1, 1 };
	static PRNGenerator<float> random_usign = { 0.0f, 1.0f };

	ParticleSystem::ParticleSystem()
	{
		// Default values
		m_max_particles = 128;
		m_emission_rate = 16.0f;
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	Ref<ParticleSystem> ParticleSystem::Create(const std::string& name)
	{
		// Check cache 
		Ref<ParticleSystem> particle_system = CreateRef<ParticleSystem>();
		particle_system->name = name;

		Ref<ParticleSystemCreatedEvent> particle_system_created_event = CreateRef<ParticleSystemCreatedEvent>(particle_system);
		EventManager::Instance().Dispatch(particle_system_created_event);

		return particle_system;
	}

	void ParticleSystem::Update(float dt)
	{
		m_time_alive += dt;
		m_particles_alive = Clamp(static_cast<uint32_t>(m_time_alive * m_emission_rate), 0, m_max_particles);

		for(int i = 0; i < m_particles_alive; i++)
		{
			Particle& particle = m_particles[i];
			float life_time_ratio = particle.time_alive / particle.life_span;

			if(particle.time_alive >= particle.life_span)
			{
				if(!m_repeating)
				{
					continue;
				}

				// Recycle if repeating
				particle.time_alive = 0;
				particle.life_span = Lerp(life_span_range.first, life_span_range.second, random_sign.Next());
				particle.position = Lerp(position_offset_range.first, position_offset_range.second, random_usign.Next());
				particle.rotation = { 0.0f, 0.0f, 0.0f };

				particle.start_scale = Vec3{1.0f, 1.0f, 1.0f} * Lerp(scale_range.first, scale_range.second, random_usign.Next());
				particle.end_scale = Vec3{1.0f, 1.0f, 1.0f} * Lerp(scale_range.first, scale_range.second, random_usign.Next());
				particle.scale = particle.start_scale;

				particle.linear_velocity = Lerp(linear_velocity_range.first, linear_velocity_range.second, random_usign.Next());
				particle.angular_velocity = Lerp(angular_velocity_range.first, angular_velocity_range.second, random_usign.Next());
				particle.color = particle.start_color;
			}

			// Forces
			particle.linear_velocity += m_gravity_scale * dt;

			// Apply changes
			particle.position += particle.linear_velocity * dt;
			particle.rotation += particle.angular_velocity * dt;

			particle.scale = Lerp(particle.start_scale, particle.end_scale, life_time_ratio);
			particle.color = Lerp(particle.start_color, particle.end_color, life_time_ratio);

			// Increment elapsed time
			particle.time_alive += dt;
		}

		// TODO: Update particle system mesh for batched rendering
	}

	void ParticleSystem::SetMaxParticles(uint32_t max_particles)
	{
		m_max_particles = max_particles;
		m_particles.resize(max_particles);

		// Make particles
		for(int i = 0; i < m_particles.size(); i++)
		{
			Particle& particle = m_particles[i];

			// Recycle if repeating
			particle.time_alive = 0;
			particle.life_span = Lerp(life_span_range.first, life_span_range.second, random_sign.Next());
			particle.position = Lerp(position_offset_range.first, position_offset_range.second, random_usign.Next());
			particle.rotation = { 0.0f, 0.0f, 0.0f };

			particle.start_scale = Vec3{1.0f, 1.0f, 1.0f} * Lerp(scale_range.first, scale_range.second, random_usign.Next());
			particle.end_scale = Vec3{1.0f, 1.0f, 1.0f} * Lerp(scale_range.first, scale_range.second, random_usign.Next());
			particle.scale = particle.start_scale;

			particle.linear_velocity = Lerp(linear_velocity_range.first, linear_velocity_range.second, random_usign.Next());
			particle.angular_velocity = Lerp(angular_velocity_range.first, angular_velocity_range.second, random_usign.Next());
			particle.color = particle.start_color;
		}
	}

	void ParticleSystem::SetParticlesAlive(uint32_t particle_count)
	{
		YASSERT(particle_count <= m_particles.size(), "Cannot increase particle count to more than maximum particle system capacity!");
		m_particles_alive = particle_count;
	}

}