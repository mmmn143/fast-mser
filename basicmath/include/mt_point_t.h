#pragma once



namespace basicmath {
	template<class T>
	class mt_point_t {
	public:

		mt_point_t() {
			m_x = -1;
			m_y = -1;
		}

		mt_point_t(T x, T y) {
			m_x = x;
			m_y = y;
		}

		mt_point_t operator+(const mt_point_t& other) const {
			return mt_point_t(m_x + other.m_x, m_y + other.m_y);
		}

		mt_point_t operator-(const mt_point_t& other) const {
			return mt_point_t(m_x - other.m_x, m_y - other.m_y);
		}

		bool operator==(const mt_point_t& other) const {
			return m_x == other.m_x && m_y == other.m_y;
		}

		f64 distance_to(const mt_point_t& other) {
			T square = (other.m_x - m_x) * (other.m_x - m_x) + (other.m_y - m_y) * (other.m_y - m_y);
			return sqrt((f64)square);
		}

		T m_x;
		T m_y;
	};

	typedef mt_point_t<int> mt_point;
	typedef mt_point_t<float> mt_point2f;
	typedef mt_point_t<double> mt_point2d;

	template<class T>
	void write(basicsys::sys_strcombine& str, const mt_point_t<T>& data) {
		str<<"["<<data.m_x<<","<<data.m_y<<"]";
	}
}