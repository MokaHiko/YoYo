#include "SceneHierarchyPanel.h"
#include <imgui.h>

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"

#include "Editor/EditorEvents.h"

SceneHierarchyPanel::SceneHierarchyPanel()
{

}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

void SceneHierarchyPanel::Draw(Scene* scene)
{
	ImGui::Begin("Entity Hierarchy");

	// Recursively draw scene hierarchy
	Entity root = scene->Root();
	DrawNodeRecursive(root, true);

	// Creating entity
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("Create Options");
		}
	}

	if (ImGui::BeginPopup("Create Options"))
	{
		if (ImGui::Button("New Entity"))
		{
			scene->Instantiate("New Entity", { 0.0f, 0.0f, 0.0f });
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

void SceneHierarchyPanel::DrawNodeRecursive(Entity node, bool open)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	static Entity focused = {};

	ImGuiTreeNodeFlags node_flags = base_flags;

	if(open)
	{
		node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	if(focused == node)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	const TransformComponent& node_transform = node.GetComponent<TransformComponent>();
	const TagComponent& node_tag = node.GetComponent<TagComponent>();

	std::string node_name = node_tag.tag;

	if(node_transform.children_count > 0)
	{
		node_name += " (" + std::to_string(node_transform.children_count) + ")";
	}

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)(node.Id()), node_flags, "%s", node_name.c_str());

	if (ImGui::IsItemClicked())
	{
		yoyo::EventManager::Instance().Dispatch(CreateRef<FocusEntityEvent>(node));
		focused = node;
	}

	if (node_open)
	{
		for (uint32_t i = 0; i < node_transform.children_count; i++)
		{
			Entity e = node_transform.children[i];
			DrawNodeRecursive(node_transform.children[i]);
		}

		ImGui::TreePop();
	}
}
