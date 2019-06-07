#include "stdafx.h"
#include "mt_mat.h"
#include "mt_mat_helper.h"
#include "mt_auto_derivative.h"
#include "mt_mat_cache.h"

namespace basicmath {

	class private_mat {
	public:

		static void set_data(u8* ptr_data, mt_Depth_Channel depth_channel, const vector<f64>& values) {
			if (values.empty()) {
				return;
			}

			set_data(ptr_data, depth_channel, &values[0], (i32)values.size());
		}

		/** 
		@param value_size if value_size is 0, all channel will be set to be values[0], otherwise the value_size should not be lower than channels.
		*/
		static void set_data(u8* ptr_data, mt_Depth_Channel depth_channel, const f64* values, int value_size = 0) {
			int channels = depth_channel.channel();
			int depth = depth_channel.depth();
			int range = channels;

			if (value_size != 0) {
				basiclog_assert2(value_size >= (i32)channels);
				range = min(value_size, channels);
			}

			for (int c = 0; c < channels; ++c) {
				int value_index = value_size == 0 ? 0 : c;

				switch (depth) {
				case mt_U8:

					basiclog_assert2(mt_helper::valid_range<u8>(values[value_index]));

					ptr_data[c] = (u8)values[value_index];
					break;
				case mt_I8:
					{
						basiclog_assert2(mt_helper::valid_range<i8>(values[value_index]));

						i8* temp_ptr_src = (i8*)ptr_data;
						temp_ptr_src[c] = (i8)values[value_index];
					}

					break;
				case mt_U16:
					{
						basiclog_assert2(mt_helper::valid_range<u16>(values[value_index]));

						u16* temp_ptr_src = (u16*)ptr_data;
						temp_ptr_src[c] = (u16)values[value_index];
					}

					break;
				case mt_I16:
					{
						basiclog_assert2(mt_helper::valid_range<i16>(values[value_index]));

						i16* temp_ptr_src = (i16*)ptr_data;
						temp_ptr_src[c] = (i16)values[value_index];
					}

					break;
				case mt_U32:
					{
						basiclog_assert2(mt_helper::valid_range<u32>(values[value_index]));

						u32* temp_ptr_src = (u32*)ptr_data;
						temp_ptr_src[c] = (u32)values[value_index];
					}

					break;
				case mt_I32:
					{
						basiclog_assert2(mt_helper::valid_range<i32>(values[value_index]));

						i32* temp_ptr_src = (i32*)ptr_data;
						temp_ptr_src[c] = (i32)values[value_index];
					}

					break;
				case mt_U64:
					{
						basiclog_assert2(mt_helper::valid_range<u64>(values[value_index]));

						u64* temp_ptr_src = (u64*)ptr_data;
						temp_ptr_src[c] = (u64)values[value_index];
					}

					break;
				case mt_I64:
					{
						basiclog_assert2(mt_helper::valid_range<i64>(values[value_index]));

						i64* temp_ptr_src = (i64*)ptr_data;
						temp_ptr_src[c] = (i64)values[value_index];
					}

					break;
				case mt_F32:
					{
						float* temp_ptr_src = (float*)ptr_data;
						temp_ptr_src[c] = (float)values[value_index];
					}

					break;
				case mt_F64:
					{
						double* temp_ptr_src = (double*)ptr_data;
						temp_ptr_src[c] = values[value_index];
					}

					break;
				default:
					basiclog_unsupport2();
					break;
				}
			}
		}

		static void get_data(vector<double>& values, const u8* ptr_data, mt_Depth_Channel depth_channel) {
			values.resize(depth_channel.channel());

			get_data(&values[0], ptr_data, depth_channel);
		}

		static void get_data(basicsys::f64* values, const u8* ptr_data, mt_Depth_Channel depth_channel) {
			int channels = depth_channel.channel();
			int depth = depth_channel.depth();

			for (int c = 0; c < channels; ++c) {
				switch (depth) {
				case mt_U8:
					values[c] = ptr_data[c];
					break;
				case mt_I8:
					{
						i8* temp_ptr_src = (i8*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_U16:
					{
						u16* temp_ptr_src = (u16*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_I16:
					{
						i16* temp_ptr_src = (i16*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_U32:
					{
						u32* temp_ptr_src = (u32*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_I32:
					{
						i32* temp_ptr_src = (i32*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_U64:
					{
						u64* temp_ptr_src = (u64*)ptr_data;
						values[c] = (f64)temp_ptr_src[c];
					}

					break;
				case mt_I64:
					{
						i64* temp_ptr_src = (i64*)ptr_data;
						values[c] = (f64)temp_ptr_src[c];
					}

					break;
				case mt_F32:
					{
						f32* temp_ptr_src = (f32*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				case mt_F64:
					{
						f64* temp_ptr_src = (f64*)ptr_data;
						values[c] = temp_ptr_src[c];
					}

					break;
				default:
					basiclog_unsupport2();
					break;
				}
			}
		}
		
		template<class T>
		static void set(mt_mat& mat, const f64* values, int value_size = 0) {
			int channels = mat.channel();
			int depth = mat.depth();

			if (value_size != 0) {
				basiclog_assert2(value_size >= channels);
			}

			basicmath_mat_request_memory(T, assign_values, channels);

			for (i32 i = 0; i < channels; ++i) {
				assign_values[i] = value_size == 0 ? (T)values[0] : (T)values[i];
			}

			b8 continuous = mat.is_continuous();
			i32 element_number = mat.element_number();
			i32 element_number_consider_channel = element_number * channels;
			i32 last_step = mat.last_dim_element_step(); 

			if (continuous) {
				b8 can_use_memset = sys_false;

				if (depth == mt_U8 || depth == mt_I8) {
					can_use_memset = value_size == 0 || value_size == 1;

					if (!can_use_memset) {
						i32 i = 1;
						for (; i < channels; ++i) {
							if (mt_helper::compare_double(values[0], values[i]) != 0) {
								break;
							}
						}

						if (i == channels) {
							can_use_memset = sys_true;
						}
					}
				}

				if (can_use_memset) {
					memset(mat.memory_data(), (u8)values[0], mat.memory_size());
				} else {
					T* ptr_data = (T*)mat.data();

					if (channels == 1) {	
						for (i32 i = 0; i < element_number; ++i) {
							ptr_data[0] = assign_values[0];

							ptr_data += last_step;
						}
					} else if (channels == 2) {
						const T& temp_val0 = (T)values[0];
						const T& temp_val1 = (T)values[value_size == 0 ? 0 : 1];
						for (i32 i = 0; i < element_number; ++i) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];

							ptr_data += last_step;
						}
					} else if (channels == 3) {
						const T& temp_val0 = (T)values[0];
						const T& temp_val1 = (T)values[value_size == 0 ? 0 : 1];
						const T& temp_val2 = (T)values[value_size == 0 ? 0 : 2];
						for (i32 i = 0; i < element_number; ++i) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];
							ptr_data[2] = assign_values[2];

							ptr_data += last_step;
						}
					} else if (channels == 4) {
						const T& temp_val0 = (T)values[0];
						const T& temp_val1 = (T)values[value_size == 0 ? 0 : 1];
						const T& temp_val2 = (T)values[value_size == 0 ? 0 : 2];
						const T& temp_val3 = (T)values[value_size == 0 ? 0 : 3];
						for (i32 i = 0; i < element_number; ++i) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];
							ptr_data[2] = assign_values[2];
							ptr_data[3] = assign_values[3];

							ptr_data += last_step;
						}
					} else {
						for (i32 i = 0; i < element_number; ++i) {
							for (i32 c = 0; c < channels; ++c) {
								ptr_data[c] = assign_values[c];
							}

							ptr_data += last_step;
						}
					}
				}
			} else if (mat.dim() == 2) {
				// 2d non-continuous mat
				u8* ptr_dim0 = mat.data();

				if (channels == 1) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						T* ptr_data = (T*)ptr_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = assign_values[0];

							ptr_data += last_step;
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 2) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						T* ptr_data = (T*)ptr_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];

							ptr_data += last_step;
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 3) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						T* ptr_data = (T*)ptr_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];
							ptr_data[2] = assign_values[2];

							ptr_data += last_step;
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 4) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						T* ptr_data = (T*)ptr_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = assign_values[0];
							ptr_data[1] = assign_values[1];
							ptr_data[2] = assign_values[2];
							ptr_data[3] = assign_values[3];

							ptr_data += last_step;
						}

						ptr_dim0 += mat.step()[0];
					}
				} else {
					u8* ptr_dim0 = mat.data();

					for (i32 row = 0; row < mat.size()[0]; ++row) {
						T* ptr_data = (T*)ptr_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								ptr_data[c] = (T)assign_values[c];
							}

							ptr_data += last_step;
						}

