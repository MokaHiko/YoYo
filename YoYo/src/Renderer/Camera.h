#pragma once

#include "Math/Math.h"

namespace yoyo
{
    enum class CameraType
    {
        Orthographic,
        Perspective
    };

    // Defualts
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;

    class Camera
    {
    public:
        Camera(const Vec3& start_position = { 0.0f }, const Vec3& up = { 0.0f, 1.0f, 0.0f }, float start_yaw = YAW, float start_pitch = PITCH);
        ~Camera();

        void UpdateCameraVectors();

        const Mat4x4& View() const {return m_view;}
        const Mat4x4& Projection() const {return m_proj;}

        const Vec3& Front() const {return m_front;}

        float yaw;
        float pitch;
        Vec3 position;
    private:
		uint32_t m_aspect_ratio;
		float m_near;
		float m_far;

        Vec3 m_front;
        Vec3 m_right;
        Vec3 m_up;
        Vec3 m_world_up;

        Mat4x4 m_view;
        Mat4x4 m_proj;
        CameraType m_type;
    };
}