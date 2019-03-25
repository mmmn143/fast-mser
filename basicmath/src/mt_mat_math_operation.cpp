#include "stdafx.h"
#include "mt_mat.h"
#include "mt_auto_derivative.h"
#include "mt_mat_helper.h"

namespace basicmath {

	class private_math_operation {
	public:

		template<class T>
		static void sigmoid(mt_mat& res, const mt_mat& src) {
			i32 dims = res.dim();

			i32 res_last_step = res.last_dim_element_step();
			i32 src_last_step = src.last_dim_element_step();
			i32 element_number = res.element_number();
			i32 channels = res.channel();

			if (res.is_continuous() && src.is_continuous()) {
				T* ptr_res = (T*)res.data();
				const T* ptr_src = (const T*)src.data();

				if (channels == 1) {
					for (i32 i = 0; i < element_number; ++i) {
						ptr_res[0] = (T)(1.0 / (1.0 + std::exp(-ptr_src[0])));

						ptr_res += res_last_step;
						ptr_src += src_last_step;
					}
				} else {
					for (i32 i = 0; i < element_number; ++i) {
						for (i32 c = 0; c < channels; ++c) {
							ptr_res[c] = (T)(1.0 / (1.0 + std::exp(-ptr_src[c])));
						}
						
						ptr_res += res_last_step;
						ptr_src += src_last_step;
					}
				}
			} else if (dims == 2) {
				i32 rows = res.size()[0];
				i32 cols = res.size()[1];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_src_dim0 = src.data();

				if (channels == 1) {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;

						for (i32 col = 0; col < cols; ++col) {
							ptr_res[0] = (T)(1.0 / (1.0 + std::exp(-ptr_src[0])));

							ptr_res += res_last_step;
							ptr_src += src_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;

						for (i32 col = 0; col < cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								ptr_res[c] = (T)(1.0 / (1.0 + std::exp(-ptr_src[c])));
							}

							ptr_res += res_last_step;
							ptr_src += src_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}
			} else if (dims == 3) {
				i32 planes = res.size()[0];
				i32 rows = res.size()[1];
				i32 cols = res.size()[2];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_src_dim0 = src.data();

				if (channels == 1) {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;

							for (i32 col = 0; col < cols; ++col) {
								ptr_res[0] = (T)(1.0 / (1.0 + std::exp(-ptr_src[0])));

								ptr_res += res_last_step;
								ptr_src += src_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}

				} else {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;

							for (i32 col = 0; col < cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									ptr_res[c] = (T)(1.0 / (1.0 + std::exp(-ptr_src[c])));
								}

								ptr_res += res_last_step;
								ptr_src += src_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}
			}else {
				mt_array_element_const_iterator src_iter(src);
				mt_array_element_iterator res_iter(res);

				for (;;) {
					const T* ptr_src = (const T*)src_iter.data();

					if (ptr_src == NULL) {
						break;
					}

					T* ptr_res = (T*)res_iter.data();

					for (i32 c = 0; c < src.channel(); ++c) {
						ptr_res[c] = (T)(1.0 / (1.0 + std::exp(-ptr_src[c])));
					}
				}
			}
		}

		template<class T>
		static void tanh(mt_mat& res, const mt_mat& src, f64 alpha = mt_Tanh_Default_Alpha, f64 belta = mt_Tanh_Default_Beta) {

			mt_array_element_const_iterator src_iter(src);
			mt_array_element_iterator res_iter(res);

			for (;;) {
				const T* ptr_src = (const T*)src_iter.data();

				if (ptr_src == NULL) {
					break;
				}

				T* ptr_res = (T*)res_iter.data();

				for (i32 c = 0; c < src.channel(); ++c) {
					ptr_res[c] = (T)alpha * std::tanh((T)belta * ptr_src[c]);
				}
			}
		}

		template<class T>
		static void relu(mt_mat& res, const mt_mat& src, f64 negative_slope) {

			mt_array_element_const_iterator src_iter(src);
			mt_array_element_iterator res_iter(res);

			for (;;) {
				const T* ptr_src = (const T*)src_iter.data();

				if (ptr_src == NULL) {
					break;
				}

				T* ptr_res = (T*)res_iter.data();

				for (i32 c = 0; c < src.channel(); ++c) {
					if (ptr_src[c] > 0) {
						ptr_res[c] = ptr_src[c];
					} else {
						ptr_res[c] = (T)(ptr_src[c] * negative_slope);
					}
				}
			}
		}

		template<class T>
		static void softmax(mt_mat& res, const mt_mat& src) {
			basiclog_assert2(2 == src.dim());
			
			const u8* ptr_src_dim0 = src.data();
			u8* ptr_dst_dim0 = res.data();

			for (i32 row = 0; row < src.size()[0]; ++row) {
				const T* ptr_src = (T*)ptr_src_dim0;				
				T max_value = *ptr_src++;

				for (int col = 1; col < src.size()[1]; ++col) {
					if (*ptr_src > max_value) {
						max_value = *ptr_src;
					}

					++ptr_src;
				}

				ptr_src = (T*)ptr_src_dim0;
				T* ptr_dst = (T*)ptr_dst_dim0;
				T sum = (T)0.0;

				for (i32 col = 0; col < src.size()[1]; ++col) {
					*ptr_dst = exp(*ptr_src++ - max_value);
					sum += *ptr_dst++;
				}

				ptr_dst = (T*)ptr_dst_dim0;

				for (i32 col = 0; col < src.size()[1]; ++col) {
					*ptr_dst++ /= sum;
				}

				ptr_src_dim0 += src.step()[0];
				ptr_dst_dim0 += res.step()[0];
			}
		}

		template<class T>
		static void activate(mt_mat& res, const mt_mat& src, mt_Activate_Type type, i32 activate_param_size, const f64* activate_params) {
			if (type == mt_Activate_Type_Softmax) {
				basiclog_assert2(src.dim() == 2);
				basiclog_assert2(src.channel() == 1);

				softmax<T>(res, src);
			} else {
				if (type == mt_Activate_Type_Linear) {
					res.set(src);
				} else {
					switch (type) {
					case mt_Activate_Type_Relu:
						basiclog_assert2(activate_param_size == 0 || activate_param_size == 1);
						private_math_operation::relu<T>(res, src, activate_param_size == 0 ? 0 : activate_params[0]);
						break;
					case mt_Activate_Type_Sigmoid:
						basiclog_assert2(activate_param_size == 0);
						private_math_operation::sigmoid<T>(res, src);
						break;
					case mt_Activate_Type_Tanh:
						basiclog_assert2(activate_param_size == 0 || activate_param_size == 2);

						if (activate_param_size == 0) {
							private_math_operation::tanh<T>(res, src);
						} else {
							private_math_operation::tanh<T>(res, src, activate_params[0], activate_params[1]);
						}
						
						break;
					default:
						basiclog_unsupport2();
						break;
					}
				}
			}
		}

		template<class T>
		static mt_mat quardratic_loss(const mt_mat& src, const mt_mat& matching_mat) {
			mt_array_element_const_iterator src_iter(src);
			mt_array_element_const_iterator matching_iter(matching_mat);

			mt_mat loss_res(1, 1, src.depth_channel());
			T* ptr_dst = loss_res.ptr<T>(0, i32(0));

			for (;;) {
				const T* ptr_src = (const T*)src_iter.data();

				if (ptr_src == NULL) {
					break;
				}

				const T* ptr_matching = (const T*)matching_mat.data();

				for (i32 c = 0; c < src.dim(); ++c) {
					if (mt_helper::is_number(ptr_matching[c])) {
						ptr_dst[c] += (ptr_src[c] - ptr_matching[c]) * (ptr_src[c] - ptr_matching[c]) / 2;
					}
				}
			}

			return loss_res;
		}

		template<class T>
		static mt_mat logarithmic_loss(const mt_mat& src, const mt_mat& matching_mat) {
			basiclog_assert2(src.channel() == 1);
			basiclog_assert2(src.dim() == 2);

			mt_mat loss_res(1, 1, src.depth_channel(), 0.0);
			T* ptr_dst = loss_res.ptr<T>(0, i32(0));

			const u8* ptr_src_dim0 = src.data();
			const u8* ptr_matching_dim0 = matching_mat.data();

			if (1 == src.size()[1]) {
				for (i32 row = 0; row < src.size()[0]; ++row) {
					const T* ptr_src_dim1 = (const T*)ptr_src_dim0;
					const T* ptr_matching_dim1 = (const T*)ptr_matching_dim0;

					if (mt_helper::is_number(ptr_matching_dim1[0])) {
						if (1 == (i32)*ptr_matching_dim1) {
							*ptr_dst -= (T)log(*ptr_src_dim1 + DBL_EPSILON);
						} else if (0 == (i32)*ptr_matching_dim1){
							*ptr_dst -= (T)log(1 - *ptr_src_dim1 + DBL_EPSILON);
						} else {
							basiclog_assert2(false);
						}
					}

					ptr_src_dim0 += src.step()[0];
					ptr_matching_dim0 += matching_mat.step()[0];
				}
			} else {
				for (i32 row = 0; row < src.size()[0]; ++row) {
					const T* ptr_src_dim1 = (const T*)ptr_src_dim0;
					const T* ptr_matching_dim1 = (const T*)ptr_matching_dim0;

					if (mt_helper::is_number(ptr_matching_dim1[0])) {
						i32 col = 0;
						for (; col < src.size()[1]; ++col) {
							if (1 == (i32)*ptr_matching_dim1) {
								*ptr_dst -= (T)log(*ptr_src_dim1 + DBL_EPSILON);
								break;
							} else {
								++ptr_matching_dim1;
								++ptr_src_dim1;
							}
						}

						basiclog_assert_message2(col < src.size()[1], L"matching_mat must be one-shot vector, you may input the predicted and matching mat in wrong order!");
					}

					ptr_src_dim0 += src.step()[0];
					ptr_matching_dim0 += matching_mat.step()[0];
				}
			}

			return loss_res;
		}

		template<class T>
		static mt_mat loss(const mt_mat& src, const mt_mat& matching_mat, mt_Loss_Type type) {
			switch (type) {
			case mt_Loss_Type_Quadratic:
				return quardratic_loss<T>(src, matching_mat);
			case mt_Loss_Type_Logarithmic:
				return logarithmic_loss<T>(src, matching_mat);
			case mt_Loss_Type_0_1:
				return mt_mat();
			default:
				return mt_mat();
			}
		}

		template<class T>
		static void symmetry_eigen(mt_mat& eigen_value, mt_mat& eigen_vector, mt_mat& mat) {
			T eps = (T)0;
			if (typeid(T) == typeid(f32)) {
				eps = (T)mt_helper::get_float_eps();
			} else if (typeid(T) == typeid(f64)) {
				eps = (T)mt_helper::get_double_eps();
			} else {
				basiclog_unsupport2();
				return;
			}

			basiclog_debug2(sys_strcombine()<<L"esplion: "<<eps);

			i32 n = mat.size()[0];
			
			eigen_value = mt_mat(1, mat.size()[0], mat.depth_channel());
			eigen_vector = mt_mat(mat.size()[0], mat.size()[0], mat.depth_channel());
			mt_mat tmp_mat = mat.clone();

			//jacobi((T*)mat.data(), mat.step()[0], n, (T*)eigen_value.data(), (T*)eigen_vector.data(), (i8*)buffer.data(), eps);
			jacobi(eigen_value, eigen_vector, mat, eps);
		}

		template<typename T> 
		static T hypot(T a, T b) {
			a = std::fabs(a);
			b = std::fabs(b);
			if (a > b) {
				b /= a;
				return a * std::sqrt(1 + b * b);
			}
			if (b > 0) {
				a /= b;
				return b * std::sqrt(1 + a * a);
			}
			return 0;
		}

		//static void jacobi(T* mat, i32 step, i32 n, T* ev, T* ec, i8* buf, T eps) {
		template<typename T> 
		static void jacobi(mt_mat& eigen_value, mt_mat& eigen_vector, mt_mat& src_mat, T eps) {
			T* mat = (T*)src_mat.data();
			i32 step = src_mat.step()[0];

			T* ev = (T*)eigen_value.data();
			T* ec = (T*)eigen_vector.data();

			vector<i32> buf_vector(2 * src_mat.size()[0]);
			i8* buf = (i8*)&buf_vector[0];
			i32 n = src_mat.size()[0];

			i32 i, j, k, m;

			step /= sizeof(mat[0]);
			if (ec) {
				for (i = 0; i < n; i++) {
					for (j = 0; j < n; j++) {
						ec[i * step + j] = (T)0;
					}
					ec[i * step + i] = (T)1;
				}
			}

			i32 iters, max_iters = n * n * 30;

			// FIXME: align buffer
			i32* indr = (i32*)buf;
			i32* indc = indr + n;
			T mv = (T)0;

			for (k = 0; k < n; k++) {
				ev[k] = mat[(step + 1) * k];
				if (k < n - 1) {
					for(m = k + 1, mv = std::fabs(mat[step * k + m]), i = k + 2; i < n; i++) {
						T val = std::fabs(mat[step * k + i]);
						if( mv < val ) {
							mv = val, m = i;
						}
					}
					indr[k] = m;
				}
				if (k > 0) {
					for (m = 0, mv = std::fabs(mat[k]), i = 1; i < k; i++) {
						T val = std::fabs(mat[step * i + k]);
						if(mv < val) {
							mv = val, m = i;
						}
					}
					indc[k] = m;
				}
			}

			if (n > 1) {
				for (iters = 0; iters < max_iters; iters++) {
					// log eigen values and eigen vectors of each iteration
					//if(basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_debug()) {
					//	basiclog_debug2(sys_strcombine() << L"iteration " << iters << L" :");
					//	for(i32 i = 0; i < n; ++i) {
					//		basiclog_debug2(sys_strcombine() << L"e" << i << L" = " << ev[i]);
					//		sys_strcombine str;
					//		str << L"v" << i << L" = (";
					//		for(j = 0; j < n; ++j) {
					//			str << ec[step * i + j];
					//			if(j != n - 1) {
					//				str << L", ";
					//			}
					//		}
					//		str << L")";
					//		basiclog_debug2(str);
					//	}
					//}

					basiclog_debug2(sys_strcombine()<<L"iteration "<<iters<<L"\n"<<L"eigen values "<<eigen_value<<L"\neigen vectors "<<eigen_vector);
					
					// find index (k,l) of pivot p
					for (k = 0, mv = std::fabs(mat[indr[0]]), i = 1; i < n - 1; i++) {
						T val = std::fabs(mat[step * i + indr[i]]);
						if (mv < val) {
							mv = val, k = i;
						}
					}
					i32 l = indr[k];
					for (i = 1; i < n; i++) {
						T val = std::fabs(mat[step * indc[i] + i]);
						if (mv < val) {
							mv = val, k = indc[i], l = i;
						}
					}

					T p = mat[step * k + l];
					if (std::fabs(p) <= eps) {
						break;
					}
					T y = (T)((ev[l] - ev[k]) * 0.5);
					T t = (T)std::fabs(y) + hypot(p, y);
					T s = hypot(p, t);
					T c = t/s;
					s = p/s; t = (p/t) * p;
					if (y < 0) {
						s = -s, t = -t;
					}
					mat[step * k + l] = 0;

					ev[k] -= t;
					ev[l] += t;

					T a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0 * c - b0 * s, v1 = a0 * s + b0 * c

					// rotate rows and columns k and l
					for (i = 0; i < k; i++) {
						rotate(mat[step * i + k], mat[step * i + l]);
					}
					for (i = k + 1; i < l; i++) {
						rotate(mat[step * k + i], mat[step * i + l]);
					}
					for (i = l + 1; i < n; i++) {
						rotate(mat[step * k + i], mat[step * l + i]);
					}

					// rotate eigenvectors
					if (ec) {
						for (i = 0; i < n; i++) {
							rotate(ec[step * k + i], ec[step * l + i]);
						}
					}
#undef rotate
				
					for (j = 0; j < 2; j++) {
						i32 idx = j == 0 ? k : l;
						if (idx < n - 1) {
							for (m = idx + 1, mv = std::fabs(mat[step * idx + m]), i = idx + 2; i < n; i++) {
								T val = std::fabs(mat[step * idx + i]);
								if (mv < val) {
									mv = val, m = i;
								}
							}
							indr[idx] = m;
						}
						if (idx > 0) {
							for (m = 0, mv = std::fabs(mat[idx]), i = 1; i < idx; i++) {
								T val = std::fabs(mat[step * i + idx]);
								if (mv < val) {
									mv = val, m = i;
								}
							}
							indc[idx] = m;
						}
					}
				}
			}
			
			// sort eigenvalues & eigenvectors
			for (k = 0; k < n - 1; k++) {
				m = k;
				for (i = k + 1; i < n; i++) {
					if (ev[m] < ev[i]) {
						m = i;
					}
				}
				if (k != m) {
					std::swap(ev[m], ev[k]);
					if (ec) {
						for (i = 0; i < n; i++) {
							std::swap(ec[step * m + i], ec[step * k + i]);
						}
					}
				}
			}
		}

		template<class T>
		static mt_mat reduce_inner_channel(const mt_mat& src, mt_mat::Reduce_Type type) {
			basicmath_mat_request_memory(i32, dst_sizes, src.dim());

			for (i32 i = 0; i < src.dim(); ++i) {
				dst_sizes[i] = 1;
			}

			mt_mat dst(src.dim(), dst_sizes, src.depth_channel(), 0);

			mt_mat mean;

			if (type == mt_mat::Reduce_Type_Standard_Unbias_Variance 
				|| type == mt_mat::Reduce_Type_Standard_Variance 
				|| type == mt_mat::Reduce_Type_Unbias_Variance
				|| type == mt_mat::Reduce_Type_Variance) {
					mean = reduce_inner_channel<T>(src, mt_mat::Reduce_Type_Mean);
			}

			if (type == mt_mat::Reduce_Type_Max || type == mt_mat::Reduce_Type_Min) {
				basicmath_mat_request_memory(f64, first_element, src.dim());
				
				src.get(first_element, 0);
				dst.set(src.dim(), first_element);

				basicmath_mat_release(first_element);
			}

			mt_array_element_const_iterator src_iter(src);

			T* ptr_res = dst.ptr<T>(0, 0);

			for (;;) {
				const T* ptr_src = (const T*)src_iter.data();

				if (ptr_src == NULL) {
					break;
				}

				T* ptr_mean = NULL;

				if (!mean.is_empty()) {
					ptr_mean = mean.ptr<T>(0, 0);
				}

				for (i32 c = 0; c < src.channel(); ++c) {
					switch (type) {
					case mt_mat::Reduce_Type_Sum:
					case mt_mat::Reduce_Type_Mean:
						ptr_res[c] += ptr_src[c];
						break;
					case mt_mat::Reduce_Type_Max:
						if (ptr_src[c] > ptr_res[c]) {
							ptr_res[c] = ptr_src[c];
						}

						break;
					case mt_mat::Reduce_Type_Min:
						if (ptr_src[c] < ptr_res[c]) {
							ptr_res[c] = ptr_src[c];
						}

						break;
					case mt_mat::Reduce_Type_Standard_Unbias_Variance:
					case mt_mat::Reduce_Type_Standard_Variance:
					case mt_mat::Reduce_Type_Unbias_Variance:
					case mt_mat::Reduce_Type_Variance:
						ptr_res[c] += (ptr_src[c] - ptr_mean[c]) * (ptr_src[c] - ptr_mean[c]);
						break;
					}
				}

			}

			if (type == mt_mat::Reduce_Type_Mean) {
				dst /= src.element_number();
			} else if (type == mt_mat::Reduce_Type_Variance) {
				dst /= src.element_number();
			} else if (type == mt_mat::Reduce_Type_Standard_Variance) {
				dst /= src.element_number();
				dst.self_pow(0.5);
			} else if (type == mt_mat::Reduce_Type_Unbias_Variance) {
				dst /= src.element_number() - 1;
			} else if (type == mt_mat::Reduce_Type_Standard_Unbias_Variance) {
				dst /= src.element_number() - 1;
				dst.self_pow(0.5);
			}

			basicmath_mat_release(dst_sizes);

			return dst;
		}

		template<class T>
		static mt_mat reduce(const mt_mat& src, mt_mat::Reduce_Type type, i32 reduce_dim) {
			if (reduce_dim == mt_mat::Reduce_Dim_Inner_Channel) {
				return reduce_inner_channel<T>(src, type);
			}

			basicmath_mat_request_memory(i32, dst_sizes, src.dim());

			for (i32 i = 0; i < src.dim(); ++i) {
				dst_sizes[i] = src.size()[i];
			}

			dst_sizes[reduce_dim] = 1; 

			mt_mat dst(src.dim(), dst_sizes, src.depth_channel(), 0);

			mt_mat mean;

			if (type == mt_mat::Reduce_Type_Standard_Unbias_Variance 
				|| type == mt_mat::Reduce_Type_Standard_Variance 
				|| type == mt_mat::Reduce_Type_Unbias_Variance
				|| type == mt_mat::Reduce_Type_Variance) {
					mean = reduce<T>(src, mt_mat::Reduce_Type_Mean, reduce_dim);
			}
			
			if (type == mt_mat::Reduce_Type_Max || type == mt_mat::Reduce_Type_Min) {
				dst.set(src.sub(0, 1, reduce_dim));
			}

			mt_array_element_const_iterator src_iter(src);

			for (;;) {
				const T* ptr_src = (const T*)src_iter.data();

				if (ptr_src == NULL) {
					break;
				}

				for (i32 i = 0; i < src.dim(); ++i) {
					dst_sizes[i] = src_iter.position()[i];
				}

				dst_sizes[reduce_dim] = 0;

				T* ptr_res = dst.ptr<T>(dst.dim(), dst_sizes, 0);
				T* ptr_mean = NULL;

				if (!mean.is_empty()) {
					ptr_mean = mean.ptr<T>(mean.dim(), dst_sizes, 0);
				}

				for (i32 c = 0; c < src.channel(); ++c) {
					switch (type) {
					case mt_mat::Reduce_Type_Sum:
					case mt_mat::Reduce_Type_Mean:
						ptr_res[c] += ptr_src[c];
						break;
					case mt_mat::Reduce_Type_Max:
						if (ptr_src[c] > ptr_res[c]) {
							ptr_res[c] = ptr_src[c];
						}

						break;
					case mt_mat::Reduce_Type_Min:
						if (ptr_src[c] < ptr_res[c]) {
							ptr_res[c] = ptr_src[c];
						}

						break;
					case mt_mat::Reduce_Type_Standard_Unbias_Variance:
					case mt_mat::Reduce_Type_Standard_Variance:
					case mt_mat::Reduce_Type_Unbias_Variance:
					case mt_mat::Reduce_Type_Variance:
						ptr_res[c] += (ptr_src[c] - ptr_mean[c]) * (ptr_src[c] - ptr_mean[c]);
						break;
					}
				}

			}

			if (type == mt_mat::Reduce_Type_Mean) {
				dst /= src.size()[reduce_dim];
			} else if (type == mt_mat::Reduce_Type_Variance) {
				dst /= src.size()[reduce_dim];
			} else if (type == mt_mat::Reduce_Type_Standard_Variance) {
				dst /= src.size()[reduce_dim];
				dst.self_pow(0.5);
			} else if (type == mt_mat::Reduce_Type_Unbias_Variance) {
				dst /= src.size()[reduce_dim] - 1;
			} else if (type == mt_mat::Reduce_Type_Standard_Unbias_Variance) {
				dst /= src.size()[reduce_dim] - 1;
				dst.self_pow(0.5);
			}

			basicmath_mat_release(dst_sizes);

			//basiclog_info2(dst);

			return dst;
		}

		template<class T>
		static void mat_operation(mt_mat& res, const mt_mat& a, const mt_mat& b, mt_mat_helper::Math_Op_Code code) {
			i32 element_number = a.element_number();
			i32 channels = a.channel();

			i32 last_res_step = res.last_dim_element_step();
			i32 last_a_step = a.last_dim_element_step();
			i32 last_b_step = b.last_dim_element_step();

			if (res.is_continuous() && a.is_continuous() && b.is_continuous()) {
				T* ptr_res = (T*)res.data();
				const T* ptr_a = (const T*)a.data();
				const T* ptr_b = (const T*)b.data();

				if (channels == 1) {
					for (i32 i = 0; i < element_number; ++i) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Add:
							ptr_res[0] = ptr_a[0] + ptr_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[0] = ptr_a[0] - ptr_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Dot_Mul:
							ptr_res[0] = ptr_a[0] * ptr_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[0] = ptr_a[0] / ptr_b[0];
							break;
						default:
							basiclog_unsupport2();
							break;
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
						ptr_b += last_b_step;
					}
				} else {
					for (i32 i = 0; i < element_number; ++i) {
						for (i32 c = 0; c < channels; ++c) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Add:
								ptr_res[c] = ptr_a[c] + ptr_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[c] = ptr_a[c] - ptr_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Dot_Mul:
								ptr_res[c] = ptr_a[c] * ptr_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[c] = ptr_a[c] / ptr_b[c];
								break;
							default:
								basiclog_unsupport2();
								break;
							}
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
						ptr_b += last_b_step;
					}
				}
			} else if (a.dim() == 2) {
				i32 rows = a.size()[0];
				i32 cols = a.size()[1];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();
				const u8* ptr_b_dim0 = b.data();

				if (channels == 1) {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;
						const T* ptr_b = (const T*)ptr_b_dim0;

						for (i32 col = 0; col < cols; ++col) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Add:
								ptr_res[0] = ptr_a[0] + ptr_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[0] = ptr_a[0] - ptr_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Dot_Mul:
								ptr_res[0] = ptr_a[0] * ptr_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[0] = ptr_a[0] / ptr_b[0];
								break;
							default:
								basiclog_unsupport2();
								break;
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
							ptr_b += last_b_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
						ptr_b_dim0 += b.step()[0];
					}
				} else {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;
						const T* ptr_b = (const T*)ptr_b_dim0;

						for (i32 col = 0; col < cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Add:
									ptr_res[c] = ptr_a[c] + ptr_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[c] = ptr_a[c] - ptr_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Dot_Mul:
									ptr_res[c] = ptr_a[c] * ptr_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[c] = ptr_a[c] / ptr_b[c];
									break;
								default:
									basiclog_unsupport2();
									break;
								}
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
							ptr_b += last_b_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
						ptr_b_dim0 += b.step()[0];
					}
				}
			} else if (a.dim() == 3) {
				i32 planes = a.size()[0];
				i32 rows = a.size()[1];
				i32 cols = a.size()[2];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();
				const u8* ptr_b_dim0 = b.data();

				if (channels == 1) {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;
						const u8* ptr_b_dim1 = ptr_b_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;
							const T* ptr_b = (const T*)ptr_b_dim1;

							for (i32 col = 0; col < cols; ++col) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Add:
									ptr_res[0] = ptr_a[0] + ptr_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[0] = ptr_a[0] - ptr_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Dot_Mul:
									ptr_res[0] = ptr_a[0] * ptr_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[0] = ptr_a[0] / ptr_b[0];
									break;
								default:
									basiclog_unsupport2();
									break;
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
								ptr_b += last_b_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
							ptr_b_dim1 += b.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
						ptr_b_dim0 += b.step()[0];
					}

				} else {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;
						const u8* ptr_b_dim1 = ptr_b_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;
							const T* ptr_b = (const T*)ptr_b_dim1;

							for (i32 col = 0; col < cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									switch (code) {
									case mt_mat_helper::Math_Op_Code_Add:
										ptr_res[c] = ptr_a[c] + ptr_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Subtract:
										ptr_res[c] = ptr_a[c] - ptr_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Dot_Mul:
										ptr_res[c] = ptr_a[c] * ptr_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Div:
										ptr_res[c] = ptr_a[c] / ptr_b[c];
										break;
									default:
										basiclog_unsupport2();
										break;
									}
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
								ptr_b += last_b_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
							ptr_b_dim1 += b.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
						ptr_b_dim0 += b.step()[0];
					}
				}
			} else {
				mt_array_element_iterator iter_res(res);
				mt_array_element_const_iterator iter_a(a);
				mt_array_element_const_iterator iter_b(b);

				for (;;) {
					T* ptr_res = (T*)iter_res.data();

					if (ptr_res == NULL) {
						break;
					}

					const T* ptr_a = (const T*)iter_a.data();
					const T* ptr_b = (const T*)iter_b.data();

					for (i32 c = 0; c < channels; ++c) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Add:
							ptr_res[c] = ptr_a[c] + ptr_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[c] = ptr_a[c] - ptr_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Dot_Mul:
							ptr_res[c] = ptr_a[c] * ptr_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[c] = ptr_a[c] / ptr_b[c];
							break;
						default:
							basiclog_unsupport2();
							break;
						}
					}
				}
			}
		}

		template<class T>
		static void mat_operation(mt_mat& res, const mt_mat& a, const vector<f64>& b, mt_mat_helper::Math_Op_Code code) {
			i32 element_number = a.element_number();
			i32 channels = a.channel();

			basicmath_mat_request_memory(T, type_b, (i32)b.size());

			for (i32 i = 0; i < (i32)b.size(); ++i) {
				type_b[i] = (T)b[i];
			}

			i32 last_res_step = res.last_dim_element_step();
			i32 last_a_step = a.last_dim_element_step();

			if (res.is_continuous() && a.is_continuous()) {
				T* ptr_res = (T*)res.data();
				const T* ptr_a = (const T*)a.data();

				if (channels == 1) {
					for (i32 i = 0; i < element_number; ++i) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Add:
							ptr_res[0] = ptr_a[0] + type_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[0] = ptr_a[0] - type_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Dot_Mul:
							ptr_res[0] = ptr_a[0] * type_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[0] = ptr_a[0] / type_b[0];
							break;
						case mt_mat_helper::Math_Op_Code_Exp:
							ptr_res[0] = (T)exp(ptr_a[0]);
							break;
						case mt_mat_helper::Math_Op_Code_Ln:
							ptr_res[0] = (T)log(ptr_a[0]);
							break;
						case mt_mat_helper::Math_Op_Code_Log:
							ptr_res[0] = (T)(log10(ptr_a[0]) / log10(type_b[0]));
							break;
						case mt_mat_helper::Math_Op_Code_Pow:
							ptr_res[0] = (T)pow(ptr_a[0], type_b[0]);
							break;
						case mt_mat_helper::Math_Op_Code_Abs:
							ptr_res[0] = std::abs(ptr_a[0]);
							break;
						default:
							basiclog_unsupport2();
							break;
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
					}
				} else {
					for (i32 i = 0; i < element_number; ++i) {
						for (i32 c = 0; c < channels; ++c) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Add:
								ptr_res[c] = ptr_a[c] + type_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[c] = ptr_a[c] - type_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Dot_Mul:
								ptr_res[c] = ptr_a[c] * type_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[c] = ptr_a[c] / type_b[c];
								break;
							case mt_mat_helper::Math_Op_Code_Exp:
								ptr_res[c] = (T)exp(ptr_a[c]);
								break;
							case mt_mat_helper::Math_Op_Code_Ln:
								ptr_res[c] = (T)log(ptr_a[c]);
								break;
							case mt_mat_helper::Math_Op_Code_Log:
								ptr_res[c] = (T)(log10(ptr_a[c]) / log10(type_b[c]));
								break;
							case mt_mat_helper::Math_Op_Code_Pow:
								ptr_res[c] = (T)pow(ptr_a[c], type_b[c]);
								break;
							case mt_mat_helper::Math_Op_Code_Abs:
								ptr_res[c] = std::abs(ptr_a[c]);
								break;
							default:
								basiclog_unsupport2();
								break;
							}
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
					}
				}
			} else if (a.dim() == 2) {
				i32 rows = a.size()[0];
				i32 cols = a.size()[1];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();

				if (channels == 1) {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;

						for (i32 col = 0; col < cols; ++col) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Add:
								ptr_res[0] = ptr_a[0] + type_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[0] = ptr_a[0] - type_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Dot_Mul:
								ptr_res[0] = ptr_a[0] * type_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[0] = ptr_a[0] / type_b[0];
								break;
							case mt_mat_helper::Math_Op_Code_Exp:
								ptr_res[0] = (T)exp(ptr_a[0]);
								break;
							case mt_mat_helper::Math_Op_Code_Ln:
								ptr_res[0] = (T)log(ptr_a[0]);
								break;
							case mt_mat_helper::Math_Op_Code_Log:
								ptr_res[0] = (T)(log10(ptr_a[0]) / log10(type_b[0]));
								break;
							case mt_mat_helper::Math_Op_Code_Pow:
								ptr_res[0] = (T)pow(ptr_a[0], type_b[0]);
								break;
							case mt_mat_helper::Math_Op_Code_Abs:
								ptr_res[0] = std::abs(ptr_a[0]);
								break;
							default:
								basiclog_unsupport2();
								break;
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				} else {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;

						for (i32 col = 0; col < cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Add:
									ptr_res[c] = ptr_a[c] + type_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[c] = ptr_a[c] - type_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Dot_Mul:
									ptr_res[c] = ptr_a[c] * type_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[c] = ptr_a[c] / type_b[c];
									break;
								case mt_mat_helper::Math_Op_Code_Exp:
									ptr_res[c] = (T)exp(ptr_a[c]);
									break;
								case mt_mat_helper::Math_Op_Code_Ln:
									ptr_res[c] = (T)log(ptr_a[c]);
									break;
								case mt_mat_helper::Math_Op_Code_Log:
									ptr_res[c] = (T)(log10(ptr_a[c]) / log10(type_b[c]));
									break;
								case mt_mat_helper::Math_Op_Code_Pow:
									ptr_res[c] = (T)pow(ptr_a[c], type_b[c]);
									break;
								case mt_mat_helper::Math_Op_Code_Abs:
									ptr_res[c] = std::abs(ptr_a[c]);
									break;
								default:
									basiclog_unsupport2();
									break;
								}
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				}
			} else if (a.dim() == 3) {
				i32 planes = a.size()[0];
				i32 rows = a.size()[1];
				i32 cols = a.size()[2];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();

				if (channels == 1) {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;

							for (i32 col = 0; col < cols; ++col) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Add:
									ptr_res[0] = ptr_a[0] + type_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[0] = ptr_a[0] - type_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Dot_Mul:
									ptr_res[0] = ptr_a[0] * type_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[0] = ptr_a[0] / type_b[0];
									break;
								case mt_mat_helper::Math_Op_Code_Exp:
									ptr_res[0] = (T)exp(ptr_a[0]);
									break;
								case mt_mat_helper::Math_Op_Code_Ln:
									ptr_res[0] = (T)log(ptr_a[0]);
									break;
								case mt_mat_helper::Math_Op_Code_Log:
									ptr_res[0] = (T)(log10(ptr_a[0]) / log10(type_b[0]));
									break;
								case mt_mat_helper::Math_Op_Code_Pow:
									ptr_res[0] = (T)pow(ptr_a[0], type_b[0]);
									break;
								case mt_mat_helper::Math_Op_Code_Abs:
									ptr_res[0] = std::abs(ptr_a[0]);
									break;
								default:
									basiclog_unsupport2();
									break;
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}

				} else {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;

							for (i32 col = 0; col < cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									switch (code) {
									case mt_mat_helper::Math_Op_Code_Add:
										ptr_res[c] = ptr_a[c] + type_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Subtract:
										ptr_res[c] = ptr_a[c] - type_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Dot_Mul:
										ptr_res[c] = ptr_a[c] * type_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Div:
										ptr_res[c] = ptr_a[c] / type_b[c];
										break;
									case mt_mat_helper::Math_Op_Code_Exp:
										ptr_res[c] = (T)exp(ptr_a[c]);
										break;
									case mt_mat_helper::Math_Op_Code_Ln:
										ptr_res[c] = (T)log(ptr_a[c]);
										break;
									case mt_mat_helper::Math_Op_Code_Log:
										ptr_res[c] = (T)(log10(ptr_a[c]) / log10(type_b[c]));
										break;
									case mt_mat_helper::Math_Op_Code_Pow:
										ptr_res[c] = (T)pow(ptr_a[c], type_b[c]);
										break;
									case mt_mat_helper::Math_Op_Code_Abs:
										ptr_res[c] = std::abs(ptr_a[c]);
										break;
									default:
										basiclog_unsupport2();
										break;
									}
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				}
			} else {
				mt_array_element_iterator iter_res(res);
				mt_array_element_const_iterator iter_a(a);

				for (;;) {
					T* ptr_res = (T*)iter_res.data();

					if (ptr_res == NULL) {
						break;
					}

					const T* ptr_a = (const T*)iter_a.data();

					for (i32 c = 0; c < channels; ++c) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Add:
							ptr_res[c] = ptr_a[c] + type_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[c] = ptr_a[c] - type_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Dot_Mul:
							ptr_res[c] = ptr_a[c] * type_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[c] = ptr_a[c] / type_b[c];
							break;
						case mt_mat_helper::Math_Op_Code_Exp:
							ptr_res[c] = (T)exp(ptr_a[c]);
							break;
						case mt_mat_helper::Math_Op_Code_Ln:
							ptr_res[c] = (T)log(ptr_a[c]);
							break;
						case mt_mat_helper::Math_Op_Code_Log:
							ptr_res[c] = (T)(log10(ptr_a[c]) / log10(type_b[c]));
							break;
						case mt_mat_helper::Math_Op_Code_Pow:
							ptr_res[c] = (T)pow(ptr_a[c], type_b[c]);
							break;
						case mt_mat_helper::Math_Op_Code_Abs:
							ptr_res[c] = std::abs(ptr_a[c]);
							break;
						default:
							basiclog_unsupport2();
							break;
						}
					}
				}
			}

			basicmath_mat_release(type_b);
		}

		template<class T>
		static void mat_operation(mt_mat& res, const vector<f64>& b, const mt_mat& a, mt_mat_helper::Math_Op_Code code) {
			i32 element_number = a.element_number();
			i32 channels = a.channel();

			basicmath_mat_request_memory(T, type_b, (i32)b.size());

			for (i32 i = 0; i < (i32)b.size(); ++i) {
				type_b[i] = (T)b[i];
			}

			i32 last_res_step = res.step()[res.dim() - 1] > 0 ? channels : -channels;
			i32 last_a_step = a.step()[a.dim() - 1] > 0 ? channels : -channels;

			if (res.is_continuous() && a.is_continuous()) {
				T* ptr_res = (T*)res.data();
				const T* ptr_a = (const T*)a.data();

				if (channels == 1) {
					for (i32 i = 0; i < element_number; ++i) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[0] = type_b[0] - ptr_a[0];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[0] = type_b[0] / ptr_a[0];
							break;
						default:
							basiclog_unsupport2();
							break;
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
					}
				} else {
					for (i32 i = 0; i < element_number; ++i) {
						for (i32 c = 0; c < channels; ++c) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[c] = type_b[c] - ptr_a[c];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[c] = type_b[c] / ptr_a[c];
								break;
							default:
								basiclog_unsupport2();
								break;
							}
						}

						ptr_res += last_res_step;
						ptr_a += last_a_step;
					}
				}
			} else if (a.dim() == 2) {
				i32 rows = a.size()[0];
				i32 cols = a.size()[1];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();

				if (channels == 1) {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;

						for (i32 col = 0; col < cols; ++col) {
							switch (code) {
							case mt_mat_helper::Math_Op_Code_Subtract:
								ptr_res[0] = type_b[0] - ptr_a[0];
								break;
							case mt_mat_helper::Math_Op_Code_Div:
								ptr_res[0] = type_b[0] / ptr_a[0];
								break;
							default:
								basiclog_unsupport2();
								break;
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				} else {
					for (i32 row = 0; row < rows; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_a = (const T*)ptr_a_dim0;

						for (i32 col = 0; col < cols; ++col) {
							for (i32 c = 0; c < channels; ++c) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[c] = type_b[c] - ptr_a[c];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[c] = type_b[c] / ptr_a[c];
									break;
								default:
									basiclog_unsupport2();
									break;
								}
							}

							ptr_res += last_res_step;
							ptr_a += last_a_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				}
			} else if (a.dim() == 3) {
				i32 planes = a.size()[0];
				i32 rows = a.size()[1];
				i32 cols = a.size()[2];

				u8* ptr_res_dim0 = res.data();
				const u8* ptr_a_dim0 = a.data();

				if (channels == 1) {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;

							for (i32 col = 0; col < cols; ++col) {
								switch (code) {
								case mt_mat_helper::Math_Op_Code_Subtract:
									ptr_res[0] = type_b[0] - ptr_a[0];
									break;
								case mt_mat_helper::Math_Op_Code_Div:
									ptr_res[0] = type_b[0] / ptr_a[0];
									break;
								default:
									basiclog_unsupport2();
									break;
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}

				} else {
					for (i32 plane = 0; plane < planes; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_a_dim1 = ptr_a_dim0;

						for (i32 row = 0; row < rows; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_a = (const T*)ptr_a_dim1;

							for (i32 col = 0; col < cols; ++col) {
								for (i32 c = 0; c < channels; ++c) {
									switch (code) {
									case mt_mat_helper::Math_Op_Code_Subtract:
										ptr_res[c] = type_b[c] - ptr_a[c];
										break;
									case mt_mat_helper::Math_Op_Code_Div:
										ptr_res[c] = type_b[c] / ptr_a[c];
										break;
									default:
										basiclog_unsupport2();
										break;
									}
								}

								ptr_res += last_res_step;
								ptr_a += last_a_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_a_dim1 += a.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_a_dim0 += a.step()[0];
					}
				}
			} else {
				mt_array_element_iterator iter_res(res);
				mt_array_element_const_iterator iter_a(a);

				for (;;) {
					T* ptr_res = (T*)iter_res.data();

					if (ptr_res == NULL) {
						break;
					}

					const T* ptr_a = (const T*)iter_a.data();

					for (i32 c = 0; c < channels; ++c) {
						switch (code) {
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res[c] = type_b[c] - ptr_a[c];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res[c] = type_b[c] / ptr_a[c];
							break;
						default:
							basiclog_unsupport2();
							break;
						}
					}
				}
			}

			basicmath_mat_release(type_b);
		}

		template<class T>
		static void operation_row_bias(mt_mat& res, const mt_mat& src, const mt_mat& bias, mt_mat_helper::Math_Op_Code code) {
			u8* ptr_res_dim0 = res.data();
			const u8* ptr_src_dim0 = src.data();
			const u8* ptr_bias_dim0 = bias.data();

			i32 src_last_step = src.last_dim_element_step();
			i32 res_last_step = res.last_dim_element_step();
			i32 bias_last_step = bias.last_dim_element_step();

			for (i32 row = 0; row < src.size()[0]; ++row) {
				T* ptr_res_dim1 = (T*)ptr_res_dim0;
				const T* ptr_src_dim1 = (const T*)ptr_src_dim0;
				const T* ptr_bias_dim1 = (const T*)ptr_bias_dim0;

				for (i32 col = 0; col < src.size()[1]; ++col) {
					for (i32 c = 0; c < src.channel(); ++c) {

						switch (code) {
						case mt_mat_helper::Math_Op_Code_Add:
							ptr_res_dim1[c] = ptr_src_dim1[c] + ptr_bias_dim1[c];
							break;
						case mt_mat_helper::Math_Op_Code_Subtract:
							ptr_res_dim1[c] = ptr_src_dim1[c] - ptr_bias_dim1[c];
							break;
						case mt_mat_helper::Math_Op_Code_Dot_Mul:
							ptr_res_dim1[c] = ptr_src_dim1[c] * ptr_bias_dim1[c];
							break;
						case mt_mat_helper::Math_Op_Code_Div:
							ptr_res_dim1[c] = ptr_src_dim1[c] / ptr_bias_dim1[c];
							break;
						}
					}

					ptr_res_dim1 += res_last_step;
					ptr_src_dim1 += src_last_step;
					ptr_bias_dim1 += bias_last_step;
				}

				ptr_res_dim0 += res.step()[0];
				ptr_src_dim0 += src.step()[0];
			}
		}

		template<class T>
		static void operation_normal_bias(mt_mat& res, const mt_mat& src, const mt_mat& bias, mt_mat_helper::Math_Op_Code code) {
			mt_array_element_iterator iter_res(res);
			mt_array_element_const_iterator iter_src(src);

			basicmath_mat_request_memory(i32, bias_indexes, bias.dim());

			for (;;) {
				T* ptr_res = (T*)iter_res.data();

				if (ptr_res == NULL) {
					break;
				}

				const T* ptr_src = (const T*)iter_src.data();

				const vector<i32>& pos = iter_src.position();

				for (i32 i = 0; i < (i32)pos.size(); ++i) {
					bias_indexes[i] = pos[i] % bias.size()[i];
				}

				const T* ptr_bias = bias.ptr<const T>(bias.dim(), bias_indexes, 0);

				for (i32 c = 0; c < bias.channel(); ++c) {

					switch (code) {
					case mt_mat_helper::Math_Op_Code_Add:
						ptr_res[c] = ptr_src[c] + ptr_bias[c];
						break;
					case mt_mat_helper::Math_Op_Code_Subtract:
						ptr_res[c] = ptr_src[c] - ptr_bias[c];
						break;
					case mt_mat_helper::Math_Op_Code_Dot_Mul:
						ptr_res[c] = ptr_src[c] * ptr_bias[c];
						break;
					case mt_mat_helper::Math_Op_Code_Div:
						ptr_res[c] = ptr_src[c] / ptr_bias[c];
						break;
					}
				}
			}
		}

		static void operation_bias(mt_mat& res, const mt_mat& src, const mt_mat& bias, mt_mat_helper::Math_Op_Code code) {
			if (log_logger::get_logger()->is_enable_debug()) {
				for (i32 i = 0; i < bias.dim(); ++i) {
					basiclog_assert2(src.size()[i] % bias.size()[i] == 0);
				}
			}

			b8 is_unit_bias = sys_true;

			for (i32 i = 0; i < bias.dim(); ++i) {
				if (bias.size()[i] != 1) {
					is_unit_bias = sys_false;
				}
			}

			if (is_unit_bias) {
				vector<f64> vec_bias;
				bias.get(vec_bias, 0);

				private_math_operation::mat_operation<mt_mat, vector<f64>>(res, src, vec_bias, code);
			} else if (bias.dim() == 2 && bias.size()[0] == 1 && bias.size()[1] == src.size()[1]) {
				switch (src.depth()) {
				case mt_F32:
					private_math_operation::operation_row_bias<f32>(res, src, bias, code);
					break;
				case mt_F64:
					private_math_operation::operation_row_bias<f64>(res, src, bias, code);
					break;
				case mt_I32:
					private_math_operation::operation_row_bias<i32>(res, src, bias, code);
					break;
				case mt_U32:
					private_math_operation::operation_row_bias<u32>(res, src, bias, code);
					break;
				case mt_I16:
					private_math_operation::operation_row_bias<i16>(res, src, bias, code);
					break;
				case mt_U16:
					private_math_operation::operation_row_bias<u16>(res, src, bias, code);
					break;
				case mt_I8:
					private_math_operation::operation_row_bias<i8>(res, src, bias, code);
					break;
				case mt_U8:
					private_math_operation::operation_row_bias<u8>(res, src, bias, code);
					break;
				default:
					basiclog_unsupport2();
					break;
				}
			} else {
				switch (src.depth()) {
				case mt_F32:
					private_math_operation::operation_normal_bias<f32>(res, src, bias, code);
					break;
				case mt_F64:
					private_math_operation::operation_normal_bias<f64>(res, src, bias, code);
					break;
				case mt_I32:
					private_math_operation::operation_normal_bias<i32>(res, src, bias, code);
					break;
				case mt_U32:
					private_math_operation::operation_normal_bias<u32>(res, src, bias, code);
					break;
				case mt_I16:
					private_math_operation::operation_normal_bias<i16>(res, src, bias, code);
					break;
				case mt_U16:
					private_math_operation::operation_normal_bias<u16>(res, src, bias, code);
					break;
				case mt_I8:
					private_math_operation::operation_normal_bias<i8>(res, src, bias, code);
					break;
				case mt_U8:
					private_math_operation::operation_normal_bias<u8>(res, src, bias, code);
					break;
				default:
					basiclog_unsupport2();
					break;
				}
			}
		}

		template<class Type_A, class Type_B>
		static void mat_operation(mt_mat& res, const Type_A& a, const Type_B& b, mt_mat_helper::Math_Op_Code code) {
			switch (res.depth()) {
			case mt_U8:
				mat_operation<u8>(res, a, b, code);
				break;
			case mt_I8:
				mat_operation<i8>(res, a, b, code);
				break;
			case mt_U16:
				mat_operation<u16>(res, a, b, code);
				break;
			case mt_I16:
				mat_operation<i16>(res, a, b, code);
				break;
			case mt_U32:
				mat_operation<u32>(res, a, b, code);
				break;
			case mt_I32:
				mat_operation<i32>(res, a, b, code);
				break;
			case mt_U64:
				mat_operation<u64>(res, a, b, code);
				break;
			case mt_I64:
				mat_operation<i64>(res, a, b, code);
				break;
			case mt_F32:
				mat_operation<f32>(res, a, b, code);
				break;
			case mt_F64:
				mat_operation<f64>(res, a, b, code);
				break;
			default:
				basiclog_unsupport2();
				break;
			}
		}

		template<class T>
		static void element_compared_mask_with_zero(mt_mat& res, const mt_mat& src, mt_mat::Compare_Task task, f64 true_mask, f64 false_mask) {
			T t_zero = T(0);
			T t_true_mask = T(true_mask);
			T t_false_mask = T(false_mask);

			i32 res_last_step = res.last_dim_element_step();
			i32 src_last_step = src.last_dim_element_step();

			u8* ptr_res_dim0 = res.data();
			const u8* ptr_src_dim0 = src.data();

			if (res.dim() == 2) {
				if (src.channel() == 1) {
					for (i32 row = 0; row < src.size()[0]; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;

						for (i32 col = 0; col < src.size()[1]; ++col) {
							switch (task) {
							case mt_mat::Compare_Task_Bigger:
								ptr_res[0] = ptr_src[0] > t_zero ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Smaller:
								ptr_res[0] = ptr_src[0] < t_zero ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Same:
								ptr_res[0] = ptr_src[0] == t_zero ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Non_Bigger:
								ptr_res[0] = ptr_src[0] <= t_zero ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Non_Smaller:
								ptr_res[0] = ptr_src[0] >= t_zero ? t_true_mask : t_false_mask;
								break;
							}

							ptr_res += res_last_step;
							ptr_src += src_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				} else {
					for (i32 row = 0; row < src.size()[0]; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;

						for (i32 col = 0; col < src.size()[1]; ++col) {
							for (i32 c = 0; c < src.channel(); ++c) {
								switch (task) {
								case mt_mat::Compare_Task_Bigger:
									ptr_res[c] = ptr_src[c] > t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Smaller:
									ptr_res[c] = ptr_src[c] < t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Same:
									ptr_res[c] = ptr_src[c] == t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Bigger:
									ptr_res[c] = ptr_src[c] <= t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Smaller:
									ptr_res[c] = ptr_src[c] >= t_zero ? t_true_mask : t_false_mask;
									break;
								}
							}

							ptr_res += res_last_step;
							ptr_src += src_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}
			} else if (res.dim() == 3) {
				if (src.channel() == 1) {
					for (i32 plane = 0; plane < src.size()[0]; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < src.size()[1]; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;

							for (i32 col = 0; col < src.size()[2]; ++col) {

								switch (task) {
								case mt_mat::Compare_Task_Bigger:
									ptr_res[0] = ptr_src[0] > t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Smaller:
									ptr_res[0] = ptr_src[0] < t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Same:
									ptr_res[0] = ptr_src[0] == t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Bigger:
									ptr_res[0] = ptr_src[0] <= t_zero ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Smaller:
									ptr_res[0] = ptr_src[0] >= t_zero ? t_true_mask : t_false_mask;
									break;
								}

								ptr_res += res_last_step;
								ptr_src += src_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}

				} else {
					for (i32 plane = 0; plane < src.size()[0]; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;

						for (i32 row = 0; row < src.size()[1]; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;

							for (i32 col = 0; col < src.size()[2]; ++col) {

								for (i32 c = 0; c < src.channel(); ++c) {
									switch (task) {
									case mt_mat::Compare_Task_Bigger:
										ptr_res[c] = ptr_src[c] > t_zero ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Smaller:
										ptr_res[c] = ptr_src[c] < t_zero ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Same:
										ptr_res[c] = ptr_src[c] == t_zero ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Non_Bigger:
										ptr_res[c] = ptr_src[c] <= t_zero ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Non_Smaller:
										ptr_res[c] = ptr_src[c] >= t_zero ? t_true_mask : t_false_mask;
										break;
									}
								}

								ptr_res += res_last_step;
								ptr_src += src_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
					}
				}
			} else {
				mt_array_element_iterator iter_res(res);
				mt_array_element_const_iterator iter_src(src);

				for (;;) {
					T* ptr_res = (T*)iter_res.data();

					if (ptr_res == NULL) {
						break;
					}

					const T* ptr_src = (const T*)iter_src.data();

					for (i32 c = 0; c < src.channel(); ++c) {
						switch (task) {
						case mt_mat::Compare_Task_Bigger:
							ptr_res[c] = ptr_src[c] > t_zero ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Smaller:
							ptr_res[c] = ptr_src[c] < t_zero ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Same:
							ptr_res[c] = ptr_src[c] == t_zero ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Non_Bigger:
							ptr_res[c] = ptr_src[c] <= t_zero ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Non_Smaller:
							ptr_res[c] = ptr_src[c] >= t_zero ? t_true_mask : t_false_mask;
							break;
						}
					}
				}
			}
		}

		template<class T>
		static void element_compared_mask(mt_mat& res, const mt_mat& src, const mt_mat& dst, mt_mat::Compare_Task task, f64 true_mask, f64 false_mask) {
			if (dst.is_empty()) {
				element_compared_mask_with_zero<T>(res, src, task, true_mask, false_mask);

				return;
			}

			T t_true_mask = T(true_mask);
			T t_false_mask = T(false_mask);

			i32 res_last_step = res.last_dim_element_step();
			i32 src_last_step = src.last_dim_element_step();
			i32 dst_last_step = dst.last_dim_element_step();
			
			u8* ptr_res_dim0 = res.data();
			const u8* ptr_src_dim0 = src.data();
			const u8* ptr_dst_dim0 = dst.data();

			if (res.dim() == 2) {
				if (src.channel() == 1) {
					for (i32 row = 0; row < src.size()[0]; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;
						const T* ptr_dst = (const T*)ptr_dst_dim0;

						for (i32 col = 0; col < src.size()[1]; ++col) {
							switch (task) {
							case mt_mat::Compare_Task_Bigger:
								ptr_res[0] = ptr_src[0] > ptr_dst[0] ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Smaller:
								ptr_res[0] = ptr_src[0] < ptr_dst[0] ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Same:
								ptr_res[0] = ptr_src[0] == ptr_dst[0] ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Non_Bigger:
								ptr_res[0] = ptr_src[0] <= ptr_dst[0] ? t_true_mask : t_false_mask;
								break;
							case mt_mat::Compare_Task_Non_Smaller:
								ptr_res[0] = ptr_src[0] >= ptr_dst[0] ? t_true_mask : t_false_mask;
								break;
							}

							ptr_res += res_last_step;
							ptr_src += src_last_step;
							ptr_dst += dst_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
						ptr_dst_dim0 += dst.step()[0];
					}
				} else {
					for (i32 row = 0; row < src.size()[0]; ++row) {
						T* ptr_res = (T*)ptr_res_dim0;
						const T* ptr_src = (const T*)ptr_src_dim0;
						const T* ptr_dst = (const T*)ptr_dst_dim0;

						for (i32 col = 0; col < src.size()[1]; ++col) {
							for (i32 c = 0; c < src.channel(); ++c) {
								switch (task) {
								case mt_mat::Compare_Task_Bigger:
									ptr_res[c] = ptr_src[c] > ptr_dst[c] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Smaller:
									ptr_res[c] = ptr_src[c] < ptr_dst[c] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Same:
									ptr_res[c] = ptr_src[c] == ptr_dst[c] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Bigger:
									ptr_res[c] = ptr_src[c] <= ptr_dst[c] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Smaller:
									ptr_res[c] = ptr_src[c] >= ptr_dst[c] ? t_true_mask : t_false_mask;
									break;
								}
							}

							ptr_res += res_last_step;
							ptr_src += src_last_step;
							ptr_dst += dst_last_step;
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
						ptr_dst_dim0 += dst.step()[0];
					}
				}
			} else if (res.dim() == 3) {
				if (src.channel() == 1) {
					for (i32 plane = 0; plane < src.size()[0]; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;
						const u8* ptr_dst_dim1 = ptr_dst_dim0;

						for (i32 row = 0; row < src.size()[1]; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;
							const T* ptr_dst = (const T*)ptr_dst_dim1;

							for (i32 col = 0; col < src.size()[2]; ++col) {

								switch (task) {
								case mt_mat::Compare_Task_Bigger:
									ptr_res[0] = ptr_src[0] > ptr_dst[0] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Smaller:
									ptr_res[0] = ptr_src[0] < ptr_dst[0] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Same:
									ptr_res[0] = ptr_src[0] == ptr_dst[0] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Bigger:
									ptr_res[0] = ptr_src[0] <= ptr_dst[0] ? t_true_mask : t_false_mask;
									break;
								case mt_mat::Compare_Task_Non_Smaller:
									ptr_res[0] = ptr_src[0] >= ptr_dst[0] ? t_true_mask : t_false_mask;
									break;
								}

								ptr_res += res_last_step;
								ptr_src += src_last_step;
								ptr_dst += dst_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
							ptr_dst_dim1 += dst.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
						ptr_dst_dim0 += dst.step()[0];
					}
					
				} else {
					for (i32 plane = 0; plane < src.size()[0]; ++plane) {
						u8* ptr_res_dim1 = ptr_res_dim0;
						const u8* ptr_src_dim1 = ptr_src_dim0;
						const u8* ptr_dst_dim1 = ptr_dst_dim0;

						for (i32 row = 0; row < src.size()[1]; ++row) {
							T* ptr_res = (T*)ptr_res_dim1;
							const T* ptr_src = (const T*)ptr_src_dim1;
							const T* ptr_dst = (const T*)ptr_dst_dim1;

							for (i32 col = 0; col < src.size()[2]; ++col) {

								for (i32 c = 0; c < src.channel(); ++c) {
									switch (task) {
									case mt_mat::Compare_Task_Bigger:
										ptr_res[c] = ptr_src[c] > ptr_dst[c] ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Smaller:
										ptr_res[c] = ptr_src[c] < ptr_dst[c] ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Same:
										ptr_res[c] = ptr_src[c] == ptr_dst[c] ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Non_Bigger:
										ptr_res[c] = ptr_src[c] <= ptr_dst[c] ? t_true_mask : t_false_mask;
										break;
									case mt_mat::Compare_Task_Non_Smaller:
										ptr_res[c] = ptr_src[c] >= ptr_dst[c] ? t_true_mask : t_false_mask;
										break;
									}
								}

								ptr_res += res_last_step;
								ptr_src += src_last_step;
								ptr_dst += dst_last_step;
							}

							ptr_res_dim1 += res.step()[1];
							ptr_src_dim1 += src.step()[1];
							ptr_dst_dim1 += dst.step()[1];
						}

						ptr_res_dim0 += res.step()[0];
						ptr_src_dim0 += src.step()[0];
						ptr_dst_dim0 += dst.step()[0];
					}
				}
			} else {
				mt_array_element_iterator iter_res(res);
				mt_array_element_const_iterator iter_src(src);
				mt_array_element_const_iterator iter_dst(dst);

				for (;;) {
					T* ptr_res = (T*)iter_res.data();

					if (ptr_res == NULL) {
						break;
					}

					const T* ptr_src = (const T*)iter_src.data();
					const T* ptr_dst = (const T*)iter_dst.data();

					for (i32 c = 0; c < src.channel(); ++c) {
						switch (task) {
						case mt_mat::Compare_Task_Bigger:
							ptr_res[c] = ptr_src[c] > ptr_dst[c] ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Smaller:
							ptr_res[c] = ptr_src[c] < ptr_dst[c] ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Same:
							ptr_res[c] = ptr_src[c] == ptr_dst[c] ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Non_Bigger:
							ptr_res[c] = ptr_src[c] <= ptr_dst[c] ? t_true_mask : t_false_mask;
							break;
						case mt_mat::Compare_Task_Non_Smaller:
							ptr_res[c] = ptr_src[c] >= ptr_dst[c] ? t_true_mask : t_false_mask;
							break;
						}
					}
				}
			}
		}
	};

	mt_mat operator+(f64 value, const mt_mat& mat) {
		return mat.operator+(value);
	}

	mt_mat operator-(f64 value, const mt_mat& mat) {
		vector<double> vec_other;
		vec_other.resize(mat.channel(), value);
		mt_mat res(mat, mt_mat::Construct_Type_Create_As_Size);

		private_math_operation::mat_operation<vector<f64>, mt_mat>(res, vec_other, mat, mt_mat_helper::Math_Op_Code_Subtract);

		if (mat.auto_derivative() != NULL && mat.auto_derivative()->math_operation_recorded()) {
			res.attach(mat.auto_derivative());

			mat.auto_derivative()->subtract(res, vec_other, mat);
		}

		return res;
	}

	mt_mat operator*(f64 value, const mt_mat& mat) {
		return mat.operator*(value);
	}

	mt_mat operator/(f64 value, const mt_mat& mat) {
		vector<double> vec_other;
		vec_other.resize(mat.channel(), value);
		mt_mat res(mat, mt_mat::Construct_Type_Create_As_Size);

		private_math_operation::mat_operation<vector<f64>, mt_mat>(res, vec_other, mat, mt_mat_helper::Math_Op_Code_Div);

		if (mat.auto_derivative() != NULL && mat.auto_derivative()->math_operation_recorded()) {
			res.attach(mat.auto_derivative());

			mat.auto_derivative()->div(res, vec_other, mat);
			basiclog_unsupport2();
		}

		return res;
	}

	mt_mat operator-(const mt_mat& mat) {
		return 0 - mat;
	}
}

mt_mat& mt_mat::operator+=(double other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	vec_other.resize(channel(), other);

	return (*this)+=(vec_other);
}

mt_mat& mt_mat::operator+=(const mt_scalar& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, other);

	return (*this)+=(vec_other);
}

mt_mat& mt_mat::operator+=(const vector<double>& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, other, mt_mat_helper::Math_Op_Code_Add);

	return *this;
}

mt_mat& mt_mat::operator+=(const mt_mat& other) {
	basiclog_assert2(is_same_memory_size(other));

	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, mt_mat>(*this, *this, other, mt_mat_helper::Math_Op_Code_Add);

	return *this;
}

mt_mat& mt_mat::operator-=(double other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	vec_other.resize(channel(), other);

	return (*this)-=(vec_other);
}

mt_mat& mt_mat::operator-=(const mt_scalar& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, other);

	return (*this)-=(vec_other);
}

mt_mat& mt_mat::operator-=(const vector<double>& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, other, mt_mat_helper::Math_Op_Code_Subtract);

	return *this;
}

mt_mat& mt_mat::operator-=(const mt_mat& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, mt_mat>(*this, *this, other, mt_mat_helper::Math_Op_Code_Subtract);

	return *this;
}

mt_mat& mt_mat::operator*=(double other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	vec_other.resize(channel(), other);

	return (*this)*=(vec_other);
}

mt_mat& mt_mat::operator*=(const mt_scalar& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, other);

	return (*this)*=(vec_other);
}

mt_mat& mt_mat::operator*=(const vector<double>& value) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, value, mt_mat_helper::Math_Op_Code_Dot_Mul);

	return *this;
}

mt_mat& mt_mat::operator*=(const mt_mat& value) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, mt_mat>(*this, *this, value, mt_mat_helper::Math_Op_Code_Dot_Mul);

	return *this;
}

mt_mat& mt_mat::operator/=(double other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	vec_other.resize(channel(), other);

	return (*this)/=(vec_other);
}

mt_mat& mt_mat::operator/=(const mt_scalar& other) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, other);

	return (*this)/=(vec_other);
}

mt_mat& mt_mat::operator/=(const vector<double>& value) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, value, mt_mat_helper::Math_Op_Code_Div);

	return *this;
}

mt_mat& mt_mat::operator/=(const mt_mat& value) {
	basiclog_assert2(m_auto_derivative == NULL || !m_auto_derivative->math_operation_recorded());

	private_math_operation::mat_operation<mt_mat, mt_mat>(*this, *this, value, mt_mat_helper::Math_Op_Code_Div);

	return *this;
}

mt_mat mt_mat::operator+(double value) const {
	vector<double> vec_other;
	vec_other.resize(channel(), value);

	return (*this) + vec_other;
}

mt_mat mt_mat::operator+(const mt_scalar& value) const {
	basiclog_assert2(channel() <= sizeof(value.value) / sizeof(f64));
	
	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, value);

	return (*this) + vec_other;
}

