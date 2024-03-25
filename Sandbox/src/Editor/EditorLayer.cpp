#include "EditorLayer.h"

#include <Core/Assert.h>
#include <Core/Application.h>
#include <Renderer/RendererLayer.h>
#include <ImGui/ImGuiLayer.h>

#include "ECS/Scene.h"

#include "Panel/SceneHierarchyPanel.h"
#include "Panel/InspectorPanel.h"
#include "Panel/ViewportPanel.h"
#include "Sandbox.h"

#include <Input/Input.h>

EditorLayer::EditorLayer(yoyo::Application* app)
{
	YASSERT(app != nullptr, "Invalid application!");
	m_app = app;
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnImGuiRender() 
{
	YASSERT(m_scene != nullptr, "Invalid scene!");

	if(yoyo::Input::GetKeyDown(yoyo::KeyCode::Key_backquote))
	{
		m_hide = !m_hide;
	}

	if(m_hide)
	{
		return;
	}

	for(Ref<IPanel>& panel : m_panels)
	{
		panel->Draw(m_scene);
	}
}

void EditorLayer::OnAttach() 
{
	m_panels.push_back(CreateRef<SceneHierarchyPanel>());
	m_panels.push_back(CreateRef<InspectorPanel>());

}

void EditorLayer::OnDetatch() {}

void EditorLayer::OnEnable() 
{
	if(yoyo::RendererLayer* renderer_layer = m_app->FindLayer<yoyo::RendererLayer>())
	{
		Ref<yoyo::Renderer> renderer = renderer_layer->GetRenderer();
		m_panels.push_back(CreateRef<ViewportPanel>(renderer));
	}

	// Get context from imgui layer
	ImGuiContext* ctx = yoyo::ImGuiLayer::GetContext();
	YASSERT(ctx != nullptr, "Invalid ImGuiContext!");
    ImGui::SetCurrentContext(ctx);
	ImGuizmo::SetImGuiContext(ctx);

	GameLayer* game_layer = m_app->FindLayer<GameLayer>();
	YASSERT(game_layer != nullptr, "Invalid game_layer!");

	// Get handle to scene
	m_scene = game_layer->GetScene();
	YASSERT(m_scene != nullptr, "Invalid scene!");

	m_hide = false;
}

void EditorLayer::OnDisable() 
{
	m_scene = nullptr;
}
