#pragma once



namespace basicmath {

	template<class T>
	class mt_range_t {
	public:

		mt_range_t() {
			m_start = -1;
			m_end = -1;
		}

		mt_range_t(T start, T end) {
			m_start = start;
			m_end = end;
		}

		
		T m_start;
		T m_end;

		T size() const {
			return m_end - m_start;
		}
	};

	typedef mt_range_t<i32> mt_range;

	template<class T>
	void write(basicsys::sys_strcombine& str, const mt_range_t<T>& data) {
		str<<"["<<data.m_start<<","<<data.m_end<<"]";
	}
}