mt_mat mt_mat::operator+(const vector<double>& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, value, mt_mat_helper::Math_Op_Code_Add);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->add(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::operator+(const mt_mat& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, mt_mat>(res, *this, value, mt_mat_helper::Math_Op_Code_Add);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		value.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->add(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::operator-(double value) const {
	vector<double> vec_other;
	vec_other.resize(channel(), value);

	return (*this) - vec_other;
}

mt_mat mt_mat::operator-(const mt_scalar& value) const {
	basiclog_assert2(channel() <= sizeof(value.value) / sizeof(f64));

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, value);

	return (*this) - vec_other;
}

mt_mat mt_mat::operator-(const vector<double>& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, value, mt_mat_helper::Math_Op_Code_Subtract);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->add(res, *this, value);
	}

	return res;
}





mt_mat mt_mat::operator-(const mt_mat& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);
	
	private_math_operation::mat_operation<mt_mat, mt_mat>(res, *this, value, mt_mat_helper::Math_Op_Code_Subtract);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		value.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->subtract(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::operator*(double value) const {
	vector<double> vec_other;
	vec_other.resize(channel(), value);

	return (*this) * vec_other;
}

mt_mat mt_mat::operator*(const mt_scalar& value) const {
	basiclog_assert2(channel() <= sizeof(value.value) / sizeof(f64));

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, value);

	return (*this) * vec_other;
}

