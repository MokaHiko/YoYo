#include "Math.h"

namespace yoyo
{
    const Vec2 operator*(const Vec2 &v1, const Vec2 &v2)
    {
        Vec2 product = {};
        product.x = v1.x * v2.x;
        product.y = v1.y * v2.y;

        return product;
    }

    const Vec3 operator*(const Vec3 & v1, const Vec3 & v2)
    {
        Vec3 product = {};
        product.x = v1.x * v2.x;
        product.y = v1.y * v2.y;
        product.z = v1.z * v2.z;

        return product;
    }

    const Vec2 operator+(const Vec2 &v1, const Vec2 &v2)
    {
        Vec2 sum = {};
        sum.x = v1.x + v2.x;
        sum.y = v1.y + v2.y;

        return sum;
    }

    const Vec3 operator+(const Vec3 &v1, const Vec3 &v2)
    {
        Vec3 sum = {};
        sum.x = v1.x + v2.x;
        sum.y = v1.y + v2.y;
        sum.z = v1.z + v2.z;

        return sum;
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

        Mat4x4 temp = m1;
        float* temp_data_ptr = temp.data;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                *dst = 
                temp_data_ptr[0] * m2.data[0 + j] + 
                temp_data_ptr[1] * m2.data[4 + j] + 
                temp_data_ptr[2] * m2.data[8 + j] + 
                temp_data_ptr[3] * m2.data[12 + j];

                dst++;
            }
            temp_data_ptr += 4;
        }

        return out_matrix;
	}
}