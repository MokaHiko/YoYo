#include "ResourceManager.h"

#include "ResourceEvent.h"

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"

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
			resource_manager = Y_NEW ResourceManager;
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
		for (auto it : ResourceManager::Instance().Cache<Mesh>())
		{
			Ref<Mesh> mesh = it.second;

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

		EventManager::Instance().Subscribe(MeshCreatedEvent::s_event_type, [](Ref<Event> event) {
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Mesh>(mesh_created_event->mesh);
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

	}

	void RuntimeResourceLayer::OnDisable()
	{
		ResourceManager::Instance().Shutdown();
	}

	void RuntimeResourceLayer::OnImGuiRender()
	{
		ImGui::Begin("Resources");

		ResourceManager& rm = ResourceManager::Instance();

		ImGui::Text("Meshes: %d", rm.Cache<Mesh>().size());
		ImGui::Text("Materials: %d", rm.Cache<Material>().size());
		ImGui::Text("Textures: %d", rm.Cache<Texture>().size());

		ImGui::End();
	}
}