#include "RenderScene.h"

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

namespace yoyo
{
	RenderScene::RenderScene()
	{
		forward_pass = CreateRef<MeshPass>();
		transparent_forward_pass = CreateRef<MeshPass>();
		shadow_pass = CreateRef<MeshPass>();
	}

	RenderScene::~RenderScene() {}

	void RenderScene::AddMeshPassObject(Ref<MeshPassObject> obj)
	{
		// Generate Render SceneId
		obj->m_id = GenerateSceneId();

		switch (obj->material->render_mode)
		{
		case(MaterialRenderMode::Opaque):
		{
			forward_pass->renderables.push_back(obj);
		}break;
		case(MaterialRenderMode::Transparent):
		{
			transparent_forward_pass->renderables.push_back(obj);
		}break;
		default:
			break;
		}

		if (obj->material->receive_shadows)
		{
			shadow_pass->renderables.push_back(obj);
		}
	}

	void RenderScene::BuildFlatBatches(const std::vector<Ref<MeshPassObject>>& objs)
	{
		for(auto& batch : forward_flat_batches)
		{
			batch->renderables.clear();
		}

		for (const Ref<MeshPassObject> obj : objs)
		{
			RenderableBatchId id = GenerateBatchId(obj->mesh, obj->material);
			auto it = std::find_if(forward_flat_batches.begin(), forward_flat_batches.end(), [&](const auto& b){
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

	RenderSceneId RenderScene::GenerateSceneId()
	{
		// TODO: Make thread safe std::scoped_lock
		return m_next_id++;
	}
}