mt_mat mt_mat::operator*(const vector<double>& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, value, mt_mat_helper::Math_Op_Code_Dot_Mul);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->dot(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::operator*(const mt_mat& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, mt_mat>(res, *this, value, mt_mat_helper::Math_Op_Code_Dot_Mul);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		value.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->dot(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::operator/(double value) const {
	vector<double> vec_other;
	vec_other.resize(channel(), value);

	return (*this) / vec_other;
}

mt_mat mt_mat::operator/(const mt_scalar& value) const {
	basiclog_assert2(channel() <= sizeof(value.value) / sizeof(f64));

	vector<double> vec_other;
	mt_helper::vec_from_scalar(vec_other, value);

	return (*this) / vec_other;
}

mt_mat mt_mat::operator/(const vector<double>& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, value, mt_mat_helper::Math_Op_Code_Div);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->div(res, *this, value);
		basiclog_unsupport2();
	}

	return res;
}

mt_mat mt_mat::operator/(const mt_mat& value) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, mt_mat>(res, *this, value, mt_mat_helper::Math_Op_Code_Div);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		value.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->div(res, *this, value);
	}

	return res;
}

mt_mat mt_mat::add_bias(const mt_mat& bias) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::operation_bias(res, *this, bias, mt_mat_helper::Math_Op_Code_Add);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		bias.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->add_bias(res, *this, bias);
	}

	return res;
}

