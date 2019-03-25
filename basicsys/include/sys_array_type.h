#pragma once

namespace basicsys {



	template<class T, i32 fixed_size = 1>
	class sys_array_type {
	public:

		sys_array_type() {

		}

		sys_array_type(const f64* value) {
			for (i32 i = 0; i < fixed_size; ++i) {
				m_value[i] = T(value[i]);
			}
		}

		b8 operator==(const sys_array_type<T, fixed_size>& other) const {
			if (sizeof(m_value) != sizeof(other.m_value)) {
				return false;
			}

			i32 number = sizeof(m_value) / sizeof(T);

			for (i32 i = 0; i < number; ++i) {
				if (m_value[i] != other.m_value[i]) {
					return sys_false;
				}
			}

			return sys_true;
		}

		b8 operator!=(const sys_array_type<T, fixed_size>& other) const {
			return !(*this == other);
		}

		i32 size() const {
			return fixed_size;
		}

		i32 memory_size() const {
			return sizeof(m_value);
		}
		
		T m_value[fixed_size];

	};

}