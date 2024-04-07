#include "RenderScene.h"

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

namespace yoyo
{
	RenderScene::RenderScene(uint32_t max_renderable_objects)
	{
		forward_pass = CreateRef<MeshPass>();
		forward_pass->renderables.resize(max_renderable_objects);

		shadow_pass = CreateRef<MeshPass>();
		shadow_pass->renderables.resize(max_renderable_objects);

		transparent_forward_pass = CreateRef<MeshPass>();
		transparent_forward_pass->renderables.resize(max_renderable_objects);
	}

	RenderScene::~RenderScene() {}

	void RenderScene::AddMeshPassObject(Ref<MeshPassObject> obj)
	{
		YASSERT(obj->m_id == NULL_RENDER_SCENE_ID, "Mesh object already added!");

		// Generate Render SceneId
		obj->m_id = GenerateSceneId();

		switch (obj->material->render_mode)
		{
		case(MaterialRenderMode::Opaque):
		{
			forward_pass->renderables[obj->m_id] = obj;
			forward_pass_count++;
		}break;
		case(MaterialRenderMode::Transparent):
		{
			transparent_forward_pass->renderables[obj->m_id] = obj;
			transparent_forward_pass_count++;
		}break;
		default:
			break;
		}

		// TODO: Make not all mesh objects have shadows
		if (obj->material->receive_shadows)
		{
			shadow_pass->renderables[obj->m_id] = obj;
			shadow_pass_count++;
		}
	}

	void RenderScene::RemoveMeshPassObject(RenderSceneId obj_id)
	{
		int current_last_index = forward_pass_count - 1;
		if (obj_id > 0)
		{
			// Swap last with delete to preserve contiguos memory
			forward_pass->renderables[obj_id] = forward_pass->renderables[current_last_index];
			forward_pass->renderables[obj_id]->m_id = obj_id; 
			
			// Release last index
			forward_pass->renderables[current_last_index].reset();
			forward_pass->renderables[current_last_index] = nullptr;
		}
		else
		{
			forward_pass->renderables[obj_id].reset();
		}
		// Cache last index
		m_free_ids.insert(current_last_index);

		// Decrement forward pass count
		forward_pass_count--;

		// Swap with to delete if there is last
		if (obj_id > 0)
		{
			shadow_pass->renderables[obj_id] = shadow_pass->renderables[shadow_pass_count - 1];
		}
		else
		{
			shadow_pass->renderables[obj_id].reset();
		}
		shadow_pass_count--;
	}

	void RenderScene::BuildFlatBatches()
	{
		for (auto& batch : forward_flat_batches)
		{
			batch->renderables.clear();
		}

		for (uint32_t i = 0; i < GetForwardPassCount(); i++)
		{
			const Ref<MeshPassObject>& obj = GetForwardPass()->renderables[i];

			RenderableBatchId id = GenerateBatchId(obj->mesh, obj->material);
			auto it = std::find_if(forward_flat_batches.begin(), forward_flat_batches.end(), [&](const auto& b) {
				return id == b->id;
			});

			if (it != forward_flat_batches.end())
			{
				(*it)->renderables.push_back(obj);
			}
			else // New batch
			{
				Ref<RenderableBatch> batch = CreateRef<RenderableBatch>(obj->mesh, obj->material);
				batch->renderables.push_back(obj);

				forward_flat_batches.push_back(batch);
			}
		}

		// TODO: Build shadow flat batches
	}

	RenderSceneId RenderScene::GenerateSceneId()
	{
		// TODO: Make thread safe std::scoped_lock
		if(m_free_ids.empty())
		{
			return m_next_id++;
		}

		uint32_t free_id = *m_free_ids.begin();
		m_free_ids.erase(m_free_ids.begin());

		return free_id;
	}
}