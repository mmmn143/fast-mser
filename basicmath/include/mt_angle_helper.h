#pragma once


namespace basicmath {

	class mt_angle_helper {
	public:

		template<class T>
		static T normlize_angle(T angle) {
			T t_360 = T(360);
			if (angle > t_360) {
				angle -= t_360;
			} else if (angle < (T)0) {
				angle += t_360;
			}

			return angle;
		}

		static f64 radian_from_angle(f64 angle);
		static f64 angle_from_radian(f64 radian);

	};

}