mt_mat mt_mat::substract_bias(const mt_mat& bias) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::operation_bias(res, *this, bias, mt_mat_helper::Math_Op_Code_Subtract);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		bias.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->substract_bias(res, *this, bias);
	}

	return res;
}

mt_mat mt_mat::mul_bias(const mt_mat& bias) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::operation_bias(res, *this, bias, mt_mat_helper::Math_Op_Code_Dot_Mul);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		bias.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->mul_bias(res, *this, bias);
	}

	return res;
}

mt_mat mt_mat::div_bias(const mt_mat& bias) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::operation_bias(res, *this, bias, mt_mat_helper::Math_Op_Code_Div);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		bias.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->div_bias(res, *this, bias);
	}

	return res;
}

mt_mat mt_mat::expand(const vector<i32>& tl_side_sizes, const vector<i32>& br_side_sizes, const mt_scalar& filled_channel_value /* = mt_scalar(0) */) const {
	if (!tl_side_sizes.empty() && !br_side_sizes.empty()) {
		basiclog_assert2(tl_side_sizes.size() == br_side_sizes.size());
	}
	
	i32 size = tl_side_sizes.empty() ? (i32)br_side_sizes.size() : (i32)tl_side_sizes.size();

	return expand(size, tl_side_sizes.empty() ? NULL : &tl_side_sizes[0], br_side_sizes.empty() ? NULL : &br_side_sizes[0], filled_channel_value);
}

