#include "stdafx.h"


namespace basicmath {

	class private_convolution {
	public:

		enum Conv_Model {
			Conv_Model_Direct,
			Conv_Model_FFT,
		};

		static Conv_Model get_suggest_model(const mt_mat& src, const mt_mat& kernel) {
			return Conv_Model_Direct;
		}

		static void conv(Conv_Model mode, i32 iter_output, vector<mt_mat>& res_channels, const vector<mt_mat>& src_channels, const vector<mt_mat>& kernel_channels, mt_Conv_Kernel_Align_Type align_type, const i32* src_steps, const i32* kernel_steps, const i32* res_steps, const i32* strides, const i32* starts) {
			void* task = NULL;
			mt_mat res_channel = res_channels[iter_output];
			private_convolution::new_task(task, mode, src_channels.front(), kernel_channels.front(), res_channels.front(), strides, starts);

			i32 src_channel_num = (i32)src_channels.size();
			i32 kernel_channel_num = (i32)kernel_channels.size();
			i32 res_channel_num = (i32)res_channels.size();

			if (align_type == mt_Conv_Kernel_Align_Type_Row || align_type == mt_Conv_Kernel_Align_Type_Col) {
				mt_mat temp = res_channel;

				if (src_channel_num > 1) {
					temp = mt_mat(res_channel, mt_mat::Construct_Type_Create_As_Size);

					i32* temp_steps = new i32[res_channels.front().dim()];
					for (int i = 0; i < res_channels.front().dim(); ++i) {
						temp_steps[i] = temp.step()[i] / temp.element_channel_size();
					}

					res_steps = temp_steps;
				}

				for (i32 iter_input = 0; iter_input < src_channel_num; ++iter_input) {
					mt_mat src_channel = src_channels[iter_input];
					mt_mat kernel_channel;

					if (align_type == mt_Conv_Kernel_Align_Type_Row) {
						kernel_channel = kernel_channels[iter_input * res_channel_num + iter_output];
					} else {
						kernel_channel = kernel_channels[iter_output * src_channel_num + iter_input];
					}

					if (mt_F32 == res_channel.depth()) {
						private_convolution::convf(task, (const float*)src_channel.memory_data(), &src_steps[0], (const float*)kernel_channel.memory_data(), &kernel_steps[0], (float*)temp.memory_data(), &res_steps[0]);
					} else if (mt_F64 == res_channel.depth()) {
						private_convolution::convd(task, (const double*)src_channel.memory_data(), &src_steps[0], (const double*)kernel_channel.memory_data(), &kernel_steps[0], (double*)temp.memory_data(), &res_steps[0]);
					} else {
						basiclog_assert2(false);
					}

					if (src_channel_num > 1) {
						if (iter_input == 0) {
							res_channel.set(temp);
						} else {
							res_channel += temp;
						}
					}
				}

				if (src_channel_num > 1) {
					delete[] res_steps;
				}
			} else {
				mt_mat src_channel;
				mt_mat kernel_channel;

				if (align_type == mt_Conv_Kernel_Align_Type_Cross_Row) {
					src_channel = src_channels[iter_output / kernel_channel_num];
					kernel_channel = kernel_channels[iter_output % kernel_channel_num];
				} else {
					src_channel = src_channels[iter_output % src_channel_num];
					kernel_channel = kernel_channels[iter_output / src_channel_num];
				}

				if (mt_F32 == res_channel.depth()) {
					private_convolution::convf(task, (const float*)src_channel.memory_data(), &src_steps[0], (const float*)kernel_channel.memory_data(), &kernel_steps[0], (float*)res_channels[iter_output].memory_data(), &res_steps[0]);
				} else if (mt_F64 == res_channel.depth()) {
					private_convolution::convd(task, (const double*)src_channel.memory_data(), &src_steps[0], (const double*)kernel_channel.memory_data(), &kernel_steps[0], (double*)res_channels[iter_output].memory_data(), &res_steps[0]);
				} else {
					basiclog_assert2(false);
				}
			}

			private_convolution::delete_task(task);
		}

