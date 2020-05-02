#include "stdafx.h"

#include "mt_mat_helper.h"

mt_mat mt_mat_helper::merge_align_dim(const vector<mt_mat>& elements, i32 dim, b8 can_share_memory /* = sys_true */) {
	if (elements.empty()) {
		return mt_mat();
	}
	
	if ((i32)elements.size() == 1 && can_share_memory) {
		return elements[0];
	}

	mt_auto_derivative* auto_derivative = check_mat_array_auto_derivative(elements);

	basicmath_mat_request_memory(i32, res_size, elements[0].dim());

	for (i32 i = 0; i < elements[0].dim(); ++i) {
		res_size[i] = elements[0].size()[i];
	}

	for (i32 i = 1; i < (i32)elements.size(); ++i) {
		res_size[dim] += elements[i].size()[dim];
	}

	mt_mat res(elements[0].dim(), res_size, elements[0].depth_channel());

	i32 dim_start_index = 0;

	for (i32 i = 0; i < (i32)elements.size(); ++i) {
		mt_mat sub_mat = res.sub(dim_start_index, dim_start_index + elements[0].size()[dim], dim);
		dim_start_index += elements[0].size()[dim];

		sub_mat.set(elements[i]);
	}

	if (auto_derivative != NULL) {
		res.attach(auto_derivative);

		for (i32 i = 0; i < (i32)elements.size(); ++i) {
			elements[i].m_auto_derivative = auto_derivative;
		}

		auto_derivative->merge_align_dim(res, elements, dim);
	}


	return res;
}

mt_mat mt_mat_helper::merge_align_channel(const vector<mt_mat>& elements, b8 can_share_memory) {
	if (elements.empty()) {
		return mt_mat();
	}

	if ((i32)elements.size() == 1 && can_share_memory) {
		return elements[0];
	}

	mt_auto_derivative* auto_derivative = check_mat_array_auto_derivative(elements);

	int dims = elements.front().dim();
	mt_Depth depth = elements.front().depth();

	const i32* sizes = elements.front().size();

	i32 total_channel = 0;

	for (i32 i = 0; i < (i32)elements.size(); ++i) {
		total_channel += elements[i].channel();
	}

	mt_mat res(dims, sizes, mt_Depth_Channel(depth, total_channel));

	i32 start_channel = 0;

	sys_for(i, elements) {
		res.sub_channel(start_channel, start_channel + elements[i].channel()).set(elements[i]);
		start_channel += elements[i].channel();
	}

	if (auto_derivative != NULL) {
		res.attach(auto_derivative);

		for (i32 i = 0; i < (i32)elements.size(); ++i) {
			elements[i].m_auto_derivative = auto_derivative;
		}

		auto_derivative->merge_align_channel(res, elements);
	}

	return res;
}

void mt_mat_helper::clone(vector<mt_mat>& dsts, const vector<mt_mat>& srcs) {
	dsts.resize(srcs.size());

	for (i32 i = 0; i < (i32)srcs.size(); ++i) {
		dsts[i] = srcs[i].clone();
	}
}

i32 mt_mat_helper::get_conv_result_size(int src_size, int kernel_size, int stride, mt_Conv_Boundary_Type boundary_type) {
	i32 res_size = 0;

	if (boundary_type == mt_Conv_Boundary_Type_Full) {
		res_size = src_size + kernel_size - 1;
	} else if (boundary_type == mt_Conv_Boundary_Type_Same) {
		res_size = src_size;
	} else if (boundary_type == mt_Conv_Boundary_Type_Valid) {
		res_size = src_size - kernel_size + 1;
	} else {
		basiclog_unsupport2();
	}

	return (res_size - 1) / stride + 1;
}

void mt_mat_helper::get_conv_result_size(i32 dims, i32* res_sizes, const i32* src_sizes, const i32* kernel_sizes, const i32* stride_sizes, mt_Conv_Boundary_Type boundary_type) {
	for (i32 i = 0; i  < dims; ++i) {
		res_sizes[i] = get_conv_result_size(src_sizes[i], kernel_sizes[i], stride_sizes == NULL ? 1 : stride_sizes[i], boundary_type);
	}
}

