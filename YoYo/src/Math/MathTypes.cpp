#include "MathTypes.h"

namespace yoyo
{
    Vec2 &Vec2::operator*=(const Vec2 &other)
    {
        this->x *= other.x;
        this->y *= other.y;

        return *this;
    }

	Vec3& Vec3::operator+=(float scalar)
	{
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;

        return *this;
	}

	Vec3& Vec3::operator-=(float scalar)
	{
        this->x -= scalar;
        this->y -= scalar;
        this->z -= scalar;

        return *this;
	}

	Vec3& Vec3::operator*=(float scalar)
	{
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;

        return *this;
	}

	Vec3& Vec3::operator/=(float scalar)
	{
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;

        return *this;
	}

    Vec3 &Vec3::operator*=(const Vec3 &other)
    {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;

        return *this;
    }

	Vec3& Vec3::operator+=(const Vec3& other)
	{
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;

        return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& other)
	{
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;

        return *this;
	}

    Mat4x4::Mat4x4()
    {
        memset(data, 0, sizeof(float) * 16);

        data[0] = 1;
        data[5] = 1;
        data[10] = 1;
        data[15] = 1;
    }

    Mat4x4::~Mat4x4() {}

	Mat4x4::Mat4x4(float val)
	{
        memset(data, val, sizeof(float) * 16);
	}

    Mat4x4 &Mat4x4::operator*=(const Mat4x4 &other)
    {
        float* dst = this->data;

        Mat4x4 temp = *this;
        float* temp_data_ptr = temp.data;

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                *dst = 
                temp_data_ptr[0] * other.data[0 + j] + 
                temp_data_ptr[1] * other.data[4 + j] + 
                temp_data_ptr[2] * other.data[8 + j] + 
                temp_data_ptr[3] * other.data[12 + j];

                dst++;
            }
            temp_data_ptr += 4;
        }

        return *this;
    }

	Mat4x4& Mat4x4::operator*=(float scalar)
	{
        for(int i = 0; i < 16; i++)
        {
            data[i] *= scalar;
        }

        return *this;
	}

    float& Mat4x4::operator[](int index)
    {
        return this->data[index];
    }
}