mt_mat mt_mat::expand(i32 size, const i32* side_sizes_1, const i32* side_sizes_2, const mt_scalar& filled_channel_value) const {
	vector<f64> vec_filled_channel_values;
	mt_helper::vec_from_scalar(vec_filled_channel_values, filled_channel_value);

	return expand(size, side_sizes_1, side_sizes_2, vec_filled_channel_values);
}

mt_mat mt_mat::expand(i32 size, const i32* side_sizes_1, const i32* side_sizes_2, const vector<f64>& filled_channel_value) const {
	if (size == 0 || (side_sizes_1 == NULL && side_sizes_2 == NULL)) {
		return *this;
	}

	basiclog_assert2(size == dim());

	basicmath_mat_request_memory(i32, new_sizes, dim());
	basicmath_mat_request_memory(mt_range, ranges, dim());

	for (i32 i = 0; i < dim(); ++i) {
		new_sizes[i] = this->size()[i];

		ranges[i].m_start = 0;

		if (NULL != side_sizes_1) {
			new_sizes[i] += side_sizes_1[i];
			ranges[i].m_start += side_sizes_1[i];
		}

		if (NULL != side_sizes_2) {
			new_sizes[i] += side_sizes_2[i];
		}

		ranges[i].m_end = ranges[i].m_start + this->size()[i];
	}

	mt_mat res(dim(), new_sizes, depth_channel());
	res.set(filled_channel_value);
	res.sub(dim(), ranges).set(*this);

	basicmath_mat_release(new_sizes);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->expand(res, *this, size, ranges);
	}

	return res;
}

