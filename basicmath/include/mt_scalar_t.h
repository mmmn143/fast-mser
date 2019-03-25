#pragma once



namespace basicmath {

	template<class T>
	class mt_scalar_t {
	public:

		mt_scalar_t(T v0 = 0, T v1 = 0, T v2 = 0, T v3 = 0) {
			value[0] = v0;
			value[1] = v1;
			value[2] = v2;
			value[3] = v3;
		}

		T& operator[](int index) {
			return value[index];
		}

		const T& operator[](int index) const {
			return value[index];
		}

		bool operator==(const mt_scalar_t& other) const {
			return mt_helper::compare_value(other.value[0], value[0]) == 0
				&& mt_helper::compare_value(other.value[1], value[1]) == 0
				&& mt_helper::compare_value(other.value[2], value[2]) == 0
				&& mt_helper::compare_value(other.value[3], value[3]) == 0;
		}

		T value[4];
	};

	typedef mt_scalar_t<double> mt_scalar;
}