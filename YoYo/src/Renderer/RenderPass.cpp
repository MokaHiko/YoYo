#include "RenderPass.h"

#include "Mesh.h"
#include "Material.h"

namespace yoyo
{
	const RenderableBatchId GenerateBatchId(Ref<IMesh>& mesh, const Ref<Material>& material)
    {
		RenderableBatchId id = static_cast<RenderableBatchId>(mesh->Hash() | (std::hash<Material>()(*material.get()) & 0xFFFFFFFF));
		return id;
    }

};