mt_mat mt_mat::sub_stride(i32 size, const i32* strides) const {
	basiclog_assert2(size == dim());

	basicmath_mat_request_memory(i32, res_sizes, dim());

	for (int i = 0; i < dim(); ++i) {
		res_sizes[i] = mt_mat_helper::get_pooling_result_size(this->size()[i], strides[i], strides[i]);
	}

	mt_mat res = *this;

	for (i32 i  = 0; i < dim(); ++i) {
		res.size()[i] = res_sizes[i];
		res.step()[i] *= strides[i];
	}

	basicmath_mat_release(res_sizes);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->sub_stride(res, *this, size, strides);
	}

	return res;
}

mt_mat mt_mat::activate(mt_Activate_Type type, const vector<f64>& activate_params) const {
	return activate(type, (i32)activate_params.size(), &activate_params[0]);
}

mt_mat mt_mat::activate(mt_Activate_Type type, i32 activate_param_size, const f64* activate_params) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	basiclog_assert2(depth() == mt_F32 || depth() == mt_F64);

	if (depth() == mt_F32) {
		private_math_operation::activate<f32>(res, *this, type, activate_param_size, activate_params);
	} else {
		private_math_operation::activate<f64>(res, *this, type, activate_param_size, activate_params);
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		vector<f64> vec_activate_params;
		mt_helper::vec_from_array(vec_activate_params, activate_param_size, activate_params);

		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->activate(res, *this, type, vec_activate_params);
	}

	return res;
}

