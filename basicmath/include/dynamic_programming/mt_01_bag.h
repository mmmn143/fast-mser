#pragma once



namespace basicmath {

	template<class T_Weight, class T_Value>
	class mt_01_bag {
	public:

		mt_01_bag(const vector<T_Weight>& weights, const vector<T_Value>& values, T_Weight max_weight) {
			for (i32 i = 0; i < (i32)weights.size(); ++i) {
				basiclog_assert2(weights[i] >= 0);
			}

			m_flags.resize((i32)weights.size(), sys_false);

			m_max_value = calculate_value(weights, values, (i32)weights.size() - 1, max_weight);

			detrermine_flag(values, (i32)weights.size() - 1, m_max_value);
		}

		T_Value max_value() {
			return m_max_value;
		}

		const vector<b8>& flag_01() const {
			return m_flags;
		}

	protected:

		class cache_info {
		public:
			cache_info() {
				m_considered_range = -1;
				m_considered_weight = -1;
			}

			i32 m_considered_range;
			T_Weight m_considered_weight;
			T_Value m_value;
		};

		/**
		f(i, j) = max(j >= w_i ? f(i - 1, j - w_i) + v_i : f(i - 1, j), f(i - 1, j))
		*/
		T_Value calculate_value(const vector<T_Weight>& weights, const vector<T_Value>& values, i32 considered_range, T_Weight considered_weight) {
			basiclog_assert2(considered_weight >= 0);

			if (considered_range < 0) {
				return 0;
			}

			// Find the value for the considered range and weight
			for (i32 i = 0; i < (i32)m_caches.size(); ++i) {
				if (mt_helper::compare_value(m_caches[i].m_considered_range, considered_range) == 0
					&& mt_helper::compare_value(m_caches[i].m_considered_weight, considered_weight) == 0) {
						return m_caches[i].m_value;
				}
			}
			
			cache_info cur_cache_info;
			cur_cache_info.m_considered_range = considered_range;
			cur_cache_info.m_considered_weight = considered_weight;

			T_Value value_without_last_considered_element = calculate_value(weights, values, considered_range - 1, considered_weight);
			
			if (considered_weight >= weights[considered_range]) {
				T_Value value_with_last_considered_elemente = calculate_value(weights, values, considered_range - 1, considered_weight - weights[considered_range]) + values[considered_range];
				cur_cache_info.m_value = std::max(value_without_last_considered_element, value_with_last_considered_elemente);

				

			} else {
				cur_cache_info.m_value = value_without_last_considered_element;
			}

			m_caches.push_back(cur_cache_info);

			return cur_cache_info.m_value;
		}

		void detrermine_flag(const vector<T_Value>& values, i32 considered_range, T_Value considered_value) {
			for (i32 i = 0; i < (i32)m_caches.size(); ++i) {
				if (mt_helper::compare_value(m_caches[i].m_value, considered_value) == 0
					&& m_caches[i].m_considered_range <= considered_range 
					&& find_value(considered_range - 1, considered_value - values[m_caches[i].m_considered_range])) {
						m_flags[m_caches[i].m_considered_range] = sys_true;

						detrermine_flag(values, considered_range - 1, considered_value - values[m_caches[i].m_considered_range]);
				}
			}
		}

		b8 find_value(i32 considered_range, T_Value considered_value) {
			if (considered_range < 0) {
				return sys_true;
			}

			for (i32 i = 0; i < (i32)m_caches.size(); ++i) {
				if (m_caches[i].m_considered_range <= considered_range
					&& mt_helper::compare_value(m_caches[i].m_value, considered_value) == 0) {
						return sys_true;
				}
			}

			return sys_false;
		}

		vector<cache_info> m_caches;
		T_Value m_max_value;
		vector<b8> m_flags;
	};

}