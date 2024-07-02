#pragma once

#include "Defines.h"

namespace yoyo
{
    union YAPI Vec2
    {
        Vec2() : x(0), y(0) { elements[0] = x; elements[1] = y; }
        Vec2(float val) : x(val), y(val) { elements[0] = x; elements[1] = y; }
        Vec2(float _x, float _y) : x(_x), y(_y) { elements[0] = x; elements[1] = y; }
        
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

        Vec2& operator*=(const Vec2& other);
    };

    union YAPI IVec2
    {
        int elements[2];

        struct
        {
            union
            {
                int x, r, s, u;
            };
            union
            {
                int y, g, t, v;
            };
        };
    };

    union YAPI Vec3
    {
        Vec3() : x(0), y(0), z(0) { elements[0] = x; elements[1] = y; elements[2] = z; }
        Vec3(float val) : x(val), y(val), z(val) { elements[0] = x; elements[1] = y; elements[2] = z; }
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { elements[0] = x; elements[1] = y; elements[2] = z; }

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

        Vec3& operator+=(float scalar);
        Vec3& operator-=(float scalar);
        Vec3& operator*=(float scalar);
        Vec3& operator/=(float scalar);

        Vec3& operator+=(const Vec3& other);
        Vec3& operator-=(const Vec3& other);
        Vec3& operator*=(const Vec3& other);

        bool operator==(const Vec3& other) const;
        bool operator!=(const Vec3& other) const;
    };

    union YAPI IVec3
    {
        int elements[3];

        struct
        {
            union
            {
                int x, r, s, u;
            };
            union
            {
                int y, g, t, v;
            };
            union
            {
                int z, b, p, w;
            };
        };

        bool operator==(const IVec3& other) const;
        bool operator!=(const IVec3& other) const;
    };

    union YAPI Vec4
    {
#ifdef YUSESIMD
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

        operator Vec3() const { return { x, y, z }; }
    };

    union YAPI IVec4
    {
#ifdef YUSESIMD
        alignas(16) _m128 data;
#endif
        alignas(16) int elements[4];

        struct
        {
            union
            {
                int x, r, s;
            };
            union
            {
                int y, g, t;
            };
            union
            {
                int z, b, p;
            };
            union
            {
                int w, a, q;
            };
        };
    };

    union YAPI Mat4x4
    {
        Mat4x4();
        ~Mat4x4();

        Mat4x4(float val);

        alignas(16) float data[16];
#ifdef YUSESIMD
        alignas(16) Vec4 rows[4];
#endif
        Mat4x4& operator*=(const Mat4x4& other);
        Mat4x4& operator*=(float scalar);
        float& operator[](int index);
    };

    // Definitions for Vec3 and IVec3 comparison operators
    inline bool Vec3::operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    inline bool Vec3::operator!=(const Vec3& other) const {
        return !(*this == other);
    }

    inline bool IVec3::operator==(const IVec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    inline bool IVec3::operator!=(const IVec3& other) const {
        return !(*this == other);
    }
}