						ptr_dim0 += mat.step()[0];
					}
				}
			} else if (mat.dim() == 3) {
				// 3d non-continuous mat
				u8* ptr_dim0 = mat.data();

				if (channels == 1) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {

							T* ptr_data = (T*)ptr_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = assign_values[0];

								ptr_data += last_step;
							}

							ptr_dim1 += mat.step()[1];
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 2) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {

							T* ptr_data = (T*)ptr_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = assign_values[0];
								ptr_data[1] = assign_values[1];

								ptr_data += last_step;
							}

							ptr_dim1 += mat.step()[1];
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 3) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {

							T* ptr_data = (T*)ptr_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = assign_values[0];
								ptr_data[1] = assign_values[1];
								ptr_data[2] = assign_values[2];

								ptr_data += last_step;
							}

							ptr_dim1 += mat.step()[1];
						}

						ptr_dim0 += mat.step()[0];
					}
				} else if (channels == 4) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {

							T* ptr_data = (T*)ptr_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = assign_values[0];
								ptr_data[1] = assign_values[1];
								ptr_data[2] = assign_values[2];
								ptr_data[3] = assign_values[3];

								ptr_data += last_step;
							}

							ptr_dim1 += mat.step()[1];
						}

						ptr_dim0 += mat.step()[0];
					}
				} else {
					u8* ptr_dim0 = mat.data();

					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {

							T* ptr_data = (T*)ptr_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {

								for (int c = 0; c < channels; ++c) {
									ptr_data[c] = assign_values[c];

									ptr_data += last_step;
								}
							}

							ptr_dim1 += mat.step()[1];
						}

						ptr_dim0 += mat.step()[0];
					}
				}

			} else {
				// normal processing
				mt_array_element_iterator iter(mat);

				for (;;) {
					T* ptr_data = (T*)iter.data();

					if (ptr_data == NULL) {
						break;
					}

					for (int c = 0; c < channels; ++c) {
						ptr_data[c] = assign_values[c];
					}
				}
			}

			basicmath_mat_release(assign_values);
		}

		template<class Dst_Type, class Src_Type>
		static void set(mt_mat& mat, const mt_mat& src) {
			int channels = mat.channel();
			i32 src_channels = src.channel();
			int depth = mat.depth();
			i32 src_depth = src.depth();

			i32 element_number = mat.element_number();
			i32 element_number_consider_channel = element_number * channels;
			i32 last_step = mat.last_dim_element_step(); 
			i32 last_src_step = src.last_dim_element_step(); 

			if (depth == src_depth && last_step == last_src_step && mat.is_continuous() && src.is_continuous()) {
				memcpy(mat.memory_data(), src.memory_data(), mat.element_size() * element_number);
			} else if (mat.dim() == 2) {
				// 2d non-continuous mat
				u8* ptr_dim0 = mat.data();
				const u8* ptr_src_dim0 = src.data();

				if (channels == 1) {

					for (i32 row = 0; row < mat.size()[0]; ++row) {
						Dst_Type* ptr_data = (Dst_Type*)ptr_dim0;
						const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = (Dst_Type)ptr_src_data[0];

							ptr_data += last_step;
							ptr_src_data += last_src_step;
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 2) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						Dst_Type* ptr_data = (Dst_Type*)ptr_dim0;
						const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = (Dst_Type)ptr_src_data[0];
							ptr_data[1] = (Dst_Type)ptr_src_data[1];

							ptr_data += last_step;
							ptr_src_data += last_src_step;
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 3) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						Dst_Type* ptr_data = (Dst_Type*)ptr_dim0;
						const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = (Dst_Type)ptr_src_data[0];
							ptr_data[1] = (Dst_Type)ptr_src_data[1];
							ptr_data[2] = (Dst_Type)ptr_src_data[2];

							ptr_data += last_step;
							ptr_src_data += last_src_step;
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 4) {
					for (i32 row = 0; row < mat.size()[0]; ++row) {
						Dst_Type* ptr_data = (Dst_Type*)ptr_dim0;
						const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							ptr_data[0] = (Dst_Type)ptr_src_data[0];
							ptr_data[1] = (Dst_Type)ptr_src_data[1];
							ptr_data[2] = (Dst_Type)ptr_src_data[2];
							ptr_data[3] = (Dst_Type)ptr_src_data[3];

							ptr_data += last_step;
							ptr_src_data += last_src_step;
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else {
					u8* ptr_dim0 = mat.data();

					for (i32 row = 0; row < mat.size()[0]; ++row) {
						Dst_Type* ptr_data = (Dst_Type*)ptr_dim0;
						const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim0;

						for (i32 col = 0; col < mat.size()[1]; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								ptr_data[c] = (Dst_Type)ptr_src_data[c];
							}
							
							ptr_data += last_step;
							ptr_src_data += last_src_step;
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}
			} else if (mat.dim() == 3) {
				// 3d non-continuous mat
				u8* ptr_dim0 = mat.data();
				const u8* ptr_src_dim0 = src.data();

				if (channels == 1) {

					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {
							Dst_Type* ptr_data = (Dst_Type*)ptr_dim1;
							const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = (Dst_Type)ptr_src_data[0];

								ptr_data += last_step;
								ptr_src_data += last_src_step;
							}

							ptr_dim1 += mat.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 2) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {
							Dst_Type* ptr_data = (Dst_Type*)ptr_dim1;
							const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = (Dst_Type)ptr_src_data[0];
								ptr_data[1] = (Dst_Type)ptr_src_data[1];

								ptr_data += last_step;
								ptr_src_data += last_src_step;
							}

							ptr_dim1 += mat.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 3) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {
							Dst_Type* ptr_data = (Dst_Type*)ptr_dim1;
							const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = (Dst_Type)ptr_src_data[0];
								ptr_data[1] = (Dst_Type)ptr_src_data[1];
								ptr_data[2] = (Dst_Type)ptr_src_data[2];

								ptr_data += last_step;
								ptr_src_data += last_src_step;
							}

							ptr_dim1 += mat.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else if (channels == 4) {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {
							Dst_Type* ptr_data = (Dst_Type*)ptr_dim1;
							const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								ptr_data[0] = (Dst_Type)ptr_src_data[0];
								ptr_data[1] = (Dst_Type)ptr_src_data[1];
								ptr_data[2] = (Dst_Type)ptr_src_data[2];
								ptr_data[3] = (Dst_Type)ptr_src_data[3];

								ptr_data += last_step;
								ptr_src_data += last_src_step;
							}

							ptr_dim1 += mat.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else {
					for (i32 plane = 0; plane < mat.size()[0]; ++plane) {
						u8* ptr_dim1 = ptr_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < mat.size()[1]; ++row) {
							Dst_Type* ptr_data = (Dst_Type*)ptr_dim1;
							const Src_Type* ptr_src_data = (const Src_Type*)ptr_src_dim1;

							for (i32 col = 0; col < mat.size()[2]; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									ptr_data[c] = (Dst_Type)ptr_src_data[c];
								}

								ptr_data += last_step;
								ptr_src_data += last_src_step;
							}

							ptr_dim1 += mat.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_dim0 += mat.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}

			} else {
				// normal processing
				mt_array_element_iterator iter(mat);
				mt_array_element_const_iterator iter_src(src);

				for (;;) {
					Dst_Type* ptr_data = (Dst_Type*)iter.data();

					if (ptr_data == NULL) {
						break;
					}

					const Src_Type* ptr_src_data = (const Src_Type*)iter_src.data();

					for (int c = 0; c < channels; ++c) {
						ptr_data[c] = (Dst_Type)ptr_src_data[c];
					}
				}
			}
		}
	};
}

