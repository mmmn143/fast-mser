#pragma once


namespace basicmath {

	template<class T>
	class mt_max_sum_for_continuous_element {
	public:

		mt_max_sum_for_continuous_element(const vector<T>& values) {
			basiclog_assert2(!values.empty());

			m_max_sum = values[0];
			T temp_max_sum = values[0];
			int temp_begin_index = 0;
			int temp_length = 1;

			m_begin_index = 0;
			m_length = 1;

			for (i32 i = 1; i < (i32)values.size(); ++i) {
				temp_max_sum += values[i];
				
				if (values[i] > temp_max_sum) {
					temp_max_sum = values[i];
					temp_begin_index = i;
					temp_length = 1;
				} else {
					temp_length += 1;
				}

				if (temp_max_sum > m_max_sum) {
					m_max_sum = temp_max_sum;
					m_begin_index = temp_begin_index;
					m_length = temp_length;
				}				
			}
		}

		T max_sum() const {
			return m_max_sum;
		}

		i32 begin_index() const {
			return m_begin_index;
		}

		i32 length() const {
			return m_length;
		}

	private:

		T m_max_sum;
		i32 m_begin_index;
		i32 m_length;

	};

}