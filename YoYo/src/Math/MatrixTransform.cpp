#include "MatrixTransform.h"

#include <math.h>

namespace yoyo
{
    Mat4x4 OrthographicProjectionMat4x4(float left, float right, float bottom, float top, float near, float far)
    {
        return Mat4x4();
    }

    Mat4x4 PerspectiveProjectionMat4x4(float fov_radians, float aspect_ratio, float near, float far)
    {
        float half_tan_fov = tan(fov_radians * 0.5f);
        Mat4x4 out_matrix = {};

        out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
        out_matrix.data[5] = 1.0f / half_tan_fov;
        out_matrix.data[10] = -((far + near) / (far - near));
        out_matrix.data[11] = -1.0f;
        out_matrix.data[14] = -((2.0f * far * near) / (far - near));

        return out_matrix;
    }

    YAPI Mat4x4 TranslationMat4x4(const Vec3 &position)
    {
        Mat4x4 out_matrix = {};
        out_matrix[12] = position.x;
        out_matrix[13] = position.y;
        out_matrix[14] = position.z;

        return out_matrix;
    }

    YAPI Mat4x4 ScaleMat4x4(const Vec3 &scale)
    {
        Mat4x4 out_matrix = {};
        out_matrix[0] = scale.x;
        out_matrix[5] = scale.y;
        out_matrix[10] = scale.z;

        return out_matrix;
    }

    YAPI Mat4x4 RotateMat4x4(float angle_radians, const Vec3 &axis)
    {
        return Mat4x4();
    }

    YAPI Mat4x4 TransposeMat4x4(Mat4x4 &matrix)
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
}
