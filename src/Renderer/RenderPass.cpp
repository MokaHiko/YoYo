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

	MeshPass::MeshPass(uint32_t max_objects)
	{
		renderables.resize(max_objects);
		renderable_id_to_index.resize(max_objects);
		renderable_ids.resize(max_objects);
	}

	void MeshPass::AddRenderable(Ref<MeshPassObject> obj)
	{
		RenderSceneId scene_id = obj->Id();
		renderables[scene_id] = obj;

		renderable_id_to_index[scene_id] = count;
		renderable_ids[count] = scene_id;

		obj->IncrementShaderPassCount();
		count++;
	}

	void MeshPass::RemoveRenderable(RenderSceneId id)
	{
		// Release renderable from shader pass
		renderables[id]->DecrementShaderPassCount();
		//renderables[id].reset();

		// Swap, if index is not the end of renderable id array
		int renderable_id_index = renderable_id_to_index[id];
		if (renderable_id_index < count - 1)
		{
			renderable_ids[renderable_id_index] = renderable_ids[count - 1];
		}

		count--;
	}
};