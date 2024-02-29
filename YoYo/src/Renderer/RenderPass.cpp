#include "RenderPass.h"

#include "Mesh.h"
#include "Material.h"

namespace yoyo
{
    RenderableBatch::RenderableBatch(Ref<Mesh> mesh_, Ref<Material> material_)
        :material(material_), mesh(mesh_), id(GenerateBatchId(mesh, material))
    {
    }

	const RenderableBatchId GenerateBatchId(const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		RenderableBatchId id = static_cast<RenderableBatchId>((std::hash<Mesh>()(*mesh.get()) & 0xFFFFFFFF00000000) | (std::hash<Material>()(*material.get()) & 0xFFFFFFFF));
		return id;
	}
};