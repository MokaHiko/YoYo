#include "MeshComponents.h"

#include "Resource/ResourceManager.h"

namespace yoyo
{
	MeshRenderer::MeshRenderer()
	{
		mesh = ResourceManager::Load<Mesh>("cube.obj");
		material = ResourceManager::Load<Material>("default_material");
	}

	MeshRenderer::~MeshRenderer()
	{
	}
}