#include "RenderableComponents.h"

#include <Resource/ResourceManager.h>

MeshRendererComponent::MeshRendererComponent()
{
	mesh = yoyo::ResourceManager::Instance().Load<yoyo::Mesh>("cube.obj");
	material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("default_material");
}

MeshRendererComponent::~MeshRendererComponent()
{
}