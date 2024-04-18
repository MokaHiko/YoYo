#include "RenderScene.h"

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

namespace yoyo
{
	RenderScene::RenderScene(uint32_t max_renderable_objects)
	{
		// TODO: Abstract individual mesh passes simmilair to shadow pass, This will allow us to do stuff like deffered rendering
		forward_pass = CreateRef<MeshPass>(max_renderable_objects);
		shadow_pass = CreateRef<MeshPass>(max_renderable_objects);
		transparent_forward_pass = CreateRef<MeshPass>(max_renderable_objects);
	}

	RenderScene::~RenderScene() {}

	void RenderScene::AddMeshPassObject(Ref<MeshPassObject> obj)
	{
		// Generate Render SceneId
		YASSERT(obj->Id() == NULL_RENDER_SCENE_ID, "Mesh object already added!");
		obj->SetId(GenerateSceneId());

	switch (obj->material->GetRenderMode())
		{
		case(MaterialRenderMode::Opaque):
		{
			forward_pass->AddRenderable(obj);
		}break;
		case(MaterialRenderMode::Transparent):
		{
			transparent_forward_pass->AddRenderable(obj);
		}break;
		default:
			YERROR("Uknown Render Mode!");
			break;
		}

		if (obj->material->IsCastingShadows())
		{
			shadow_pass->AddRenderable(obj);
		}
	}

	void RenderScene::RemoveMeshPassObject(Ref<MeshPassObject> obj)
	{
		RenderSceneId scene_id = obj->Id();
		YASSERT(scene_id != NULL_RENDER_SCENE_ID, "Cannot remove invalid mesh object scene id!");
		
		switch (obj->material->GetRenderMode())
		{
		case(MaterialRenderMode::Opaque):
		{
			forward_pass->RemoveRenderable(scene_id);
		}break;
		case(MaterialRenderMode::Transparent):
		{
			YERROR("No Transparent pass!");
		}break;
		default:
			YERROR("Uknown Render Mode!");
			break;
		}

		if (obj->material->IsCastingShadows())
		{
			shadow_pass->RemoveRenderable(scene_id);
		}

		// Cache if no shader passes are using it
		if (obj->ShaderPassCount() == 0)
		{
			m_free_ids.emplace(scene_id);
		}
	}

	void RenderScene::BuildFlatBatches()
	{
		// Build shadow flat batches
		{
			for (auto& batch : shadow_flat_batches)
			{
				batch->renderables.clear();
			}

			for (uint32_t i = 0; i < GetShadowPassCount(); i++)
			{
				const Ref<MeshPassObject>& obj = shadow_pass->renderables[shadow_pass->renderable_ids[i]];

				if (!obj)
				{
					YWARN("Fragmented shadow mesh pass objects");
					continue;
				}

				RenderableBatchId id = GenerateBatchId(obj->mesh, obj->material);
				auto it = std::find_if(shadow_flat_batches.begin(), shadow_flat_batches.end(), [&](const auto& b) {
					return id == b->id;
				});

				if (it != shadow_flat_batches.end())
				{
					(*it)->renderables.push_back(obj);
				}
				else // New batch
				{
					Ref<RenderableBatch> batch = CreateRef<RenderableBatch>(obj->mesh, obj->material);
					batch->renderables.push_back(obj);
					shadow_flat_batches.push_back(batch);
				}
			}
		}

		{
			for (auto& batch : forward_flat_batches)
			{
				batch->renderables.clear();
			}

			for (uint32_t i = 0; i < GetForwardPassCount(); i++)
			{
				const Ref<MeshPassObject>& obj = forward_pass->renderables[forward_pass->renderable_ids[i]];

				if (!obj)
				{
					YINFO("id: %d, index: %d",forward_pass->renderable_ids[i], i);
					YWARN("Fragmented forward mesh pass objects");
					continue;
				}

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
		}
	}

	RenderSceneId RenderScene::GenerateSceneId()
	{
		// TODO: Make thread safe std::scoped_lock
		if (m_free_ids.empty())
		{
			return m_next_id++;
		}

		uint32_t free_id = m_free_ids.front();
		m_free_ids.pop();

		return free_id;
	}
}