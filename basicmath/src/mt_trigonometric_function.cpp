#include "stdafx.h"

#include "mt_trigonometric_function.h"

namespace basicmath {
	static f64 mt_angle_cos_cache[360];
	static f64 mt_angle_sin_cache[360];
	static f64 mt_angle_tan_cache[360];

	class mt_auto_trigonometric_function_initializatizer {
	public:

		mt_auto_trigonometric_function_initializatizer() {
			for (i32 i = 0; i < 360; ++i) {
				f64 radian = mt_angle_helper::radian_from_angle(i);
				mt_angle_cos_cache[i] = cos(radian);
				mt_angle_sin_cache[i] = sin(radian);
			}
		}

	};




	static mt_auto_trigonometric_function_initializatizer mt_auto_trigonometric_function_initializatizer_instance;
}

f64 mt_trigonometric_function::angle_sin(i32 angle) {
	basiclog_assert2(angle >= 0 && angle < 360);

	return mt_angle_sin_cache[angle];
}

f64 mt_trigonometric_function::angle_cos(i32 angle) {
	basiclog_assert2(angle >= 0 && angle < 360);

	return mt_angle_cos_cache[angle];
}