	private:
		static void new_task(void*& task, Conv_Model mode, const mt_mat& src, const mt_mat& kernel, const mt_mat& res, const i32* strides, const i32* starts) {

#if defined basicsys_enable_mkl
			if (res.depth() == mt_F32) {
				basiclog_assert2(VSL_STATUS_OK == vslsConvNewTask(&task, mode == Conv_Model_Direct ? VSL_CONV_MODE_DIRECT : VSL_CONV_MODE_FFT, res.dim(), src.size(), kernel.size(), res.size()));
			} else if (res.depth() == mt_F64) {
				basiclog_assert2(VSL_STATUS_OK == vsldConvNewTask(&task, mode == Conv_Model_Direct ? VSL_CONV_MODE_DIRECT : VSL_CONV_MODE_FFT, res.dim(), src.size(), kernel.size(), res.size()));
			} else {
				basiclog_unsupport2();
			}

			basiclog_assert2(VSL_STATUS_OK == vslConvSetDecimation(task, &strides[0]));
			basiclog_assert2(VSL_STATUS_OK == vslConvSetStart(task, &starts[0]));
#else
			basiclog_unsupport2();
#endif
		}

		static void delete_task(void*& task) {
#if defined basicsys_enable_mkl
			vslConvDeleteTask(&task);
#else
			basiclog_unsupport2();
#endif
		}

		static void convd(void* task, const f64* src, const i32* src_steps, const f64* kernel, const i32* kernel_steps, f64* res , const i32* res_steps) {


#if defined basicsys_enable_mkl
			basiclog_assert2(VSL_STATUS_OK == vsldConvExec(task, src, src_steps, kernel, kernel_steps, res, res_steps));
#else
			basiclog_unsupport2();
#endif
		}

		static void convf(void* task, const f32* src, const i32* src_steps, const f32* kernel, const i32* kernel_steps, f32* res , const i32* res_steps) {


#if defined basicsys_enable_mkl
			basiclog_assert2(VSL_STATUS_OK == vslsConvExec(task, src, src_steps, kernel, kernel_steps, res, res_steps));
#else
			basiclog_unsupport2();
#endif
		}

	};

}

