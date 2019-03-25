#pragma once



namespace basicmath {

	template<class T>
	class mt_top_k {
	public:

		mt_top_k(i32 k, b8 want_big_data = sys_true) {
			m_k = k;

			m_want_big_data = want_big_data;

			m_datas.reserve(m_k);
		}

		void traverse(const T& val) {
			if ((i32)m_datas.size() < m_k) {
				m_datas.push_back(val);

				if ((i32)m_datas.size() == m_k) {
					if (m_want_big_data) {
						mt_sort::init_heap<T>(m_datas, sys_false);
					} else {
						mt_sort::init_heap<T>(m_datas, sys_true);
					}
				}

			} else {
				if (m_want_big_data) {
					if (val > m_datas[0]) {
						m_datas[0] = val;

						mt_sort::heap_adjust<T>(0, &m_datas[0], m_k, sys_false);
					}
				} else {
					if (val < m_datas[0]) {
						m_datas[0] = val;

						mt_sort::heap_adjust<T>(0, &m_datas[0], m_k, sys_true);
					}
				}
			}
		}

		const vector<T>& get_top_k() {
			return m_datas;
		}

		void get_sorted_top_k(vector<T>& datas) {
			datas = m_datas;

			if (m_want_big_data) {
				sort(datas.begin(), datas.end(), greater<T>());
			} else {
				sort(datas.begin(), datas.end(), less<T>());
			}
		}

	protected:

		i32 m_k;
		b8 m_want_big_data;
		vector<T> m_datas;
	};


}