#include "EditorLayer.h"

#include <Core/Assert.h>
#include <Core/Application.h>
#include <ImGui/ImGuiLayer.h>

#include "ECS/Scene.h"

#include "Panel/SceneHierarchyPanel.h"
#include "Panel/InspectorPanel.h"
#include "Sandbox.h"

EditorLayer::EditorLayer(yoyo::Application* app)
{
	yoyo::Assert(app != nullptr, "Invalid application!");
	m_app = app;
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnImGuiRender() 
{
	yoyo::Assert(m_scene != nullptr, "Invalid scene!");

	for(Ref<IPanel>& panel : m_panels)
	{
		panel->Draw(m_scene);
	}
}

void EditorLayer::OnEnable() 
{
	// Get context from imgui layer
	ImGuiContext* ctx = yoyo::ImGuiLayer::GetContext();
	yoyo::Assert(ctx != nullptr, "Invalid ImGuiContext!");
    ImGui::SetCurrentContext(ctx);

	GameLayer* game_layer = m_app->FindLayer<GameLayer>();
	yoyo::Assert(game_layer != nullptr, "Invalid game_layer!");

	m_scene = game_layer->GetScene();
	yoyo::Assert(m_scene != nullptr, "Invalid scene!");

	m_panels.push_back(CreateRef<SceneHierarchyPanel>());
	m_panels.push_back(CreateRef<InspectorPanel>());
}

void EditorLayer::OnDisable() 
{
}
