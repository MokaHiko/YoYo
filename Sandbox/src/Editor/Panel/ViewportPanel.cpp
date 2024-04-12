#include "ViewportPanel.h"

#include <ImGui/ImGuiLayer.h>

#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <Renderer/Camera.h>
#include <Math/MatrixTransform.h>

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/RenderableComponents.h"

#include "Editor/EditorEvents.h"
#include "Input/Input.h"

#include "Physics/Collider.h"

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
			if(cam->GetType() == yoyo::CameraType::Orthographic)
			{
				ImGuizmo::SetOrthographic(true);
			}
			else
			{
				ImGuizmo::SetOrthographic(false);
			}
			ImGuizmo::SetDrawlist();

			float window_width = (float)(ImGui::GetWindowWidth());
			float window_height = (float)(ImGui::GetWindowHeight());
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);

			// Camera
			const yoyo::Mat4x4& view = cam->View();
			yoyo::Mat4x4 proj = cam->Projection();

			// Entity transform
			TransformComponent& transform = m_focused_entity.GetComponent<TransformComponent>();
			yoyo::Mat4x4 transform_matrix = transform.model_matrix;

			static ImGuizmo::OPERATION operations[] {
				ImGuizmo::TRANSLATE,
				ImGuizmo::ROTATE,
				ImGuizmo::SCALE
			};
			static int operation_index = 0;

			if(yoyo::Input::GetKeyDown(yoyo::KeyCode::Key_w))
			{	
				operation_index = 0;
			}

			if(yoyo::Input::GetKeyDown(yoyo::KeyCode::Key_e))
			{	
				operation_index = 1;
			}

			if(yoyo::Input::GetKeyDown(yoyo::KeyCode::Key_r))
			{	
				operation_index = 2;
			}

			ImGuizmo::Manipulate(view.data, proj.data, operations[operation_index], ImGuizmo::LOCAL, transform_matrix.data, NULL, false);

			yoyo::Vec3 translation, scale = {};
			translation = yoyo::PositionFromMat4x4(transform_matrix);
			scale = yoyo::ScaleFromMat4x4(transform_matrix);
			yoyo::Quat quat_rotation = yoyo::RotationFromMat4x4(transform_matrix);

			// Visualize colliders
			psx::BoxColliderComponent* box_collider = nullptr;
			if(m_focused_entity.TryGetComponent<psx::BoxColliderComponent>(&box_collider))
			{
				const yoyo::Vec3& half_extents = box_collider->GetHalfExtents();
				yoyo::Mat4x4 colldier_matrix = yoyo::TranslationMat4x4(translation) * yoyo::ScaleMat4x4(half_extents * 2.0f);
				ImGuizmo::DrawCubes(view.data, proj.data, colldier_matrix.data, 1);
			}

			// yoyo::Mat4x4 identity = {};
			// ImGuizmo::DrawGrid(view.data,proj.data, identity.data, 1000.0f);

			if (ImGuizmo::IsUsing())
			{
				transform.position = translation;
				//transform.scale = scale;
				transform.quat_rotation = quat_rotation;
			}
		}
	}
	ImGui::End();
}