void mt_mat::conv(mt_mat& res, const mt_mat& kernel, mt_Conv_Boundary_Type boundary_type /* = mt_Conv_Boundary_Type_Valid */, mt_Conv_Kernel_Align_Type align_type /* = mt_Conv_Kernel_Align_Type_Row */, const int* conv_strides /* = NULL */) const {
	if (&res == this) {
		mt_mat temp;
		conv(temp, kernel, boundary_type, align_type, conv_strides);

		res = temp;
		return;
	}
	
	basiclog_assert2(depth() == mt_F32 || depth() == mt_F64);

	basiclog_assert2(kernel.dim() == dim());

	if (mt_Conv_Boundary_Type_Valid == boundary_type) {
		int min_dims = min(kernel.dim(), dim());

		for (int i = 0; i < min_dims; ++i) {
			basiclog_assert2(size()[i] >= kernel.size()[i]);
		}
	}

	private_convolution::Conv_Model mode = private_convolution::get_suggest_model(*this, kernel);

	basicmath_mat_request_memory(i32, full_sizes, dim());
	basicmath_mat_request_memory(i32, sizes, dim());
	basicmath_mat_request_memory(i32, starts, dim());
	basicmath_mat_request_memory(i32, temp_strides, dim());

	for (int i = 0; i < dim(); ++i) {
		full_sizes[i] = size()[i] + kernel.size()[i] - 1;
		sizes[i] = full_sizes[i];
		starts[i] = 0;
		temp_strides[i] = 1;
	}

	if (conv_strides != NULL) {
		for (int i = 0; i < dim(); ++i) {
			temp_strides[i] = conv_strides[i];
		}
	}

	if (mt_Conv_Boundary_Type_Full != boundary_type) {
		if (mt_Conv_Boundary_Type_Valid == boundary_type) {
			for (int i = 0; i < dim(); ++i) {
				sizes[i] = size()[i] - kernel.size()[i] + 1;
			}
		} else if (mt_Conv_Boundary_Type_Same == boundary_type){
			for (int i = 0; i < dim(); ++i) {
				sizes[i] = size()[i];
			}
		}

		for (int i = 0; i < dim(); ++i) {
			starts[i] = (full_sizes[i] - sizes[i] + 1) / 2;
		}
	}

	for (int i = 0; i < dim(); ++i) {
		sizes[i] = (sizes[i] - 1) / temp_strides[i] + 1;
	}

	i32 res_channel_num = 0;

	if (align_type == mt_Conv_Kernel_Align_Type_Row || align_type == mt_Conv_Kernel_Align_Type_Col) {
		res_channel_num = kernel.channel() / channel();
		basiclog_assert2(kernel.channel() % channel() == 0);
	} else {
		res_channel_num = kernel.channel() * channel();
	}

	res.create(dim(), sizes, mt_Depth_Channel(depth(), res_channel_num));

	mt_mat temp_src = *this;
	mt_mat temp_kernel = kernel;
	temp_src.attach(NULL);
	temp_kernel.attach(NULL);

	vector<mt_mat> res_channels;
	vector<mt_mat> src_channels;
	vector<mt_mat> kernel_channels;

	i32 channel_need_consider_continuous_memory = 16;

	if (channel() > channel_need_consider_continuous_memory) {
		temp_src.split(src_channels, false);
	} else {
		temp_src.all_channel(src_channels);
	}

	if (kernel.channel() > channel_need_consider_continuous_memory) {
		temp_kernel.split(kernel_channels, false);
	} else {
		temp_kernel.all_channel(kernel_channels);
	}

	if (res.channel() > channel_need_consider_continuous_memory) {
		res.split(res_channels, false);
	} else {
		res.all_channel(res_channels);
	}

	//reuse the full_sizes and sizes memory
	i32* src_steps = full_sizes;
	i32* kernel_steps = sizes;
	basicmath_mat_request_memory(i32, res_steps, dim());

	for (int i = 0; i < dim(); ++i) {
		src_steps[i] = src_channels.front().step()[i] / src_channels.front().element_channel_size();
		kernel_steps[i] = kernel_channels.front().step()[i] / kernel_channels.front().element_channel_size();
		res_steps[i] = res_channels.front().step()[i] / res_channels.front().element_channel_size();
	}

	if (mode == private_convolution::Conv_Model_FFT) {
		for (i32 iter_output = 0; iter_output < res.channel(); ++iter_output) {
			private_convolution::conv(mode, iter_output, res_channels, src_channels, kernel_channels, align_type, src_steps, kernel_steps, res_steps, temp_strides, starts);
		}
	} else {
#pragma omp parallel for
		for (i32 iter_output = 0; iter_output < res.channel(); ++iter_output) {
			private_convolution::conv(mode, iter_output, res_channels, src_channels, kernel_channels, align_type, src_steps, kernel_steps, res_steps, temp_strides, starts);

			if (res.channel() > channel_need_consider_continuous_memory) {
				res.channel_at(iter_output).set(res_channels[iter_output]);
			}

			if (align_type == mt_Conv_Kernel_Align_Type_Cross_Row) {
				//basiclog_info2(*this);
				//basiclog_info2(kernel);
				//basiclog_info2(res_channels[iter_output]);
			}
		}
	}

	basicmath_mat_release(full_sizes);
	basicmath_mat_release(sizes);
	basicmath_mat_release(starts);
	basicmath_mat_release(temp_strides);
	basicmath_mat_release(res_steps);

	if (m_auto_derivative != NULL && m_auto_derivative->stage() == mt_auto_derivative::Stage_Record_Computing) {
		res.m_auto_derivative = m_auto_derivative;
		kernel.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->cnov(res, *this, kernel, boundary_type, temp_strides);
	}
}

mt_mat mt_mat::conv(const mt_mat& kernel, mt_Conv_Boundary_Type boundary_type /* = mt_Conv_Boundary_Type_Valid */, mt_Conv_Kernel_Align_Type align_type, const int* conv_strides /* = NULL */) const {	
	mt_mat res;
	conv(res, kernel, boundary_type, align_type, conv_strides);

	return res;
}