int mt_mat_helper::get_pooling_result_size(int src_size, int kernel_size, int stride) {
	int res_size = 0;
	int cur_pos = 0;

	for (;;) {
		if (cur_pos >= src_size) {
			break;
		}

		if (cur_pos + kernel_size >= src_size) {
			++res_size;
			break;
		}

		cur_pos += stride;
		++res_size;	
	}

	return res_size;
}

void mt_mat_helper::get_pooling_result_size(i32 dims, i32* res_sizes, const i32* src_sizes, const i32* kernel_sizes, const i32* stride_sizes) {
	for (i32 i = 0; i  < dims; ++i) {
		res_sizes[i] = get_pooling_result_size(src_sizes[i], kernel_sizes[i], stride_sizes[i]);
	}
}

void mt_mat_helper::save(const string& file_path, const mt_mat& mat, b8 text_file) {
	if (text_file) {
		sys_string_file_buffer_writer buffer(file_path);
		save(&buffer, mat);
	} else {
		sys_byte_file_buffer_writer buffer(file_path);
		save(&buffer, mat);
	}
}

void mt_mat_helper::save(sys_buffer_writer* buffer, const mt_mat& mat) {
	sys_json_writer writer(buffer);

	writer<<"basicmath_mat"<<mat;
}

mt_mat mt_mat_helper::load(const string& file_path, b8 text_file /* = sys_true */) {
	if (text_file) {
		sys_string_file_buffer_reader buffer(file_path);
		return load(&buffer);
	} else {
		sys_byte_file_buffer_reader buffer(file_path);
		return load(&buffer);
	}
}

mt_mat mt_mat_helper::load(sys_buffer_reader* buffer) {
	mt_mat res;

	sys_json_reader reader(buffer);

	reader["basicmath_mat"]>>res;

	return res;
}

mt_mat mt_mat_helper::add(const vector<mt_mat>& elements, b8 can_share_memory) {
	if (elements.empty()) {
		return mt_mat();
	}

	if (elements.size() == 1 && can_share_memory) {
		return elements.front();
	}

	mt_auto_derivative* auto_derivative = check_mat_array_auto_derivative(elements);

	mt_mat res(elements[0], mt_mat::Construct_Type_Create_As_Size);
	res.set(elements[0]);

	for (i32 i = 1; i < (i32)elements.size(); ++i) {
		mt_mat temp = elements[i];
		temp.attach(NULL);

		res += temp;
	}

	if (auto_derivative != NULL) {
		res.attach(auto_derivative);

		for (i32 i = 0; i < (i32)elements.size(); ++i) {
			elements[i].m_auto_derivative = auto_derivative;
		}

		auto_derivative->add(res, elements);
	}

	return res;
}

mt_mat mt_mat_helper::dot(const vector<mt_mat>& elements, b8 can_share_memory) {
	if (elements.empty()) {
		return mt_mat();
	}

	if (elements.size() == 1 && can_share_memory) {
		return elements.front();
	}

	mt_auto_derivative* auto_derivative = check_mat_array_auto_derivative(elements);

	mt_mat res(elements[0], mt_mat::Construct_Type_Create_As_Size);
	res.set(elements[0]);

	for (i32 i = 1; i < (i32)elements.size(); ++i) {
		mt_mat temp = elements[i];
		temp.attach(NULL);

		res *= temp;
	}

	if (auto_derivative != NULL) {
		res.attach(auto_derivative);

		for (i32 i = 0; i < (i32)elements.size(); ++i) {
			elements[i].m_auto_derivative = auto_derivative;
		}

		auto_derivative->dot(res, elements);
	}

	return res;
}

mt_auto_derivative* mt_mat_helper::check_mat_array_auto_derivative(const vector<mt_mat>& elements) {
	mt_auto_derivative* auto_derivative = NULL;

	for (i32 i = 0; i < (i32)elements.size(); ++i) {
		if (elements[i].m_auto_derivative != NULL) {
			if (auto_derivative != NULL && auto_derivative != elements[i].m_auto_derivative) {
				basiclog_assert_message2(sys_false, "all mats must have only one auto_derivative!");
			}

			auto_derivative = elements[i].m_auto_derivative;
		}
	}

	return auto_derivative;
}

b8 mt_mat_helper::need_omp_for_multi_channel(const mt_mat& mat) {
	return mat.channel() >= 3 && mat.element_number() > 1000;
}