mt_mat& mt_mat::self_activate(mt_Activate_Type type, const vector<f64>& activate_params) {
	return self_activate(type, (i32)activate_params.size(), &activate_params[0]);
}

mt_mat& mt_mat::self_activate(mt_Activate_Type type, i32 activate_param_size, const f64* activate_params) {
	on_vaule_changed();
	basiclog_assert2(depth() == mt_F32 || depth() == mt_F64);

	if (depth() == mt_F32) {
		private_math_operation::activate<f32>(*this, *this, type, activate_param_size, activate_params);
	} else {
		private_math_operation::activate<f64>(*this, *this, type, activate_param_size, activate_params);
	}

	return *this;
}

mt_mat mt_mat::loss(const mt_mat& matching_mat, mt_Loss_Type type) const {
	basiclog_assert2(depth() == mt_F32 || depth() == mt_F64);
	basiclog_assert2(dim() == matching_mat.dim());
	basiclog_assert2(is_same_size(matching_mat));

	mt_mat res;

	if (depth() == mt_F32) {
		res = private_math_operation::loss<f32>(*this, matching_mat, type);
	} else {
		res = private_math_operation::loss<f64>(*this, matching_mat, type);
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		matching_mat.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->loss(res, *this, matching_mat, type);
	}

	return res;
}

