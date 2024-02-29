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

	YAPI Quat QuatFromAxisAngle(Vec3 axis, float angle, bool normalize)
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

	YAPI const Quat operator*(const Quat& q1, const Quat& q2)
	{
		Quat out_Quaternion = { 0, 0, 0, 1 };

		out_Quaternion.x = q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;

		out_Quaternion.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;

		out_Quaternion.z = q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;

		out_Quaternion.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;

		return out_Quaternion;
	}
}