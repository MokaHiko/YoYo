#include "InspectorPanel.h"

#include <Events/Event.h>
#include <imgui.h>

#include "Editor/EditorEvents.h"
#include "ECS/Components/Components.h"

#include "ECS/Components/RenderableComponents.h"

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
		ImGui::Text("Position: ");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);
		ImGui::DragFloat3("##Position", transform.position.elements);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Rotation: ");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);
		ImGui::DragFloat3("##Rotation", transform.rotation.elements);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Scale: ");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(label_width * 3.0f);
		ImGui::DragFloat3("##Scale", transform.scale.elements);
		ImGui::PopItemWidth();

		ImGui::EndTable();
		}, true);

	DrawComponentUI<MeshRendererComponent>("MeshRenderer", m_focused_entity, [](MeshRendererComponent& mesh_renderer) {
		HelpMarker("Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window.");
		if (ImGui::TreeNode("Mesh"))
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::BeginChild("Details", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 150), ImGuiChildFlags_None, window_flags);
			ImGui::Separator();
			ImGui::Text("Name: %s", mesh_renderer.mesh->name.c_str());
			ImGui::Text("Id: %u", mesh_renderer.mesh->Id());
			ImGui::Text("Vertices: %d", mesh_renderer.mesh->vertices.size());
			ImGui::Text("Indices: %d", mesh_renderer.mesh->indices.size());
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

			ImGui::DragFloat3("Color", material->color.elements);


			ImGui::SeparatorText("Properties");
			for(auto& it : material->GetProperties())
			{
				ImGui::Text("%s", it.first.c_str());
			}

			// bool receive_shadows; // Sets whether material receives shadows
			// bool instanced; // Sets whether material uses instancing.
			// MaterialRenderMode render_mode; // Render mode of material

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

				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			ImGui::TreePop();
		}
		}, false);
}
