#include "stdafx.h"

namespace basicmath {

	class private_pooling {
	public:

		template<class T>
		static void pooling(mt_mat& res, mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) {
			basiclog_assert2(src.dim() == size);
			vector<i32> src_iter_indexs(src.dim());

			// optimational can reduce the cost from 1500ms to 300ms on a 1000 * 1000 mat with kernel = 5 * 5 and stride = 5 * 5!
			if (src.dim() == 2) {
				if (src.channel() == 1) {
					pooling_2d_channel_1<T>(res, mask_mat, src, pooling_type, size, kernel_sizes, strides);
				} else {
					pooling_2d_channel_n<T>(res, mask_mat, src, pooling_type, size, kernel_sizes, strides);
				}
			} else if (src.dim() == 3) {
				if (src.channel() == 1) {
					pooling_3d_channel_1<T>(res, mask_mat, src, pooling_type, size, kernel_sizes, strides);
				} else {
					pooling_3d_channel_n<T>(res, mask_mat, src, pooling_type, size, kernel_sizes, strides);
				}
			} else {
				// normal pooling for any dimensions
				basicmath_mat_request_memory(i32, src_start_indexs, src.dim());

				mt_array_element_iterator res_iter(res);
				mt_array_index_iterator kernel_iter(src.dim(), kernel_sizes);
				vector<vector<i32>> all_indexes;
				kernel_iter.all_index(all_indexes);

				for (;;) {
					T* ptr_data = (T*)res_iter.data();

					if (ptr_data == NULL) {
						break;
					}

					const vector<i32>& cur_indexs = res_iter.position();

					for (int i = 0; i < src.dim(); ++i) {
						src_start_indexs[i] = cur_indexs[i] * strides[i];
					}

					if (pooling_type == mt_Pooling_Type_First_Value) {
						const T* ptr_src_data = src.ptr<T>(src.dim(), src_start_indexs, 0);

						for (i32 c = 0; c < src.channel(); ++c) {
							ptr_data[c] = ptr_src_data[c];
						}
					} else if (pooling_type == mt_Pooling_Type_Mean || pooling_type == mt_Pooling_Type_Sum) {
						int in_src_number = 0;

						for (i32 iter_index = 0; iter_index < (i32)all_indexes.size(); ++iter_index) {
							for (int i = 0; i < src.dim(); ++i) {
								src_iter_indexs[i] = src_start_indexs[i] + all_indexes[iter_index][i];
							}

							if (src.is_valid_index(src_iter_indexs)) {
								const T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);
								++in_src_number;

								for (i32 c = 0; c < src.channel(); ++c) {
									ptr_data[c] += ptr_src_data[c];
								}
							}
						}

						if (pooling_type == mt_Pooling_Type_Mean) {
							for (i32 c = 0; c < src.channel(); ++c) {
								ptr_data[c] /= in_src_number;
							}
						}
					} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min) {
						i32* ptr_mask_data = mask_mat.ptr<i32>(cur_indexs, 0);
						int in_src_number = 0;

						for (i32 iter_index = 0; iter_index < (i32)all_indexes.size(); ++iter_index) {
							for (int i = 0; i < src.dim(); ++i) {
								src_iter_indexs[i] = src_start_indexs[i] + all_indexes[iter_index][i];
							}

							if (src.is_valid_index(src_iter_indexs)) {
								const T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);
								++in_src_number;
								i32 mask_index = mt_helper::index_from_multi_index(src_iter_indexs, src.dim(), src.size());

								if (in_src_number == 1) {
									for (i32 c = 0; c < src.channel(); ++c) {
										ptr_data[c] = ptr_src_data[c];
										ptr_mask_data[c] = mask_index;
									}
								} else {
									if (pooling_type == mt_Pooling_Type_Max) {
										for (i32 c = 0; c < src.channel(); ++c) {
											if (ptr_src_data[c] > ptr_data[c]) {
												ptr_data[c] = ptr_src_data[c];
												ptr_mask_data[c] = mask_index;
											}
										}
									} else {
										for (i32 c = 0; c < src.channel(); ++c) {
											if (ptr_src_data[c] < ptr_data[c]) {
												ptr_data[c] = ptr_src_data[c];
												ptr_mask_data[c] = mask_index;
											}
										}
									}
								}
							}
						}
					} else if (pooling_type == mt_Pooling_Type_Index) {
						i32* ptr_mask_data = mask_mat.ptr<i32>(cur_indexs, 0);

						mt_helper::multi_index_from_index(src_iter_indexs, *ptr_mask_data, src.dim(), src.size());

						basiclog_assert2(src.is_valid_index(src_iter_indexs));

						const T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);

