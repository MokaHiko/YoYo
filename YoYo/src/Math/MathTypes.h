#pragma once

#include "Defines.h"

namespace yoyo
{
    union YAPI Vec2
    {
        float elements[2];

        struct
        {
            union
            {
                float x, r, s, u;
            };
            union
            {
                float y, g, t, v;
            };
        };

        Vec2 &operator*=(const Vec2 &other);
    };

    union YAPI Vec3
    {
        float elements[3];

        struct
        {
            union
            {
                float x, r, s, u;
            };
            union
            {
                float y, g, t, v;
            };
            union
            {
                float z, b, p, w;
            };
        };

        Vec3 &operator+=(float scalar);
        Vec3 &operator-=(float scalar);
        Vec3 &operator*=(float scalar);
        Vec3 &operator/=(float scalar);

        Vec3 &operator+=(const Vec3 &other);
        Vec3 &operator-=(const Vec3 &other);
        Vec3 &operator*=(const Vec3 &other);
    };

    union YAPI Vec4
    {
#ifdef YUSESIMD
        // Use SIMD
        alignas(16) _m128 data;
#endif
        alignas(16) float elements[4];
        struct
        {
            union
            {
                float x, r, s;
            };
            union
            {
                float y, g, t;
            };
            union
            {
                float z, b, p;
            };
            union
            {
                float w, a, q;
            };
        };

        operator Vec3() const { return {x,y,z}; }
    };

    struct YAPI Quat
    {
    private:
        Vec4 m_quat;
    };

    /*
        By Default the YoYo math library assumes column major.
        Matrices by default are the identity matrix nxn.
    */
    union YAPI Mat4x4
    {
        Mat4x4();
        ~Mat4x4();

        Mat4x4(float val);

        alignas(16) float data[16];
#ifdef YUSESIMD
        alignas(16) Vec4 rows[4];
#endif
        Mat4x4 &operator*=(const Mat4x4 &other);
        Mat4x4 &operator*=(float scalar);
        float &operator[](int index);
    };

};