mt_mat::reference_info::reference_info() {
	m_ref_mutex = sys_thread_lock::create_mutex();
	m_ref_number = 1;
}

mt_mat::reference_info::~reference_info() {
	sys_thread_lock::release_mutex(m_ref_mutex);
}

void mt_mat::reference_info::add_ref() {
	sys_thread_lock lock(m_ref_mutex);
	{
		++m_ref_number;
	}
}

b8 mt_mat::reference_info::sub_ref() {
	sys_thread_lock lock(m_ref_mutex);
	{
		--m_ref_number;
	}

	return m_ref_number == 0;
}

mt_mat::mt_mat() 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL){

}

mt_mat::mt_mat(int cols, mt_Depth_Channel depth) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {cols};
		*this = s_mat_cache.get(1, sizes, depth);
}

mt_mat::mt_mat(int row, int col, mt_Depth_Channel depth) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {row, col};
		*this = s_mat_cache.get(2, sizes, depth);
}

mt_mat::mt_mat(int plane, int row, int col, mt_Depth_Channel depth) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {plane, row, col};
		*this = s_mat_cache.get(3, sizes, depth);
}

mt_mat::mt_mat(int dims, const int* sizes, mt_Depth_Channel depth) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(dims, sizes, depth);
}

mt_mat::mt_mat(const vector<i32>& sizes, mt_Depth_Channel depth) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(sizes, depth);
}