						for (i32 c = 0; c < src.channel(); ++c) {
							ptr_data[c] = ptr_src_data[c];
						}
					}
				}

				basicmath_mat_release(src_start_indexs);
			}

			//basicmath_mat_release(res_sizes);
		}

		template<class T>
		static void unpooling(mt_mat& src, const mt_mat& mask_mat, const mt_mat& pooling_res_mat, mt_Pooling_Type pooling_type, i32 size, const int* kernel_sizes, const int* strides) {
			sys_timer timer(L"unpooling", sys_false);
			timer.begin();

			basiclog_assert2(pooling_res_mat.dim() == size);

			if (pooling_res_mat.dim() == 2) {
				if (pooling_res_mat.channel() == 1) {
					unpooling_2d_channel_1<T>(src, mask_mat, pooling_res_mat, pooling_type, size, kernel_sizes, strides);
				} else {
					unpooling_2d_channel_n<T>(src, mask_mat, pooling_res_mat, pooling_type, size, kernel_sizes, strides);
				}
			} else if (pooling_res_mat.dim() == 3) {
				if (pooling_res_mat.channel() == 1) {
					unpooling_3d_channel_1<T>(src, mask_mat, pooling_res_mat, pooling_type, size, kernel_sizes, strides);
				} else {
					unpooling_3d_channel_n<T>(src, mask_mat, pooling_res_mat, pooling_type, size, kernel_sizes, strides);
				}
			} else {
				basicmath_mat_request_memory(i32, src_start_indexs, pooling_res_mat.dim());
				vector<i32> src_iter_indexs(pooling_res_mat.dim());

				mt_array_element_const_iterator res_iter(pooling_res_mat);
				mt_array_index_iterator kernel_iter(src.dim(), kernel_sizes);
				vector<vector<i32>> all_indexes;
				kernel_iter.all_index(all_indexes);

				for (;;) {
					const T* ptr_data = (const T*)res_iter.data();

					if (ptr_data == NULL) {
						break;
					}

					const vector<i32>& cur_indexs = res_iter.position();

					for (int i = 0; i < src.dim(); ++i) {
						src_start_indexs[i] = cur_indexs[i] * strides[i];
					}

					if (pooling_type == mt_Pooling_Type_First_Value) {
						T* ptr_src_data = src.ptr<T>(src.dim(), src_start_indexs, 0);

						for (i32 c = 0; c < src.channel(); ++c) {
							ptr_src_data[c] += ptr_data[c];
						}
					} else if (pooling_type == mt_Pooling_Type_Sum) {
						for (i32 iter_index = 0; iter_index < (i32)all_indexes.size(); ++iter_index) {
							for (int i = 0; i < src.dim(); ++i) {
								src_iter_indexs[i] = src_start_indexs[i] + all_indexes[iter_index][i];
							}

							if (src.is_valid_index(src_iter_indexs)) {
								T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);

								for (i32 c = 0; c < src.channel(); ++c) {
									ptr_src_data[c] += ptr_data[c];
								}
							}
						}
					} else if (pooling_type == mt_Pooling_Type_Mean) {
						int in_src_number = 0;

						for (i32 iter_index = 0; iter_index < (i32)all_indexes.size(); ++iter_index) {
							for (int i = 0; i < src.dim(); ++i) {
								src_iter_indexs[i] = src_start_indexs[i] + all_indexes[iter_index][i];
							}

							if (src.is_valid_index(src_iter_indexs)) {
								++in_src_number;
							}
						}

						for (i32 iter_index = 0; iter_index < (i32)all_indexes.size(); ++iter_index) {
							for (int i = 0; i < src.dim(); ++i) {
								src_iter_indexs[i] = src_start_indexs[i] + all_indexes[iter_index][i];
							}

							if (src.is_valid_index(src_iter_indexs)) {
								T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);

								for (i32 c = 0; c < src.channel(); ++c) {
									ptr_src_data[c] += T(ptr_data[c] / (f64)in_src_number);
								}
							}
						}

					} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min || pooling_type == mt_Pooling_Type_Index) {
						const i32* ptr_mask_data = mask_mat.ptr<i32>(cur_indexs, 0);

						mt_helper::multi_index_from_index(src_iter_indexs, *ptr_mask_data, src.dim(), src.size());
						T* ptr_src_data = src.ptr<T>(src_iter_indexs, 0);

						for (i32 c = 0; c < src.channel(); ++c) {
							ptr_src_data[c] += ptr_data[c];
						}
					}
				}

				basicmath_mat_release(src_start_indexs);
			}

			timer.end();
		}

		private:

			template<class T>
			static void pooling_2d_channel_1(mt_mat& res, mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) {
				i32 src_last_step = src.last_dim_element_step();
				
				u8* ptr_result_dim0 = res.data();
				i32 mat_start_row = 0;
				i32 res_rows = res.size()[0];
				i32 res_cols = res.size()[1];
				i32 src_rows = src.size()[0];
				i32 src_cols = src.size()[1];

				if (pooling_type == mt_Pooling_Type_Sum || pooling_type == mt_Pooling_Type_Mean) {
					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (i32 col = 0; col < res_cols; ++col) {
							int valid_kernel_element_number = 0;

							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (i32 pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									*ptr_result += *ptr_mat;
									++valid_kernel_element_number;	
									ptr_mat += src_last_step;
								}
							}

							if (pooling_type == mt_Pooling_Type_Mean) {
								*ptr_result /= valid_kernel_element_number;
							}

							++ptr_result;
							mat_start_col += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Max) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (int*)ptr_mask_dim0;
						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {
							*ptr_result = src.at<T>(mat_start_row, mat_start_col, 0);
							*ptr_mask = mat_start_row * src_cols + mat_start_col;

							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									if (*ptr_mat > *ptr_result) {
										*ptr_result = *ptr_mat;
										*ptr_mask = pooling_row * src_cols + pooling_col;
									}

									ptr_mat += src_last_step;
								}
							}

							++ptr_result;
							++ptr_mask;

							mat_start_col += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Min) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (i32*)ptr_mask_dim0;

						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (i32 col = 0; col < res_cols; ++col) {
							*ptr_result = src.at<T>(mat_start_row, mat_start_col, 0);
							*ptr_mask = mat_start_row * src_cols + mat_start_col;

							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									if (*ptr_mat < *ptr_result) {
										*ptr_result = *ptr_mat;
										*ptr_mask = pooling_row * src_cols + pooling_col;
									}

									ptr_mat += src_last_step;
								}
							}

							mat_start_col += strides[1];
							++ptr_result;
							++ptr_mask;
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;

						i32 mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {
							*ptr_result++ = src.at<T>(mat_start_row, mat_start_col, 0);
							mat_start_col += strides[1];
						}

						ptr_result_dim0 += res.step()[0];

						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Index) {
					u8* ptr_mask_dim0 = mask_mat.data();
					i32 mask_last_step = mask_mat.last_dim_element_step();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (i32*)ptr_mask_dim0;

						for (int col = 0; col < res_cols; ++col) {
							*ptr_result++ = src.at<T>(ptr_mask[0] / src_cols, ptr_mask[0] % src_cols, 0);
							ptr_mask += mask_last_step;
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void pooling_2d_channel_n(mt_mat& res, mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) {
				i32 src_last_step = src.last_dim_element_step();
				i32 channels = src.channel();

				u8* ptr_result_dim0 = res.data();
				i32 mat_start_row = 0;
				
				i32 res_rows = res.size()[0];
				i32 res_cols = res.size()[1];
				i32 src_rows = src.size()[0];
				i32 src_cols = src.size()[1];

				if (pooling_type == mt_Pooling_Type_Sum || pooling_type == mt_Pooling_Type_Mean) {
					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (i32 col = 0; col < res_cols; ++col) {
							int valid_kernel_element_number = 0;	
							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (i32 pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {

									for (i32 c = 0; c < src.channel(); ++c) {
										ptr_result[c] += ptr_mat[c];
									}

									++valid_kernel_element_number;	
									ptr_mat += src_last_step;
								}
							}

							if (pooling_type == mt_Pooling_Type_Mean) {
								for (i32 c = 0; c < src.channel(); ++c) {
									ptr_result[c] /= valid_kernel_element_number;
								}
								
							}

							// res mat is continuous and memory positive order
							ptr_result += channels;
							mat_start_col += strides[1];
						}

						mat_start_row += strides[0];
						ptr_result_dim0 += res.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Max) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (int*)ptr_mask_dim0;

						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {

							for (i32 c = 0; c < channels; ++c) {
								ptr_result[c] = src.at<T>(mat_start_row, mat_start_col, c);
								ptr_mask[c] = mat_start_row * src_cols + mat_start_col;
							}
							
							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									for (i32 c = 0; c < src.channel(); ++c) {
										if (ptr_mat[c] > ptr_result[c]) {
											ptr_result[c] = ptr_mat[c];
											ptr_mask[c] = pooling_row * src_cols + pooling_col;
										}
									}

									ptr_mat += src_last_step;
								}
							}

							mat_start_col += strides[1];

							// res and mask mat is continuous and memory positive order
							ptr_result += channels;
							ptr_mask += channels;
						}

						mat_start_row += strides[0];

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Min) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (i32*)ptr_mask_dim0;
						i32 mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);
						i32 mat_start_col = 0;

						for (i32 col = 0; col < res_cols; ++col) {

							for (i32 c = 0; c < channels; ++c) {
								ptr_result[c] = src.at<T>(mat_start_row, mat_start_col, c);
								ptr_mask[c] = mat_start_row * src_cols + mat_start_col;
							}

							i32 mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								const T* ptr_mat = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									for (i32 c = 0; c < src.channel(); ++c) {
										if (ptr_mat[c] < ptr_result[c]) {
											ptr_result[c] = ptr_mat[c];
											ptr_mask[c] = pooling_row * src_cols + pooling_col;
										}
									}

									ptr_mat += src_last_step;
								}
							}

							mat_start_col += strides[1];

							// res and mask mat is continuous and memory positive order
							ptr_result += channels;
							ptr_mask += channels;
						}

						mat_start_row += strides[0];

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;

						i32 mat_start_col = 0;
						for (int col = 0; col < res_cols; ++col) {

							for (i32 c = 0; c < channels; ++c) {
								ptr_result[c] = src.at<T>(mat_start_row, mat_start_col, c);
							}

							mat_start_col += strides[1];
							ptr_result += channels;
						}

						mat_start_row += strides[0];

						ptr_result_dim0 += res.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Index) {
					u8* ptr_mask_dim0 = mask_mat.data();
					i32 mask_last_step = mask_mat.last_dim_element_step();

					for (i32 row = 0; row < res_rows; ++row) {	
						T* ptr_result = (T*)ptr_result_dim0;
						i32* ptr_mask = (i32*)ptr_mask_dim0;
						i32 mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								ptr_result[c] = src.at<T>(ptr_mask[c] / src_cols, ptr_mask[c] % src_cols, 0);
							}

							mat_start_col += strides[1];

							// mask mat may be not continuous
							ptr_mask += mask_last_step;
							ptr_result += channels;
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else {
					basiclog_unsupport2();
				}
			}



			template<class T>
			static void pooling_3d_channel_1(mt_mat& res, mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) {
				i32 src_last_step = src.last_dim_element_step();

				u8* ptr_result_dim0 = res.data();
				i32 mat_start_plane = 0;
				i32 res_planes = res.size()[0];
				i32 res_rows = res.size()[1];
				i32 res_cols = res.size()[2];
				i32 src_planes = src.size()[0];
				i32 src_rows = src.size()[1];
				i32 src_cols = src.size()[2];
				i32 src_plane_element_number = src.size()[1] * src.size()[2];

				if (pooling_type == mt_Pooling_Type_Sum || pooling_type == mt_Pooling_Type_Mean) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (i32 col = 0; col < res_cols; ++col) {
								int valid_kernel_element_number = 0;

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (i32 pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											*ptr_result += *ptr_mat;
											ptr_mat += src_last_step;

											++valid_kernel_element_number;	
										}
									}
								}


								if (pooling_type == mt_Pooling_Type_Mean) {
									*ptr_result /= valid_kernel_element_number;
								}

								++ptr_result;
								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Max) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								*ptr_result = src.at<T>(mat_stop_plane, mat_start_row, mat_start_col, 0);
								*ptr_mask = mat_stop_plane * src_plane_element_number + mat_start_row * src.size()[2] + mat_start_col;

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											if (*ptr_mat > *ptr_result) {
												*ptr_result = *ptr_mat;
												*ptr_mask = pooling_plane * src_plane_element_number + pooling_row * src.size()[2] + pooling_col;
											}

											ptr_mat += src_last_step;
										}
									}
								}

								++ptr_result;
								++ptr_mask;

								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];

							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Min) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								*ptr_result = src.at<T>(mat_start_plane, mat_start_row, mat_start_col, 0);
								*ptr_mask = mat_stop_plane * src_plane_element_number + mat_start_row * src.size()[2] + mat_start_col;

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											if (*ptr_mat < *ptr_result) {
												*ptr_result = *ptr_mat;
												*ptr_mask = pooling_plane * src_plane_element_number + pooling_row * src.size()[2] + pooling_col;
											}

											ptr_mat += src_last_step;
										}
									}
								}

								++ptr_result;
								++ptr_mask;

								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];

							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_plane += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;

						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32 mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								*ptr_result++ = src.at<T>(mat_start_plane, mat_start_row, mat_start_col, 0);
								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Index) {
					u8* ptr_mask_dim0 = mask_mat.data();
					i32 mask_last_step = mask_mat.last_dim_element_step();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							for (int col = 0; col < res_cols; ++col) {
								i32 src_plane = *ptr_mask / src_plane_element_number;
								i32 src_row = (*ptr_mask - src_plane * src_plane_element_number) /  src_cols;
								i32 src_col = *ptr_mask % src_cols;

								*ptr_result++ = src.at<T>(src_plane, src_row, src_col, 0);

								ptr_mask += mask_last_step;
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}

				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void pooling_3d_channel_n(mt_mat& res, mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) {
				i32 src_last_step = src.last_dim_element_step();
				i32 channels = res.channel();
				u8* ptr_result_dim0 = res.data();
				i32 mat_start_plane = 0;
				i32 res_planes = res.size()[0];
				i32 res_rows = res.size()[1];
				i32 res_cols = res.size()[2];
				i32 src_planes = src.size()[0];
				i32 src_rows = src.size()[1];
				i32 src_cols = src.size()[2];

				i32 src_plane_element_number = src.size()[1] * src.size()[2];

				if (pooling_type == mt_Pooling_Type_Sum || pooling_type == mt_Pooling_Type_Mean) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (i32 col = 0; col < res_cols; ++col) {
								int valid_kernel_element_number = 0;

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (i32 pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (i32 pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {

											for (i32 c = 0; c < channels; ++c) {
												ptr_result[c] += ptr_mat[c];
											}
											
											ptr_mat += src_last_step;
											++valid_kernel_element_number;	
										}
									}
								}

								if (pooling_type == mt_Pooling_Type_Mean) {
									for (i32 c = 0; c < channels; ++c) {
										ptr_result[c] /= valid_kernel_element_number;
									}
									
								}

								ptr_result += channels;
								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Max) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									ptr_result[c] = src.at<T>(mat_start_plane, mat_start_row, mat_start_col, c);
									ptr_mask[c] = mat_stop_plane * src_plane_element_number + mat_start_row * src_cols + mat_start_col;
								}

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {

											for (i32 c = 0; c < channels; ++c) {
												if (ptr_mat[c] > ptr_result[c]) {
													ptr_result[c] = ptr_mat[c];
													ptr_mask[c] = pooling_plane * src_plane_element_number + pooling_row * src.size()[2] + pooling_col;
												}
											}

											ptr_mat += src_last_step;
										}
									}
								}

								ptr_result += channels;
								ptr_mask += channels;

								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];

							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Min) {
					u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							i32 mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);
							i32 mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									ptr_result[c] = src.at<T>(mat_stop_plane, mat_start_row, mat_start_col, 0);
									ptr_mask[c] = mat_stop_plane * src_plane_element_number + mat_start_row * src_cols + mat_start_col;
								}

								i32 mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										const T* ptr_mat = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											for (i32 c = 0; c < channels; ++c) {
												if (ptr_mat[c] < ptr_result[c]) {
													ptr_result[c] = ptr_mat[c];
													ptr_mask[c] = pooling_plane * src_plane_element_number + pooling_row * src.size()[2] + pooling_col;
												}
											}

											ptr_mat += src_last_step;
										}
									}
								}

								ptr_result += channels;
								ptr_mask += channels;

								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];

							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];

						mat_start_plane += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;

						i32 mat_start_row = 0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32 mat_start_col = 0;


							for (int col = 0; col < res_cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									ptr_result[c] = src.at<T>(mat_start_plane, mat_start_row, mat_start_col, c);
								}
								
								ptr_result += channels;
								mat_start_col += strides[2];
							}

							ptr_result_dim1 += res.step()[1];
							mat_start_row += strides[1];
						}

						ptr_result_dim0 += res.step()[0];
						mat_start_plane += strides[0];
					}

				} else if (pooling_type == mt_Pooling_Type_Index) {
					u8* ptr_mask_dim0 = mask_mat.data();
					i32 mask_last_step = mask_mat.last_dim_element_step();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						u8* ptr_result_dim1 = ptr_result_dim0;
						u8* ptr_mask_dim1 = ptr_mask_dim0;

						for (i32 row = 0; row < res_rows; ++row) {	
							T* ptr_result = (T*)ptr_result_dim1;
							i32* ptr_mask = (i32*)ptr_mask_dim1;

							for (int col = 0; col < res_cols; ++col) {
								i32 src_plane = *ptr_mask / src_plane_element_number;
								i32 src_row = (*ptr_mask - src_plane * src_plane_element_number) /  src_cols;
								i32 src_col = *ptr_mask % src_cols;

								for (i32 c = 0; c < channels; ++c) {
									ptr_result[c] = src.at<T>(src_plane, src_row, src_col, c);
								}
								
								ptr_result += channels;
								ptr_mask += mask_last_step;
							}

							ptr_result_dim1 += res.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];
						}

						ptr_result_dim0 += res.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}

				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void unpooling_2d_channel_1(mt_mat& src, const mt_mat& mask_mat, const mt_mat& pooling_res_mat, mt_Pooling_Type pooling_type, i32 size, const i32* kernel_sizes, const i32* strides) {
				i32 mask_last_step = mask_mat.last_dim_element_step();
				i32 pooling_res_last_step = pooling_res_mat.last_dim_element_step();

				i32 mat_start_row = 0;
				const u8* ptr_pooling_res_dim0 = pooling_res_mat.data();

				i32 src_rows = src.size()[0];
				i32 src_cols = src.size()[1];
				i32 res_rows = pooling_res_mat.size()[0];
				i32 res_cols = pooling_res_mat.size()[1];

				if (pooling_type == mt_Pooling_Type_Mean) {
					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);

						int mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {					
							int mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);

							int area = (mat_stop_row - mat_start_row) * (mat_stop_col - mat_start_col);
							T unpooling_data = *ptr_pooling_res / area;

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								T* ptr_src = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									*ptr_src++ += unpooling_data;
								}
							}

							ptr_pooling_res += pooling_res_last_step;
							mat_start_col += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min || pooling_type == mt_Pooling_Type_Index) {
					const u8* ptr_mask_dim0 = mask_mat.data();

					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int* ptr_mask_dim1 = (int*)ptr_mask_dim0;

						for (int col = 0; col < res_cols; ++col) {

							int src_row = (*ptr_mask_dim1) / src_cols;
							int src_col = *ptr_mask_dim1 % src_cols;

							src.at<T>(src_row, src_col, 0) += *ptr_pooling_res;
							ptr_pooling_res += pooling_res_last_step;
							ptr_mask_dim1 += mask_last_step;
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {
							src.at<T>(mat_start_row, mat_start_col, 0) += *ptr_pooling_res;

							ptr_pooling_res += pooling_res_last_step;
							mat_start_col += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_row += strides[0];
					}
				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void unpooling_2d_channel_n(mt_mat& src, const mt_mat& mask_mat, const mt_mat& pooling_res_mat, mt_Pooling_Type pooling_type, i32 size, const i32* kernel_sizes, const i32* strides) {
				i32 mask_last_step = mask_mat.last_dim_element_step();
				i32 pooling_res_last_step = pooling_res_mat.last_dim_element_step();
				i32 channels = pooling_res_mat.channel();
				i32 mat_start_row = 0;
				const u8* ptr_pooling_res_dim0 = pooling_res_mat.data();

				i32 src_rows = src.size()[0];
				i32 src_cols = src.size()[1];
				i32 res_rows = pooling_res_mat.size()[0];
				i32 res_cols = pooling_res_mat.size()[1];

				if (pooling_type == mt_Pooling_Type_Mean) {
					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int mat_stop_row = min(mat_start_row + kernel_sizes[0], src_rows);

						int mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {					
							int mat_stop_col = min(mat_start_col + kernel_sizes[1], src_cols);
							int area = (mat_stop_row - mat_start_row) * (mat_stop_col - mat_start_col);

							for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
								T* ptr_src = src.ptr<T>(pooling_row, mat_start_col, 0);
								for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
									for (i32 c = 0; c < channels; ++c) {
										ptr_src[c] += ptr_pooling_res[c] / area;
									}
									
									ptr_src += channels;
								}
							}

							ptr_pooling_res += pooling_res_last_step;
							mat_start_col += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_row += strides[0];
					}
				} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min || pooling_type == mt_Pooling_Type_Index) {
					const u8* ptr_mask_dim0 = mask_mat.data();

					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int* ptr_mask_dim1 = (int*)ptr_mask_dim0;

						for (int col = 0; col < res_cols; ++col) {

							int src_row = (*ptr_mask_dim1) / src_cols;
							int src_col = *ptr_mask_dim1 % src_cols;

							for (i32 c = 0; c < channels; ++c) {
								src.at<T>(src_row, src_col, c) += ptr_pooling_res[c];
							}

							ptr_pooling_res += pooling_res_last_step;
							ptr_mask_dim1 += mask_last_step;
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}
				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (int row = 0; row < res_rows; ++row) {
						const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim0;
						int mat_start_col = 0;

						for (int col = 0; col < res_cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								src.at<T>(mat_start_row, mat_start_col, c) += ptr_pooling_res[c];
							}
							
							ptr_pooling_res += pooling_res_last_step;
							mat_start_col += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_row += strides[0];
					}
				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void unpooling_3d_channel_1(mt_mat& src, const mt_mat& mask_mat, const mt_mat& pooling_res_mat, mt_Pooling_Type pooling_type, i32 size, const i32* kernel_sizes, const i32* strides) {
				i32 mask_last_step = mask_mat.last_dim_element_step();
				i32 pooling_res_last_step = pooling_res_mat.last_dim_element_step();

				i32 mat_start_plane = 0;
				const u8* ptr_pooling_res_dim0 = pooling_res_mat.data();

				i32 src_planes = src.size()[0];
				i32 src_rows = src.size()[1];
				i32 src_cols = src.size()[2];
				i32 res_planes = pooling_res_mat.size()[0];
				i32 res_rows = pooling_res_mat.size()[1];
				i32 res_cols = pooling_res_mat.size()[2];

				i32 src_plane_element_number = src.size()[1] * src.size()[2];

				if (pooling_type == mt_Pooling_Type_Mean) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							int mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);

							int mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {					
								int mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								int area = (mat_stop_plane - mat_start_plane) * (mat_stop_row - mat_start_row) * (mat_stop_col - mat_start_col);
								T unpooling_data = *ptr_pooling_res / area;

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										T* ptr_src = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											*ptr_src++ += unpooling_data;
										}
									}
								}


								ptr_pooling_res += pooling_res_last_step;
								mat_start_col += strides[2];
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							mat_start_row += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_plane += strides[0];
					}
					
				} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min || pooling_type == mt_Pooling_Type_Index) {
					const u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;
						const u8* ptr_mask_dim1 = ptr_mask_dim0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							const int* ptr_mask = (const int*)ptr_mask_dim1;

							for (int col = 0; col < res_cols; ++col) {

								i32 src_plane = *ptr_mask / src_plane_element_number;
								i32 src_row = (*ptr_mask - src_plane * src_plane_element_number) /  src_cols;
								i32 src_col = *ptr_mask % src_cols;

								src.at<T>(src_plane, src_row, src_col, 0) += *ptr_pooling_res;
								ptr_pooling_res += pooling_res_last_step;
								ptr_mask_dim1 += mask_last_step;
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}

				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;
						i32 mat_start_row = 0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							int mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								src.at<T>(mat_start_plane, mat_start_row, mat_start_col, 0) += *ptr_pooling_res;

								ptr_pooling_res += pooling_res_last_step;
								mat_start_col += strides[2];
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							mat_start_row += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_plane += strides[0];
					}

				} else {
					basiclog_unsupport2();
				}
			}

			template<class T>
			static void unpooling_3d_channel_n(mt_mat& src, const mt_mat& mask_mat, const mt_mat& pooling_res_mat, mt_Pooling_Type pooling_type, i32 size, const i32* kernel_sizes, const i32* strides) {
				i32 mask_last_step = mask_mat.last_dim_element_step();
				i32 pooling_res_last_step = pooling_res_mat.last_dim_element_step();
				i32 channels = src.channel();
				i32 mat_start_plane = 0;
				const u8* ptr_pooling_res_dim0 = pooling_res_mat.data();

				i32 src_planes = pooling_res_mat.size()[0];
				i32 src_rows = pooling_res_mat.size()[1];
				i32 src_cols = pooling_res_mat.size()[2];
				i32 res_planes = pooling_res_mat.size()[0];
				i32 res_rows = pooling_res_mat.size()[1];
				i32 res_cols = pooling_res_mat.size()[2];

				i32 src_plane_element_number = src.size()[1] * src.size()[2];

				if (pooling_type == mt_Pooling_Type_Mean) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;

						i32 mat_stop_plane = min(mat_start_plane + kernel_sizes[0], src_planes);
						i32 mat_start_row = 0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							int mat_stop_row = min(mat_start_row + kernel_sizes[1], src_rows);

							int mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {					
								int mat_stop_col = min(mat_start_col + kernel_sizes[2], src_cols);

								int area = (mat_stop_plane - mat_start_plane) * (mat_stop_row - mat_start_row) * (mat_stop_col - mat_start_col);

								for (i32 pooling_plane = mat_start_plane; pooling_plane < mat_stop_plane; ++pooling_plane) {
									for (int pooling_row = mat_start_row; pooling_row < mat_stop_row; ++pooling_row) {
										T* ptr_src = src.ptr<T>(pooling_plane, pooling_row, mat_start_col, 0);
										for (int pooling_col = mat_start_col; pooling_col < mat_stop_col; ++pooling_col) {
											for (i32 c = 0; c < channels; ++c) {
												ptr_src[c] += ptr_pooling_res[c] / area;
											}

											ptr_src += channels;
										}
									}
								}

								ptr_pooling_res += pooling_res_last_step;
								mat_start_col += strides[2];
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							mat_start_row += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_plane += strides[0];
					}

					//basiclog_info2(src);
				} else if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min || pooling_type == mt_Pooling_Type_Index) {
					const u8* ptr_mask_dim0 = mask_mat.data();

					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;
						const u8* ptr_mask_dim1 = ptr_mask_dim0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							const int* ptr_mask = (const int*)ptr_mask_dim1;

							for (int col = 0; col < res_cols; ++col) {

								i32 src_plane = *ptr_mask / src_plane_element_number;
								i32 src_row = (*ptr_mask - src_plane * src_plane_element_number) /  src_cols;
								i32 src_col = *ptr_mask % src_cols;

								for (i32 c = 0; c < channels; ++c) {
									src.at<T>(src_plane, src_row, src_col, c) += ptr_pooling_res[c];
								}
								
								ptr_pooling_res += pooling_res_last_step;
								ptr_mask_dim1 += mask_last_step;
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							ptr_mask_dim1 += mask_mat.step()[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						ptr_mask_dim0 += mask_mat.step()[0];
					}

				} else if (pooling_type == mt_Pooling_Type_First_Value) {
					for (i32 plane = 0; plane < res_planes; ++plane) {
						const u8* ptr_pooling_res_dim1 = ptr_pooling_res_dim0;
						i32 mat_start_row = 0;

						for (int row = 0; row < res_rows; ++row) {
							const T* ptr_pooling_res = (const T*)ptr_pooling_res_dim1;
							int mat_start_col = 0;

							for (int col = 0; col < res_cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									src.at<T>(mat_start_plane, mat_start_row, mat_start_col, c) += ptr_pooling_res[c];
								}
								
								ptr_pooling_res += pooling_res_last_step;
								mat_start_col += strides[2];
							}

							ptr_pooling_res_dim1 += pooling_res_mat.step()[1];
							mat_start_row += strides[1];
						}

						ptr_pooling_res_dim0 += pooling_res_mat.step()[0];
						mat_start_plane += strides[0];
					}

				} else {
					basiclog_unsupport2();
				}
			}
	};


}


