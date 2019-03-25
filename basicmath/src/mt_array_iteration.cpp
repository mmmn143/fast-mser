#include "stdafx.h"

#include "mt_array_iteration.h"

static const int mt_Array_Copy = 0;
static const int mt_Array_Cmp = 1;


namespace basicmath {
	class private_array_iteration {
	public:

		static void memcpy(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size) {
			u8* ptr_dst_data = dst;
			const u8* ptr_src_data = src;
			
			if (ndims == 1) {
				for (i32 i = 0; i < sizes[0]; ++i) {
					for (i32 c = 0; c < element_size; ++c) {
						ptr_dst_data[c] = ptr_src_data[c];
					}

					ptr_dst_data += dst_steps[0];
					ptr_src_data += src_steps[0];
				}


			} else if (ndims == 2) {
				u8* ptr_dst_data_dim0 = dst;
				const u8* ptr_src_data_dim0 = src;

				for (i32 row = 0; row < sizes[0]; ++row) {
					ptr_dst_data = ptr_dst_data_dim0;
					ptr_src_data = ptr_src_data_dim0;

					for (i32 col = 0; col < sizes[1]; ++col) {
						for (i32 c = 0; c < element_size; ++c) {
							ptr_dst_data[c] = ptr_src_data[c];
						}

						ptr_dst_data += dst_steps[1];
						ptr_src_data += src_steps[1];
					}

					ptr_dst_data_dim0 += dst_steps[0];
					ptr_src_data_dim0 += src_steps[0];
				}
			} else if (ndims == 3) {
				u8* ptr_dst_data_dim0 = dst;
				const u8* ptr_src_data_dim0 = src;

				for (i32 plane = 0; plane < sizes[0]; ++plane) {
					u8* ptr_dst_data_dim1 = ptr_dst_data_dim0;
					const u8* ptr_src_data_dim1 = ptr_src_data_dim0;

					for (i32 row = 0; row < sizes[1]; ++row) {
						ptr_dst_data = ptr_dst_data_dim0;
						ptr_src_data = ptr_src_data_dim0;

						for (i32 col = 0; col < sizes[2]; ++col) {
							for (i32 c = 0; c < element_size; ++c) {
								ptr_dst_data[c] = ptr_src_data[c];
							}
						
							ptr_dst_data += dst_steps[2];
							ptr_src_data += src_steps[2];
						}
		
						ptr_dst_data_dim1 += dst_steps[1];
						ptr_src_data_dim1 += src_steps[1];
					}

					ptr_dst_data_dim0 += dst_steps[0];
					ptr_src_data_dim0 += src_steps[0];
				}
			}
		}

		static b8 memcmp(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size) {
			u8* ptr_dst_data = dst;
			const u8* ptr_src_data = src;

			if (ndims == 1) {
				for (i32 i = 0; i < sizes[0]; ++i) {
					for (i32 c = 0; c < element_size; ++c) {
						if (ptr_dst_data[c] != ptr_src_data[c]) {
							return sys_false;
						}
					}

					ptr_dst_data += dst_steps[0];
					ptr_src_data += src_steps[0];
				}


			} else if (ndims == 2) {
				u8* ptr_dst_data_dim0 = dst;
				const u8* ptr_src_data_dim0 = src;

				for (i32 row = 0; row < sizes[0]; ++row) {
					ptr_dst_data = ptr_dst_data_dim0;
					ptr_src_data = ptr_src_data_dim0;

					for (i32 col = 0; col < sizes[1]; ++col) {
						for (i32 c = 0; c < element_size; ++c) {
							if (ptr_dst_data[c] != ptr_src_data[c]) {
								return sys_false;
							}
						}

						ptr_dst_data += dst_steps[1];
						ptr_src_data += src_steps[1];
					}

					ptr_dst_data_dim0 += dst_steps[0];
					ptr_src_data_dim0 += src_steps[0];
				}
			} else if (ndims == 3) {
				u8* ptr_dst_data_dim0 = dst;
				const u8* ptr_src_data_dim0 = src;

				for (i32 plane = 0; plane < sizes[0]; ++plane) {
					u8* ptr_dst_data_dim1 = ptr_dst_data_dim0;
					const u8* ptr_src_data_dim1 = ptr_src_data_dim0;

					for (i32 row = 0; row < sizes[1]; ++row) {
						ptr_dst_data = ptr_dst_data_dim0;
						ptr_src_data = ptr_src_data_dim0;

						for (i32 col = 0; col < sizes[2]; ++col) {
							for (i32 c = 0; c < element_size; ++c) {
								if (ptr_dst_data[c] != ptr_src_data[c]) {
									return sys_false;
								}
							}

							ptr_dst_data += dst_steps[2];
							ptr_src_data += src_steps[2];
						}

						ptr_dst_data_dim1 += dst_steps[1];
						ptr_src_data_dim1 += src_steps[1];
					}

					ptr_dst_data_dim0 += dst_steps[0];
					ptr_src_data_dim0 += src_steps[0];
				}
			}

			return sys_true;
		}
	};
}

