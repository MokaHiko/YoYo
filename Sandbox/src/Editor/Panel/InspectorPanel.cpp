#include "InspectorPanel.h"

#include <Events/Event.h>
#include <ImGui/ImGuiLayer.h>

#include <Math/MatrixTransform.h>

#include "Editor/EditorEvents.h"
#include "ECS/Components/Components.h"

#include "ECS/Components/RenderableComponents.h"
#include "Physics/PhysicsTypes.h"

#include <Renderer/Shader.h>

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void MatrixInput(const char* name, yoyo::Mat4x4& mat)
{
	if (ImGui::TreeNode(name))
	{
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 4.0f);
		for (int i = 0; i < 4; i++)
		{
			ImGui::DragFloat("##label", &mat.data[0 + i], 0.1f); ImGui::SameLine();
			ImGui::DragFloat("##label", &mat.data[4 + i], 0.1f); ImGui::SameLine();
			ImGui::DragFloat("##label", &mat.data[8 + i], 0.1f); ImGui::SameLine();
			ImGui::DragFloat("##label", &mat.data[12 + i], 0.1f);
		}
		ImGui::PopItemWidth();

		ImGui::TreePop();
	}
}

InspectorPanel::InspectorPanel()
{
	yoyo::EventManager::Instance().Subscribe(FocusEntityEvent::s_event_type, [&](Ref<yoyo::Event> event) {
		Ref<FocusEntityEvent> focused_event = std::static_pointer_cast<FocusEntityEvent>(event);
		m_focused_entity = focused_event->entity;
		return false;
		});
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::Draw(Scene* scene)
{
	ImGui::Begin("Inspector");

	if (Entity e = m_focused_entity)
	{
		const TagComponent& tag = e.GetComponent<TagComponent>();
		ImGui::Text("%s", tag.tag.c_str());

		ImGui::Separator();

		DrawComponents();
	}

	ImGui::End();
}

void InspectorPanel::DrawComponents()
{
	DrawComponentUI<TransformComponent>("Transform", m_focused_entity, [](TransformComponent& transform) {
		ImGui::BeginTable("TransformTable", 2);

		float label_width = ImGui::GetContentRegionAvail().x * 0.25f;
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, label_width);
		ImGui::TableSetupColumn("Vec3 Value", ImGuiTableColumnFlags_WidthStretch, label_width * 3.0f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Position");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);
		ImGui::DragFloat3("##Position", transform.position.elements);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Rotation");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);

		yoyo::Vec3 degrees = { yoyo::RadToDeg(transform.rotation.x), yoyo::RadToDeg(transform.rotation.y), yoyo::RadToDeg(transform.rotation.z) };
		if (ImGui::DragFloat3("##Rotation", degrees.elements))
		{
			transform.quat_rotation = yoyo::QuatFromAxisAngle({ 1.0f, 0.0f, 0.0f }, yoyo::DegToRad(degrees.x))
									* yoyo::QuatFromAxisAngle({ 0.0f, 1.0f, 0.0f }, yoyo::DegToRad(degrees.y))
									* yoyo::QuatFromAxisAngle({ 0.0f, 0.0f, 1.0f }, yoyo::DegToRad(degrees.z));
		}

		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Scale");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);
		ImGui::DragFloat3("##Scale", transform.scale.elements);
		ImGui::PopItemWidth();

		ImGui::EndTable();

		static char* matrix_type_strings[] = { "model", "translation", "rotation", "scale" };
		static bool matrix_type_bools[] = { false, false, false, false };
		static int index = 0;

		if (ImGui::Button(matrix_type_strings[index])) { ImGui::OpenPopup("MatrixTypePopUp"); }
		if (ImGui::BeginPopup("MatrixTypePopUp"))
		{
			if (ImGui::Selectable(matrix_type_strings[0], matrix_type_bools[0]))
			{
				memset(matrix_type_bools, 0, sizeof(matrix_type_bools));
				matrix_type_bools[0] = true;
				index = 0;
			}

			if (ImGui::Selectable(matrix_type_strings[1], matrix_type_bools[1]))
			{
				memset(matrix_type_bools, 0, sizeof(matrix_type_bools));
				matrix_type_bools[1] = true;
				index = 1;
			}

			if (ImGui::Selectable(matrix_type_strings[2], matrix_type_bools[2]))
			{
				memset(matrix_type_bools, 0, sizeof(matrix_type_bools));
				matrix_type_bools[2] = true;
				index = 2;
			}

			if (ImGui::Selectable(matrix_type_strings[3], matrix_type_bools[3]))
			{
				memset(matrix_type_bools, 0, sizeof(matrix_type_bools));
				matrix_type_bools[3] = true;
				index = 3;
			}

			ImGui::EndPopup();
		}

		if (matrix_type_bools[0])
		{
			MatrixInput("##label", transform.model_matrix);
		}
		else if (matrix_type_bools[1])
		{
			MatrixInput("##label", transform.LocalTranslationMatrix());
		}
		else if (matrix_type_bools[2])
		{
			MatrixInput("##label", transform.LocalRotationMatrix());
		}
		else if (matrix_type_bools[3])
		{
			MatrixInput("##label", transform.LocalScaleMatrix());
		}

		}, true);

	DrawComponentUI<MeshRendererComponent>("MeshRenderer", m_focused_entity, [](MeshRendererComponent& mesh_renderer) {
		HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
		if (ImGui::TreeNode("Mesh"))
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("Details", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 150), ImGuiChildFlags_None, window_flags);
			ImGui::Separator();

			switch (mesh_renderer.type)
			{
			case yoyo::MeshType::Static: {
				Ref<yoyo::StaticMesh> mesh = std::static_pointer_cast<yoyo::StaticMesh>(mesh_renderer.mesh);
				ImGui::Text("Name: %s", mesh->name.c_str());
				ImGui::Text("Id: %u", mesh->Id());
				ImGui::Text("Vertices: %d", mesh->vertices.size());
				ImGui::Text("Indices: %d", mesh->indices.size());
			}break;

			case yoyo::MeshType::Skinned: {
				Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(mesh_renderer.mesh);
				ImGui::Text("Name: %s", mesh->name.c_str());
				ImGui::Text("Id: %u", mesh->Id());
				ImGui::Text("Vertices: %d", mesh->vertices.size());
				ImGui::Text("Indices: %d", mesh->indices.size());

				if (ImGui::TreeNode("Bones"))
				{
					for (int i = 0; i < mesh->bones.size(); i++)
					{
						const std::string name = std::to_string(i);
						MatrixInput(name.c_str(), mesh->bones[i]);
					}
					ImGui::TreePop();
				}
			}break;

			default:
				ImGui::Text("Uknown Mesh Type");
				HelpMarker("This indicates an error in how the asset was loaded!");
				break;
			}

			ImGui::EndChild();

			ImGui::SameLine();

			// Child 2: rounded border
			{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("Viewport", ImVec2(0, 150), ImGuiChildFlags_Border, window_flags);

				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

			ImGui::TreePop();
		}

		Ref<yoyo::Material> material = mesh_renderer.material;
		const std::string material_name = "Material: " + material->name;
		if (ImGui::TreeNode(material_name.c_str()))
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("Details", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 300), ImGuiChildFlags_None, window_flags);
			ImGui::Text("Id: %u", material->Id());

			const char* names[] =
			{
				"Uknown",
				"Opaque",
				"Transparent"
			};

			ImGui::DragFloat3("Color", material->color.elements, 0.01f);

			// Simple selection popup (if you want to show the current selection inside the Button itself,
			// you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
			int& render_mode = (int&)material->render_mode;
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
			for (auto& it : material->GetProperties())
			{
				const yoyo::MaterialProperty& prop = it.second;
				const std::string name = it.first.c_str();

				switch (prop.type)
				{
				case(yoyo::MaterialPropertyType::Vec4):
				{
					if (ImGui::DragFloat4(name.c_str(), (float*)((char*)material->PropertyData() + prop.offset), 0.01f))
					{
						material->SetProperty(name.c_str(), (float*)((char*)material->PropertyData() + prop.offset));
					}
				}break;

				case(yoyo::MaterialPropertyType::Float32):
				{
					if (ImGui::DragFloat(name.c_str(), (float*)((char*)material->PropertyData() + prop.offset), 0.1f))
					{
						material->SetProperty(name.c_str(), (float*)((char*)material->PropertyData() + prop.offset));
					}
				}break;

				case(yoyo::MaterialPropertyType::Int32):
				{
					ImGui::InputInt(name.c_str(), (int32_t*)((char*)material->PropertyData() + prop.offset));
					{
						material->SetProperty(name.c_str(), (float*)((char*)material->PropertyData() + prop.offset));
					}
				}break;

				default:
				{
					ImGui::Text("%s [?] Uknown Property Type", name.c_str());
				}break;
				}
			}

			bool recieve_shadows = material->receive_shadows;
			if (ImGui::Checkbox("Recieve Shadows", &recieve_shadows))
			{
				material->receive_shadows = recieve_shadows;
			}

			const std::string shader_name = "Shader : " + mesh_renderer.material->shader->name;
			if (ImGui::TreeNode(shader_name.c_str()))
			{
				ImGui::Checkbox("Instanced", &mesh_renderer.material->shader->instanced);
				ImGui::TreePop();
			}
			ImGui::EndChild();

			ImGui::SameLine();

			// Child 2: rounded border
			{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("Viewport", ImVec2(0, 150), ImGuiChildFlags_Border, window_flags);
				ImGui::Image(material->MainTexture(), ImVec2{ 150,150 });
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			ImGui::TreePop();
		}
		}, false);

	DrawComponentUI<psx::RigidBodyComponent>("RigidBody", m_focused_entity, [](psx::RigidBodyComponent& rb) {
		float mass = rb.GetMass();
		if (ImGui::DragFloat("Mass", &mass, 1.0f))
		{
			rb.SetMass(mass);
		}
		});

	// class RigidBodyComponent
	// {
	// public:
	// 	void AddForce(const yoyo::Vec3& force, ForceMode type);

	// 	// Locks an axis of rotation
	// 	void LockRotationAxis(const yoyo::Vec3& axis);
	// private:	
	// 	friend class PhysicsWorld;
	// 	physx::PxRigidActor* actor;
	// };
}