mt_mat mt_mat::pooling(mt_mat& mask_mat, mt_Pooling_Type pooling_type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides) const {
	basicmath_mat_request_memory(i32, res_sizes, dim());

	for (int i = 0; i < dim(); ++i) {
		res_sizes[i] = mt_mat_helper::get_pooling_result_size(this->size()[i], kernel_sizes[i], strides[i]);
	}

	mt_mat res(dim(), res_sizes, depth_channel(), 0);

	if (pooling_type == mt_Pooling_Type_Max || pooling_type == mt_Pooling_Type_Min) {
		mask_mat = mt_mat(dim(), res_sizes, mt_Depth_Channel(mt_I32, channel()), -1);
	}

	switch (depth()) {
	case mt_U8:
		private_pooling::pooling<u8>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I8:
		private_pooling::pooling<i8>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U16:
		private_pooling::pooling<u16>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I16:
		private_pooling::pooling<i16>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U32:
		private_pooling::pooling<u32>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I32:
		private_pooling::pooling<i32>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U64:
		private_pooling::pooling<u64>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I64:
		private_pooling::pooling<i64>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_F32:
		private_pooling::pooling<f32>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_F64:
		private_pooling::pooling<f64>(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	default:
		basiclog_unsupport2();
		break;
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.attach(m_auto_derivative);

		m_auto_derivative->pooling(res, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
	}

	basicmath_mat_release(res_sizes);

	return res;
}

mt_mat mt_mat::pooling(mt_mat& mask_mat, mt_Pooling_Type pooling_type, const vector<basicsys::i32>& kernel_sizes, const vector<basicsys::i32>& strides) const {
	return pooling(mask_mat, pooling_type, (i32)kernel_sizes.size(), &kernel_sizes[0], &strides[0]);
}

mt_mat mt_mat::unpooling(const int* src_size, const mt_mat& mask_mat, mt_Pooling_Type pooling_type, i32 size, const int* kernel_sizes, const int* strides) const {	
	basiclog_assert2(mask_mat.is_empty());
	
	mt_mat src(dim(), src_size, depth_channel(), 0);
	
	switch (depth()) {
	case mt_U8:
		private_pooling::unpooling<u8>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I8:
		private_pooling::unpooling<u8>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U16:
		private_pooling::unpooling<u16>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I16:
		private_pooling::unpooling<i16>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U32:
		private_pooling::unpooling<u32>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I32:
		private_pooling::unpooling<i32>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_U64:
		private_pooling::unpooling<u64>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_I64:
		private_pooling::unpooling<i64>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_F32:
		private_pooling::unpooling<f32>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	case mt_F64:
		private_pooling::unpooling<f64>(src, mask_mat, *this, pooling_type, size, kernel_sizes, strides);
		break;
	default:
		basiclog_unsupport2();
		break;
	}

	return src;
}

mt_mat mt_mat::unpooling(const int* src_size, const mt_mat& mask_mat, mt_Pooling_Type pooling_type, const vector<basicsys::i32>& kernel_sizes, const vector<basicsys::i32>& strides) const {	
	return unpooling(src_size, mask_mat, pooling_type, (i32)kernel_sizes.size(), &kernel_sizes[0], &strides[0]);
}