mt_mat::mt_mat(int cols, mt_Depth_Channel depth, f64 init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {cols};
		*this = s_mat_cache.get(1, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int row, int col, mt_Depth_Channel depth, f64 init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {row, col};
		*this = s_mat_cache.get(2, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int plane, int row, int col, mt_Depth_Channel depth, f64 init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {plane, row, col};
		*this = s_mat_cache.get(3, sizes, depth);

		set(init_value);
}

mt_mat::mt_mat(int dims, const int* sizes, mt_Depth_Channel depth, f64 init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(dims, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(const vector<i32>& sizes, mt_Depth_Channel depth, f64 init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int cols, mt_Depth_Channel depth, const mt_scalar& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {cols};
		*this = s_mat_cache.get(1, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int row, int col, mt_Depth_Channel depth, const mt_scalar& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {row, col};
		*this = s_mat_cache.get(2, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int plane, int row, int col, mt_Depth_Channel depth, const mt_scalar& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {plane, row, col};
		*this = s_mat_cache.get(3, sizes, depth);

		set(init_value);
}

mt_mat::mt_mat(int dims, const int* sizes, mt_Depth_Channel depth, const mt_scalar& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(dims, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(const vector<i32>& sizes, mt_Depth_Channel depth, const mt_scalar& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int cols, mt_Depth_Channel depth, const vector<f64>& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {cols};
		*this = s_mat_cache.get(1, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int row, int col, mt_Depth_Channel depth, const vector<f64>& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {

		int sizes[] = {row, col};
		*this = s_mat_cache.get(2, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int plane, int row, int col, mt_Depth_Channel depth, const vector<f64>& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {plane, row, col};
		*this = s_mat_cache.get(3, sizes, depth);

		set(init_value);
}

mt_mat::mt_mat(int dims, const int* sizes, mt_Depth_Channel depth, const vector<f64>& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(dims, sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(const vector<i32>& sizes, mt_Depth_Channel depth, const vector<f64>& init_value) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = s_mat_cache.get(sizes, depth);
		set(init_value);
}

mt_mat::mt_mat(int cols, mt_Depth_Channel depth, u8* data, const int* steps, u8* share_mmeory) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {cols};
		*this = mt_mat(1, sizes, depth, data, steps);
}

mt_mat::mt_mat(int row, int col, mt_Depth_Channel depth, u8* data, const int* steps, u8* share_mmeory) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {row, col};
		*this = mt_mat(2, sizes, depth, data, steps);
}

mt_mat::mt_mat(int plane, int row, int col, mt_Depth_Channel depth, u8* data, const int* steps, u8* share_mmeory) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		int sizes[] = {plane, row, col};
		*this = mt_mat(3, sizes, depth, data, steps);
}

mt_mat::mt_mat(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, u8* data, const i32* steps, u8* share_mmeory) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		m_dims = dims;
		m_depth_channel = depth_channel;

		if (m_dims > 4) {
			m_dynamic_size_step_size = m_dims;
			m_dynamic_size_steps = new int[m_dynamic_size_step_size * 2];
		}

		for (int i = 0; i < dims; ++i) {
			size()[i] = sizes[i];
		}

		m_data = data;

		if (share_mmeory != NULL) {
			m_shared_memory = share_mmeory;
			m_reference = new reference_info();
		}
		
		if (steps == NULL) {
			fill_auto_step();
		} else {
			for (int i = 0; i < m_dims; ++i) {
				step()[i] = steps[i];
			}
		}
}

mt_mat::mt_mat(const vector<i32>& sizes, mt_Depth_Channel depth_channel, u8* data, const vector<i32>& steps, u8* share_mmeory) 
	: m_data(NULL)
	, m_dynamic_size_steps(NULL)
	, m_dynamic_size_step_size(0)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_shared_memory(NULL) {
		*this = mt_mat((i32)sizes.size(), &sizes[0], depth_channel, data, steps.empty() ? NULL : &steps[0]);
}

mt_mat::mt_mat(const mt_mat& other, Construct_Type type)
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {

		if (type == Construct_Type_Operator_Equal) {
			*this = other;
		} else {
			*this = s_mat_cache.get(other.dim(), other.size(), other.depth_channel());
		}
}

mt_mat::mt_mat(const mt_mat& other, f64 init_value)
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {

		*this = s_mat_cache.get(other.dim(), other.size(), other.depth_channel());
		set(init_value);
}

mt_mat::mt_mat(const mt_mat& other, const mt_scalar& init_value)
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {

		*this = s_mat_cache.get(other.dim(), other.size(), other.depth_channel());
		set(init_value);
}

mt_mat::mt_mat(const mt_mat& other, const vector<f64>& init_value)
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {

		*this = s_mat_cache.get(other.dim(), other.size(), other.depth_channel());
		set(init_value);
}

mt_mat::mt_mat(const mt_mat& other, i32 dims, const mt_range* ranges) 
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {
		*this = other.sub(dims, ranges);
}

mt_mat::mt_mat(const mt_mat& other, const vector<mt_range>& ranges) 
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {
		*this = other.sub(ranges);
}

mt_mat::mt_mat(const mt_mat& other, const mt_range& range, int dim) 
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {
		*this = other.sub(range, dim);
}

mt_mat::mt_mat(const mt_mat& other, const mt_rect& roi) 
	: m_data(NULL)
	, m_reference(NULL)
	, m_dims(0)
	, m_auto_derivative(NULL)
	, m_dynamic_size_step_size(0)
	, m_dynamic_size_steps(NULL)
	, m_shared_memory(NULL) {
		*this = other.sub(roi);
}

mt_mat::~mt_mat() {
	try_deallocate();

	if (m_dynamic_size_steps != 0) {
		free(m_dynamic_size_steps);
	}
}

void mt_mat::create(i32 cols, mt_Depth_Channel depth_channel) {
	i32 sizes[] = {cols};

	create(1, sizes, depth_channel);
}

void mt_mat::create(i32 rows, i32 cols, mt_Depth_Channel depth_channel) {
	i32 sizes[] = {rows, cols};

	create(2, sizes, depth_channel);
}

void mt_mat::create(i32 planes, i32 rows, i32 cols, mt_Depth_Channel depth_channel) {
	i32 sizes[] = {planes, rows, cols};

	create(3, sizes, depth_channel);
}

void mt_mat::create(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel) {
	on_vaule_changed();

	if (dims == dim() && depth_channel == this->depth_channel()) {
		b8 flag = sys_true;
		
		for (i32 i = 0; i < dims; ++i) {
			if (size()[i] != sizes[i]) {
				flag = sys_false;
			}
		}

		if (flag) {
			return;
		}
	}

	m_depth_channel = depth_channel;

	try_deallocate();

	if (dims > 4 && m_dynamic_size_step_size < dims) {

		if (m_dynamic_size_steps != NULL) {
			free(m_dynamic_size_steps);
		}

		m_dynamic_size_step_size = dims;
		m_dynamic_size_steps = (i32*)malloc(sizeof(i32) * (m_dynamic_size_step_size * 2));
	}

	m_dims = dims;

	int data_size = element_size();
	for (int i = 0; i < m_dims; ++i) {
		size()[i] = sizes[i];
		data_size *= sizes[i];
	}

	m_data = (u8*)malloc(sizeof(u8) * data_size);
	m_shared_memory = m_data;
	m_reference = new reference_info();

	fill_auto_step();
}

mt_mat mt_mat::derivative(const mt_mat& other) const {
	return m_auto_derivative->derivative(other, *this);
}

b8 mt_mat::is_zero() const {
	mt_array_element_const_iterator iter(*this);

	for (;;) {
		const u8* ptr_data = iter.data();

		if (ptr_data == NULL) {
			break;
		}

		for (i32 c = 0; c < channel(); ++c) {
			if (ptr_data[c] != 0) {
				return sys_false;
			}
		}
	}

	return sys_true;
}

b8 mt_mat::is_empty() const {
	return m_data == NULL;
}

mt_mat& mt_mat::set_eye(b8 left_high) {
	basiclog_assert2(dim() == 2);
	on_vaule_changed();

}

b8 mt_mat::operator==(const mt_mat& other) const {
	if (other.depth_channel() != depth_channel()) {
		return false;
	}

	for (int c = 0; c < dim(); ++c) {
		if (size()[c] != other.size()[c]) {
			return false;
		}
	}

	if (depth() == mt_F32 || depth() == mt_F64) {
		mt_array_element_const_iterator other_iter(other);
		mt_array_element_const_iterator cur_iter(*this);

		for (;;) {
			const u8* ptr_src = other_iter.data();
			const u8* ptr_dst = cur_iter.data();

			if (ptr_src == NULL) {
				break;
			}

			if (depth() == mt_F32) {
				const float* ptr_float_src = (const float*)ptr_src;
				const float* ptr_float_dst = (const float*)ptr_dst;

				for (int c = 0; c < channel(); ++c) {
					if (mt_helper::compare_float(ptr_float_src[c], ptr_float_dst[c]) != 0) {
						return false;
					}
				}
			} else {
				const double* ptr_float_src = (const double*)ptr_src;
				const double* ptr_float_dst = (const double*)ptr_dst;

				for (int c = 0; c < channel(); ++c) {
					if (mt_helper::compare_double(ptr_float_src[c], ptr_float_dst[c]) != 0) {
						return false;
					}
				}
			}
		}

		return true;
	} else {
		return mt_array_iteration::array_cmp(other.data(), data(), dim(), size(), other.step(), step(), element_size());
	}
}

b8 mt_mat::operator!=(const mt_mat& other) const {
	return !(*this == other);
}

b8 mt_mat::is_memory_shared(const mt_mat& other) const {
	return m_shared_memory == other.m_shared_memory;
}

b8 mt_mat::is_same(const mt_mat& other) const {
	if (m_shared_memory != other.m_shared_memory) {
		return sys_false;
	}

	if (m_data != other.m_data) {
		return sys_false;
	}

	if (m_dims != other.m_dims) {
		return sys_false;
	}

	if (channel() != other.channel()) {
		return sys_false;
	}

	for (int i = 0; i < m_dims; ++i) {
		if (size()[i] != other.size()[i] || step()[i] != other.step()[i]) {
			return sys_false;
		}
	}

	return sys_true;
}

mt_mat& mt_mat::set_incremental(double value, b8 same_value_for_multi_channel) {
	on_vaule_changed();
	mt_array_memory_block_iterator iter(*this);

	basicmath_mat_request_memory(f64, channel_buffer, channel());

	for (;;) {
		u8* ptr_src = iter.data();

		if (ptr_src == NULL) {
			break;
		}

		for (int i = 0; i < iter.block_element_number(); ++i) {
			if (same_value_for_multi_channel) {
				private_mat::set_data(ptr_src, m_depth_channel, &value, 0);
				++value;
			} else {
				for (int c = 0; c < channel(); ++c) {
					channel_buffer[c] = value++;
				}

				private_mat::set_data(ptr_src, m_depth_channel, channel_buffer, channel());
			}

			ptr_src += iter.block_element_step();
		}
	}

	basicmath_mat_release(channel_buffer);
	return *this;
}

mt_mat& mt_mat::copy_from(const mt_mat& other) {
	on_vaule_changed();

	create(other.dim(), other.size(), other.depth_channel());
	set(other);

	return *this;
}

mt_mat& mt_mat::set(const mt_mat& other) {
	on_vaule_changed();
	basiclog_assert2(is_same_size(other));

	if (depth() == other.depth()) {
		switch (depth()) {
		case mt_U8:
			private_mat::set<u8, u8>(*this, other);
			break;
		case mt_I8:
			private_mat::set<i8, i8>(*this, other);
			break;
		case mt_U16:
			private_mat::set<u16, u16>(*this, other);
			break;
		case mt_I16:
			private_mat::set<i16, i16>(*this, other);
			break;
		case mt_U32:
			private_mat::set<u32, u32>(*this, other);
			break;
		case mt_I32:
			private_mat::set<i32, i32>(*this, other);
			break;
		case mt_U64:
			private_mat::set<u64, u64>(*this, other);
			break;
		case mt_I64:
			private_mat::set<i64, i64>(*this, other);
			break;
		case mt_F32:
			private_mat::set<f32, f32>(*this, other);
			break;
		case mt_F64:
			private_mat::set<f64, f64>(*this, other);
			break;
		default:
			basiclog_unsupport2();
			break;
		}
	} else {
		if (depth() == mt_U8) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<u8, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<u8, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<u8, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<u8, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<u8, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<u8, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<u8, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<u8, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<u8, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<u8, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_I8) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<i8, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<i8, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<i8, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<i8, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<i8, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<i8, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<i8, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<i8, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<i8, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<i8, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_U16) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<u16, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<u16, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<u16, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<u16, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<u16, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<u16, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<u16, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<u16, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<u16, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<u16, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_I16) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<i16, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<i16, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<i16, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<i16, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<i16, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<i16, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<i16, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<i16, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<i16, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<i16, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_U32) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<u32, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<u32, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<u32, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<u32, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<u32, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<u32, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<u32, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<u32, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<u32, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<u32, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_I32) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<i32, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<i32, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<i32, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<i32, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<i32, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<i32, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<i32, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<i32, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<i32, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<i32, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_U64) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<u64, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<u64, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<u64, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<u64, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<u64, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<u64, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<u64, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<u64, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<u64, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<u64, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_I64) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<i64, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<i64, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<i64, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<i64, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<i64, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<i64, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<i64, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<i64, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<i64, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<i64, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_F32) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<f32, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<f32, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<f32, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<f32, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<f32, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<f32, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<f32, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<f32, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<f32, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<f32, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else if (depth() == mt_F64) {
			switch (depth()) {
			case mt_U8:
				private_mat::set<f64, u8>(*this, other);
				break;
			case mt_I8:
				private_mat::set<f64, i8>(*this, other);
				break;
			case mt_U16:
				private_mat::set<f64, u16>(*this, other);
				break;
			case mt_I16:
				private_mat::set<f64, i16>(*this, other);
				break;
			case mt_U32:
				private_mat::set<f64, u32>(*this, other);
				break;
			case mt_I32:
				private_mat::set<f64, i32>(*this, other);
				break;
			case mt_U64:
				private_mat::set<f64, u64>(*this, other);
				break;
			case mt_I64:
				private_mat::set<f64, i64>(*this, other);
				break;
			case mt_F32:
				private_mat::set<f64, f32>(*this, other);
				break;
			case mt_F64:
				private_mat::set<f64, f64>(*this, other);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		} else {
			basiclog_unsupport2();
		}
	}

	return *this;
}

mt_mat& mt_mat::set(double value) {
	return set(0, &value);
}

mt_mat& mt_mat::set(const mt_scalar& value) {
	return set(4, value.value);
}

mt_mat& mt_mat::set(i32 size, const f64* value) {
	on_vaule_changed();

	switch (depth()) {
	case mt_U8:
		private_mat::set<u8>(*this, value, size);
		break;
	case mt_I8:
		private_mat::set<i8>(*this, value, size);
		break;
	case mt_U16:
		private_mat::set<u16>(*this, value, size);
		break;
	case mt_I16:
		private_mat::set<i16>(*this, value, size);
		break;
	case mt_U32:
		private_mat::set<u32>(*this, value, size);
		break;
	case mt_I32:
		private_mat::set<i32>(*this, value, size);
		break;
	case mt_U64:
		private_mat::set<u64>(*this, value, size);
		break;
	case mt_I64:
		private_mat::set<i64>(*this, value, size);
		break;
	case mt_F32:
		private_mat::set<f32>(*this, value, size);
		break;
	case mt_F64:
		private_mat::set<f64>(*this, value, size);
		break;
	}

	return *this;
}

mt_mat& mt_mat::set(const vector<double>& value) {
	return set((i32)value.size(), &value[0]);
}

mt_mat& mt_mat::set(double value, basicsys::i32 index1, basicsys::i32 index2) {
	int dims = 2;
	int indexs[] = {index1, index2};

	return set(value, dims, indexs);
}

mt_mat& mt_mat::set(double value, basicsys::i32 index1, basicsys::i32 index2, basicsys::i32 index3) {
	int dims = 3;
	int indexs[] = {index1, index2, index3};

	return set(value, dims, indexs);
}

mt_mat& mt_mat::set(double value, basicsys::i32 dims, const basicsys::i32* indexs) {
	u8* ptr_data = ptr<u8>(dims, indexs, 0);
	private_mat::set_data(ptr_data, depth_channel(), &value, 0);

	return *this;
}

mt_mat& mt_mat::set(const mt_scalar& value, basicsys::i32 index1, basicsys::i32 index2) {
	int dims = 2;
	int indexs[] = {index1, index2};

	return set(value, dims, indexs);
}

mt_mat& mt_mat::set(const mt_scalar& value, basicsys::i32 index1, basicsys::i32 index2, basicsys::i32 index3) {
	int dims = 3;
	int indexs[] = {index1, index2, index3};

	return set(value, dims, indexs);
}

mt_mat& mt_mat::set(const mt_scalar& value, basicsys::i32 dims, const basicsys::i32* indexs) {
	on_vaule_changed();

	double values[] = {value[0], value[1], value[2], value[3]};

	u8* ptr_data = ptr<u8>(dims, indexs, 0);
	private_mat::set_data(ptr_data, depth_channel(), values, 4);

	return *this;
}

mt_mat& mt_mat::set(const double* values, basicsys::i32 dims, const basicsys::i32* indexs) {
	on_vaule_changed();

	u8* ptr_data = ptr<u8>(dims, indexs, 0);
	private_mat::set_data(ptr_data, depth_channel(), values, channel());

	return *this;
}

mt_scalar mt_mat::get(basicsys::i32 index0, basicsys::i32 index1) const {
	i32 indexes[] = {index0, index1};
	mt_scalar res;

	get(res.value, 2, indexes);

	return res;
}

void mt_mat::get(vector<double>& values, basicsys::i32 index0) const {
	const u8* ptr_data = ptr<u8>(index0, 0);
	private_mat::get_data(values, ptr_data, depth_channel());
}

void mt_mat::get(vector<double>& values, basicsys::i32 dim, const basicsys::i32* indexs) const {
	const u8* ptr_data = ptr<u8>(dim, indexs, 0);
	private_mat::get_data(values, ptr_data, depth_channel());
}

void mt_mat::get(double* values, basicsys::i32 index0) const {
	const u8* ptr_data = ptr<u8>(index0, 0);
	private_mat::get_data(values, ptr_data, depth_channel());
}

void mt_mat::get(double* values, basicsys::i32 dims, const basicsys::i32* indexs) const {
	const u8* ptr_data = ptr<u8>(dims, indexs, 0);
	private_mat::get_data(values, ptr_data, depth_channel());
}

mt_mat mt_mat::clone() const {
	mt_mat res(m_dims, size(), m_depth_channel);
	res.set(*this);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.attach(m_auto_derivative);

		m_auto_derivative->clone(res, *this);
	}

	return res;
}

