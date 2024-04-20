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
		memset(renderable_id_to_index.data(), -1, renderable_id_to_index.size() * sizeof(int));

		renderable_ids.resize(max_objects);
		memset(renderable_ids.data(), NULL_RENDER_SCENE_ID, renderable_ids.size() * sizeof(RenderSceneId));
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
		renderables[id].reset();

		// Swap the last of id array, if deleted index is not the end of array
		int to_delete_index = renderable_id_to_index[id];
		if (to_delete_index < count - 1)
		{
			// Update flat renderable id array
			renderable_ids[to_delete_index] = renderable_ids[count - 1];

			// Update renderable id to index mapper
			renderable_id_to_index[renderable_ids[to_delete_index]] = to_delete_index;
		}

#ifdef Y_DEBUG
		renderable_id_to_index[id] = -1;
#endif

		count--;
	}
};