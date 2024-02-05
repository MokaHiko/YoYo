#include "ResourceManager.h"

#include "ResourceEvent.h"

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

namespace yoyo
{
	ResourceManager::ResourceManager() {}

	ResourceManager::~ResourceManager() {}

	void ResourceManager::Init()
	{
	}

	void ResourceManager::Shutdown()
	{
	}

    void ResourceManager::Update()
    {
		// TODO: Move to dirty list

		for(auto it : m_mesh_cache)
		{
			Ref<Mesh> mesh = it.second;

			MeshDirtyFlags flags = mesh->DirtyFlags();
			if(flags == MeshDirtyFlags::Clean)
			{
				continue;
			}

			if((flags & MeshDirtyFlags::Unuploaded) == MeshDirtyFlags::Unuploaded)
			{
				mesh->UploadMeshData();
			}

			if((flags & MeshDirtyFlags::VertexDataChange) == MeshDirtyFlags::VertexDataChange)
			{
				// TODO: Update vertex data in gpu
			}
		}

		for(auto it : m_texture_cache)
		{
			Ref<Texture> texture = it.second;

			TextureDirtyFlags flags = texture->DirtyFlags();
			if(flags == TextureDirtyFlags::Clean)
			{
				continue;
			}

			if((flags & TextureDirtyFlags::Unuploaded) == TextureDirtyFlags::Unuploaded)
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
		ResourceManager::Init();

		EventManager::Instance()->Subscribe(MeshCreatedEvent::s_event_type, [](Ref<Event> event){
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent>(event);
			return ResourceManager::OnMeshCreated(mesh_created_event->mesh);
		});

		EventManager::Instance()->Subscribe(ShaderCreatedEvent::s_event_type, [](Ref<Event> event){
			const auto& shader_created_event = std::static_pointer_cast<ShaderCreatedEvent>(event);
			return ResourceManager::OnShaderCreated(shader_created_event->shader);
		});

		EventManager::Instance()->Subscribe(TextureCreatedEvent::s_event_type, [](Ref<Event> event){
			const auto& texture_created_event = std::static_pointer_cast<TextureCreatedEvent>(event);
			return ResourceManager::OnTextureCreated(texture_created_event->texture);
		});
	}

	void RuntimeResourceLayer::OnDetatch()
	{
		// TODO: Unsubscribe


		ResourceManager::Shutdown();
	}

	void RuntimeResourceLayer::OnUpdate(float dt)
	{
		ResourceManager::Update();
	}

	void RuntimeResourceLayer::OnEnable()
	{
	}

	void RuntimeResourceLayer::OnDisable()
	{
	}
}