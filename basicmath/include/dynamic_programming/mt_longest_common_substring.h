#pragma once

namespace basicmath {

	template<class T>
	class mt_longest_common_substring {
	public:

		mt_longest_common_substring(const vector<T>& str_a, const vector<T>& str_b) {
			basiclog_assert2(!str_a.empty() && !str_b.empty());

			m_length = 0;
			m_a_begin_index = -1;
			m_b_begin_index = -1;

			m_a_size = (i32)str_a.size();
			m_b_size = (i32)str_b.size();

			m_maxtrix.resize(m_a_size * m_b_size, 0);

			for (i32 i = 0; i < m_a_size; ++i) {
				m_maxtrix[i] = (str_b[0] == str_a[i] ? 1 : 0);

				for (i32 j = 1; j < m_b_size; ++j) {
					m_maxtrix[j * m_a_size] = (str_a[0] == str_b[j] ? 1 : 0);

					if (i > 0 && j > 0) {
						if (str_a[i] == str_b[j]) {
							m_maxtrix[j * m_a_size + i] = m_maxtrix[(j - 1) * m_a_size + i - 1] + 1;
						}
					}

					if (m_length < m_maxtrix[j * m_a_size + i]) {
						m_length = m_maxtrix[j * m_a_size + i];
						m_a_begin_index = i - m_length + 1;
						m_b_begin_index = j - m_length + 1;
					}
				}
			}
		}

		i32 a_begin_index() const {
			return m_a_begin_index;
		}

		i32 b_begin_index() const {
			return m_b_begin_index;
		}

		i32 length() const {
			return m_length;
		}

		void all_a_begin_index(vector<i32>& indexes) {
			indexes.clear();

			for (i32 i = 0; i < (i32)m_maxtrix.size(); ++i) {
				if (m_maxtrix[i] == m_length) {
					indexes.push_back(i % m_a_size);
				}
			}
		}

		void all_b_begin_index(vector<i32>& indexes) {
			indexes.clear();

			for (i32 i = 0; i < (i32)m_maxtrix.size(); ++i) {
				if (m_maxtrix[i] == m_length) {
					indexes.push_back(i / m_a_size);
				}
			}
		}

	private:
		i32 m_a_size;
		i32 m_b_size;
		i32 m_a_begin_index;
		i32 m_b_begin_index;
		i32 m_length;

		vector<i32> m_maxtrix;
	};


}