void mt_mat::convert(mt_mat& res, mt_Depth depth) const {
	res.create(dim(), size(), mt_Depth_Channel(depth, channel()));

	mt_array_element_const_iterator cur_iter(*this);
	mt_array_element_iterator other_iter(res);

	vector<double> values;

	for (;;) {
		const u8* ptr_cur = cur_iter.data();
		u8* ptr_other = other_iter.data();

		if (ptr_cur == NULL) {
			break;
		}

		private_mat::get_data(values, ptr_cur, depth_channel());
		private_mat::set_data(ptr_other, res.depth_channel(), &values[0], (i32)values.size());
	}
}

mt_mat mt_mat::convert(mt_Depth depth) const {
	mt_mat res;
	convert(res, depth);

	return res;
}

void mt_mat::operator =(const mt_mat& other) {
	if (this == &other) {
		return;
	}

	try_deallocate();

	if (other.m_dims > 4 && m_dynamic_size_step_size < other.m_dims) {
		if (m_dynamic_size_steps != NULL) {
			free(m_dynamic_size_steps);
		}

		m_dynamic_size_step_size = other.m_dims;
	}

	m_dims = other.m_dims;

	for (int i = 0; i < other.m_dims; ++i) {
		step()[i] = other.step()[i];
		size()[i] = other.size()[i];
	}

	m_depth_channel = other.m_depth_channel;
	m_data = other.m_data;
	m_shared_memory = other.m_shared_memory;
	m_reference = other.m_reference;
	m_auto_derivative = other.m_auto_derivative;

	if (NULL != m_reference) {
		m_reference->add_ref();
	}
}

