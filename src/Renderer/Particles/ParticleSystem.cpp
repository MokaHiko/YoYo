#include "ParticleSystem.h"

#include "ParticleSystemEvents.h"

#include "Renderer/RenderPass.h"
#include "Math/Random.h"

namespace yoyo
{
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
		static PRNGenerator<float> random_velocity = { 0.0f, 15.0f };
		static PRNGenerator<int> random_sign = { -1, 1 };
		static PRNGenerator<float> random_life_span = { 0.0f, 10.0f };

		for(Particle& particle : m_particles)
		{
			// Increment elapsed time
			particle.time_alive += dt;

			if(particle.time_alive >= particle.life_span)
			{
				// Recycle
				particle.time_alive = 0.0f;
				particle.linear_velocity = {random_velocity.Next() * random_sign.Next(), random_velocity.Next(), random_velocity.Next() * random_sign.Next()};
				particle.position = {0.0f, 0.0f, 0.0f};
				particle.rotation = {0.0f, 0.0f, 0.0f};

				// Dead
				// return;
			}

			// Forces
			particle.linear_velocity += m_gravity_scale * dt;

			// Apply changes
			particle.position += particle.linear_velocity * dt;
			particle.rotation += particle.angular_velocity * dt;
		}

		// Update particle system mesh
	}

}