#pragma once

#include "mt_mat.h"
#include "mt_array_iteration.h"

namespace basicmath {

	template<class T> 
	class mt_mat_t {
	public:

		static mt_Depth depth_from_data_type() {
			if (typeid(T) == typeid(basicsys::u8)) {
				return mt_U8;
			}

			if (typeid(T) == typeid(basicsys::i8)) {
				return mt_I8;
			}

			if (typeid(T) == typeid(basicsys::u16)) {
				return mt_U16;
			}

			if (typeid(T) == typeid(basicsys::i16)) {
				return mt_I16;
			}

			if (typeid(T) == typeid(basicsys::u32)) {
				return mt_U32;
			}

			if (typeid(T) == typeid(basicsys::i32)) {
				return mt_I32;
			}

			if (typeid(T) == typeid(basicsys::u64)) {
				return mt_U64;
			}

			if (typeid(T) == typeid(basicsys::i64)) {
				return mt_I64;
			}

			if (typeid(T) == typeid(basicsys::f32)) {
				return mt_F32;
			}

			if (typeid(T) == typeid(basicsys::f64)) {
				return mt_F64;
			}

			return mt_User;
		}

		mt_mat_t(int rows, int cols, int channels) {
			int sizes[] = {rows, cols};
			m_mat = mt_mat(2, sizes, mt_Depth_Channel(depth_from_data_type(), channels));
		}

		mt_mat_t(int planes, int rows, int cols, int channels) {
				int sizes[] = {planes, rows, cols};
				m_mat = mt_mat(3, sizes, mt_Depth_Channel(depth_from_data_type(), channels));
		}

		static mt_mat read(const vector<T>& datas) {
			basiclog_assert2(!datas.empty());

			mt_mat res(1, (basicsys::i32)datas.size(), mt_Depth_Channel(depth_from_data_type(), 1));

			T* ptr_data = (T*)res.data();

			sys_for(i, datas) {
				ptr_data[i] = datas[i];
			}

			return res;
		}

		static mt_mat read(const vector<vector<T>>& datas) {
			basiclog_assert2(!datas.front().empty());

			mt_mat res((basicsys::i32)datas.size(), (basicsys::i32)datas.front().size(), mt_Depth_Channel(depth_from_data_type(), 1));

			T* ptr_data = (T*)res.data();

			sys_for(i, datas) {
				sys_for(j, datas[i]) {
					*ptr_data = datas[i][j];
					++ptr_data;
				}
			}

			return res;
		}

		mt_mat read(double value, ...) {
			va_list arg;
			va_start(arg, value);

			bool first = true;

			mt_array_element_iterator iter(m_mat);

			for (;;) {
				T* ptr_src = (T*)iter.data();

				if (ptr_src == NULL) {
					break;
				}

				for (int c = 0; c < m_mat.channel(); ++c) {
					if (first) {
						ptr_src[c] = (T)value;
						first = false;
					} else {
						if (typeid(T) == typeid(float)) {
							((float*)ptr_src)[c] = (float)va_arg(arg, double);
						} else {
							ptr_src[c] = va_arg(arg, T);
						}
					}
				}
			}

			va_end(arg);
			return m_mat;
		}

		mt_mat m_mat;
	};

	//template<class T>
	//void write(basicsys::sys_strcombine& str, const mt_mat_t<T>& data) {
	//	write(str, data.m_mat);
	//}

}