int mt_array_iteration::get_continuous_dim(int ndims, const int* sizes, const int* steps, int element_size) {
	int res = ndims;

	if (element_size != abs(steps[ndims - 1])) {
		return res;
	}

	--res;

	for (int i = ndims - 2; i >= 0; --i) {
		if (steps[i] == steps[i + 1] * sizes[i + 1]) {
			--res;
		} else {
			break;
		}
	}

	return res;
}

void mt_array_iteration::array_copy(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size) {
	array_iteration(dst, src, ndims, sizes, dst_steps, src_steps, element_size, mt_Array_Copy);
}

b8 mt_array_iteration::array_cmp(const u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size) {
	return array_iteration(const_cast<u8*>(dst), src, ndims, sizes, dst_steps, src_steps, element_size, mt_Array_Cmp);
}

b8 mt_array_iteration::array_iteration(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size, int type) {
	int src_continuous_dim = get_continuous_dim(ndims, sizes, src_steps, element_size);
	int dst_continuous_dim = get_continuous_dim(ndims, sizes, dst_steps, element_size);

	int max_continuous_dim = max(src_continuous_dim, dst_continuous_dim);

	if (max_continuous_dim != 0) {
		if (ndims <= 4) {
			if (type == mt_Array_Copy) {
				private_array_iteration::memcpy(dst, src, ndims, sizes, dst_steps, src_steps, element_size);
			} else {
				return private_array_iteration::memcmp(dst, src, ndims, sizes, dst_steps, src_steps, element_size);
			}
		}		
	}

	if (mt_helper::is_same_symbol(src_steps[max_continuous_dim], dst_steps[max_continuous_dim])) {
		mt_array_memory_block_const_iterator src_iteartor(src, ndims, sizes, src_steps, max_continuous_dim, element_size);
		mt_array_memory_block_iterator dst_iteartor(dst, ndims, sizes, dst_steps, max_continuous_dim, element_size);

		for (;;) {
			const u8* ptr_src = src_iteartor.memory_start();
			u8* ptr_dst = dst_iteartor.memory_start();

			if (ptr_src == NULL) {
				break;
			}

			if (type == mt_Array_Copy) {
				memcpy(ptr_dst, ptr_src, src_iteartor.block_size());
			} else if (type == mt_Array_Cmp) {
				int res = memcmp(ptr_dst, ptr_src, src_iteartor.block_size());

				if (res != 0) {
					return sys_false;
				}
			}
		}
	} else {
		mt_array_element_const_iterator src_iteartor(src, ndims, sizes, src_steps, element_size);
		mt_array_element_iterator dst_iteartor(dst, ndims, sizes, dst_steps, element_size);

		for (;;) {
			const u8* ptr_src = src_iteartor.data();
			u8* ptr_dst = dst_iteartor.data();

			if (ptr_src == NULL) {
				break;
			}

			if (type == mt_Array_Copy) {
				memcpy(ptr_dst, ptr_src, src_iteartor.element_size());
			} else if (type == mt_Array_Cmp) {
				int res = memcmp(ptr_dst, ptr_src, src_iteartor.element_size());

				if (res != 0) {
					return sys_false;
				}
			}
		}
	}

	return sys_true;
}

mt_array_index_iterator::mt_array_index_iterator() {

}

mt_array_index_iterator::mt_array_index_iterator(int dim, const i32* sizes) {
	init(dim, sizes);
}

void mt_array_index_iterator::init(int dims, const i32* sizes) {
	m_dims = dims;
	m_sizes = sizes;
	m_access_number = 0;
	m_total_number = 1;

	for (int i = 0; i < m_dims; ++i) {
		m_total_number *= sizes[i];
	}

	m_cur_position.assign(m_dims, 0);
}

b8 mt_array_index_iterator::next() {
	if (m_access_number == 0) {
		++m_access_number;

		return sys_true;
	} else if (m_access_number == m_total_number) {
		return sys_false;
	}

	++m_access_number;
	++m_cur_position[m_dims - 1];

	for (int i = m_dims - 1; i >= 0; --i) {
		if (m_cur_position[i] == m_sizes[i]) {
			++m_cur_position[i - 1];

			for (int j = i; j < m_dims; ++j) {
				m_cur_position[j] = 0;
			}
		}
	}

	return sys_true;
}

i32 mt_array_index_iterator::size() const {
	return mt_helper::mutiply(m_sizes, m_sizes + m_dims);
}

void mt_array_index_iterator::all_index(vector<vector<i32>>& all_indexes) const {
	mt_array_index_iterator temp(m_dims, m_sizes);
	all_indexes.resize(size());

	i32 index = 0;

	while (temp.next()) {
		all_indexes[index++] = temp.position();
	}
}

