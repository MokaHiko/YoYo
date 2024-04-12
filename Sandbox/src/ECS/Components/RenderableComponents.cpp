#include "RenderableComponents.h"

#include <Resource/ResourceManager.h>
#include <Renderer/Animation.h>
#include <Renderer/Particles/ParticleSystem.h>

MeshRendererComponent::MeshRendererComponent(const std::string& mesh_name, const std::string& material_name)
{
	mesh = yoyo::ResourceManager::Instance().Load<yoyo::StaticMesh>(mesh_name);
	material = yoyo::ResourceManager::Instance().Load<yoyo::Material>(material_name);
}

MeshRendererComponent::MeshRendererComponent()
{
	mesh = yoyo::ResourceManager::Instance().Load<yoyo::StaticMesh>("Cube");
	material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("default_material");
}

MeshRendererComponent::~MeshRendererComponent()
{
}

AnimatorComponent::AnimatorComponent() 
{
	animator = CreateRef<yoyo::Animator>();
}
