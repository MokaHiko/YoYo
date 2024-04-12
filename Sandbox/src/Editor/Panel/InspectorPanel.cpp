#include "InspectorPanel.h"

#include <Events/Event.h>
#include <ImGui/ImGuiLayer.h>

#include <Math/MatrixTransform.h>

#include "Editor/EditorEvents.h"
#include "ECS/Components/Components.h"

#include "ECS/Components/RenderableComponents.h"
#include "Physics/PhysicsTypes.h"

#include <Renderer/Shader.h>
#include <Renderer/Animation.h>
#include <Renderer/Material.h>

#include <Scripts/CameraController.h>
#include <ParticleSystem/Particles.h>

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

void MaterialInspectorNode(Ref<yoyo::Material> material)
{
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

		yoyo::Vec4 mat_color = material->GetColor();
		if (ImGui::DragFloat3("Color", mat_color.elements, 0.01f))
		{
			material->SetColor(mat_color);
		}

		// Simple selection popup (if you want to show the current selection inside the Button itself,
		// you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
		int& render_mode = (int&)material->GetRenderMode();
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
			ImGui::BeginChild("Viewport", ImVec2(0, 150), ImGuiChildFlags_Border, window_flags);
			ImGui::Image(material->MainTexture(), ImVec2{ 150,150 });
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
		ImGui::TreePop();
	}
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
				ImGui::Text("Vertices: %d", mesh->GetVertexCount());
				ImGui::Text("Indices: %d", mesh->GetIndexCount());
			}break;

			case yoyo::MeshType::Skinned: {
				Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(mesh_renderer.mesh);
				ImGui::Text("Name: %s", mesh->name.c_str());
				ImGui::Text("Mesh Id: %u", mesh->Id());
				ImGui::Text("Renderer Id: %u", mesh_renderer.mesh_object->Id());
				ImGui::Text("Vertices: %d", mesh->GetVertexCount());
				ImGui::Text("Indices: %d", mesh->GetIndexCount());

				const std::string bone_header = "Skeletal Hierarchy : " + mesh->skeletal_hierarchy->name + "(" + std::to_string(mesh->bones.size()) + ")";
				if (ImGui::TreeNode(bone_header.c_str()))
				{
					// TODO: Traverse heirarchy
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
		MaterialInspectorNode(material);
		}, false);

	DrawComponentUI<AnimatorComponent>("Animator", m_focused_entity, [](AnimatorComponent& animator_component) {
		Ref<yoyo::Animator> animator = animator_component.animator;

		if (!animator)
		{
			ImGui::Text("Null reference to animator!");
			return;
		}

		if (ImGui::TreeNodeEx("Animations", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (int i = 0; i < animator->animations.size(); i++)
			{
				Ref<yoyo::Animation>& animation = animator->animations[i];

				ImGuiTreeNodeFlags flags = {};
				flags |= i == animator->GetCurrentAnimationIndex() ? ImGuiTreeNodeFlags_Selected : 0;
				if (ImGui::TreeNodeEx(animation->name.c_str(), flags))
				{
					ImGui::Text("Duration: %.2f s", animation->Duration());
					ImGui::DragFloat("Ticks", &animation->ticks);
					ImGui::DragFloat("Ticks Per Second", &animation->ticks_per_second);

					if (ImGui::TreeNodeEx("Timeline", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Timeline");
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		});

	DrawComponentUI<psx::RigidBodyComponent>("RigidBody", m_focused_entity, [](psx::RigidBodyComponent& rb) {
		float mass = rb.GetMass();
		if (ImGui::DragFloat("Mass", &mass, 1.0f))
		{
			rb.SetMass(mass);
		}
	});

	DrawComponentUI<psx::BoxColliderComponent>("BoxCollider", m_focused_entity, [](psx::BoxColliderComponent& box_coillder) {
		yoyo::Vec3 half_extents = box_coillder.GetHalfExtents();
		if (ImGui::DragFloat3("Extents", half_extents.elements))
		{
			box_coillder.SetHalfExtents(half_extents);
		}
	});


	DrawComponentUI<ParticleSystemComponent>("ParticleSystem", m_focused_entity, [](ParticleSystemComponent& particle_system_component) {
		int max_particles = particle_system_component.GetMaxParticles();
		if (ImGui::DragInt("Max particles", &max_particles))
		{
			particle_system_component.SetMaxParticles(max_particles);
		}

		yoyo::Vec3 gravity_scale = particle_system_component.GetGravityScale();
		if (ImGui::DragFloat3("Max particles", gravity_scale.elements))
		{
			particle_system_component.SetGravityScale(gravity_scale);
		}

		int emission_rate = particle_system_component.GetEmissionRate();
		if (ImGui::DragInt("Max particles", &emission_rate))
		{
			particle_system_component.SetEmissionRate(emission_rate);
		}

		static const char* particle_space_strings[] = 
		{
			"Local",
			"World",
    	};

		int simulation_space = (int)particle_system_component.GetSimulationSpace();
		YASSERT(simulation_space < (int)yoyo::ParticleSystemSpace::Max, "Uknown particle simulation space!");

		ImGui::Text("Simulation Space"); ImGui::SameLine();
		if (ImGui::Button(particle_space_strings[simulation_space])) { ImGui::OpenPopup("SimulationSpacePopUp"); }

		if (ImGui::BeginPopup("SimulationSpacePopUp"))
		{
			if (ImGui::Selectable(particle_space_strings[0], 0 == simulation_space))
			{
				particle_system_component.SetSimulationSpace((yoyo::ParticleSystemSpace)0);
			}

			if (ImGui::Selectable(particle_space_strings[1], 1 == simulation_space))
			{
				particle_system_component.SetSimulationSpace((yoyo::ParticleSystemSpace)1);
			}

			ImGui::EndPopup();
		}

		const std::vector<Ref<yoyo::Material>>& materials = particle_system_component.GetMaterials();
		for(int i = 0; i < materials.size(); i++)
		{
			MaterialInspectorNode(materials[i]);
		}
	});

	// TODO: Script template
	DrawComponentUI<CameraControllerComponent>("RigidBody", m_focused_entity, [](CameraControllerComponent& camera_component) {
		Entity target = camera_component.follow_target;

		ImGui::Checkbox("Follow", &camera_component.follow);
		ImGui::Text("%u", camera_component.follow_target);

		ImGui::DragFloat3("Follow Offset", camera_component.follow_offset.elements);

		ImGui::DragFloat("Pitch", &camera_component.pitch);
		ImGui::DragFloat("Yaw", &camera_component.yaw);
		});
}