mt_array_element_iterator::mt_array_element_iterator(u8* data, int ndims, const int* sizes, const int* steps, int element_size) {
	init_construct(data, ndims, sizes, steps, element_size);
}

mt_array_element_iterator::mt_array_element_iterator(mt_mat& mat) {
	init_construct(mat.data(), mat.dim(), mat.size(), mat.step(), mat.element_channel_size());
}

void mt_array_element_iterator::init_construct(u8* data, int ndims, const int* sizes, const int* steps, int element_size) {
	m_dims = ndims;

	m_sizes = sizes;
	m_int_steps = steps;

	m_ptr_dim_datas.assign(m_dims, data);
	m_cur_position.assign(m_dims, 0);

	m_accessed_count = 0;
	m_element_number = 1;

	for (int i = 0; i < m_dims; ++i) {
		m_element_number *= m_sizes[i];
	}

	m_element_size = element_size;
}

u8* mt_array_element_iterator::data() {
	if (m_accessed_count == 0) {
		++m_accessed_count;
		return m_ptr_dim_datas[m_dims - 1];
	} else if (m_accessed_count == m_element_number) {
		return NULL;
	}

	m_ptr_dim_datas[m_dims - 1] += m_int_steps[m_dims - 1];

	++m_cur_position[m_dims - 1];
	++m_accessed_count;


	for (int i = m_dims - 1; i > 0; --i) {
		if (m_cur_position[i] == m_sizes[i]) {
			++m_cur_position[i - 1];
			m_ptr_dim_datas[i - 1] += m_int_steps[i - 1];

			for (int j = i; j < m_dims; ++j) {
				m_cur_position[j] = 0;
				m_ptr_dim_datas[j] = m_ptr_dim_datas[i - 1];
			}
		}
	}

	return m_ptr_dim_datas[m_dims - 1];
}

mt_array_element_const_iterator::mt_array_element_const_iterator(const u8* data, int ndims, const int* sizes, const int* steps, int element_size) 
: mt_array_element_iterator(const_cast<u8*>(data), ndims, sizes, steps, element_size) {
}

mt_array_element_const_iterator::mt_array_element_const_iterator(const mt_mat& mat) 
	: mt_array_element_iterator(*(const_cast<mt_mat*>(&mat))) {
}

const u8* mt_array_element_const_iterator::data() {
	return __super::data();
}

mt_array_memory_block_iterator::mt_array_memory_block_iterator(u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size) {
	init_construct(data, ndims, sizes, steps, dim, element_size);
}

void mt_array_memory_block_iterator::init_construct(u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size) {
	if (dim == ndims) {
		m_block_element_number = 1;
		m_element_step = element_size;
		m_element_iterator = mt_array_element_iterator(data, ndims, sizes, steps, element_size);
	} else {
		m_block_element_number = 1;
		for (int i = dim; i < ndims; ++i) {
			m_block_element_number *= sizes[i];
		}

		m_element_step = steps[ndims - 1];

		if (m_element_step < 0) {
			data += (m_block_element_number - 1) * m_element_step;
		}

		if (dim == 0) {
			int size = 1;
			int step = steps[dim] * sizes[dim];

			m_element_iterator = mt_array_element_iterator(data, 1, &size, &step, element_size);
		} else {
			m_element_iterator = mt_array_element_iterator(data, dim, sizes, steps, element_size);
		}	
	}
}

mt_array_memory_block_iterator::mt_array_memory_block_iterator(mt_mat& mat) {
	int continious_dim = mt_array_iteration::get_continuous_dim(mat.dim(), mat.size(), mat.step(), mat.depth());
	init_construct(mat.data(), mat.dim(), mat.size(), mat.step(), continious_dim, mat.element_channel_size());
}

u8* mt_array_memory_block_iterator::data() {
	u8* ptr_data = m_element_iterator.data();

	if (ptr_data == NULL) {
		return NULL;
	}

	if (m_element_step < 0) {
		ptr_data -= (block_element_number() - 1) * m_element_step;
	}

	return ptr_data;
}

u8* mt_array_memory_block_iterator::memory_start() {
	return m_element_iterator.data();
}

int mt_array_memory_block_iterator::block_number() const {
	return m_element_iterator.element_number();
}

int mt_array_memory_block_iterator::block_size() const {
	return m_block_element_number * abs(m_element_step);
}

int mt_array_memory_block_iterator::block_element_number() const {
	return m_block_element_number;
}

mt_array_memory_block_const_iterator::mt_array_memory_block_const_iterator(const u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size) 
	: mt_array_memory_block_iterator(const_cast<u8*>(data), ndims, sizes, steps, dim, element_size) {
}

mt_array_memory_block_const_iterator::mt_array_memory_block_const_iterator(const mt_mat& mat) 
	: mt_array_memory_block_iterator(*(const_cast<mt_mat*>(&mat))) {
}

const u8* mt_array_memory_block_const_iterator::data() {
	return __super::data();
}