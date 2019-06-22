#pragma once

#include "mt_scalar_t.h"
#include "mt_range_t.h"
#include <limits>

namespace basicmath {

	

	class mt_helper {
	public:

		template<class T>
		static bool is_same_symbol(const T& v1, const T& v2) {
			if ((v1 >= 0 && v2 >= 0)
				|| (v1 <= 0 && v2 <= 0)) {
					return true;
			}

			return false;
		}
		
		static b8 is_nan(f64 val);
		static b8 is_infinity(f64 val);
		static b8 is_number(f64 val);

		template<class T>
		static i64 neibour_float(T val) {
			return (val > 0 ? (i64)(val + 0.5) : (i64)(val - 0.5));
		}

		template<class T>
		static b8 valid_range(f64 value) {
			if (typeid(T) == typeid(f32) || typeid(T) == typeid(f64)) {
				return sys_true;
			} else {
				if (typeid(T) == typeid(i8)) {
					return value <= mt_S8_Max && value >= mt_S8_Min;
				} else if (typeid(T) == typeid(u8)) {
					return value <= mt_U8_Max && value >= mt_U8_Min;
				}

				return sys_true;
			}
		}

		static void set_float_eps(float eps);
		static float get_float_eps();
		static void set_double_eps(double eps);
		static double get_double_eps();

		static int compare_float(float a, float b);
		static int compare_double(double a, double b);

		template<class T>
		static i32 compare_value(const T& a, const T& b) {
			if (typeid(T) == typeid(float)) {
				return compare_float((float)a, (float)b);
			} else if (typeid(T) == typeid(double)) {
				return compare_double((double)a, (double)b);
			} else {
				T v = a - b;

				if (v == 0) {
					return 0;
				} else if (v > 0) {
					return 1;
				} else {
					return -1;
				}
			}
		}

		template<class T>
		static T max(const T& a, const T& b) {
			return a > b ? a : b;
		}

		template<class T>
		static T min(const T& a, const T& b) {
			return a < b ? a : b;
		}

		template<class T>
		static T compute_min(i32 size, const T* values) {
			T min_value = values[0];

			for (int i = 1; i < size; ++i) {
				if (values[i] < min_value) {
					min_value = values[i];
				}
			}

			return min_value;
		}

		template<class T>
		static T compute_max(i32 size, const T* values) {
			T max_value = values[0];

			for (int i = 1; i < size; ++i) {
				if (values[i] > max_value) {
					max_value = values[i];
				}
			}

			return max_value;
		}

		template<class T>
		static T compute_min_in_abs(const T* values, int size) {
			T min_value = values[0];

			for (int i = 1; i < size; ++i) {
				if (abs(values[i]) < abs(min_value)) {
					min_value = values[i];
				}
			}

			return min_value;
		}

		template<class T>
		static T compute_abs_min(const T* values, int size) {
			T min_value = abs(values[0]);

			for (int i = 1; i < size; ++i) {
				if (abs(values[i]) < min_value) {
					min_value = abs(values[i]);
				}
			}

			return min_value;
		}

		template<class T>
		static T median_with_changed_order(vector<T>& data) {
			basiclog_assert2(!data.empty());

			T* data_begin = &data[0]; 
			T* data_end = &data[0] + (i32)data.size();

			int middleIndex = (int)(data_end - data_begin - 1) / 2;
			T* middlePointer = data_begin + middleIndex;
			nth_element(data_begin, middlePointer, data_end);
			return *middlePointer;
		}

		template<class T>
		static T median_with_unchanged_order(const vector<T>& data) {
			vector<T> temp = data;

			return median_with_changed_order(temp);
		}

		template<class T>
		static i32 index_of_max_value(i32 size, const T* data_start) {
			i32 index = 0;

			for (i32 i = 1; i < size; ++i) {
				if (data_start[i] > data_start[index]) {
					index = i;
				}
			}

			return index;
		}

		template<class T>
		static i32 index_of_min_value(i32 size, const T* data_start) {
			i32 index = 0;

			for (i32 i = 1; i < size; ++i) {
				if (data_start[i] < data_start[index]) {
					index = i;
				}
			}

			return index;
		}

		template<class T>
		static void vec_from_scalar(vector<T>& dst, const mt_scalar_t<T>& src) {
			dst.resize(sizeof(src.value) / sizeof(src.value[0]));

			for (i32 i = 0; i < (i32)dst.size(); ++i) {
				dst[i] = src.value[i];
			}
		}

		template<class T>
		static vector<T> vec_from_array(i32 size, const T* data) {
			vector<T> dst;
			vec_from_array(dst, size, data);

			return dst;
		}

		template<class T>
		static void vec_from_array(vector<T>& dst, i32 size, const T* data) {
			dst.resize(size);

			for (i32 i = 0; i < size; ++i) {
				dst[i] = data[i];
			}
		}

		template<class T>
		static void sub_vector(vector<T>& sub, const vector<T>& src, const mt_range& range) {
			sub.resize(range.size());

			for (i32 i = range.m_start; i < range.m_end; ++i) {
				sub[i - range.m_start] = src[i];
			}
		}

		static i32 index_from_multi_index(const vector<i32>& indexs, i32 size, const i32* sizes) {
			basiclog_assert2((i32)indexs.size() == size);

			i32 res = indexs[size - 1];
			i32 cur_dim_element_number = 1;

			for (i32 i = size - 2; i >= 0; --i) {
				cur_dim_element_number *= sizes[i + 1];

				res += indexs[i] * cur_dim_element_number;
			}

			return res;
		}

