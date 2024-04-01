#include "MatrixTransform.h"

#include "Core/Assert.h"
#include "Quaternion.h"

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

	YAPI float Trace4x4(const Mat4x4& matrix)
	{
        return matrix.data[0] + matrix.data[5] + matrix.data[10] + matrix.data[15];
	}

    YAPI Mat4x4 QuatToMat4x4(const Quat& q)
    {
        Mat4x4 out_matrix = {};

        // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

        Quat n = NormalizeQuat(q);

        // TODO: In row major this needs to be transposed
        out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
        out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
        out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

        out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
        out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
        out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

        out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
        out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
        out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

        return out_matrix;
    }

    YAPI Mat4x4 InverseMat4x4(const Mat4x4& matrix)
    {
        const float* m = matrix.data;

        float t0 = m[10] * m[15];
        float t1 = m[14] * m[11];
        float t2 = m[6] * m[15];
        float t3 = m[14] * m[7];
        float t4 = m[6] * m[11];
        float t5 = m[10] * m[7];
        float t6 = m[2] * m[15];
        float t7 = m[14] * m[3];
        float t8 = m[2] * m[11];
        float t9 = m[10] * m[3];
        float t10 = m[2] * m[7];
        float t11 = m[6] * m[3];
        float t12 = m[8] * m[13];
        float t13 = m[12] * m[9];
        float t14 = m[4] * m[13];
        float t15 = m[12] * m[5];
        float t16 = m[4] * m[9];
        float t17 = m[8] * m[5];
        float t18 = m[0] * m[13];
        float t19 = m[12] * m[1];
        float t20 = m[0] * m[9];
        float t21 = m[8] * m[1];
        float t22 = m[0] * m[5];
        float t23 = m[4] * m[1];

        Mat4x4 out_matrix;
        float* o = out_matrix.data;

        o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) -
            (t1 * m[5] + t2 * m[9] + t5 * m[13]);
        o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) -
            (t0 * m[1] + t7 * m[9] + t8 * m[13]);
        o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) -
            (t3 * m[1] + t6 * m[5] + t11 * m[13]);
        o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) -
            (t4 * m[1] + t9 * m[5] + t10 * m[9]);

        float d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

        o[0] = d * o[0];
        o[1] = d * o[1];
        o[2] = d * o[2];
        o[3] = d * o[3];
        o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) -
            (t0 * m[4] + t3 * m[8] + t4 * m[12]));
        o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) -
            (t1 * m[0] + t6 * m[8] + t9 * m[12]));
        o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) -
            (t2 * m[0] + t7 * m[4] + t10 * m[12]));
        o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) -
            (t5 * m[0] + t8 * m[4] + t11 * m[8]));
        o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) -
            (t13 * m[7] + t14 * m[11] + t17 * m[15]));
        o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) -
            (t12 * m[3] + t19 * m[11] + t20 * m[15]));
        o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) -
            (t15 * m[3] + t18 * m[7] + t23 * m[15]));
        o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) -
            (t16 * m[3] + t21 * m[7] + t22 * m[11]));
        o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) -
            (t16 * m[14] + t12 * m[6] + t15 * m[10]));
        o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) -
            (t18 * m[10] + t21 * m[14] + t13 * m[2]));
        o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) -
            (t22 * m[14] + t14 * m[2] + t19 * m[6]));
        o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) -
            (t20 * m[6] + t23 * m[10] + t17 * m[2]));

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

        if(!(FloatCompare(scale.x, scale.y) && FloatCompare(scale.y, scale.z)))
        {
            // YERROR("Non uniform scale: %.2f %.2f %.2f", scale.x, scale.y, scale.z);
        }

        return scale;
    }

	YAPI Quat RotationFromMat4x4(const Mat4x4& matrix)
	{
        Mat4x4 rot_mat = matrix;

        // Normalize rotation
        for(int i = 0; i < 3; i++)
        {
            float scale_factor = rot_mat[0 + i ] * rot_mat[0 + i] + rot_mat[4 + i] * rot_mat[4 + i] + rot_mat[8 + i] * rot_mat[8 + i];
            float inverse_scale_factor = 1.0f / scale_factor;

            rot_mat[0 + i] *= inverse_scale_factor; rot_mat[4 + i] *= inverse_scale_factor; rot_mat[8 + i] *= inverse_scale_factor;
        }

        Quat quat = {};
        quat.w = sqrt(fmax(0, 1 + Trace4x4(rot_mat)) / 2.0f);
        quat.x = sqrt(fmax(0, 1 + rot_mat[0] - rot_mat[5] - rot_mat[10]) / 4.0f);
        quat.y = sqrt(fmax(0, 1 - rot_mat[0] + rot_mat[5] - rot_mat[10]) / 4.0f);
        quat.z = sqrt(fmax(0, 1 - rot_mat[0] - rot_mat[5] + rot_mat[10]) / 4.0f);

        // Correct signs
        if(rot_mat[0] - rot_mat[5] - rot_mat[10] < 0)
        {
            quat.x *= -1.0f;
        }

        if(rot_mat[0] - rot_mat[5] - rot_mat[10] < 0)
        {
            quat.x *= -1.0f;
        }

        if(rot_mat[5] - rot_mat[0] - rot_mat[10] < 0)
        {
            quat.y *= -1.0f;
        }

        if(rot_mat[10] - rot_mat[0] - rot_mat[5] < 0)
        {
            quat.z *= -1.0f;
        }

        return NormalizeQuat(quat);
	}

}