b8 mt_mat::is_same_size(const mt_mat& other) const {
	if (m_dims == other.m_dims && channel() == other.channel()) {
		b8 same_flag = true;
		for (int i = 0; i < m_dims; ++i) {
			if (size()[i] != other.size()[i]) {
				same_flag = sys_false;
				break;
			}
		}

		if (same_flag) {
			return sys_true;
		}
	}

	return sys_false;
}

b8 mt_mat::is_same_memory_size(const mt_mat& other) const {
	return depth() == other.depth() && is_same_size(other);
}

int mt_mat::element_size() const {
	return m_depth_channel.size();
}

int mt_mat::element_channel_size() const {
	return m_depth_channel.depth_size();
}

i32 mt_mat::memory_size() const {
	return element_size() * element_number();
}

i32 mt_mat::channel_memory_size() const {
	return element_channel_size() * element_number();
}

u8* mt_mat::memory_data() {
	u8* ptr_data = data();

	for (int i = 0; i < dim(); ++i) {
		if (step()[i] < 0) {
			ptr_data += (size()[i] - 1) * step()[i];
		}
	}

	return ptr_data;
}

const u8* mt_mat::memory_data() const {
	return (const_cast<mt_mat*>(this))->memory_data();
}

int mt_mat::element_number() const {
	int res = 1;

	for (int i = 0; i < m_dims; ++i) {
		res *= size()[i];
	}

	return res;
}

b8 mt_mat::is_min_abs_step_equal_element_size() const {
	return element_size() == mt_helper::compute_abs_min(step(), dim());
}

b8 mt_mat::is_continuous() const {
	return mt_array_iteration::get_continuous_dim(dim(), size(), step(), element_size()) == 0;
}

void mt_mat::try_deallocate() {
	if (m_reference != NULL) {

		if (m_reference->sub_ref()) {
			delete m_reference;
			if (m_shared_memory != NULL) {
				free(m_shared_memory);
			}
		}
	}
}

mt_mat mt_mat::increase_dim(int added_dim) const {
	int splited_sizes[2];

	if (added_dim == m_dims) {
		added_dim -= 1;
		splited_sizes[0] = size()[m_dims - 1];
		splited_sizes[1] = 1;
	} else {
		splited_sizes[0] = 1;
		splited_sizes[1] = size()[added_dim];
	}

	return mt_mat::split_dim(added_dim, 2, splited_sizes);
}

mt_mat mt_mat::decrease_dim(int deleted_dim) const {
	basiclog_assert_message2(size()[deleted_dim] == 1, L"Only the dimension of size is 1 can be deleted!");

	if (deleted_dim == 0) {
		return combine_dim(0, 2);
	} else {
		return combine_dim(deleted_dim - 1, 2);
	}
}

mt_mat mt_mat::reshape_2d(i32 rows, i32 cols, i32 new_channel) const {
	i32 sizes[] = {rows, cols};
	return reshape(2, sizes, new_channel);
}

mt_mat mt_mat::reshape_3d(i32 planes, i32 rows, i32 cols, i32 new_channel) const {
	i32 sizes[] = {planes, rows, cols};
	return reshape(3, sizes, new_channel);
}

mt_mat mt_mat::reshape(const vector<int>& sizes, i32 new_channel) const {
	return reshape((i32)sizes.size(), &sizes[0], new_channel);
}

mt_mat mt_mat::reshape(int dims, const int* sizes, i32 new_channel) const {
	basiclog_assert2(is_continuous());

	i32 current_total_size = mt_helper::mutiply<i32>(size(), size() + dim());
	i32 reshape_total_size = mt_helper::mutiply<i32>(sizes, sizes + dims);

	if (new_channel != -1) {
		basiclog_assert2(current_total_size * channel() == reshape_total_size * new_channel);
	} else {
		basiclog_assert2(current_total_size == reshape_total_size);
	}

	mt_mat res;
	res.m_dims = dims;

	if (res.m_dims > 4 && m_dynamic_size_step_size < res.m_dims) {
		res.m_dynamic_size_step_size = res.m_dims;
		res.m_dynamic_size_steps = new int[m_dynamic_size_step_size * 2];
	}

	for (int i = 0; i < res.m_dims; ++i) {
		res.size()[i] = sizes[i];
	}

	res.m_reference = m_reference;

	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_data = m_data;

	if (new_channel == -1) {
		res.m_depth_channel = m_depth_channel;
	} else {
		res.m_depth_channel = mt_Depth_Channel(depth(), new_channel);
	}
	
	res.m_shared_memory = m_shared_memory;

	res.fill_auto_step();

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.attach(m_auto_derivative);

		m_auto_derivative->reshape(res, *this);
	}

	return res;
}

mt_mat mt_mat::reshape_channel(i32 new_channel) const {
	if (new_channel == channel()) {
		return *this;
	}

	basiclog_assert2((new_channel % channel() == 0) || (channel() % new_channel == 0));

	i32 new_size = size()[dim() - 1] * channel() / new_channel;

	basicmath_mat_request_memory(i32, new_sizes, dim());

	for (i32 i = 0; i < dim() - 1; ++i) {
		new_sizes[i] = size()[i];
	}

	new_sizes[dim() - 1] = new_size;

	mt_mat res = reshape(dim(), new_sizes, new_channel);

	basicmath_mat_release(new_sizes);

	return res;
}

mt_mat mt_mat::split_dim(int dim, int splited_dims, int* splited_sizes) const {
	mt_mat res;
	res.m_dims = m_dims + splited_dims - 1;

	if (res.m_dims > mt_Mat_Normal_Support_Dim) {
		res.m_dynamic_size_step_size = res.m_dims;
		res.m_dynamic_size_steps = new int[m_dynamic_size_step_size * 2];
	}

	for (int i = 0; i < splited_dims; ++i) {
		res.size()[i + dim] = splited_sizes[i];
	}

	res.step()[dim + splited_dims - 1] = step()[dim];
	for (int i = splited_dims - 2; i >= 0; --i) {
		res.step()[i + dim] = res.step()[i + dim + 1] * res.size()[i + dim + 1];
	}

	for (int i = 0; i < dim; ++i) {
		res.size()[i] = size()[i];
		res.step()[i] = step()[i];
	}

	for (int i = res.m_dims - 1; i > dim + splited_dims - 1; --i) {
		res.size()[i] = size()[i - splited_dims + 1];
		res.step()[i] = step()[i - splited_dims + 1];
	}

	res.m_reference = m_reference;
	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_depth_channel = m_depth_channel;
	res.m_data = m_data;
	res.m_shared_memory = m_shared_memory;

	return res;
}

mt_mat mt_mat::combine_dim(int combined_dim_start, int combined_dim_count) const {
	bool order_dim = true;

	for (int i = 0; i < combined_dim_count - 1; ++i) {
		if (step()[combined_dim_start + i] < step()[i + combined_dim_start + 1]) {
			order_dim = false;
			break;
		}
	}

	if (!order_dim) {
		basiclog_warning2(L"combine dim for a mat with unordered dim, this will reduce the performance! You should better input an ordered mat");
		return this->clone().combine_dim(combined_dim_start, combined_dim_count);
	}

	mt_mat res;

	res.m_dims = m_dims - combined_dim_count + 1;

	for (int i = 0; i < combined_dim_start; ++i) {
		res.size()[i] = size()[i];
		res.step()[i] = step()[i];
	}

	res.size()[combined_dim_start] = size()[combined_dim_start];

	for (int i = 1; i < combined_dim_count; ++i) {
		res.size()[combined_dim_start] *= size()[combined_dim_start + i];
	}

	res.step()[combined_dim_start] = step()[combined_dim_start + combined_dim_count - 1];

	for (int i = combined_dim_start + 1; i < res.m_dims; ++i) {
		res.size()[i] = size()[i + combined_dim_count - 1];
		res.step()[i] = step()[i + combined_dim_count - 1];
	}

	res.m_depth_channel = m_depth_channel;
	res.m_reference = m_reference;
	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_data = m_data;
	res.m_shared_memory = m_shared_memory;

	return res;
}

