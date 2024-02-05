#include "Camera.h"

#include "Core/Log.h"
#include "Math/MatrixTransform.h"

namespace yoyo
{
	Camera::Camera(const Vec3& start_position, const Vec3& up, float start_yaw, float start_pitch)
		: m_near(0.1f), m_far(1000.0f)
	{
		position = start_position;
		yaw = start_yaw;
		pitch = start_pitch;

		m_up = up;
		m_aspect_ratio = 720.0f / 480.0f;
		UpdateCameraVectors();

		YTRACE("Camera Created!");
	}

	Camera::~Camera()
	{
	}

	void Camera::UpdateCameraVectors()
	{
		// Calculate new front vector
		Vec3 front = {};
		front.x = Cos(DegToRad(yaw)) * Cos(DegToRad(pitch));
		front.y = Sin(DegToRad(pitch));
		front.z = Sin(DegToRad(yaw)) * Cos(DegToRad(pitch));

		m_front = Normalize(front);

		m_right = Normalize(Cross(m_front, m_world_up));
		m_up = Normalize(Cross(m_right, m_front));

		m_proj = PerspectiveProjectionMat4x4(DegToRad(90.0f), m_aspect_ratio, m_near, m_far);
		m_proj[5] *= -1; // Reconfigure y values as positive for vulkan

		m_view = LookAtMat4x4(position, position + m_front, m_up);
	}

}
