#include "ViewportPanel.h"

#include <ImGui/ImGuiLayer.h>

#include <Renderer/Renderer.h>
#include <Math/MatrixTransform.h>

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/RenderableComponents.h"

#include "Editor/EditorEvents.h"

ViewportPanel::ViewportPanel(Ref<yoyo::Renderer> renderer)
	:m_renderer(renderer)
{
	yoyo::EventManager::Instance().Subscribe(FocusEntityEvent::s_event_type, [&](Ref<yoyo::Event> event) {
		Ref<FocusEntityEvent> focused_event = std::static_pointer_cast<FocusEntityEvent>(event);
		m_focused_entity = focused_event->entity;
		return false;
	});
}

ViewportPanel::~ViewportPanel() {}

void ViewportPanel::Draw(Scene* scene)
{
	static bool open = true;
	ImGui::Begin("Viewport", &open, ImGuiWindowFlags_NoMove);

	auto viewport_size = ImGui::GetContentRegionAvail();
	auto viewport_pos_s = ImGui::GetCursorScreenPos();

	// TODO: Get aspect from application
	yoyo::Vec2 aspects = yoyo::Vec2{ 1920.0f, 1080.0f };

	ImGui::Image(m_renderer->GetViewPortTexture(), viewport_size);

	// Gizmos
	if (Entity camera = scene->FindEntityWithComponent<CameraComponent>())
	{
		Ref<yoyo::Camera> cam = camera.GetComponent<CameraComponent>().camera;
		if (m_focused_entity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float window_width = (float)(ImGui::GetWindowWidth());
			float window_height = (float)(ImGui::GetWindowHeight());
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);

			// Camera
			const yoyo::Mat4x4& view = cam->View();
			yoyo::Mat4x4 proj = cam->Projection();
			proj[5] *= -1;

			// Entity transform
			TransformComponent& transform = m_focused_entity.GetComponent<TransformComponent>();
			yoyo::Mat4x4 transform_matrix = transform.model_matrix;

			ImGuizmo::Manipulate(view.data, proj.data, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, transform_matrix.data);

			yoyo::Vec3 translation, rotation, scale = {};
			translation = yoyo::PositionFromMat4x4(transform_matrix);
			scale = yoyo::ScaleFromMat4x4(transform_matrix);
			// DecomposeTransform(transform_matrix, translation, rotation, scale);

			if (ImGuizmo::IsUsing())
			{
				transform.position = translation;
				transform.scale = scale;

				// TODO: Fix rotations 
				// glm::vec3 delta_rotation = rotation - transform.rotation;
				// transform.rotation += delta_rotation;
			}
		}
	}
	ImGui::End();
}
