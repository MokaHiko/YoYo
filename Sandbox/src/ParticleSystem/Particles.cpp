#include "Particles.h"
#include "ECS/Components/Components.h"

#include <Renderer/Shader.h>
#include <Renderer/Material.h>

#include <Resource/ResourceManager.h>

#include <Math/Random.h>
#include <Math/MatrixTransform.h>

#include <Renderer/RendererLayer.h>

ParticleSystemComponent::ParticleSystemComponent() 
{
	if(!m_particle_system)
	{
		m_particle_system = yoyo::ParticleSystem::Create("");
	}
}

ParticleSystemComponent::~ParticleSystemComponent() {}

const std::vector<yoyo::Particle>& ParticleSystemComponent::GetParticles() const {return m_particle_system->GetParticles();}

std::vector<yoyo::Particle>& ParticleSystemComponent::GetParticles() {return m_particle_system->GetParticles();}

const uint32_t ParticleSystemComponent::GetMaxParticles() const {return m_particle_system->GetMaxParticles();}

void ParticleSystemComponent::SetMaxParticles(uint32_t size) 
{
	m_particle_system->GetParticles().resize(size);
}

const yoyo::Vec3& ParticleSystemComponent::GetGravityScale() const
{
	return m_particle_system->GetGravityScale();
}

void ParticleSystemComponent::SetGravityScale(const yoyo::Vec3& gravity_scale)
{
	m_particle_system->SetGravityScale(gravity_scale);
}

const float ParticleSystemComponent::GetEmissionRate() const
{
	return m_particle_system->GetEmissionRate();
}

void ParticleSystemComponent::SetEmissionRate(float emission_rate) 
{
	m_particle_system->SetEmissionRate(emission_rate);
}

const yoyo::ParticleSystemType ParticleSystemComponent::GetType() const
{
	return m_particle_system->GetType();
}

void ParticleSystemComponent::SetType(yoyo::ParticleSystemType type) 
{
	return m_particle_system->SetType(type);
}

const yoyo::ParticleSystemSpace ParticleSystemComponent::GetSimulationSpace() const
{
	return m_particle_system->GetSimulationSpace();
}

void ParticleSystemComponent::SetSimulationSpace(yoyo::ParticleSystemSpace simulation_space)
{
	return m_particle_system->SetSimulationSpace(simulation_space);
}

void ParticleSystemComponent::AddMaterial(Ref<yoyo::Material> material) 
{
	YASSERT(material, "Cannot add null material!");
	m_materials.push_back(material);
}

void ParticleSystemManager::Init()
{
	Ref<yoyo::Shader> unlit_particle_shader = yoyo::ResourceManager::Instance().Load<yoyo::Shader>("unlit_particle_instanced_shader");
	Ref<yoyo::Material> particle_instanced_material = yoyo::Material::Create(unlit_particle_shader, "default_particle_material");
	particle_instanced_material->ToggleCastShadows(false);
	particle_instanced_material->ToggleReceiveShadows(false);

	particle_instanced_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/prototype_512x512_white.yo"));
	particle_instanced_material->SetColor(yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	particle_instanced_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	particle_instanced_material->SetVec4("specular_color", yoyo::Vec4{ 1.0, 1.0f, 1.0f, 1.0f });

	Ref<yoyo::StaticMesh> quad = yoyo::StaticMesh::Create("particle_quad");
	quad->GetVertices() =
	{
		{{-1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  0.00}},
		{{ 1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  0.00}},
		{{ 1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  1.00}},
		{{-1.00, -1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  0.00}},
		{{ 1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {1.00,  1.00}},
		{{-1.00,  1.00,  0.00}, {0.00, 0.00, 0.00},  {0.00,  0.00,  1.00},  {0.00,  1.00}},
	};
}

void ParticleSystemManager::Shutdown() 
{

}

void ParticleSystemManager::Update(float dt) {
	for (auto entity : GetScene()->Registry().view<TransformComponent, ParticleSystemComponent>())
	{
		Entity e(entity, GetScene());
		const TransformComponent& transform = e.GetComponent<TransformComponent>();

		ParticleSystemComponent& particle_system_component = e.GetComponent<ParticleSystemComponent>();
		particle_system_component.m_particle_system->Update(dt);

		const auto& particles = particle_system_component.GetParticles();
		for(int i = 0; i < particle_system_component.GetMaxParticles(); i++)
		{
			particle_system_component.m_particle_renderable_objects[i]->model_matrix = transform.model_matrix * yoyo::TranslationMat4x4(particles[i].position);
		}
	}
}

void ParticleSystemManager::OnComponentCreated(Entity entity, ParticleSystemComponent& particle_system_component)
{
	Entity e(entity, GetScene());

	if(!e.IsValid())
	{
		YERROR("Invalid entity handle holding particle system!");
		return;
	}

	const TransformComponent& transform = e.GetComponent<TransformComponent>();

	// Create particle system
    particle_system_component.m_particle_system = yoyo::ParticleSystem::Create("");

	particle_system_component.SetMaxParticles(512);
	static yoyo::PRNGenerator<float> random_velocity = { -100.0f, 100.0f };
	static yoyo::PRNGenerator<float> random_life_span = { 0.0f, 5.0f };

	// Instanced particles
	{
		Ref<yoyo::StaticMesh> quad = yoyo::ResourceManager::Instance().Load<yoyo::StaticMesh>("particle_quad");

		// TODO: Delete Render packet
		static yoyo::RenderPacket* packet = YNEW yoyo::RenderPacket;
	   	if(packet->IsProccessed())
        {
            packet->Reset();
        }

		std::vector<Ref<yoyo::MeshPassObject>>& particle_renderables = particle_system_component.m_particle_renderable_objects;
		particle_renderables .resize(particle_system_component.GetMaxParticles());
		std::vector<yoyo::Particle>& particles = particle_system_component.GetParticles();

		// TODO: Random material based on uniform distribution
		Ref<yoyo::Material> material = nullptr;
		if(particle_system_component.m_materials.empty())
		{
			particle_system_component.m_materials.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Material>("default_particle_material"));
		}
		material = particle_system_component.m_materials[0];

		for (int i = 0; i < particle_system_component.GetMaxParticles(); i++)
		{
			particles[i].life_span = random_life_span.Next();
			particles[i].linear_velocity = { random_velocity.Next(), random_velocity.Next(), random_velocity.Next() };

			particle_renderables[i] = CreateRef<yoyo::MeshPassObject>();
			particle_renderables[i]->mesh = quad;
			particle_renderables[i]->material = material;
			particle_renderables[i]->model_matrix = transform.model_matrix * yoyo::TranslationMat4x4(particles[i].position);

			packet->new_objects.push_back(particle_renderables[i]);
		}

		m_renderer_layer->SendRenderPacket(packet);
	}

	// Batched
}

void ParticleSystemManager::OnComponentDestroyed(Entity e, ParticleSystemComponent& transform)
{
}
