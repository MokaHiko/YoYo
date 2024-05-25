#include "Camera.h"

#include "Core/Log.h"
#include "Math/MatrixTransform.h"

namespace yoyo
{
	Camera::Camera(const Vec3& start_position, const Vec3& up, float start_yaw, float start_pitch, float aspect_ratio, float fov)
		: m_near(0.01f), m_far(1000.0f), m_type(CameraType::Perspective)
	{
		m_world_up = WORLD_UP;

		position = start_position;
		yaw = start_yaw;
		pitch = start_pitch;

		m_up = up;
		m_aspect_ratio = aspect_ratio;
		m_fov = fov;

		UpdateCameraVectors();

		YTRACE("Camera Created!");
	}

	Camera::~Camera() {}

	void Camera::UpdateCameraVectors()
	{
		// TODO: Do on app resize event
		switch (m_type)
		{
		case(CameraType::Perspective):
		{
			m_proj = PerspectiveProjectionMat4x4(DegToRad(m_fov), m_aspect_ratio, m_near, m_far);
			m_proj[5] *= 1; // Reconfigure y values as positive for vulkan
		}break;
		case(CameraType::Orthographic):
		{
			float width = 16 * 6.0f;
			float height = 9 * 6.0f;

			float half_width = static_cast<float>(width)  / 2.0f;
			float half_height = static_cast<float>(height) / 2.0f;
			m_proj = OrthographicProjectionMat4x4(-half_width, half_width, -half_height, half_height, -1000.0f, 1000);
			m_proj[5] *= 1.0f;
		}break;
		default:
			break;
		}


		if (true)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// Calculate new front vector
		Vec3 front = {};
		front.x = Cos(DegToRad(yaw)) * Cos(DegToRad(pitch));
		front.y = Sin(DegToRad(pitch));
		front.z = Sin(DegToRad(yaw)) * Cos(DegToRad(pitch));

		m_front = Normalize(front);

		m_right = Normalize(Cross(m_front, m_world_up));
		m_up = Normalize(Cross(m_right, m_front));

		m_view = LookAtMat4x4(position, position + m_front, m_up);
	}

	void Camera::SetType(CameraType type)
	{
		m_type = type;
	}
}