		static void multi_index_from_index(vector<i32>& indexs, i32 index, i32 size, const i32* sizes) {
			indexs.resize(size);

			i32 dim_element_number = 1;

			for (i32 i = 1; i < size; ++i) {
				dim_element_number *= sizes[i];
			}

			for (i32 i = 0; i < size; ++i) {
				i32 cur_dim_index = index % dim_element_number;
				index = index - cur_dim_index * dim_element_number;
				dim_element_number /= sizes[i + 1];

				indexs[i] = cur_dim_index;
			}
		}

		static i32 factorial(i32 n) {
			int res = 1;
			for (int i = 1; i <= n; ++i) {
				res *= i;
			}

			return res;
		}

		static i32 combination(i32 n, i32 k) {
			basiclog_assert2(n >= k);
			return factorial(n) / (factorial(k) * factorial(n - k));
		}

		template<class T>
		static T add(const T* start, const T* stop) {
			T res = 0;

			for (const T* iter = start; iter != stop; ++iter) {
				res += *iter;
			}

			return res;
		}

		template<class T>
		static T add(const vector<T>& elements) {
			if (elements.empty()) {
				return 0;
			}

			return add(&elements[0], &elements[0] + (i32)elements.size());
		}

		template<class T>
		static T mutiply(const T* start, const T* stop) {
			T res = 1;

			for (const T* iter = start; iter != stop; ++iter) {
				res *= *iter;
			}

			return res;
		}

		template<class T>
		static T mutiply(const vector<T>& elements) {
			if (elements.empty()) {
				return 0;
			}

			return mutiply(&elements[0], &elements[0] + (i32)elements.size());
		}

		template<class T>
		static b8 is_vec_value(const vector<T>& vec, const T& v) {
			b8 res = sys_true;

			for (i32 i = 0; i < (i32)vec.size(); ++i) {
				if (typeid(T) == typeid(float)) {
					if (compare_float((f32)vec[i], (f32)v) != 0) {
						return sys_false;
					}
				} else if (typeid(T) == typeid(double)) {
					if (compare_double((f64)vec[i], (f64)v) != 0) {
						return sys_false;
					}
				} else {
					if (vec[i] != v) {
						return sys_false;
					}
				}
			}

			return sys_true;
		}

		static i32 reverse_i32(i32 val);

		static i32 number_of_1(i32 n);
		static i32 number_of_1(i64 n);

		template<class T>
		static void integral_array(vector<T>& res, const vector<T>& src) {
			res[0] = 0;

			for (int i = 0; i < (int)src.size(); ++i) {
				res[i + 1] = res[i] + src[i];
			}
		}

		template<class T>
		static void integral_array(T* res, const T* src_start, const T* src_end) {
			*res = 0;

			int offset = 0;
			for (const T* iter = src_start; iter < src_end; ++iter, ++offset) {
				*(res + offset + 1) = *(res + offset) + *iter;
			}
		}

		template<class T>
		static void pointer_from_instance(vector<T*>& res, const vector<T>& src) {
			res.resize(src.size());

			for (int i = 0; i < (int)src.size(); ++i) {
				res[i] = const_cast<T*>(&src[i]);
			}
		}

		template<class T>
		static void instance_from_pointer(vector<T>& res, const vector<T*>& src) {
			result.resize(res.size());

			for (int i = 0; i < (int)res.size(); ++i) {
				result[i] = *src[i];
			}
		}

		template<class T>
		static void odd_even_sort(vector<T>& res, b8 odd_as_front, b8 reserve_origin_order) {
			if (reserve_origin_order) {
				b8 switch_flag = sys_false;

				for (i32 i = 0; i < (i32)res.size() - 1; ++i) {
					switch_flag = sys_false;
					
					for (i32 j = 0; j < (i32)res.size() - 1 - i; ++j) {
						if (odd_as_front) {
							if (res[j] % 2 == 0 && res[j + 1] % 2 == 1) {
								swap(res[j], res[j + 1]);
								switch_flag = sys_true;
							}
						} else {
							if (res[j] % 2 == 1 && res[j + 1] % 2 == 0) {
								swap(res[j], res[j + 1]);
								switch_flag = sys_true;
							}
						}
					}

					if (!switch_flag) {
						break;
					}
				}
			} else {
				i32 low = 0;
				i32 high = (i32)res.size() - 1;

				while (low < high) {
					while (low < high && ((odd_as_front && res[low] % 2 == 1) || (!odd_as_front && res[low] % 2 == 0))) {
						++low;
					}

					while (low < high && ((odd_as_front && res[high] % 2 == 0) || (!odd_as_front && res[high] % 2 == 1))) {
						--high;
					}

					if (low < high) {
						swap(res[low], res[high]);
						++low;
						--high;
					}
				}
			}
		}

		template<class T>
		static void flip_vector(vector<T>& data) {
			if (data.empty()) {
				return;
			}

			i32 half_size = (i32)data.size()  / 2;
			
			for (i32 i = 0; i < half_size; ++i) {
				swap(data[i], data[i + (i32)data.size() - i - 1]);
			}
		}

		static u32 one_number(u32 value);

		static void enable_omp_mkl(b8 enable);

		static i32 get_number_procs();
	};

	template<class T>
	bool operator==(const vector<T>& vec1, const vector<T>& vec2) {
		if (vec1.size() != vec2.size()) {
			return false;
		}

		for (int i = 0; i < (int)vec1.size(); ++i) {
			if (mt_helper::compare_value<T>(vec1[i], vec2[i]) != 0) {
				return false;
			}
		}

		return true;
	}

	
}

#ifndef __OPENCV_CORE_CVSTD_HPP__

#define basicmath_std_abs

namespace std {
	static inline u8 abs(u8 v) {
		return v;
	}

	static inline u16 abs(u16 v) {
		return v;
	}

	static inline u32 abs(u32 v) {
		return v;
	}

	static inline u64 abs(u64 v) {
		return v;
	}
}
#endif
