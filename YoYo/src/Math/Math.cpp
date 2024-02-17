#include "Math.h"

#include "Core/Log.h"

#include <math.h>

namespace yoyo
{
    const Vec2 operator*(const Vec2& v1, const Vec2& v2)
    {
        Vec2 product = {};
        product.x = v1.x * v2.x;
        product.y = v1.y * v2.y;

        return product;
    }

    const Vec3 operator*(const Vec3& v1, const Vec3& v2)
    {
        Vec3 product = {};
        product.x = v1.x * v2.x;
        product.y = v1.y * v2.y;
        product.z = v1.z * v2.z;

        return product;
    }

    const Vec2 operator+(const Vec2& v1, const Vec2& v2)
    {
        Vec2 sum = {};
        sum.x = v1.x + v2.x;
        sum.y = v1.y + v2.y;

        return sum;
    }

    const Vec3 operator+(const Vec3& v1, const Vec3& v2)
    {
        Vec3 sum = {};
        sum.x = v1.x + v2.x;
        sum.y = v1.y + v2.y;
        sum.z = v1.z + v2.z;

        return sum;
    }

	const Vec3 operator*(const Vec3& v1, float scalar)
	{
        Vec3 out = {};
        out.x = v1.x * scalar;
        out.y = v1.y * scalar;
        out.z = v1.z * scalar;

        return out;
	}

	const Vec3 operator*(Vec3 v1, float scalar)
	{
        Vec3 out = {};
        out.x = v1.x * scalar;
        out.y = v1.y * scalar;
        out.z = v1.z * scalar;

        return out;
	}

    const Vec4 operator*(const Vec4& v1, float scalar)
    {
        Vec4 out = {};
        out.x = v1.x * scalar;
        out.y = v1.y * scalar;
        out.z = v1.z * scalar;
        out.w = v1.w * scalar;

        return out;
    }

    const Vec3 operator/(const Vec3& v1, float scalar)
    {
        YASSERT(scalar != 0, "Cannot divide by 0!");
        return { v1.x / scalar, v1.y / scalar, v1.z / scalar };
    }

    const Mat4x4 operator*(const Mat4x4& v1, float scalar)
    {
        Mat4x4 out = v1;
        out *= scalar;

        return out;
    }

    const Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2)
    {
        Mat4x4 out_matrix = {};
        float* dst = out_matrix.data;

        Mat4x4 temp = m2;
        float* temp_data_ptr = temp.data;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                *dst =
                    temp_data_ptr[0] * m1.data[0 + j] +
                    temp_data_ptr[1] * m1.data[4 + j] +
                    temp_data_ptr[2] * m1.data[8 + j] +
                    temp_data_ptr[3] * m1.data[12 + j];

                dst++;
            }
            temp_data_ptr += 4;
        }

        return out_matrix;
    }

    const float Length(const Vec3& v1)
    {
        return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
    }

    const Vec3 Normalize(const Vec3& v1)
    {
        return v1 / Length(v1);
    }

    const Vec3 Cross(const Vec3& v1, const Vec3& v2)
    {
        return { v1.y * v2.z - v1.z * v2.y,
                 v1.z * v2.x - v1.x * v2.z,
                 v1.x * v2.y - v1.y * v2.x };
    }

    const float Dot(const Vec3& v1, const Vec3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    float Cos(float rad)
    {
        return cos(rad);
    }

    float Sin(float rad)
    {
        return sin(rad);
    }

	float Tan(float rad)
	{
		return tan(rad);
	}

    float DegToRad(float deg)
    {
        return deg * (Y_PI / 180.0f);
    }

    float RadToDeg(float rad)
    {
        return (rad * 180.0f) / Y_PI;
    }
}