mt_mat mt_mat::repeat_2d(i32 nrows, i32 ncols, i32 nchannels /* = 1 */) const {
	i32 nsizes[2] = {nrows, ncols};

	return repeat(2, nsizes, nchannels);
}

mt_mat mt_mat::repeat_3d(i32 nplanes, i32 nrows, i32 ncols, i32 nchannels /* = 1 */) const {
	i32 nsizes[3] = {nplanes, nrows, ncols};

	return repeat(3, nsizes, nchannels);
}

mt_mat mt_mat::repeat(i32 nsize, i32 dim, i32 nchannels) const {
	basicmath_mat_request_memory(i32, nsizes, this->dim());

	for (i32 i = 0; i < this->dim(); ++i) {
		nsizes[i] = 1;
	}

	nsizes[dim] = nsize;
	
	return repeat(this->dim(), nsizes, nchannels);

	basicmath_mat_release(nsizes);
}

mt_mat mt_mat::repeat(const vector<i32>& nsizes, i32 nchannels) const {
	return repeat((i32)nsizes.size(), &nsizes[0], nchannels);
}

mt_mat mt_mat::repeat(i32 dims, const i32* nsizes, i32 nchannels) const {
	basiclog_assert2(nsizes != NULL);
	basiclog_assert2(dims == dim());

	basicmath_mat_request_memory(i32, dst_sizes, dim());

	b8 size_changed = sys_false;

	for (i32 i = 0; i < dim(); ++i) {
		dst_sizes[i] = size()[i] * nsizes[i];

		if (nsizes[i] > 1) {
			size_changed = sys_true;
		}
	}

	if (!size_changed && nchannels == 1) {
		return clone();
	}

	mt_mat res(dim(), dst_sizes, mt_Depth_Channel(depth(), channel() * nchannels));

	mt_array_index_iterator iter(dim(), nsizes);

	vector<mt_range> start_ranges;
	start_ranges.resize(dim());

	while (iter.next()) {

		for (i32 i = 0; i < dim(); ++i) {
			start_ranges[i].m_start = iter.position()[i] * size()[i];
			start_ranges[i].m_end = start_ranges[i].m_start + size()[i];
		}
			
		for (i32 c = 0; c < nchannels; ++c) {
			i32 start_channel = c * channel();
			mt_mat copy_dst = res.sub(start_ranges).sub_channel(start_channel, start_channel + channel());
			copy_dst.set(*this);
		}
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.attach(m_auto_derivative);

		m_auto_derivative->repeat(res, *this);
	}

	basicmath_mat_release(dst_sizes);

	return res;
}

mt_mat mt_mat::t() const {
	basiclog_assert2(m_dims == 2);

	return swap_dim(0, 1);
}

mt_mat mt_mat::swap_dim(int dim_a, int dim_b) const {
	mt_mat res = *this;

	swap(res.size()[dim_a], res.size()[dim_b]);
	swap(res.step()[dim_a], res.step()[dim_b]);

	return res;
}

void mt_mat::fill_auto_step() {
	step()[m_dims - 1] = element_size();

	for (int i = m_dims - 2; i >= 0; --i) {
		step()[i] = step()[i + 1] * size()[i + 1];
	}
}

void mt_mat::on_vaule_changed() {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());
}

mt_mat mt_mat::flip(int dim) const {
	mt_mat res = *this;

	res.m_data += (res.size()[dim] - 1) * res.step()[dim];
	res.step()[dim] = -res.step()[dim];

	return res;
}

mt_mat mt_mat::flip(const vector<int>& dim_indexs) const {
	return flip((int)dim_indexs.size(), &dim_indexs[0]);
}

mt_mat mt_mat::flip(int size, const int* dims) const {
	basiclog_assert2(size <= dim());

	basicmath_mat_request_memory(b8, flip_flags, dim());

	for (i32 i = 0; i < size; ++i) {
		flip_flags[i] = sys_false;
	}

	for (i32 i = 0; i < size; ++i) {
		flip_flags[dims[i]] = sys_true;
	}

	mt_mat res = flip(dim(), flip_flags);

	basicmath_mat_release(flip_flags);

	return res;
}

mt_mat mt_mat::flip(const vector<basicsys::b8> flip_flags) const {
	return flip((i32)flip_flags.size(), &flip_flags[0]);
}

mt_mat mt_mat::flip(i32 size, const basicsys::b8* flip_flags) const {
	basiclog_assert2(size == dim());

	mt_mat res = *this;

	for (int i = 0; i < res.m_dims; ++i) {
		if (flip_flags[i]) {
			res.m_data += (res.size()[i] - 1) * res.step()[i];
			res.step()[i] = -res.step()[i];
		}
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.attach(m_auto_derivative);

		m_auto_derivative->flip(res, *this, size, flip_flags);
	}

	return res;
}

mt_mat mt_mat::flip_all_dim() const {
	basicmath_mat_request_memory(b8, flip_flags, dim());

	for (i32 i = 0; i < dim(); ++i) {
		flip_flags[i] = sys_true;
	}

	mt_mat res = flip(dim(), flip_flags);

	basicmath_mat_release(flip_flags);

	return res;
}

mt_mat mt_mat::channel_as_last_dim() const {
	mt_mat res;
	res.m_dims = m_dims + 1;

	if (res.m_dims > mt_Mat_Normal_Support_Dim) {
		res.m_dynamic_size_step_size = res.m_dims;
		res.m_dynamic_size_steps = new int[m_dynamic_size_step_size * 2];
	}

	for (int i = 0; i < m_dims; ++i) {
		res.size()[i] = size()[i];
		res.step()[i] = step()[i];
	}

	res.size()[res.m_dims - 1] = channel();
	res.step()[res.m_dims - 1] = element_channel_size();

	res.m_depth_channel = mt_Depth_Channel(depth(), 1);
	res.m_reference = m_reference;
	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_data = m_data;
	res.m_shared_memory = m_shared_memory;

	return res;
}

mt_mat mt_mat::last_dim_as_channel() const {
	basiclog_assert_message2(m_dims > 1, L"dimension must be exceed 1, you can use increase_dim() method!");
	basiclog_assert_message2(channel() == 1, L"channel in the the last dimension must be 1!");
	basiclog_assert_message2(step()[m_dims - 1] > 0, L"step in the last dimension must be positive, maybe you used the flip() method!");
	basiclog_assert_message2(mt_array_iteration::get_continuous_dim(m_dims, size(), step(), element_channel_size()) < m_dims, L"data in the last dimension must be continuous, maybe you used the t() or swap() method!");

	mt_mat res;
	res.m_dims = m_dims - 1;

	for (int i = 0; i < res.m_dims; ++i) {
		res.size()[i] = size()[i];
		res.step()[i] = step()[i];
	}

	res.m_depth_channel = mt_Depth_Channel(depth(), size()[m_dims - 1]);

	res.m_reference = m_reference;
	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_data = m_data;
	res.m_shared_memory = m_shared_memory;

	return res;
}

void mt_mat::split(vector<mt_mat>& channels, b8 can_share_memory) const {
	channels.resize(channel());

	if (channel() == 1 && can_share_memory) {
		channels[0] = *this;
	} else {
		sys_for(i, channels) {
			channels[i] = channel_at(i).clone();
		}
	}
}

mt_mat mt_mat::channel_at(int channel) const {
	return sub_channel(channel, channel + 1);
}

