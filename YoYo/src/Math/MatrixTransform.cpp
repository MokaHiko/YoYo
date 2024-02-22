#include "MatrixTransform.h"

namespace yoyo
{
    Mat4x4 OrthographicProjectionMat4x4(float left, float right, float bottom, float top, float near, float far)
    {
        Mat4x4 out_matrix = {};

        float rl = 1.0f / (right - left);
        float tb = 1.0f / (top - bottom);
        float fn = 1.0f / (far - near);

        out_matrix.data[0] = 2.0f * rl;
        out_matrix.data[5] = 2.0f * tb;
        out_matrix.data[10] = -2.0f * fn;

        out_matrix.data[12] = -(right + left) * rl;
        out_matrix.data[13] = -(top + bottom) * tb;
        out_matrix.data[14] = -(far + near) * fn;

        return out_matrix;
    }

    Mat4x4 PerspectiveProjectionMat4x4(float fov_radians, float aspect_ratio, float near, float far)
    {
        Mat4x4 out_matrix = {};
        float half_tan_fov = Tan(fov_radians * 0.5f);

        out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
        out_matrix.data[5] = 1.0f / half_tan_fov;
        out_matrix.data[10] = -((far + near) / (far - near));
        out_matrix.data[11] = -1.0f;
        out_matrix.data[14] = -((2.0f * far * near) / (far - near));

        return out_matrix;
    }

    Mat4x4 LookAtMat4x4(const Vec3& position, const Vec3& target, const Vec3& up) {
        // RH
        Mat4x4 out_matrix = {};

        Vec3 z_axis = {};
        z_axis.x = target.x - position.x;
        z_axis.y = target.y - position.y;
        z_axis.z = target.z - position.z;

        z_axis = Normalize(z_axis);
        Vec3 x_axis = Normalize(Cross(z_axis, up));
        Vec3 y_axis = Cross(x_axis, z_axis);

        out_matrix.data[0] = x_axis.x;
        out_matrix.data[1] = y_axis.x;
        out_matrix.data[2] = -z_axis.x;
        out_matrix.data[3] = 0;
        out_matrix.data[4] = x_axis.y;
        out_matrix.data[5] = y_axis.y;
        out_matrix.data[6] = -z_axis.y;
        out_matrix.data[7] = 0;
        out_matrix.data[8] = x_axis.z;
        out_matrix.data[9] = y_axis.z;
        out_matrix.data[10] = -z_axis.z;
        out_matrix.data[11] = 0;
        out_matrix.data[12] = -Dot(x_axis, position);
        out_matrix.data[13] = -Dot(y_axis, position);
        out_matrix.data[14] = Dot(z_axis, position);
        out_matrix.data[15] = 1.0f;

        return out_matrix;
    }

    YAPI Mat4x4 TranslationMat4x4(const Vec3& position)
    {
        Mat4x4 out_matrix = {};
        out_matrix[12] = position.x;
        out_matrix[13] = position.y;
        out_matrix[14] = position.z;

        return out_matrix;
    }

    YAPI Mat4x4 ScaleMat4x4(const Vec3& scale)
    {
        Mat4x4 out_matrix = {};
        out_matrix[0] = scale.x;
        out_matrix[5] = scale.y;
        out_matrix[10] = scale.z;

        return out_matrix;
    }

    Mat4x4 RotateEulerX(float angle_radians) {
        Mat4x4 out_matrix = {};
        float c = Cos(angle_radians);
        float s = Sin(angle_radians);

        out_matrix.data[5] = c;
        out_matrix.data[6] = s;
        out_matrix.data[9] = -s;
        out_matrix.data[10] = c;
        return out_matrix;
    }


    Mat4x4 RotateEulerY(float angle_radians)
    {
        Mat4x4 out_matrix = {};
        float c = Cos(angle_radians);
        float s = Sin(angle_radians);

        out_matrix.data[0] = c;
        out_matrix.data[2] = -s;
        out_matrix.data[8] = s;
        out_matrix.data[10] = c;

        return out_matrix;
    }


    Mat4x4 RotateEulerZ(float angle_radians) {
        Mat4x4 out_matrix = {};

        float c = Cos(angle_radians);
        float s = Sin(angle_radians);

        out_matrix.data[0] = c;
        out_matrix.data[1] = s;
        out_matrix.data[4] = -s;
        out_matrix.data[5] = c;
        return out_matrix;
    }

    YAPI Mat4x4 RotateMat4x4(float angle_radians, const Vec3& axis)
    {
        return RotateEulerY(angle_radians);
    }

    YAPI Mat4x4 RotateEulerMat4x4(const Vec3& angles)
    {
        return RotateEulerX(angles.x) * RotateEulerY(angles.y) * RotateEulerZ(angles.z);
    }

    YAPI Mat4x4 TransposeMat4x4(Mat4x4& matrix)
    {
        Mat4x4 out_matrix = {};

        out_matrix.data[0] = matrix.data[0];
        out_matrix.data[1] = matrix.data[4];
        out_matrix.data[2] = matrix.data[8];
        out_matrix.data[3] = matrix.data[12];
        out_matrix.data[4] = matrix.data[1];
        out_matrix.data[5] = matrix.data[5];
        out_matrix.data[6] = matrix.data[9];
        out_matrix.data[7] = matrix.data[13];
        out_matrix.data[8] = matrix.data[2];
        out_matrix.data[9] = matrix.data[6];
        out_matrix.data[10] = matrix.data[10];
        out_matrix.data[11] = matrix.data[14];
        out_matrix.data[12] = matrix.data[3];
        out_matrix.data[13] = matrix.data[7];
        out_matrix.data[14] = matrix.data[11];
        out_matrix.data[15] = matrix.data[15];

        return out_matrix;
    }
	YAPI Vec3 PositionFromMat4x4(const Mat4x4& matrix)
    {
        Vec3 pos = {};
        pos.x = matrix.data[12];
        pos.y = matrix.data[13];
        pos.z = matrix.data[14];
        return pos;
    }
	YAPI Vec3 ScaleFromMat4x4(const Mat4x4& matrix)
    {
        Vec3 scale = {};
        scale.x = matrix.data[0];
        scale.y = matrix.data[5];
        scale.z = matrix.data[10];
        return scale;
    }

}
