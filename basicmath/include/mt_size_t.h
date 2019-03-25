#pragma once



namespace basicmath {
	template<class T>
	class mt_size_t {
	public:

		mt_size_t(int width = -1, int height = -1) {
			m_width = width;
			m_height = height;
		}

		T m_width;
		T m_height;
	};

	typedef mt_size_t<int> mt_size;
}