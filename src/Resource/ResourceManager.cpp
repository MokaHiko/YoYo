#include "ResourceManager.h"

#include "ResourceEvent.h"

#include "Renderer/Mesh.h"
#include "Renderer/SkinnedMesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Animation.h"

#include <imgui.h>

namespace yoyo
{
	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
	}

	ResourceManager& ResourceManager::Instance()
	{
		static ResourceManager* resource_manager = nullptr;

		if (!resource_manager)
		{
			resource_manager = YNEW ResourceManager;
		}

		return *resource_manager;
	}

	void ResourceManager::Init()
	{
	}

	void ResourceManager::Shutdown()
	{
	}

	void ResourceManager::Update()
	{
		// TODO: Move to dirty list
		for (auto it : ResourceManager::Instance().Cache<StaticMesh>())
		{
			Ref<StaticMesh> mesh = it.second;
			MeshDirtyFlags flags = mesh->DirtyFlags();

			if (flags == MeshDirtyFlags::Clean)
			{
				continue;
			}

			if ((flags & MeshDirtyFlags::Unuploaded) == MeshDirtyFlags::Unuploaded)
			{
				mesh->UploadMeshData();
			}

			if ((flags & MeshDirtyFlags::VertexDataChange) == MeshDirtyFlags::VertexDataChange)
			{
				// TODO: Update vertex data in gpu
			}
		}

		for (auto it : ResourceManager::Instance().Cache<SkinnedMesh>())
		{
			Ref<SkinnedMesh> mesh = it.second;
			MeshDirtyFlags flags = mesh->DirtyFlags();
			if (flags == MeshDirtyFlags::Clean)
			{
				continue;
			}

			if ((flags & MeshDirtyFlags::Unuploaded) == MeshDirtyFlags::Unuploaded)
			{
				mesh->UploadMeshData();
			}

			if ((flags & MeshDirtyFlags::VertexDataChange) == MeshDirtyFlags::VertexDataChange)
			{
				// TODO: Update vertex data in gpu
			}

			if ((flags & MeshDirtyFlags::BoneDataChanged) == MeshDirtyFlags::BoneDataChanged) 
			{
				// mesh->UpdateMeshData();
			}
		}

		for (auto it : ResourceManager::Instance().Cache<Texture>())
		{
			Ref<Texture> texture = it.second;

			TextureDirtyFlags flags = texture->DirtyFlags();
			if (flags == TextureDirtyFlags::Clean)
			{
				continue;
			}

			if ((flags & TextureDirtyFlags::Unuploaded) == TextureDirtyFlags::Unuploaded)
			{
				texture->UploadTextureData();
			}
		}
	}

	RuntimeResourceLayer::~RuntimeResourceLayer()
	{
	}

	void RuntimeResourceLayer::OnAttach()
	{
	}

	void RuntimeResourceLayer::OnDetatch()
	{
		// TODO: Unsubscribe
	}

	void RuntimeResourceLayer::OnUpdate(float dt)
	{
		ResourceManager::Instance().Update();
	}

	void RuntimeResourceLayer::OnEnable()
	{
		ResourceManager::Instance().Init();

		EventManager::Instance().Subscribe(MeshCreatedEvent<StaticMesh>::s_event_type, [](Ref<Event> event) {
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent<StaticMesh>>(event);
			Ref<StaticMesh> mesh = std::static_pointer_cast<StaticMesh>(mesh_created_event->mesh);
			return ResourceManager::Instance().OnResourceCreated<StaticMesh>(mesh);
		});

		EventManager::Instance().Subscribe(MeshCreatedEvent<SkinnedMesh>::s_event_type, [](Ref<Event> event) {
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent<SkinnedMesh>>(event);
			Ref<SkinnedMesh> mesh = std::static_pointer_cast<SkinnedMesh>(mesh_created_event->mesh);
			return ResourceManager::Instance().OnResourceCreated<SkinnedMesh>(mesh);
		});

		EventManager::Instance().Subscribe(ShaderCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& shader_created_event = std::static_pointer_cast<ShaderCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Shader>(shader_created_event->shader);
		});

		EventManager::Instance().Subscribe(TextureCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& texture_created_event = std::static_pointer_cast<TextureCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Texture>(texture_created_event->texture);
		});

		EventManager::Instance().Subscribe(MaterialCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& material_created_event = std::static_pointer_cast<MaterialCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Material>(material_created_event->material);
		});

		EventManager::Instance().Subscribe(SkeletalHierarchyCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& skeletal_hierarchy_created_event = std::static_pointer_cast<SkeletalHierarchyCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<SkeletalHierarchy>(skeletal_hierarchy_created_event->skeletal_hierarchy);
		});

		EventManager::Instance().Subscribe(AnimationCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& animation_created_event = std::static_pointer_cast<AnimationCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Animation>(animation_created_event->animation);
		});
	}

	void RuntimeResourceLayer::OnDisable()
	{
		ResourceManager::Instance().Shutdown();
	}

	void RuntimeResourceLayer::OnImGuiRender()
	{
		ImGui::Begin("Resources");

		ResourceManager& rm = ResourceManager::Instance();

		if (ImGui::TreeNode("Assets"))
		{
			// ImGui::Text("Meshes: %d", rm.Cache<Mesh>().size());
			ImGui::Text("Materials: %d", rm.Cache<Material>().size());
			ImGui::Text("Textures: %d", rm.Cache<Texture>().size());
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Memory Map"))
		{
			static std::array<uint64_t, (size_t)MemoryTag::MaximumValue> memory_map;
			memset(memory_map.data(), 0, sizeof(uint64_t) * (size_t)MemoryTag::MaximumValue);

			for (const auto& it : GetMemoryMap())
			{
				YASSERT(it.first, "Invalid memory in memory map!");

				const MemoryAllocation& mem_alloc = it.second;
				memory_map[(int)mem_alloc.tag] += mem_alloc.size;
			}

			for (int i = 0; i < memory_map.size(); i++)
			{
				ImGui::Text("[%s] : %.3lf kb", MemoryTagStrings[i], memory_map[i] / 1000.0f);
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}