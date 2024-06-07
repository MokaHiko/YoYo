#include "ResourceManager.h"

#include "ResourceEvent.h"

#include "Renderer/Mesh.h"
#include "Renderer/SkinnedMesh.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Animation.h"

#include <ImGui/ImGuiLayer.h>

namespace yoyo
{
	ResourceManager::ResourceManager()
	{
	}

	ResourceManager::~ResourceManager()
	{
	}

	ResourceManager &ResourceManager::Instance()
	{
		static ResourceManager *resource_manager = nullptr;

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

		for (auto& it : ResourceManager::Instance().Cache<SkinnedMesh>())
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

			if (flags == TextureDirtyFlags::TypeChange)
			{
				// TODO: Rebuild texture
			}

			if (flags == TextureDirtyFlags::SamplerType)
			{
				// TODO:
			}

			if ((flags & TextureDirtyFlags::DataChange) == TextureDirtyFlags::DataChange)
			{
				texture->UploadTextureData();
			}

			if ((flags & TextureDirtyFlags::Unuploaded) == TextureDirtyFlags::Unuploaded)
			{
				texture->UploadTextureData();
			}
		}

		for (auto it : ResourceManager::Instance().Cache<Material>())
		{
			Ref<Material> material = it.second;

			MaterialDirtyFlags flags = material->DirtyFlags();
			if (flags == MaterialDirtyFlags::Clean)
			{
				continue;
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

		EventManager::Instance().Subscribe(MeshCreatedEvent<StaticMesh>::s_event_type, [](Ref<Event> event)
										   {
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent<StaticMesh>>(event);
			Ref<StaticMesh> mesh = std::static_pointer_cast<StaticMesh>(mesh_created_event->mesh);
			return ResourceManager::Instance().OnResourceCreated<StaticMesh>(mesh); });

		EventManager::Instance().Subscribe(MeshCreatedEvent<SkinnedMesh>::s_event_type, [](Ref<Event> event)
										   {
			const auto& mesh_created_event = std::static_pointer_cast<MeshCreatedEvent<SkinnedMesh>>(event);
			Ref<SkinnedMesh> mesh = std::static_pointer_cast<SkinnedMesh>(mesh_created_event->mesh);
			return ResourceManager::Instance().OnResourceCreated<SkinnedMesh>(mesh); });

		EventManager::Instance().Subscribe(ShaderCreatedEvent::s_event_type, [](Ref<Event> event)
										   {
			const auto& shader_created_event = std::static_pointer_cast<ShaderCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Shader>(shader_created_event->shader); });

		EventManager::Instance().Subscribe(TextureCreatedEvent::s_event_type, [](Ref<Event> event)
										   {
			const auto& texture_created_event = std::static_pointer_cast<TextureCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Texture>(texture_created_event->texture); });

		EventManager::Instance().Subscribe(MaterialCreatedEvent::s_event_type, [](Ref<Event> event)
										   {
			const auto& material_created_event = std::static_pointer_cast<MaterialCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Material>(material_created_event->material); });

		EventManager::Instance().Subscribe(SkeletalHierarchyCreatedEvent::s_event_type, [](Ref<Event> event)
										   {
			const auto& skeletal_hierarchy_created_event = std::static_pointer_cast<SkeletalHierarchyCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<SkeletalHierarchy>(skeletal_hierarchy_created_event->skeletal_hierarchy); });

		EventManager::Instance().Subscribe(AnimationCreatedEvent::s_event_type, [](Ref<Event> event)
										   {
			const auto& animation_created_event = std::static_pointer_cast<AnimationCreatedEvent>(event);
			return ResourceManager::Instance().OnResourceCreated<Animation>(animation_created_event->animation); });
	}

	void RuntimeResourceLayer::OnDisable()
	{
		ResourceManager::Instance().Shutdown();
	}

	void RuntimeResourceLayer::OnImGuiRender()
	{
		ImGui::Begin("Resources");

		ResourceManager &rm = ResourceManager::Instance();

		const std::string static_mesh_node_node = "Static Meshes(" + std::to_string(rm.Cache<StaticMesh>().size()) + ")";
		if (ImGui::TreeNode(static_mesh_node_node.c_str()))
		{
			// Texture View
			for (const auto &it : rm.Cache<StaticMesh>())
			{
				if (ImGui::TreeNode(it.second->name.c_str()))
				{
					ImGui::Text("vertices: %d", it.second->GetVertexCount());
					ImGui::Text("indices: %d", it.second->GetIndexCount());
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		const std::string material_node_name = "Materials (" + std::to_string(rm.Cache<Material>().size()) + ")";
		if (ImGui::TreeNode(material_node_name.c_str()))
		{
			for (auto &it : rm.Cache<Material>())
			{
				Ref<Material> &material = it.second;
				if (ImGui::TreeNode(material->name.c_str()))
				{
					ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
					ImGui::BeginChild("Details", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 300), ImGuiChildFlags_None, window_flags);
					ImGui::Text("Id: %u", material->Id());

					const char *names[] =
						{
							"Uknown",
							"Opaque",
							"Transparent"};

					yoyo::Vec4 mat_color = material->GetColor();
					if (ImGui::DragFloat3("Color", mat_color.elements, 0.01f))
					{
						material->SetColor(mat_color);
					}

					// Simple selection popup (if you want to show the current selection inside the Button itself,
					// you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
					int &render_mode = (int &)material->GetRenderMode();
					if (ImGui::Button("Select RenderMode"))
						ImGui::OpenPopup("my_select_popup");
					ImGui::SameLine();
					ImGui::TextUnformatted(render_mode == -1 ? "<None>" : names[render_mode]);
					if (ImGui::BeginPopup("my_select_popup"))
					{
						ImGui::SeparatorText("Render Mode");
						for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						{
							if (ImGui::Selectable(names[i]))
								render_mode = i;
						}
						ImGui::EndPopup();
					}

					ImGui::SeparatorText("Properties");
					for (auto &it : material->GetProperties())
					{
						const yoyo::MaterialProperty &prop = it.second;
						const std::string name = it.first.c_str();

						switch (prop.type)
						{
						case (yoyo::MaterialPropertyType::Vec4):
						{
							if (ImGui::DragFloat4(name.c_str(), (float *)((char *)material->PropertyData() + prop.offset), 0.01f))
							{
								material->SetProperty(name.c_str(), (float *)((char *)material->PropertyData() + prop.offset));
							}
						}
						break;

						case (yoyo::MaterialPropertyType::Float32):
						{
							if (ImGui::DragFloat(name.c_str(), (float *)((char *)material->PropertyData() + prop.offset), 0.1f))
							{
								material->SetProperty(name.c_str(), (float *)((char *)material->PropertyData() + prop.offset));
							}
						}
						break;

						case (yoyo::MaterialPropertyType::Int32):
						{
							ImGui::InputInt(name.c_str(), (int32_t *)((char *)material->PropertyData() + prop.offset));
							{
								material->SetProperty(name.c_str(), (float *)((char *)material->PropertyData() + prop.offset));
							}
						}
						break;

						default:
						{
							ImGui::Text("%s [?] Uknown Property Type", name.c_str());
						}
						break;
						}
					}

					bool recieve_shadows = material->IsReceivingShadows();
					if (ImGui::Checkbox("Recieve Shadows", &recieve_shadows))
					{
						material->ToggleReceiveShadows(recieve_shadows);
					}

					bool cast_shadows = material->IsCastingShadows();
					if (ImGui::Checkbox("Cast Shadows", &cast_shadows))
					{
						material->ToggleCastShadows(cast_shadows);
					}

					const std::string shader_name = "Shader : " + material->shader->name;
					if (ImGui::TreeNode(shader_name.c_str()))
					{
						ImGui::Checkbox("Instanced", &material->shader->instanced);
						ImGui::TreePop();
					}
					ImGui::EndChild();

					ImGui::SameLine();

					// Child 2: rounded border
					{
						ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
						ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
						ImGui::BeginChild("Viewport", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2.0f), ImGuiChildFlags_Border, window_flags);
						ImGui::Image(material->MainTexture(), ImGui::GetContentRegionAvail());
						ImGui::EndChild();
						ImGui::PopStyleVar();
					}
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		const std::string texture_node = "Textures (" + std::to_string(rm.Cache<Texture>().size()) + ")";
		if (ImGui::TreeNode(texture_node.c_str()))
		{
			// Texture View
			for (const auto &it : rm.Cache<Texture>())
			{
				if (ImGui::TreeNode(it.second->name.c_str()))
				{
					ImGui::Text("w: %d h : %d", it.second->width, it.second->height);
					ImGui::Text("format: %s", TextureFormatStrings[(int)it.second->format]);
					ImGui::Image(it.second, ImVec2{150.0f, 150.0f});
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Memory Map"))
		{
			static std::array<uint64_t, (size_t)MemoryTag::MaximumValue> memory_map;
			memset(memory_map.data(), 0, sizeof(uint64_t) * (size_t)MemoryTag::MaximumValue);

			for (const auto &it : GetMemoryMap())
			{
				YASSERT(it.first, "Invalid memory in memory map!");

				const MemoryAllocation &mem_alloc = it.second;
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