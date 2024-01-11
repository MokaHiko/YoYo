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

	const Mat4x4 operator*(const Mat4x4& v1, float scalar)
	{
        Mat4x4 out = v1;
        out *= scalar;

        return out;
	}
}