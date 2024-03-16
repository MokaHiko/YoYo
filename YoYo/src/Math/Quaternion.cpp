#include "Quaternion.h"

namespace yoyo
{
	YAPI float NormalQuat(const Quat& q)
	{
		return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	YAPI Quat NormalizeQuat(const Quat& q)
	{
		float normal = NormalQuat(q);
		return { q.x / normal, q.y / normal, q.z / normal, q.w / normal };
	}

	YAPI Quat QuatFromAxisAngle(const Vec3& axis, float angle, bool normalize)
	{
		const float half_angle = 0.5f * angle;
		float s = Sin(half_angle);
		float c = Cos(half_angle);

		Quat q = { s * axis.x, s * axis.y, s * axis.z, c };
		if (normalize)
		{
			return NormalizeQuat(q);
		}

		return q;
	}

	YAPI Vec3 EulerAnglesFromQuat(const Quat& q)
	{
		/*
		impl_src: https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/

		Convert a quaternion into euler angles(roll, pitch, yaw)
		roll is rotation around x in radians(counterclockwise)
		pitch is rotation around y in radians(counterclockwise)
		yaw is rotation around z in radians(counterclockwise)
		*/
		float t0 = 2.0 * (q.w * q.x + q.y * q.z);
		float t1 = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		float roll_x = atan2(t0, t1);

		float t2 = +2.0 * (q.w * q.y - q.z * q.x);
		t2 = t2 > 1.0 ? 1.0 : t2;
		t2 = t2 < -1.0 ? -1.0 : t2;
		float pitch_y = asin(t2);

		float t3 = 2.0 * (q.w * q.z + q.x * q.y);
		float t4 = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
		float yaw_z = atan2(t3, t4);

		return { roll_x, pitch_y, yaw_z };
	}


	YAPI const Quat operator*(const Quat& q1, const Quat& q2)
	{
		Quat out_Quaternion = { 0, 0, 0, 1 };

		out_Quaternion.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;

		out_Quaternion.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;

		out_Quaternion.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;

		out_Quaternion.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;

		return out_Quaternion;
	}

	Quat& Quat::operator*=(const Quat& other)
	{
		Quat out_Quaternion = { 0, 0, 0, 1 };

		out_Quaternion.x = this->x * other.w + this->y * other.z - this->z * other.y + this->w * other.x;

		out_Quaternion.y = -this->x * other.z + this->y * other.w + this->z * other.x + this->w * other.y;

		out_Quaternion.z = this->x * other.y - this->y * other.x + this->z * other.w + this->w * other.z;

		out_Quaternion.w = -this->x * other.x - this->y * other.y - this->z * other.z + this->w * other.w;

		*this = out_Quaternion;
		return *this;
	}
}