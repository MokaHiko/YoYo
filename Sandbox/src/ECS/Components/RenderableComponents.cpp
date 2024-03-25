#include "RenderableComponents.h"

#include <Resource/ResourceManager.h>

MeshRendererComponent::MeshRendererComponent(const std::string& mesh_name, const std::string& material_name)
{
	mesh = yoyo::ResourceManager::Instance().Load<yoyo::StaticMesh>(mesh_name);
	material = yoyo::ResourceManager::Instance().Load<yoyo::Material>(material_name);
}

MeshRendererComponent::MeshRendererComponent()
{
	mesh = yoyo::ResourceManager::Instance().Load<yoyo::StaticMesh>("cubeCube");
	material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("default_material");
}

MeshRendererComponent::~MeshRendererComponent()
{
}