mt_mat& mt_mat::self_pow(f64 number) {
	on_vaule_changed();

	vector<f64> params;
	params.resize(channel(), number);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, params, mt_mat_helper::Math_Op_Code_Pow);

	return *this;
}

mt_mat& mt_mat::self_exp() {
	on_vaule_changed();
	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Exp);

	return *this;
}

mt_mat& mt_mat::self_log(double base) {
	on_vaule_changed();

	if (mt_helper::compare_double(base, mt_E) == 0) {
		private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Ln);
	} else {
		vector<f64> params;
		params.resize(channel(), base);

		private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, params, mt_mat_helper::Math_Op_Code_Log);
	}

	return *this;
}

mt_mat& mt_mat::self_abs() {
	on_vaule_changed();

	private_math_operation::mat_operation<mt_mat, vector<f64>>(*this, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Abs);

	return *this;
}

mt_mat mt_mat::pow(f64 number) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);
	
	vector<f64> params;
	params.resize(channel(), number);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, params, mt_mat_helper::Math_Op_Code_Pow);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->pow(res, *this, number);
	}

	return res;
}

mt_mat mt_mat::exp() const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Exp);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->exp(res, *this);
	}

	return res;
}

mt_mat mt_mat::log(double base /* = mt_E */) const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);

	if (mt_helper::compare_double(base, mt_E) == 0) {
		private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Ln);
	} else {
		vector<f64> params;
		params.resize(channel(), base);

		private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, params, mt_mat_helper::Math_Op_Code_Log);
	}

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->log(res, *this, base);
	}

	return res;
}

mt_mat mt_mat::abs() const {
	mt_mat res(*this, mt_mat::Construct_Type_Create_As_Size);


	private_math_operation::mat_operation<mt_mat, vector<f64>>(res, *this, vector<f64>(), mt_mat_helper::Math_Op_Code_Abs);

	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		m_auto_derivative->abs(res, *this);
	}

	return res;
}

void mt_mat::symmetry_eigen(mt_mat& eigen_value, mt_mat& eigen_vector) const {
	basiclog_assert2(dim() == 2);
	basiclog_assert2(depth_channel() == mt_F32C1 || depth_channel() == mt_F64C1);

	if (depth() == mt_F32) {
		private_math_operation::symmetry_eigen<f32>(eigen_value, eigen_vector, clone());
	} else {
		private_math_operation::symmetry_eigen<f64>(eigen_value, eigen_vector, clone());
	}
}

mt_mat mt_mat::reduce(mt_mat::Reduce_Type type, i32 reduce_dim) const {
	if (depth() == mt_F32) {
		return private_math_operation::reduce<f32>(*this, type, reduce_dim);
	} else {
		return private_math_operation::reduce<f64>(*this, type, reduce_dim);
	}
}

mt_mat mt_mat::element_compared_mask(const mt_mat& other, mt_mat::Compare_Task task, f64 true_mask, f64 false_mask) const {
	basiclog_assert2(is_same_size(other));
	
	if (other.depth() != depth()) {
		mt_mat temp = other.convert(depth());

		return element_compared_mask(temp, task, true_mask, false_mask);
	}

	mt_mat res(other, mt_mat::Construct_Type_Create_As_Size);

	switch (depth()) {
	case mt_U8:
		private_math_operation::element_compared_mask<u8>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_I8:
		private_math_operation::element_compared_mask<i8>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_U16:
		private_math_operation::element_compared_mask<u16>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_I16:
		private_math_operation::element_compared_mask<i16>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_U32:
		private_math_operation::element_compared_mask<u32>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_I32:
		private_math_operation::element_compared_mask<i32>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_U64:
		private_math_operation::element_compared_mask<u64>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_I64:
		private_math_operation::element_compared_mask<i64>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_F32:
		private_math_operation::element_compared_mask<f32>(res, *this, other, task, true_mask, false_mask);
		break;
	case mt_F64:
		private_math_operation::element_compared_mask<f64>(res, *this, other, task, true_mask, false_mask);
		break;
	}

	return res;
}