void mt_mat::all_channel(vector<mt_mat>& channels) const {
	channels.resize(channel());

	sys_for(i, channels) {
		channels[i] = channel_at(i);
	}
}

mt_mat mt_mat::sub_channel(i32 start_channel, i32 stop_channel) const {
	if (start_channel == 0 && stop_channel == channel()) {
		return *this;
	}

	mt_mat res = *this;
	res.m_depth_channel = mt_Depth_Channel(depth(), stop_channel - start_channel);

	res.m_data += start_channel * element_channel_size();

	if (m_auto_derivative != NULL) {
		res.attach(m_auto_derivative);

		m_auto_derivative->sub_channel(res, *this, start_channel, stop_channel);
	}

	return res;
}

mt_mat mt_mat::row(int row) const {
	basiclog_assert(L"mt_mat", m_dims == 2);

	return index(row, 0);
}

mt_mat mt_mat::col(int col) const {
	basiclog_assert(L"mt_mat", m_dims == 2);

	return index(col, 1);
}

mt_mat mt_mat::plane(int plane) const {
	basiclog_assert(L"mt_mat", m_dims == 3);

	return index(plane, 2);
}

mt_mat mt_mat::front(int number, int dim /* = 0 */) const {
	return sub(mt_range(0, number), dim);
}

mt_mat mt_mat::back(int number, int dim /* = 0 */) const {
	return sub(mt_range(size()[dim] - number, size()[dim]), dim);
}

mt_mat mt_mat::index(int index, int dim) const {
	return sub(mt_range(index, index + 1), dim);
}

mt_mat mt_mat::sub(int start_index, int stop_index, int dim /* = 0 */) const {
	return sub(mt_range(start_index, stop_index), dim);
}

mt_mat mt_mat::sub(const vector<mt_range>& ranges) const {
	return sub((i32)ranges.size(), &ranges[0]);
}

mt_mat mt_mat::sub(i32 dims, const mt_range* ranges) const {
	if (dims == 0) {
		return *this;
	}

	mt_mat res;

	basiclog_assert2(dims == m_dims && ranges != NULL);

	res.m_dims = m_dims;
	res.m_depth_channel = m_depth_channel;
	res.m_shared_memory = m_shared_memory;
	res.m_reference = m_reference;
	if (m_reference != NULL) {
		m_reference->add_ref();
	}

	res.m_auto_derivative = m_auto_derivative;

	if (res.m_dims > 4) {
		res.m_dynamic_size_step_size = res.m_dims;
		res.m_dynamic_size_steps = new int[res.m_dynamic_size_step_size * 2];
	}

	res.m_data = m_data;

	for (int i = 0; i < res.m_dims; ++i) {
		basiclog_assert(L"mt_mat", ranges[i].size() > 0 && (ranges[i].m_end <= size()[i]));

		res.m_data += ranges[i].m_start * step()[i];
		res.step()[i] = step()[i];

		if (ranges[i].m_end == -1) {
			res.size()[i] = size()[i] - ranges[i].m_start;
		} else {
			res.size()[i] = ranges[i].size();
		}
	}		

	if (m_auto_derivative != NULL) {
		res.attach(m_auto_derivative);

		m_auto_derivative->sub(res, *this, dims, ranges);
	}

	return res;
}

mt_mat mt_mat::sub(const mt_rect& roi) const {
	basiclog_assert2(dim() == 2);

	mt_range ranges[2];
	ranges[0].m_start = roi.m_top;
	ranges[0].m_end = roi.m_top + roi.m_height;
	ranges[1].m_start = roi.m_left;
	ranges[1].m_end = roi.m_left + roi.m_width;

	return sub(2, ranges);
}

mt_mat mt_mat::sub(const mt_range& range, int dim) const {
	basicmath_mat_request_memory(mt_range, ranges, m_dims);

	for (i32 i = 0; i < m_dims; ++i) {
		if (i == dim) {
			ranges[i] = range;
		} else {
			ranges[i].m_start = 0;
			ranges[i].m_end = size()[i];
		}
	}

	basicmath_mat_release(ranges);
	return sub(m_dims, ranges);

	/*basiclog_assert(L"mt_mat", range.is_valid() && (range.m_end <= this->size()[dim]));

	mt_mat res;

	res.m_dims = m_dims;
	res.m_depth_channel = m_depth_channel;
	res.m_modified_number = m_modified_number;
	res.m_shared_memory = m_shared_memory;
	res.m_reference = m_reference;
	res.m_auto_derivative = m_auto_derivative;
	++*res.m_reference;

	if (res.m_dims > 4) {
	res.m_dynamic_size_step_size = res.m_dims;
	res.m_dynamic_size_steps = new int[res.m_dynamic_size_step_size * 2];
	}

	res.m_data = m_data;

	for (int i = 0; i < m_dims; ++i) {
	if (dim != i) {
	res.size()[i] = size()[i];
	} else {
	res.m_data += range.m_start * step()[i];

	if (range.m_end == -1) {
	res.size()[i] = size()[i] - range.m_start;
	} else {
	res.size()[i] = range.size();
	}
	}

	res.step()[i] = step()[i];
	}

	if (m_auto_derivative != NULL) {
	res.attach(m_auto_derivative);

	vector<mt_range> ranges;

	for (i32 i = 0; i )
	}

	return res;*/
}

vector<int> mt_mat::get_index(const u8* ptr_data) const {
	vector<int> index;
	get_index(index, ptr_data);

	return index;
}

void mt_mat::get_index(vector<int>& index, const u8* ptr_data) const {
	index.clear();
	index.assign(m_dims, -1);

	bool same_symbol = true;
	u8* memory_start_data = m_data;

	int positive_number = 0;

	for (int i = 0; i < m_dims; ++i) {
		if (step()[i] > 0) {
			++positive_number;
		} else if (positive_number > 0) {
			break;
		}
	}

	if (positive_number == 0 || positive_number == m_dims) {
		int offset = (int)(ptr_data - memory_start_data);
		basicmath_mat_request_memory(b8, flags, m_dims);
		mt_array_iteration::array_assign<b8>((u8*)flags, sys_false, m_dims);

		int count = 0;

		for (;;) {
			int max_abs_step = 0;
			int max_index = -1;

			for (int i = 0; i < m_dims; ++i) {
				if (flags[i] == 0 && std::abs(step()[i]) > max_abs_step) {
					max_abs_step = std::abs(step()[i]);
					max_index = i;
				}
			}

			index[max_index] = offset / step()[max_index];
			offset -= index[max_index] * step()[max_index];
			flags[max_index] = sys_true;

			++count;

			if (count == m_dims) {
				break;
			}
		}

		basicmath_mat_release(flags);
	} else {
		basicmath_mat_request_memory(b8, flip_flags, m_dims);

		for (i32 i = 0; i < m_dims; ++i) {
			flip_flags[i] = step()[i] > 0 ? sys_false : sys_true;
		}

		mt_mat temp_with_positive_step = flip(m_dims, flip_flags);
		temp_with_positive_step.get_index(index, ptr_data);

		for (i32 i = 0; i < m_dims; ++i) {
			if (step()[i] != temp_with_positive_step.step()[i]) {
				index[i] = size()[i] - 1 - index[i];
			}
		}

		basicmath_mat_release(flip_flags);
	}
}

b8 mt_mat::is_step_positive() const {
	for (i32 i = 0; i < m_dims; ++i) {
		if (step()[i] < 0) {
			return sys_false;
		}
	}

	return sys_true;
}

b8 mt_mat::is_step_negative() const {
	for (i32 i = 0; i < m_dims; ++i) {
		if (step()[i] > 0) {
			return sys_false;
		}
	}

	return sys_true;
}

b8 mt_mat::is_symmetrical() const {
	return sys_true;
}