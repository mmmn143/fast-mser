#include "stdafx.h"
#include "img_fast_mser_v1.h"

#define get_real_for_merged(region)	\
	for (;region != NULL && region->m_region_flag == mser_region::Flag_Merged; region = region->m_parent){}	\

#define get_real_parent_for_merged(region, region_parent)	\
	for (region_parent = region->m_parent; region_parent != NULL && region_parent->m_region_flag == mser_region::Flag_Merged; region_parent = region_parent->m_parent){}	\

#define get_set_real_parent_for_merged(region, region_parent)	\
	for (region_parent = region->m_parent; region_parent != NULL && region_parent->m_region_flag == mser_region::Flag_Merged; region_parent = region_parent->m_parent){}	\
	region->m_parent = region_parent;\

#define get_real_tail(tail, real_tail)	\
	real_tail = tail;\
	while (m_linked_points[real_tail].m_ref != -1) { \
	real_tail = m_linked_points[real_tail].m_ref; \
	}\
	if (real_tail != tail) {\
		m_linked_points[tail].m_ref = real_tail;\
	}

#define bound_pointer_assgin(dst, src)	\
	dst->m_left = src->m_left; dst->m_right = src->m_right; dst->m_top = src->m_top; dst->m_bottom = src->m_bottom;

#define bound_pointer_adjust(dst, src)	\
	if (src->m_left < dst->m_left) dst->m_left = src->m_left;	\
	if (src->m_right > dst->m_right) dst->m_right = src->m_right;	\
	if (src->m_top < dst->m_top) dst->m_top = src->m_top;	\
	if (src->m_bottom > dst->m_bottom) dst->m_bottom = src->m_bottom;
	
img_fast_mser_v1::img_fast_mser_v1()
: img_mser_base() {

	m_linked_points = NULL;
	m_masked_image = NULL;
	m_heap = NULL;

	m_linked_point_size = 0;
	m_masked_image_size = 0;
	m_heap_size = 0;

	m_gray_order_regions = NULL;
	m_gray_order_region_size = 0;

	m_boundary_regions = NULL;
	m_boundary_region_size = 0;

	m_merged_point_index = 0;
	m_merged_point_size = 0;

	m_merged_points = NULL;
}

img_fast_mser_v1::~img_fast_mser_v1() {
	basicsys_free(m_heap);
	basicsys_free(m_linked_points);
	basicsys_free(m_masked_image);
	basicsys_free(m_boundary_regions);
}

void img_fast_mser_v1::clear_memory_cache() {
	basicsys_free(m_heap);
	basicsys_free(m_linked_points);
	basicsys_free(m_masked_image);
	basicsys_free(m_boundary_regions);
	basicsys_free(m_merged_points);

	m_heap_size = 0;
	m_masked_image_size = 0;
	m_linked_point_size = 0;
	m_boundary_region_size = 0;
	m_merged_point_size = 0;

	vector<parallel_info> temp;
	m_pinfo.swap(temp);
}

void img_fast_mser_v1::allocate_memory(const mt_mat& img, const img_mask_info<u8>& mask) {
	u32 h_size = img.size()[0] * img.size()[1] + 257 * m_parallel_thread_number;

	if (h_size != m_heap_size) {
		if (m_heap != NULL) {
			free(m_heap);
		}

		m_heap_size = h_size;
		m_heap = (i16**)malloc(sizeof(i16*) * m_heap_size);
	}
	
	if (m_parallel_thread_number == 1) {
		allocate_memory_parallel_1(img, mask);
	} else if (m_parallel_thread_number == 2) {
		allocate_memory_parallel_2(img, mask);
	} else if (m_parallel_thread_number == 4) {
		allocate_memory_parallel_4(img, mask);
	} else if (m_parallel_thread_number == 8) {
		allocate_memory_parallel_8(img, mask);
	} else if (m_parallel_thread_number == 16) {
		allocate_memory_parallel_16(img, mask);
	} else if (m_parallel_thread_number == 32) {
		allocate_memory_parallel_32(img, mask);
	}

	if (img_Connected_Type_8_Neibour == m_connected_type) {
		m_dir_max = 0x1000;
		m_dir_mask = 0x1e00;
	} else {
		m_dir_max = 0x800;
		m_dir_mask = 0x0e00;
	}
}

void img_fast_mser_v1::allocate_memory_parallel_1(const mt_mat& img, const img_mask_info<u8>& mask) {
	m_pinfo.resize(1);

	// left right split
	u32 width = (u32)img.size()[1];
	u32 height = (u32)img.size()[0];

	u32 pt_size = (width + 2) * (height + 2); 

	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + 
		sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	parallel_info& pinfo = m_pinfo[0];
	pinfo.m_left = 0;
	pinfo.m_top = 0;
	pinfo.m_width = width;
	pinfo.m_height = height;
	pinfo.m_width_with_boundary = width + 2;
	pinfo.m_height_with_boundary = height + 2;
	pinfo.m_masked_image_data = m_masked_image;
	pinfo.m_link_points = m_linked_points;

	pinfo.m_heap = m_heap;
	pinfo.m_heap_start[0] = &pinfo.m_heap[0];

	pinfo.m_boundary_regions[0] = NULL;
	pinfo.m_boundary_regions[1] = NULL;
	pinfo.m_boundary_regions[2] = NULL;
	pinfo.m_boundary_regions[3] = NULL;

	i32 row_step = width + 2;

	if (img_Connected_Type_8_Neibour == m_connected_type) {
		pinfo.m_dir[0] = 1;
		pinfo.m_dir[1] = 1 - row_step;
		pinfo.m_dir[2] = -row_step;
		pinfo.m_dir[3] = -1 - row_step;
		pinfo.m_dir[4] = -1;
		pinfo.m_dir[5] = -1 + row_step;
		pinfo.m_dir[6] = row_step;
		pinfo.m_dir[7] = 1 + row_step;
	} else {
		pinfo.m_dir[0] = 1;
		pinfo.m_dir[1] = -row_step;
		pinfo.m_dir[2] = -1;
		pinfo.m_dir[3] = row_step;
	}
}

void img_fast_mser_v1::allocate_memory_parallel_2(const mt_mat& img, const img_mask_info<u8>& mask) {
	m_pinfo.resize(2);

	// left right split
	u32 left_width = (u32)img.size()[1] / 2;
	u32 height = (u32)img.size()[0];

	u32 right_width = (u32)img.size()[1] - left_width;

	u32 horizontal_index[2] = {0, left_width};
	u32 widths[2] = {left_width, right_width};

	m_orderd_indexes.resize(1);
	m_orderd_indexes[0] = (i32*)malloc(sizeof(i32) * img.size()[0]);

	if (m_merged_point_size == 0) {
		m_merged_point_size = 1000;
		m_merged_points = (i32*)malloc(sizeof(i32) * m_merged_point_size);
	}

	m_merged_point_index = 0;

	u32 pt_size = (left_width + 2) * (height + 2) + (right_width + 2) * (height + 2); 

	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	//boundary regions size
	u32 boundary_size = img.size()[0] * 2;

	if (m_boundary_region_size != boundary_size) {
		if (m_boundary_regions != NULL) {
			free(m_boundary_regions);
		}

		m_boundary_region_size = boundary_size;
		m_boundary_regions = (mser_region**)malloc(sizeof(mser_region*) * m_boundary_region_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + sizeof(mser_region*) * (m_boundary_region_size)
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;
	i32 memory_offset_for_boundary_region = 0;

	for (u32 h = 0; h < 2; ++h) {
		parallel_info& pinfo = m_pinfo[i];
		pinfo.m_left = horizontal_index[h];
		pinfo.m_top = 0;
		pinfo.m_width = widths[h];
		pinfo.m_height = height;
		pinfo.m_width_with_boundary = widths[h] + 2;
		pinfo.m_height_with_boundary = height + 2;
		pinfo.m_masked_image_data = m_masked_image + memory_offset_with_boundary;
		pinfo.m_link_points = m_linked_points + memory_offset_with_boundary;

		pinfo.m_heap = m_heap + memory_offset_for_heap;
		pinfo.m_heap_start[0] = &pinfo.m_heap[0];

		if (i == 0) {
			//right
			pinfo.m_boundary_regions[0] = NULL;
			pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
			memory_offset_for_boundary_region += pinfo.m_height;
			pinfo.m_boundary_regions[2] = NULL;
			pinfo.m_boundary_regions[3] = NULL;
		} else if (i == 1) {
			//left
			pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
			memory_offset_for_boundary_region += pinfo.m_height;
			pinfo.m_boundary_regions[1] = NULL;
			pinfo.m_boundary_regions[2] = NULL;
			pinfo.m_boundary_regions[3] = NULL;
		}

		i32 row_step = widths[h] + 2;

		if (img_Connected_Type_8_Neibour == m_connected_type) {
			pinfo.m_dir[0] = 1;
			pinfo.m_dir[1] = 1 - row_step;
			pinfo.m_dir[2] = -row_step;
			pinfo.m_dir[3] = -1 - row_step;
			pinfo.m_dir[4] = -1;
			pinfo.m_dir[5] = -1 + row_step;
			pinfo.m_dir[6] = row_step;
			pinfo.m_dir[7] = 1 + row_step;
		} else {
			pinfo.m_dir[0] = 1;
			pinfo.m_dir[1] = -row_step;
			pinfo.m_dir[2] = -1;
			pinfo.m_dir[3] = row_step;
		}

		memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;

		i32 patch_size = pinfo.m_width * pinfo.m_height;
		memory_offset_for_region += patch_size;
		memory_offset_for_heap += patch_size + 257;

		++i;
	}
}

void img_fast_mser_v1::allocate_memory_parallel_4(const mt_mat& img, const img_mask_info<u8>& mask) {
	m_pinfo.resize(4);

	u32 left_width = (u32)img.size()[1] / 2;
	u32 top_height = (u32)img.size()[0] / 2;

	u32 right_width = (u32)img.size()[1] - left_width;
	u32 bottom_height = (u32)img.size()[0] - top_height;

	u32 horizontal_index[2] = {0, left_width};
	u32 vertical_index[2] = {0, top_height};
	u32 widths[2] = {left_width, right_width};
	u32 heights[2] = {top_height, bottom_height};

	m_orderd_indexes.resize(2);
	m_orderd_indexes[0] = (i32*)malloc(sizeof(i32) * img.size()[0]);
	m_orderd_indexes[1] = (i32*)malloc(sizeof(i32) * img.size()[1]);

	if (m_merged_point_size == 0) {
		m_merged_point_size = 1000;
		m_merged_points = (i32*)malloc(sizeof(i32) * m_merged_point_size);
	}

	m_merged_point_index = 0;
	
	u32 pt_size = (left_width + 2) * (top_height + 2) + (left_width + 2) * (bottom_height + 2) + (right_width + 2) * (top_height + 2) + (right_width + 2) * (bottom_height + 2); 
	
	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	//boundary regions size
	u32 boundary_size = img.size()[0] * 2 + img.size()[1] * 2;

	if (m_boundary_region_size != boundary_size) {
		if (m_boundary_regions != NULL) {
			free(m_boundary_regions);
		}

		m_boundary_region_size = boundary_size;
		m_boundary_regions = (mser_region**)malloc(sizeof(mser_region*) * m_boundary_region_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + sizeof(mser_region*) * (m_boundary_region_size)
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;
	i32 memory_offset_for_boundary_region = 0;

	for (u32 v = 0; v < 2; ++v) {
		for (u32 h = 0; h < 2; ++h) {
			parallel_info& pinfo = m_pinfo[i];
			pinfo.m_left = horizontal_index[h];
			pinfo.m_top = vertical_index[v];
			pinfo.m_width = widths[h];
			pinfo.m_height = heights[v];
			pinfo.m_width_with_boundary = widths[h] + 2;
			pinfo.m_height_with_boundary = heights[v] + 2;
			pinfo.m_masked_image_data = m_masked_image + memory_offset_with_boundary;
			pinfo.m_link_points = m_linked_points + memory_offset_with_boundary;

			pinfo.m_heap = m_heap + memory_offset_for_heap;
			pinfo.m_heap_start[0] = &pinfo.m_heap[0];

			if (i == 0) {
				//right bottom
				pinfo.m_boundary_regions[0] = NULL;
				pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
				pinfo.m_boundary_regions[2] = NULL;
				pinfo.m_boundary_regions[3] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else if (i == 1) {
				//left bottom
				pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
				pinfo.m_boundary_regions[1] = NULL;
				pinfo.m_boundary_regions[2] = NULL;
				pinfo.m_boundary_regions[3] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else if (i == 2) {
				//right top
				pinfo.m_boundary_regions[0] = NULL;
				pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
				pinfo.m_boundary_regions[2] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
				pinfo.m_boundary_regions[3] = NULL;
			} else {
				//left top
				pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
				pinfo.m_boundary_regions[1] = NULL;
				pinfo.m_boundary_regions[2] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
				pinfo.m_boundary_regions[3] = NULL;
			}

			i32 row_step = widths[h] + 2;

			if (img_Connected_Type_8_Neibour == m_connected_type) {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = 1 - row_step;
				pinfo.m_dir[2] = -row_step;
				pinfo.m_dir[3] = -1 - row_step;
				pinfo.m_dir[4] = -1;
				pinfo.m_dir[5] = -1 + row_step;
				pinfo.m_dir[6] = row_step;
				pinfo.m_dir[7] = 1 + row_step;
			} else {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = -row_step;
				pinfo.m_dir[2] = -1;
				pinfo.m_dir[3] = row_step;
			}

			memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;
			
			i32 patch_size = pinfo.m_width * pinfo.m_height;
			memory_offset_for_region += patch_size;
			memory_offset_for_heap += patch_size + 257;
			
			++i;
		}
	}
}

void img_fast_mser_v1::allocate_memory_parallel_8(const mt_mat& img, const img_mask_info<u8>& mask) {
	// 2 rows * 4 cols
	m_pinfo.resize(8);

	u32 p_width = (u32)img.size()[1] / 4;
	u32 p_height = (u32)img.size()[0] / 2;

	u32 horizontal_index[4] = {0, p_width, 2 * p_width, 3 * p_width};
	u32 vertical_index[2] = {0, p_height};
	u32 widths[4] = {p_width, p_width, p_width, (u32)img.size()[1] - 3 * p_width};
	u32 heights[2] = {p_height, (u32)img.size()[0] - p_height};

	m_orderd_indexes.resize(2);
	m_orderd_indexes[0] = (i32*)malloc(sizeof(i32) * img.size()[0]);
	m_orderd_indexes[1] = (i32*)malloc(sizeof(i32) * img.size()[1]);

	if (m_merged_point_size == 0) {
		m_merged_point_size = 1000;
		m_merged_points = (i32*)malloc(sizeof(i32) * m_merged_point_size);
	}

	m_merged_point_index = 0;

	u32 pt_size = 0;

	for (i32 v = 0; v < 2; ++v) {
		for (i32 h = 0; h < 4; ++h) {
			pt_size += (widths[h] + 2) * (heights[v] + 2);
		}
	}
		
	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	//boundary regions size
	u32 boundary_size = img.size()[0] * 6 + img.size()[1] * 2;

	if (m_boundary_region_size != boundary_size) {
		if (m_boundary_regions != NULL) {
			free(m_boundary_regions);
		}

		m_boundary_region_size = boundary_size;
		m_boundary_regions = (mser_region**)malloc(sizeof(mser_region*) * m_boundary_region_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + sizeof(mser_region*) * (m_boundary_region_size)
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;
	i32 memory_offset_for_boundary_region = 0;

	for (u32 v = 0; v < 2; ++v) {
		for (u32 h = 0; h < 4; ++h) {
			parallel_info& pinfo = m_pinfo[i];
			pinfo.m_left = horizontal_index[h];
			pinfo.m_top = vertical_index[v];
			pinfo.m_width = widths[h];
			pinfo.m_height = heights[v];
			pinfo.m_width_with_boundary = widths[h] + 2;
			pinfo.m_height_with_boundary = heights[v] + 2;
			pinfo.m_masked_image_data = m_masked_image + memory_offset_with_boundary;
			pinfo.m_link_points = m_linked_points + memory_offset_with_boundary;

			pinfo.m_heap = m_heap + memory_offset_for_heap;
			pinfo.m_heap_start[0] = &pinfo.m_heap[0];

			b8 has_left = sys_false;
			b8 has_right = sys_false;
			b8 has_top = sys_false;
			b8 has_bottom = sys_false;

			if (i != 0 && i != 4) {
				has_left = sys_true;
			}

			if (i <= 3) {
				has_bottom = sys_true;
			} else {
				has_top = sys_true;
			}

			if (i != 3 && i != 7) {
				has_right = sys_true;
			}

			if (has_left) {
				pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[0] = NULL;
			}

			if (has_right) {
				pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[1] = NULL;
			}

			if (has_top) {
				pinfo.m_boundary_regions[2] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[2] = NULL;
			}

			if (has_bottom) {
				pinfo.m_boundary_regions[3] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[3] = NULL;
			}

			i32 row_step = widths[h] + 2;

			if (img_Connected_Type_8_Neibour == m_connected_type) {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = 1 - row_step;
				pinfo.m_dir[2] = -row_step;
				pinfo.m_dir[3] = -1 - row_step;
				pinfo.m_dir[4] = -1;
				pinfo.m_dir[5] = -1 + row_step;
				pinfo.m_dir[6] = row_step;
				pinfo.m_dir[7] = 1 + row_step;
			} else {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = -row_step;
				pinfo.m_dir[2] = -1;
				pinfo.m_dir[3] = row_step;
			}

			memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;

			i32 patch_size = pinfo.m_width * pinfo.m_height;
			memory_offset_for_region += patch_size;
			memory_offset_for_heap += patch_size + 257;

			++i;
		}
	}
}

void img_fast_mser_v1::allocate_memory_parallel_16(const mt_mat& img, const img_mask_info<u8>& mask) {
	// 4 rows * 4 cols
	m_pinfo.resize(16);

	u32 p_width = (u32)img.size()[1] / 4;
	u32 p_height = (u32)img.size()[0] / 4;

	u32 horizontal_index[4] = {0, p_width, 2 * p_width, 3 * p_width};
	u32 vertical_index[4] = {0, p_height, 2 * p_height, 3 * p_height};
	u32 widths[4] = {p_width, p_width, p_width, (u32)img.size()[1] - 3 * p_width};
	u32 heights[4] = {p_height, p_height, p_height, (u32)img.size()[0] - 3 * p_height};

	m_orderd_indexes.resize(2);
	m_orderd_indexes[0] = (i32*)malloc(sizeof(i32) * img.size()[0]);
	m_orderd_indexes[1] = (i32*)malloc(sizeof(i32) * img.size()[1]);

	if (m_merged_point_size == 0) {
		m_merged_point_size = 1000;
		m_merged_points = (i32*)malloc(sizeof(i32) * m_merged_point_size);
	}

	m_merged_point_index = 0;

	u32 pt_size = 0;

	for (i32 v = 0; v < 4; ++v) {
		for (i32 h = 0; h < 4; ++h) {
			pt_size += (widths[h] + 2) * (heights[v] + 2);
		}
	}

	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	//boundary regions size
	u32 boundary_size = img.size()[0] * 6 + img.size()[1] * 6;

	if (m_boundary_region_size != boundary_size) {
		if (m_boundary_regions != NULL) {
			free(m_boundary_regions);
		}

		m_boundary_region_size = boundary_size;
		m_boundary_regions = (mser_region**)malloc(sizeof(mser_region*) * m_boundary_region_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + sizeof(mser_region*) * (m_boundary_region_size)
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;
	i32 memory_offset_for_boundary_region = 0;

	for (u32 v = 0; v < 4; ++v) {
		for (u32 h = 0; h < 4; ++h) {
			parallel_info& pinfo = m_pinfo[i];
			pinfo.m_left = horizontal_index[h];
			pinfo.m_top = vertical_index[v];
			pinfo.m_width = widths[h];
			pinfo.m_height = heights[v];
			pinfo.m_width_with_boundary = widths[h] + 2;
			pinfo.m_height_with_boundary = heights[v] + 2;
			pinfo.m_masked_image_data = m_masked_image + memory_offset_with_boundary;
			pinfo.m_link_points = m_linked_points + memory_offset_with_boundary;

			pinfo.m_heap = m_heap + memory_offset_for_heap;
			pinfo.m_heap_start[0] = &pinfo.m_heap[0];

			b8 has_left = sys_false;
			b8 has_right = sys_false;
			b8 has_top = sys_false;
			b8 has_bottom = sys_false;

			if (i % 4 != 0) {
				has_left = sys_true;
			}

			if (i <= 11) {
				has_bottom = sys_true;
			} 
			
			if (i > 3) {
				has_top = sys_true;
			}

			if (i % 4 != 3) {
				has_right = sys_true;
			}

			if (has_left) {
				pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[0] = NULL;
			}

			if (has_right) {
				pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[1] = NULL;
			}

			if (has_top) {
				pinfo.m_boundary_regions[2] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[2] = NULL;
			}

			if (has_bottom) {
				pinfo.m_boundary_regions[3] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[3] = NULL;
			}

			i32 row_step = widths[h] + 2;

			if (img_Connected_Type_8_Neibour == m_connected_type) {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = 1 - row_step;
				pinfo.m_dir[2] = -row_step;
				pinfo.m_dir[3] = -1 - row_step;
				pinfo.m_dir[4] = -1;
				pinfo.m_dir[5] = -1 + row_step;
				pinfo.m_dir[6] = row_step;
				pinfo.m_dir[7] = 1 + row_step;
			} else {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = -row_step;
				pinfo.m_dir[2] = -1;
				pinfo.m_dir[3] = row_step;
			}

			memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;

			i32 patch_size = pinfo.m_width * pinfo.m_height;
			memory_offset_for_region += patch_size;
			memory_offset_for_heap += patch_size + 257;

			++i;
		}
	}
}


void img_fast_mser_v1::allocate_memory_parallel_32(const mt_mat& img, const img_mask_info<u8>& mask) {
	// 4 rows * 8 cols
	m_pinfo.resize(32);

	u32 p_width = (u32)img.size()[1] / 8;
	u32 p_height = (u32)img.size()[0] / 4;

	u32 horizontal_index[8] = {0, p_width, 2 * p_width, 3 * p_width, 4 * p_width, 5 * p_width, 6 * p_width, 7 * p_width};
	u32 vertical_index[4] = {0, p_height, 2 * p_height, 3 * p_height};
	u32 widths[8] = {p_width, p_width, p_width, p_width, p_width, p_width, p_width, (u32)img.size()[1] - 7 * p_width};
	u32 heights[4] = {p_height, p_height, p_height, (u32)img.size()[0] - 3 * p_height};

	m_orderd_indexes.resize(2);
	m_orderd_indexes[0] = (i32*)malloc(sizeof(i32) * img.size()[0]);
	m_orderd_indexes[1] = (i32*)malloc(sizeof(i32) * img.size()[1]);

	if (m_merged_point_size == 0) {
		m_merged_point_size = 1000;
		m_merged_points = (i32*)malloc(sizeof(i32) * m_merged_point_size);
	}

	m_merged_point_index = 0;

	u32 pt_size = 0;

	for (i32 v = 0; v < 4; ++v) {
		for (i32 h = 0; h < 8; ++h) {
			pt_size += (widths[h] + 2) * (heights[v] + 2);
		}
	}

	if (pt_size != m_linked_point_size) {
		if (m_linked_points != NULL) {
			free(m_linked_points);
		}

		m_linked_point_size = pt_size;
		m_linked_points = (linked_point*)malloc(sizeof(linked_point) * m_linked_point_size);
	}

	if (pt_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = pt_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	//boundary regions size
	u32 boundary_size = img.size()[0] * 14 + img.size()[1] * 6;

	if (m_boundary_region_size != boundary_size) {
		if (m_boundary_regions != NULL) {
			free(m_boundary_regions);
		}

		m_boundary_region_size = boundary_size;
		m_boundary_regions = (mser_region**)malloc(sizeof(mser_region*) * m_boundary_region_size);
	}

	m_channel_total_running_memory += sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_linked_point_size + sizeof(mser_region*) * (m_boundary_region_size)
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;
	i32 memory_offset_for_boundary_region = 0;

	for (u32 v = 0; v < 4; ++v) {
		for (u32 h = 0; h < 8; ++h) {
			parallel_info& pinfo = m_pinfo[i];
			pinfo.m_left = horizontal_index[h];
			pinfo.m_top = vertical_index[v];
			pinfo.m_width = widths[h];
			pinfo.m_height = heights[v];
			pinfo.m_width_with_boundary = widths[h] + 2;
			pinfo.m_height_with_boundary = heights[v] + 2;
			pinfo.m_masked_image_data = m_masked_image + memory_offset_with_boundary;
			pinfo.m_link_points = m_linked_points + memory_offset_with_boundary;

			pinfo.m_heap = m_heap + memory_offset_for_heap;
			pinfo.m_heap_start[0] = &pinfo.m_heap[0];

			b8 has_left = sys_false;
			b8 has_right = sys_false;
			b8 has_top = sys_false;
			b8 has_bottom = sys_false;

			if (i % 8 != 0) {
				has_left = sys_true;
			}

			if (i <= 23) {
				has_bottom = sys_true;
			} 

			if (i > 7) {
				has_top = sys_true;
			}

			if (i % 8 != 7) {
				has_right = sys_true;
			}

			if (has_left) {
				pinfo.m_boundary_regions[0] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[0] = NULL;
			}

			if (has_right) {
				pinfo.m_boundary_regions[1] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_height;
			} else {
				pinfo.m_boundary_regions[1] = NULL;
			}

			if (has_top) {
				pinfo.m_boundary_regions[2] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[2] = NULL;
			}

			if (has_bottom) {
				pinfo.m_boundary_regions[3] = m_boundary_regions + memory_offset_for_boundary_region;
				memory_offset_for_boundary_region += pinfo.m_width;
			} else {
				pinfo.m_boundary_regions[3] = NULL;
			}

			i32 row_step = widths[h] + 2;

			if (img_Connected_Type_8_Neibour == m_connected_type) {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = 1 - row_step;
				pinfo.m_dir[2] = -row_step;
				pinfo.m_dir[3] = -1 - row_step;
				pinfo.m_dir[4] = -1;
				pinfo.m_dir[5] = -1 + row_step;
				pinfo.m_dir[6] = row_step;
				pinfo.m_dir[7] = 1 + row_step;
			} else {
				pinfo.m_dir[0] = 1;
				pinfo.m_dir[1] = -row_step;
				pinfo.m_dir[2] = -1;
				pinfo.m_dir[3] = row_step;
			}

			memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;

			i32 patch_size = pinfo.m_width * pinfo.m_height;
			memory_offset_for_region += patch_size;
			memory_offset_for_heap += patch_size + 257;

			++i;
		}
	}
}

void img_fast_mser_v1::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	if (m_parallel_thread_number == 1) {
		build_tree_parallel_1(src, mask, gray_mask);
	} else if (m_parallel_thread_number == 2) {
		build_tree_parallel_2(src, mask, gray_mask);
	} else if (m_parallel_thread_number == 4) {
		build_tree_parallel_4(src, mask, gray_mask);
	} else if (m_parallel_thread_number == 8) {
		build_tree_parallel_8(src, mask, gray_mask);
	} else if (m_parallel_thread_number == 16) {
		build_tree_parallel_16(src, mask, gray_mask);
	} else if (m_parallel_thread_number == 32) {
		build_tree_parallel_32(src, mask, gray_mask);
	}
	
	if (gray_mask == 255 || m_from_min_max[1] == sys_false) {
		for (i32 i = 0; i < (i32)m_pinfo.size(); ++i) {
			m_channel_total_running_memory += m_pinfo[i].m_mser_regions.memory_size();
		}
	}
}

void img_fast_mser_v1::build_tree_parallel_1(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	make_tree_patch(m_pinfo[0], src, mask, gray_mask, 0);
}

void img_fast_mser_v1::build_tree_parallel_2(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(2)
	for (i32 i = 0; i < 2; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	i64 tick = sys_timer::get_tick_cout();
	merge_tree_parallel_2(src, gray_mask);
	sys_alg_analyzer::add("merging_process", sys_timer::get_tick_cout() - tick);
}

void img_fast_mser_v1::build_tree_parallel_4(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 4; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	i64 tick = sys_timer::get_tick_cout();
	merge_tree_parallel_4(src, gray_mask);
	sys_alg_analyzer::add("merging_process", sys_timer::get_tick_cout() - tick);
}

void img_fast_mser_v1::build_tree_parallel_8(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(8)
	for (i32 i = 0; i < 8; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	i64 tick = sys_timer::get_tick_cout();
	merge_tree_parallel_8(src, gray_mask);
	sys_alg_analyzer::add("merging_process", sys_timer::get_tick_cout() - tick);
}


void img_fast_mser_v1::build_tree_parallel_16(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(16)
	for (i32 i = 0; i < 16; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	i64 tick = sys_timer::get_tick_cout();
	merge_tree_parallel_16(src, gray_mask);
	sys_alg_analyzer::add("merging_process", sys_timer::get_tick_cout() - tick);
}

void img_fast_mser_v1::build_tree_parallel_32(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(32)
	for (i32 i = 0; i < 32; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	i64 tick = sys_timer::get_tick_cout();
	merge_tree_parallel_32(src, gray_mask);
	sys_alg_analyzer::add("merging_process", sys_timer::get_tick_cout() - tick);
}


void img_fast_mser_v1::make_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask, u8 patch_index) {
	sys_timer timer("make_tree_patch");
	timer.begin();

	process_tree_patch(pinfo, img, mask, gray_mask);
	
	i32 erCount = 0;

	pinfo.m_mser_regions.init(2000, sys_false);

	i16*** heap_cur = pinfo.m_heap_start;
	connected_comp* comptr = pinfo.m_comp; 
	i16* ioptr = pinfo.m_masked_image_data + 1 + pinfo.m_width_with_boundary;
	i16* imgptr = ioptr;
	
	linked_point* ptsptr = pinfo.m_link_points;
	i32* dir = pinfo.m_dir;

	i32 offset = 0;
	comptr->m_gray_level = 257;	//process mask
	comptr++;
	comptr->m_gray_level = (*imgptr)&0x01ff;

	mser_region* cur_region;
	basicmath_block_memory_get(pinfo.m_mser_regions, cur_region, mser_region);
	basicmath_block_memory_add(pinfo.m_mser_regions);

	init_comp(comptr, cur_region, patch_index);
	*imgptr |= 0x8000;
	heap_cur += (*imgptr)&0x01ff;

	int mser_number = 0;
	u16 dir_masked = 0;
	i32 pt_index;

	for ( ; ; ) {
		// take tour of all the 4 or 8 directions
		while ( (dir_masked = ((*imgptr)&m_dir_mask)) < m_dir_max ) {
			// get the neighbor
			i16* imgptr_nbr = imgptr+dir[dir_masked>>9];
			// if the neighbor is not visited yet
			if ( *imgptr_nbr >= 0 ) {

				*imgptr_nbr |= 0x8000; // mark it as visited

				offset = ((*imgptr_nbr)&0x01ff)-((*imgptr)&0x01ff);

				if (offset < 0) {
					// when the value of neighbor smaller than current
					// push current to boundary heap and make the neighbor to be the current one
					// create an empty comp
					(*heap_cur)++;
					**heap_cur = imgptr;
					*imgptr += 0x200;
					heap_cur += offset;

					imgptr = imgptr_nbr;
					comptr++;
					comptr->m_gray_level = (*imgptr)&0x01ff;


					basicmath_block_memory_get(pinfo.m_mser_regions, cur_region, mser_region);
					basicmath_block_memory_add(pinfo.m_mser_regions);
					init_comp(comptr, cur_region, patch_index);
					continue;
				} else {
					// otherwise, push the neighbor to boundary heap
					heap_cur[offset]++;
					*heap_cur[offset] = imgptr_nbr;
				}
			}

			*imgptr += 0x200;
		}

		// get the current location
		u8 gray = (*imgptr)&0x01ff;
		i32 imsk = (i32)(imgptr-ioptr);		

		ptsptr->m_y = imsk / pinfo.m_width_with_boundary;
		ptsptr->m_x = imsk - (ptsptr->m_y * pinfo.m_width_with_boundary);	
		
		if (*imgptr & Boundary_Yes_Mask) {
			//boundary
			if (ptsptr->m_x == 0 && pinfo.m_boundary_regions[0] != NULL) {
				//left boundary
				pinfo.m_boundary_regions[0][ptsptr->m_y] = comptr->m_region;
			}
			
			if (ptsptr->m_x == pinfo.m_width - 1 && pinfo.m_boundary_regions[1] != NULL) {
				//right boundary
				pinfo.m_boundary_regions[1][ptsptr->m_y] = comptr->m_region;
			}

			if (ptsptr->m_y == 0 && pinfo.m_boundary_regions[2] != NULL) {
				pinfo.m_boundary_regions[2][ptsptr->m_x] = comptr->m_region;
			} 
			
			if (ptsptr->m_y == pinfo.m_height - 1 && pinfo.m_boundary_regions[3] != NULL) {
				pinfo.m_boundary_regions[3][ptsptr->m_x] = comptr->m_region;
			}

			comptr->m_region->m_boundary_region = 1;
		}

		ptsptr->m_x += pinfo.m_left;
		ptsptr->m_y += pinfo.m_top;

		pt_index = (i32)(ptsptr - m_linked_points);

		if (comptr->m_size > 0) {
			ptsptr->m_next = comptr->m_head;
			m_linked_points[comptr->m_head].m_prev = pt_index;

			ptsptr->m_prev = -1;
			ptsptr->m_ref = -1;

			if (ptsptr->m_x < comptr->m_left) {
				comptr->m_left = ptsptr->m_x;
			} else if (ptsptr->m_x > comptr->m_right) {
				comptr->m_right = ptsptr->m_x;
			}

			if (ptsptr->m_y < comptr->m_top) {
				comptr->m_top = ptsptr->m_y;
			} else if (ptsptr->m_y > comptr->m_bottom) {
				comptr->m_bottom = ptsptr->m_y;
			}

		} else {
			ptsptr->m_prev = -1;
			ptsptr->m_next = -1;
			ptsptr->m_ref = -1;
			comptr->m_tail = pt_index;

			comptr->m_left = ptsptr->m_x;
			comptr->m_right = ptsptr->m_x;
			comptr->m_top = ptsptr->m_y;
			comptr->m_bottom = ptsptr->m_y;
		}

		comptr->m_head = pt_index;
		++comptr->m_size;

		++ptsptr;

		// get the next pixel from boundary heap
		if ( **heap_cur ) {
			imgptr = **heap_cur;
			(*heap_cur)--;
		} else {
			++erCount;
			comptr->m_region->m_head = comptr->m_head;
			comptr->m_region->m_tail = comptr->m_tail;
			comptr->m_region->m_size = comptr->m_size;
			comptr->m_region->m_unmerged_size = comptr->m_size;
			bound_pointer_assgin(comptr->m_region, comptr);

			b8 reused = sys_false;

			if (comptr->m_region->m_size < m_min_point && comptr->m_region->m_boundary_region == 0) {
				basicmath_block_memory_reuse(pinfo.m_mser_regions, comptr->m_region);

				//pinfo.m_mser_regions.assert_addr(comptr->m_region);

				//mser_region* r = (mser_region*)(((u8*)pinfo.m_mser_regions.m_begin) + ((u8*)comptr->m_region - (u8*)pinfo.m_mser_regions.m_begin));
				//mser_region* r2 = pinfo.m_mser_regions.m_begin + (comptr->m_region - pinfo.m_mser_regions.m_begin);

				//
				//pinfo.m_mser_regions.assert_addr(pinfo.m_mser_regions.m_begin + (comptr->m_region - pinfo.m_mser_regions.m_begin));

				//if (pinfo.m_mser_regions.m_reuse_number > 0) { 
				//	cur_region = pinfo.m_mser_regions.m_begin + pinfo.m_mser_regions.m_reuse_indexes[pinfo.m_mser_regions.m_reuse_number - 1]; 
				//} else { 
				//	cur_region = pinfo.m_mser_regions.m_cur_data; 
				//}

				//pinfo.m_mser_regions.assert_addr(cur_region);

				//basicmath_block_memory_get(pinfo.m_mser_regions, cur_region, mser_region);

				reused = sys_true;
			}

			heap_cur++;
			i16 pixel_val = 0;
			for ( i32 i = ((*imgptr)&0x01ff)+1; i < 257; i++ ) {
				if ( **heap_cur ) {
					pixel_val = i;
					break;
				}

				heap_cur++;
			}

			if ( pixel_val ) {
				imgptr = **heap_cur;
				(*heap_cur)--;

				if (pixel_val < comptr[-1].m_gray_level) {
					// check the stablity and push a new history, increase the gray level
					basicmath_block_memory_get(pinfo.m_mser_regions, cur_region, mser_region);
					basicmath_block_memory_add(pinfo.m_mser_regions);

					if (comptr->m_region != cur_region) {
						comptr->m_region->m_parent = cur_region;
						cur_region->m_boundary_region = comptr->m_region->m_boundary_region;
					}

					comptr[0].m_gray_level = pixel_val;
					new_region(comptr, cur_region, patch_index);
					
					mser_number += 1;
				} else {
					//basiclog_assert2(pixel_val == comptr[-1].m_gray_level);

					// keep merging top two comp in stack until the grey level >= pixel_val
					for ( ; ; ) {
						//BASICML_ASSERT(pixel_val <= comptr[-1].m_gray_level);


						comptr--;
						merge_comp(comptr+1, comptr);

						if ( pixel_val <= comptr[0].m_gray_level) {
							break;
						}
						
						//Notice that it seems here never can be reached!
						//BASICML_ASSERT(pixel_val <= comptr[-1].m_gray_level);

						if (pixel_val < comptr[-1].m_gray_level) {
							// check the stablity here otherwise it wouldn't be an ER
							basicmath_block_memory_get(pinfo.m_mser_regions, cur_region, mser_region);
							basicmath_block_memory_add(pinfo.m_mser_regions);

							if (comptr->m_region != cur_region) {
								comptr->m_region->m_parent = cur_region;
								cur_region->m_boundary_region = comptr->m_region->m_boundary_region;
							}

							comptr[0].m_gray_level = pixel_val;
							new_region(comptr, cur_region, patch_index);

							mser_number += 1;
							break;
						}
					}
				}
			} else
				break;
		}
	}

	//i32 sum = 0;

	//for (i32 i = 0; i < pinfo.m_mser_regions.m_element_number; ++i) {
	//	if (pinfo.m_mser_regions.at(i).m_parent != NULL) {
	//		sum += pinfo.m_mser_regions.at(i).m_parent->m_size;
	//	}
	//}

	//basiclog_info2(sum);

	pinfo.m_er_number = pinfo.m_mser_regions.m_element_number;
	pinfo.m_linked_points_end = ptsptr;

	timer.end();

	//mser_region* cur_region = pinfo.m_mser_regions;
	//for (; cur_region != pinfo.m_mser_regions_end; ++cur_region) {
	//	basiclog_assert2(cur_region->m_merged_has_child == 0);
	//	basiclog_assert2(cur_region->m_merged_parent_changed == 0);
	//}

	//i32 nullParentCount = 0;

	//for (i32 i = 0; i < erCount; ++i) {
	//	i32 m_size = startRegion[i].m_size;
	//	if (NULL == startRegion[i].m_parent)	{
	//		++nullParentCount;
	//	} 
	//}

	//BASICML_ASSERT(1 == nullParentCount);
	//BASICML_ASSERT(erCount == region - startRegion);

	basiclog_info2(sys_strcombine()<<"er number "<<erCount << " region number " << (pinfo.m_er_number) << " mser number " << mser_number);
}

void img_fast_mser_v1::process_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask) {
	sys_timer timer("process_tree_patch");
	timer.begin();
	
	const u8* img_data = img.ptr<u8>(pinfo.m_top, pinfo.m_left, 0);
	u32 img_cpt = img.step()[0] - pinfo.m_width;
	i16* masked_image_data = pinfo.m_masked_image_data;
	u32* level_size = pinfo.m_level_size;

	i32 masked_image_width = pinfo.m_width_with_boundary;
	i32 row;
	i32 col;
	u8 gray = 0;

	if (gray_mask == 0 || !m_from_min_max[0]) {
		memset(pinfo.m_level_size, 0, sizeof(u32) * 257);

		//set the first row to be -1
		for (col = 0; col < masked_image_width; ++col) {
			*masked_image_data++ = -1;
		}

		i16 left_mask = (pinfo.m_boundary_regions[0] == NULL) ? 0 : Boundary_Yes_Mask;
		i16 right_mask = (pinfo.m_boundary_regions[1] == NULL) ? 0 : Boundary_Yes_Mask;
		i16 top_mask = (pinfo.m_boundary_regions[2] == NULL) ? 0 : Boundary_Yes_Mask;
		i16 bottom_mask = (pinfo.m_boundary_regions[3] == NULL) ? 0 : Boundary_Yes_Mask;

		i16 top_left_combined_mask = left_mask | top_mask;

		if (pinfo.m_width == 1) {
			top_left_combined_mask |= right_mask;
		}

		if (pinfo.m_height == 1) {
			top_left_combined_mask |= bottom_mask;
		}

		i16 left_combined_mask = left_mask;

		if (pinfo.m_width == 1) {
			left_combined_mask |= right_mask;
		}

		i16 top_combined_mask = top_mask;

		if (pinfo.m_height == 1) {
			top_combined_mask |= bottom_mask;
		}

		if (mask.m_mask_type == img_Mask_Type_Null) {
			if (gray_mask == 0) {
				//set middle row
				//first real image row
				*masked_image_data++ = -1;

				//top-left
				level_size[*img_data]++;
				*masked_image_data++ = (*img_data++ | top_left_combined_mask);

				//top
				for (col = 1; col < pinfo.m_width - 1; ++col) {
					level_size[*img_data]++;
					*masked_image_data++ = (*img_data++ | top_combined_mask);
				}

				if (pinfo.m_width > 1) {
					//top-right
					level_size[*img_data]++;
					*masked_image_data++ = (*img_data++ | (right_mask | top_combined_mask));
				}

				//boundary
				*masked_image_data++ = -1;
				img_data += img_cpt;

				for (row = 1; row < pinfo.m_height - 1; ++row) {
					*masked_image_data++ = -1;

					//first real image col
					level_size[*img_data]++;

					*masked_image_data++ = (*img_data++ | left_combined_mask);

					for (col = 1; col < pinfo.m_width - 1; ++col) {
						level_size[*img_data]++;
						*masked_image_data++ = *img_data++;
					}

					if (pinfo.m_width > 1) {
						//last real image col
						level_size[*img_data]++;
						*masked_image_data++ = (*img_data++ | right_mask);
					}

					*masked_image_data++ = -1;
					img_data += img_cpt;
				}

				if (pinfo.m_height > 1) {
					//last real image row
					*masked_image_data++ = -1;

					//bottom-left
					level_size[*img_data]++;

					*masked_image_data++ = (*img_data++ | (bottom_mask | left_combined_mask));

					//bottom
					for (col = 1; col < pinfo.m_width - 1; ++col) {
						level_size[*img_data]++;
						*masked_image_data++ = (*img_data++ | bottom_mask);
					}

					if (pinfo.m_width > 1) {
						//bottom-right
						level_size[*img_data]++;
						*masked_image_data++ = (*img_data++ | (right_mask | bottom_mask));
					}

					//boundary
					*masked_image_data++ = -1;
					img_data += img_cpt;
				}
			} else {
				//set middle row
				//first real image row
				*masked_image_data++ = -1;

				gray = 0xff - *img_data++;

				//top-left
				level_size[gray]++;
				*masked_image_data++ = (gray | top_left_combined_mask);

				//top
				for (col = 1; col < pinfo.m_width - 1; ++col) {
					gray = 0xff - *img_data++;

					level_size[gray]++;
					*masked_image_data++ = (gray | top_combined_mask);
				}

				if (pinfo.m_width > 1) {
					//top-right
					gray = 0xff - *img_data++;

					level_size[gray]++;
					*masked_image_data++ = (gray | (right_mask | top_combined_mask));
				}

				//boundary
				*masked_image_data++ = -1;
				img_data += img_cpt;

				for (row = 1; row < pinfo.m_height - 1; ++row) {
					*masked_image_data++ = -1;

					//first real image col
					gray = 0xff - *img_data++;
					level_size[gray]++;

					*masked_image_data++ = (gray | left_combined_mask);

					for (col = 1; col < pinfo.m_width - 1; ++col) {
						gray = 0xff - *img_data++;
						level_size[gray]++;
						*masked_image_data++ = gray;
					}

					if (pinfo.m_width > 1) {
						//last real image col
						gray = 0xff - *img_data++;
						level_size[gray]++;
						*masked_image_data++ = (gray | right_mask);
					}

					*masked_image_data++ = -1;
					img_data += img_cpt;
				}

				if (pinfo.m_height > 1) {
					//last real image row
					*masked_image_data++ = -1;

					//bottom-left
					gray = 0xff - *img_data++;
					level_size[gray]++;

					*masked_image_data++ = (gray | (bottom_mask | left_combined_mask));

					//bottom
					for (col = 1; col < pinfo.m_width - 1; ++col) {
						gray = 0xff - *img_data++;
						level_size[gray]++;
						*masked_image_data++ = (gray | bottom_mask);
					}

					if (pinfo.m_width > 1) {
						//bottom-right
						gray = 0xff - *img_data++;
						level_size[gray]++;
						*masked_image_data++ = (gray | (right_mask | bottom_mask));
					}

					//boundary
					*masked_image_data++ = -1;
					img_data += img_cpt;
				}
			}
		}

		//set the last row to be -1
		for (col = 0; col < masked_image_width; ++col) {
			*masked_image_data++ = -1;
		}
	} else {
		masked_image_data += masked_image_width + 1;
		
		i16 boundary_flag = 0;
		//set the first row to be -1
		if (mask.m_mask_type == img_Mask_Type_Null) {
			//set middle row
			for (row = 0; row < pinfo.m_height; ++row) {
				for (col = 0; col < pinfo.m_width; ++col) {
					boundary_flag = *masked_image_data & Boundary_Yes_Mask;
					gray = 0xff - *masked_image_data & 0x00ff;
					*masked_image_data = gray | boundary_flag;
					masked_image_data++;
				}

				masked_image_data += 2;
			}
		}

		i32 temp;
		for(i32 i = 0; i < 128; ++i) {
			temp = level_size[i];
			level_size[i] = level_size[255 - i];
			level_size[255 - i] = temp;
		}
	}

	pinfo.m_heap_start[0][0] = 0;
	for ( i32 i = 1; i < 257; i++ ) {
		pinfo.m_heap_start[i] = pinfo.m_heap_start[i-1]+level_size[i-1]+1;
		pinfo.m_heap_start[i][0] = 0;
	}

	timer.end();
}

void img_fast_mser_v1::init_comp(connected_comp* comptr, mser_region* region, u8 patch_index) {
	comptr->m_size = 0;

	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_size = 0;
	region->m_unmerged_size = 0;
	region->m_parent = NULL;
	region->m_calculated_var = 0;
	region->m_boundary_region = 0;
	region->m_patch_index = patch_index;
	comptr->m_region = region;	
}

void img_fast_mser_v1::new_region(connected_comp* comptr, mser_region* region, u8 patch_index) {
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_size = 0;
	region->m_unmerged_size = 0;
	region->m_parent = NULL;
	region->m_calculated_var = 0;
	region->m_patch_index = patch_index;
	
	comptr->m_region = region;
}

void img_fast_mser_v1::merge_tree_parallel_4(const mt_mat& src, u8 gray_mask) {	
	m_merged_number = 0;

	// New merging processing
	//i32 gray_number[256];
	//i32 gray_integral[257];

	//// rows 2 * cols 2; First we merge align vertical direction, then horizontal direction
	//u32 v_partition_indexes[2];
	//v_partition_indexes[0] = 0;
	//v_partition_indexes[1] = 2;
	//merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 2, 1, sys_true, gray_number, gray_integral);

	//u32 h_partition_indexes[2];
	//h_partition_indexes[0] = 0;
	//h_partition_indexes[1] = 1;
	//merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 2, 2, sys_false, gray_number, gray_integral);

	i32 left_width = m_pinfo[0].m_width;
	i32 left_height = m_pinfo[0].m_height;
	i32 rows = src.size()[0];
	i32 cols = src.size()[1];
	i32 row_step = src.step()[0];
	i32 col_step = src.step()[1];

	for (i32 i = 0; i < 2; ++i) {
		i32 gray_number[256];
		i32 gray_integral[257];
		memset(gray_number, 0, 256 * sizeof(i32));

		if (i == 0) {
			i32* order_indexes = m_orderd_indexes[i];

			const u8* data = src.data() + left_width - 1;

			for (i32 row = 0; row < rows; ++row) {
				u8 g = *data;

				if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
					g = * (data + col_step);
				}

				++gray_number[g];
				
				data += row_step;
			}

			mt_helper::integral_array(gray_integral, gray_number, gray_number + 256);

			data = src.data() + left_width - 1;

			for (i32 row = 0; row < rows; ++row) {
				u8 g = *data;

				if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
					g = * (data + col_step);
				}

				order_indexes[gray_integral[g]++] = row;

				data += row_step;
			}
		} else {
			i32* order_indexes = m_orderd_indexes[i];


			const u8* data = src.ptr<u8>(left_height - 1, 0);

			for (i32 col = 0; col < cols; ++col) {
				u8 g = *data;

				if ((gray_mask == 0) ? (* (data + row_step) < g) : (* (data + row_step) > g)) {
					g = * (data + col_step);
				}

				++gray_number[g];

				data += col_step;
			}

			mt_helper::integral_array(gray_integral, gray_number, gray_number + 256);

			data = src.ptr<u8>(left_height - 1, 0);

			for (i32 col = 0; col < cols; ++col) {
				u8 g = *data;

				if ((gray_mask == 0) ? (* (data + row_step) < g) : (* (data + row_step) > g)) {
					g = * (data + col_step);
				}

				order_indexes[gray_integral[g]++] = col;

				data += col_step;
			}
		}
	}

	merge_tree_parallel_4_step_with_order(0, rows, cols);
	merge_tree_parallel_4_step_with_order(1, rows, cols);

	// Merge without sort pixels
	//merge_tree_parallel_4_step(1);
	//merge_tree_parallel_4_step(2);
	//merge_tree_parallel_4_step(3);

	basiclog_info2(sys_strcombine()<<"max merged points: "<<m_merged_point_size);
	basiclog_info2(sys_strcombine()<<"merged er number: "<<m_merged_number);
}

void img_fast_mser_v1::merge_tree_parallel_8(const mt_mat& src, u8 gray_mask) {
	m_merged_number = 0;

	i32 gray_number[256];
	i32 gray_integral[257];

	// 2 * 4
	// first we merge ERs align with vertical direction
	u32 v_partition_indexes[2];
	v_partition_indexes[0] = 0;
	v_partition_indexes[1] = 4;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 2, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 2;
	v_partition_indexes[1] = 6;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 2, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 1;
	v_partition_indexes[1] = 5;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 2, 1, sys_true, gray_number, gray_integral);

	
	u32 h_partition_indexes[4];
	h_partition_indexes[0] = 0;
	h_partition_indexes[1] = 1;
	h_partition_indexes[2] = 2;
	h_partition_indexes[3] = 3;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 4, 4, sys_false, gray_number, gray_integral);

	basiclog_info2(sys_strcombine()<<"merged er number: "<<m_merged_number);
}

void img_fast_mser_v1::merge_tree_parallel_16(const mt_mat& src, u8 gray_mask) {
	m_merged_number = 0;

	i32 gray_number[256];
	i32 gray_integral[257];

	u32 v_partition_indexes[4];
	v_partition_indexes[0] = 0;
	v_partition_indexes[1] = 4;
	v_partition_indexes[2] = 8;
	v_partition_indexes[3] = 12;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 2;
	v_partition_indexes[1] = 6;
	v_partition_indexes[2] = 10;
	v_partition_indexes[3] = 14;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 1;
	v_partition_indexes[1] = 5;
	v_partition_indexes[2] = 9;
	v_partition_indexes[3] = 13;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	u32 h_partition_indexes[4];
	h_partition_indexes[0] = 0;
	h_partition_indexes[1] = 1;
	h_partition_indexes[2] = 2;
	h_partition_indexes[3] = 3;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 4, 4, sys_false, gray_number, gray_integral);

	h_partition_indexes[0] = 8;
	h_partition_indexes[1] = 9;
	h_partition_indexes[2] = 10;
	h_partition_indexes[3] = 11;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 4, 4, sys_false, gray_number, gray_integral);

	h_partition_indexes[0] = 4;
	h_partition_indexes[1] = 5;
	h_partition_indexes[2] = 6;
	h_partition_indexes[3] = 7;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 4, 4, sys_false, gray_number, gray_integral);
	basiclog_info2(sys_strcombine()<<"merged er number: "<<m_merged_number);
}


void img_fast_mser_v1::merge_tree_parallel_32(const mt_mat& src, u8 gray_mask) {
	m_merged_number = 0;

	i32 gray_number[256];
	i32 gray_integral[257];

	u32 v_partition_indexes[4];
	v_partition_indexes[0] = 0;
	v_partition_indexes[1] = 8;
	v_partition_indexes[2] = 16;
	v_partition_indexes[3] = 24;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 2;
	v_partition_indexes[1] = 10;
	v_partition_indexes[2] = 18;
	v_partition_indexes[3] = 26;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 4;
	v_partition_indexes[1] = 12;
	v_partition_indexes[2] = 20;
	v_partition_indexes[3] = 28;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 6;
	v_partition_indexes[1] = 14;
	v_partition_indexes[2] = 22;
	v_partition_indexes[3] = 30;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);


	v_partition_indexes[0] = 1;
	v_partition_indexes[1] = 9;
	v_partition_indexes[2] = 17;
	v_partition_indexes[3] = 25;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);


	v_partition_indexes[0] = 5;
	v_partition_indexes[1] = 13;
	v_partition_indexes[2] = 21;
	v_partition_indexes[3] = 29;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	v_partition_indexes[0] = 3;
	v_partition_indexes[1] = 11;
	v_partition_indexes[2] = 19;
	v_partition_indexes[3] = 27;

	merge_tree_parallel_step(src, gray_mask, v_partition_indexes, 4, 1, sys_true, gray_number, gray_integral);

	u32 h_partition_indexes[8];
	h_partition_indexes[0] = 0;
	h_partition_indexes[1] = 1;
	h_partition_indexes[2] = 2;
	h_partition_indexes[3] = 3;
	h_partition_indexes[4] = 4;
	h_partition_indexes[5] = 5;
	h_partition_indexes[6] = 6;
	h_partition_indexes[7] = 7;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 8, 8, sys_false, gray_number, gray_integral);

	h_partition_indexes[0] = 16;
	h_partition_indexes[1] = 17;
	h_partition_indexes[2] = 18;
	h_partition_indexes[3] = 19;
	h_partition_indexes[4] = 20;
	h_partition_indexes[5] = 21;
	h_partition_indexes[6] = 22;
	h_partition_indexes[7] = 23;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 8, 8, sys_false, gray_number, gray_integral);

	h_partition_indexes[0] = 8;
	h_partition_indexes[1] = 9;
	h_partition_indexes[2] = 10;
	h_partition_indexes[3] = 11;
	h_partition_indexes[4] = 12;
	h_partition_indexes[5] = 13;
	h_partition_indexes[6] = 14;
	h_partition_indexes[7] = 15;

	merge_tree_parallel_step(src, gray_mask, h_partition_indexes, 8, 8, sys_false, gray_number, gray_integral);
	basiclog_info2(sys_strcombine()<<"merged er number: "<<m_merged_number);
}

void img_fast_mser_v1::merge_tree_parallel_2(const mt_mat& src, u8 gray_mask) {	
	m_merged_number = 0;

	i32 left_width = m_pinfo[0].m_width;
	i32 left_height = m_pinfo[0].m_height;
	i32 rows = src.size()[0];
	i32 cols = src.size()[1];
	i32 row_step = src.step()[0];
	i32 col_step = src.step()[1];

	i32 gray_number[256];
	i32 gray_integral[257];
	memset(gray_number, 0, 256 * sizeof(i32));

	i32* order_indexes = m_orderd_indexes[0];

	const u8* data = src.data() + left_width - 1;

	for (i32 row = 0; row < rows; ++row) {
		u8 g = *data;
		if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
			g = * (data + col_step);
		}

		++gray_number[g];

		data += row_step;
	}

	mt_helper::integral_array(gray_integral, gray_number, gray_number + 256);

	data = src.data() + left_width - 1;

	for (i32 row = 0; row < rows; ++row) {
		u8 g = *data;

		if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
			g = * (data + col_step);
		}

		order_indexes[gray_integral[g]++] = row;

		data += row_step;
	}

	merge_tree_parallel_2_step_with_order(rows, cols);

	basiclog_info2(sys_strcombine()<<"max merged points: "<<m_merged_point_size);

	basiclog_info2(sys_strcombine()<<"merged er number: "<<m_merged_number);
}



void img_fast_mser_v1::merge_tree_parallel_2_step_with_order(i32 rows, i32 cols) {
	mser_region** top_a_base = &m_pinfo[0].m_boundary_regions[1][0];
	mser_region** top_b_base = &m_pinfo[1].m_boundary_regions[0][0];

	mser_region* a;
	mser_region* b;
	i32 half_height = m_pinfo[0].m_height;
	i32* ordered_indexes = m_orderd_indexes[0];
	u32 split_patch_index = 0;

	for (i32 i = 0; i < rows; ++i) {
		i32 index = ordered_indexes[i];
		a = top_a_base[index];
		b = top_b_base[index];

		//basiclog_info2(sys_strcombine()<<"min gray "<<a->m_gray_level<<" "<< b->m_gray_level);

		get_real_for_merged(a);
		get_real_for_merged(b);

		connect(a, b, split_patch_index);
	}
}

void img_fast_mser_v1::merge_tree_parallel_4_step_with_order(u8 merged_flag, i32 rows, i32 cols) {
	if (merged_flag == 0) {
		mser_region** top_a_base = &m_pinfo[0].m_boundary_regions[1][0];
		mser_region** top_b_base = &m_pinfo[1].m_boundary_regions[0][0];
		mser_region** bottom_a_base = &m_pinfo[2].m_boundary_regions[1][0];
		mser_region** bottom_b_base = &m_pinfo[3].m_boundary_regions[0][0];

		mser_region* a;
		mser_region* b;
		i32 half_height = m_pinfo[0].m_height;
		i32* ordered_indexes = m_orderd_indexes[0];
		u32 split_patch_index;

		for (i32 i = 0; i < rows; ++i) {
			i32 index = ordered_indexes[i];
			if (index < half_height) {
				a = top_a_base[index];
				b = top_b_base[index];

				//basiclog_info2(sys_strcombine()<<"min gray "<<a->m_gray_level<<" "<< b->m_gray_level);

				split_patch_index = 0;
			} else {
				a = bottom_a_base[index - half_height];
				b = bottom_b_base[index - half_height];
				split_patch_index = 2;
			}

			get_real_for_merged(a);
			get_real_for_merged(b);

			connect(a, b, split_patch_index);
		}
	} else {
		mser_region** left_a_base = &m_pinfo[0].m_boundary_regions[3][0];
		mser_region** left_b_base = &m_pinfo[2].m_boundary_regions[2][0];
		mser_region** right_a_base = &m_pinfo[1].m_boundary_regions[3][0];
		mser_region** right_b_base = &m_pinfo[3].m_boundary_regions[2][0];

		mser_region* a;
		mser_region* b;
		i32 half_width = m_pinfo[0].m_width;
		i32* ordered_indexes = m_orderd_indexes[1];
		u32 split_patch_index = 1;

		for (i32 i = 0; i < cols; ++i) {
			i32 index = ordered_indexes[i];
			if (index < half_width) {
				a = left_a_base[index];
				b = left_b_base[index];
			} else {
				a = right_a_base[index - half_width];
				b = right_b_base[index - half_width];
			}

			get_real_for_merged(a);
			get_real_for_merged(b);

			connect(a, b, split_patch_index);
		}
	}
}

void img_fast_mser_v1::merge_tree_parallel_4_step(u8 merged_flag) {
	if (merged_flag == 1 || merged_flag == 2) {
		u32 left_parallel_index = 0;
		u32 right_parallel_index = 1;

		if (merged_flag == 2) {
			left_parallel_index = 2;
			right_parallel_index = 3;
		}

		//top left and top right
		mser_region** a_base = &m_pinfo[left_parallel_index].m_boundary_regions[1][0];
		mser_region** b_base = &m_pinfo[right_parallel_index].m_boundary_regions[0][0];
		mser_region* a;
		mser_region* b;
		u32 split_region_index = left_parallel_index;

		for (i32 i = 0; i < m_pinfo[left_parallel_index].m_height; ++i) {
			a = *a_base;
			b = *b_base;

			get_real_for_merged(a);
			get_real_for_merged(b);
			
			connect(a, b, split_region_index);
			
			++a_base;
			++b_base;
		}
	} else {
		//whole
		u32 top_parallel_index = 0;
		u32 bottom_parallel_index = 2;

		mser_region** top_link_points_base;
		mser_region** bottom_link_points_base;
		mser_region* a;
		mser_region* b;

		u32 split_region_index = 1;

		for (u32 p = 0; p < 2; ++p) {
			top_parallel_index += p;
			bottom_parallel_index += p;

			top_link_points_base = &m_pinfo[top_parallel_index].m_boundary_regions[3][0];
			bottom_link_points_base = &m_pinfo[bottom_parallel_index].m_boundary_regions[2][0];

			for (i32 i = 0; i < m_pinfo[top_parallel_index].m_width; ++i) {
				a = *top_link_points_base;
				b = *bottom_link_points_base;

				get_real_for_merged(a);
				get_real_for_merged(b);

				connect(a, b, split_region_index);

				++top_link_points_base;
				++bottom_link_points_base;
			}
		}
	}
}

void img_fast_mser_v1::merge_tree_parallel_step(const mt_mat& src, u8 gray_mask, u32* parallel_indexes, i32 size, u32 parallel_step, b8 left_right, i32* gray_number, i32* gray_integral) {
	memset(gray_number, 0, 256 * sizeof(i32));

	i32 rows = src.size()[0];
	i32 cols = src.size()[1];
	i32 row_step = src.step()[0];
	i32 col_step = src.step()[1];

	mser_region** a_base;
	mser_region** b_base;
	mser_region* a;
	mser_region* b;

	u32 split_patch_index;

	if (left_right) {
		// left right merge
		// first we sort pixels

		i32* order_indexes = m_orderd_indexes[0];

		const u8* base = src.data() + m_pinfo[parallel_indexes[0]].m_left + m_pinfo[parallel_indexes[0]].m_width - 1;
		const u8* data = base;

		for (i32 row = 0; row < rows; ++row) {
			u8 g = *data;

			if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
				g = * (data + col_step);
			}

			++gray_number[g];

			data += row_step;
		}

		mt_helper::integral_array(gray_integral, gray_number, gray_number + 256);

		data = base;

		for (i32 row = 0; row < rows; ++row) {
			u8 g = *data;

			if ((gray_mask == 0) ? (* (data + col_step) < g) : (* (data + col_step) > g)) {
				g = * (data + col_step);
			}

			order_indexes[gray_integral[g]++] = row;

			data += row_step;
		}

		/// next we merge

		i32 p_height = m_pinfo[parallel_indexes[0]].m_height;
		
		for (i32 i = 0; i < rows; ++i) {
			i32 index = order_indexes[i];
			i32 v_partition_index = index / p_height;

			if (v_partition_index >= size - 1) {
				v_partition_index = size - 1;
			}

			i32 v_partition_inner_row = index - v_partition_index * p_height;
			
			a_base = &m_pinfo[parallel_indexes[v_partition_index]].m_boundary_regions[1][0];
			b_base = &m_pinfo[parallel_indexes[v_partition_index] + parallel_step].m_boundary_regions[0][0];

			a = a_base[v_partition_inner_row];
			b = b_base[v_partition_inner_row];

			split_patch_index = (u32)v_partition_index;

			get_real_for_merged(a);
			get_real_for_merged(b);

			connect(a, b, split_patch_index);
		}
	} else {
		// left right merge
		// first we sort pixels

		i32* order_indexes = m_orderd_indexes[1];


		const u8* base = src.ptr<u8>(m_pinfo[parallel_indexes[0]].m_top + m_pinfo[parallel_indexes[0]].m_height - 1, 0);
		const u8* data = base;

		for (i32 col = 0; col < cols; ++col) {
			u8 g = *data;

			if ((gray_mask == 0) ? (* (data + row_step) < g) : (* (data + row_step) > g)) {
				g = * (data + col_step);
			}

			++gray_number[g];

			data += col_step;
		}

		mt_helper::integral_array(gray_integral, gray_number, gray_number + 256);

		data = base;

		for (i32 col = 0; col < cols; ++col) {
			u8 g = *data;

			if ((gray_mask == 0) ? (* (data + row_step) < g) : (* (data + row_step) > g)) {
				g = * (data + col_step);
			}

			order_indexes[gray_integral[g]++] = col;

			data += col_step;
		}

		i32 p_width = m_pinfo[parallel_indexes[0]].m_width;

		split_patch_index = parallel_indexes[size - 1];

		for (i32 i = 0; i < cols; ++i) {
			i32 index = order_indexes[i];
			i32 v_partition_index = index / p_width;

			if (v_partition_index >= size - 1) {
				v_partition_index = size - 1;
			}

			i32 v_partition_inner_col = index - v_partition_index * p_width;

			a_base = &m_pinfo[parallel_indexes[v_partition_index]].m_boundary_regions[3][0];
			b_base = &m_pinfo[parallel_indexes[v_partition_index] + parallel_step].m_boundary_regions[2][0];

			a = a_base[v_partition_inner_col];
			b = b_base[v_partition_inner_col];

			get_real_for_merged(a);
			get_real_for_merged(b);

			connect(a, b, split_patch_index);
		}
	}
}

void img_fast_mser_v1::connect(mser_region* bigger, mser_region* smaller, u32 split_patch_index) {
	if (bigger == smaller) {
		return;
	}

	mser_region* last_break_regions[2] = {NULL, NULL};	//

#define region_index(r) ((r->m_patch_index <= split_patch_index) ? 0 : 1)

	mser_region* swap_temp;
	mser_region* bigger_parent;
	mser_region* smaller_parent;
	b8 changed = sys_false;

	for (;;) {
		if (smaller == NULL) {
			basiclog_assert2(bigger == NULL);
			break;
		}

		if (bigger != NULL && smaller->m_gray_level > bigger->m_gray_level) {
			swap_temp = bigger;
			bigger = smaller;
			smaller = swap_temp;
		}

		get_set_real_parent_for_merged(smaller, smaller_parent);

		if (smaller_parent == bigger && !changed) {
			break;
		}

		if (bigger == NULL) {
			if (smaller_parent != NULL && last_break_regions[region_index(smaller)] == smaller) {
				disconnect(smaller_parent);
				last_break_regions[region_index(smaller_parent)] = smaller_parent;

				m_linked_points[smaller_parent->m_tail].m_next = smaller->m_head;
				m_linked_points[smaller->m_head].m_prev = smaller_parent->m_tail;

				smaller_parent->m_tail = smaller->m_tail;
				
				smaller_parent->m_size += smaller->m_size;
				smaller_parent->m_unmerged_size = smaller_parent->m_size;
				changed = sys_true;

				bound_pointer_adjust(smaller_parent, smaller);
			}

			smaller = smaller_parent;
		} else {
			get_set_real_parent_for_merged(bigger, bigger_parent);

			if (smaller->m_gray_level == bigger->m_gray_level) {
				smaller->m_region_flag = mser_region::Flag_Merged;
				m_merged_number += 1;

				if (last_break_regions[region_index(smaller)] != smaller) {
					disconnect(smaller);
					last_break_regions[region_index(smaller)] = smaller;
				}

				if (last_break_regions[region_index(bigger)] != bigger) {
					disconnect(bigger);
					last_break_regions[region_index(bigger)] = bigger;
				}

				bigger->m_size += smaller->m_size;
				bigger->m_unmerged_size = bigger->m_size;

				m_linked_points[bigger->m_tail].m_next = smaller->m_head;
				m_linked_points[smaller->m_head].m_prev = bigger->m_tail;
				bigger->m_tail = smaller->m_tail;

				smaller->m_parent = bigger;

				bound_pointer_adjust(bigger, smaller);

				smaller = bigger;
				bigger = smaller_parent;
				changed = sys_true;
			} else if (smaller_parent == NULL || smaller_parent->m_gray_level > bigger->m_gray_level) {
				if (last_break_regions[region_index(smaller)] != smaller) {
					disconnect(smaller);
					last_break_regions[region_index(smaller)] = smaller;
				}

				if (last_break_regions[region_index(bigger)] != bigger) {
					disconnect(bigger);
					last_break_regions[region_index(bigger)] = bigger;
				}

				bigger->m_size += smaller->m_size;
				bigger->m_unmerged_size = bigger->m_size;

				m_linked_points[bigger->m_tail].m_next = smaller->m_head;
				m_linked_points[smaller->m_head].m_prev = bigger->m_tail;
				bigger->m_tail = smaller->m_tail;

				smaller->m_parent = bigger;

				bound_pointer_adjust(bigger, smaller);

				smaller = bigger;
				bigger = smaller_parent;
				changed = sys_true;
			} else {
				if (smaller_parent != NULL && last_break_regions[region_index(smaller)] == smaller) {
					disconnect(smaller_parent);
					last_break_regions[region_index(smaller_parent)] = smaller_parent;
					
					m_linked_points[smaller_parent->m_tail].m_next = smaller->m_head;
					m_linked_points[smaller->m_head].m_prev = smaller_parent->m_tail;
					smaller_parent->m_tail = smaller->m_tail;

					smaller_parent->m_size += smaller->m_size;
					smaller_parent->m_unmerged_size = smaller_parent->m_size;

					changed = sys_true;

					bound_pointer_adjust(smaller_parent, smaller);
				}

				smaller = smaller_parent;
			}

			if (smaller == bigger) {
				bigger = NULL;
			}
		}
	}

	for (i32 i = 0; i < (i32)m_merged_point_index; ++i) {
		m_linked_points[m_merged_points[i]].m_ref = -1;
	}

	m_merged_point_index = 0;
}



void img_fast_mser_v1::disconnect(mser_region* r) {
	i32 real_tail;
	get_real_tail(r->m_tail, real_tail);

	if (m_linked_points[r->m_head].m_prev != -1) {
		m_linked_points[m_linked_points[r->m_head].m_prev].m_next = m_linked_points[real_tail].m_next;
	}

	if (m_linked_points[real_tail].m_next != -1) {
		m_linked_points[m_linked_points[real_tail].m_next].m_prev = m_linked_points[r->m_head].m_prev;
	}

	mser_region* parent;
	get_set_real_parent_for_merged(r, parent);

	if (parent != NULL) {		
		i32 parent_real_tail;
		get_real_tail(parent->m_tail, parent_real_tail);

		parent->m_size -= r->m_unmerged_size;

		if (r->m_tail == parent->m_tail || parent_real_tail == real_tail) {
			m_linked_points[real_tail].m_ref = m_linked_points[r->m_head].m_prev;
			m_merged_points[m_merged_point_index++] = real_tail;

			if (m_merged_point_index == m_merged_point_size) {
				i32 new_merged_point_size = m_merged_point_size * 2;
				i32* new_merged_points = (i32*)malloc(new_merged_point_size * sizeof(i32));
				memcpy(new_merged_points, m_merged_points, sizeof(i32) * m_merged_point_size);
				free(m_merged_points);

				m_merged_point_size = new_merged_point_size;
				m_merged_points = new_merged_points;
			}
		}
	}

	r->m_tail = real_tail;
	m_linked_points[r->m_head].m_prev = -1;
	m_linked_points[r->m_tail].m_next = -1;
}

void img_fast_mser_v1::recognize_mser() {
	recognize_mser_parallel();
}

void img_fast_mser_v1::recognize_mser_parallel() {
	u32 totalUnkonwSize = 0;
	i32 bad_variance_number = 0;

	i32 tn = 0;
	i32 nms_count = 0;

	if (m_parallel_thread_number == 1) {
		recognize_mser_parallel_worker(0);
	
	} else if (m_parallel_thread_number == 2) {
#pragma omp parallel for num_threads(2)
		for (i32 i = 0; i < m_parallel_thread_number; ++i) {
			recognize_mser_parallel_worker(i);
		}
	} else if (m_parallel_thread_number == 4) {
#pragma omp parallel for num_threads(4)
		for (i32 i = 0; i < m_parallel_thread_number; ++i) {
			recognize_mser_parallel_worker(i);
		}
	} else if (m_parallel_thread_number == 8) {
#pragma omp parallel for num_threads(8)
		for (i32 i = 0; i < m_parallel_thread_number; ++i) {
			recognize_mser_parallel_worker(i);
		}
	} else if (m_parallel_thread_number == 16) {
#pragma omp parallel for num_threads(16)
		for (i32 i = 0; i < m_parallel_thread_number; ++i) {
			recognize_mser_parallel_worker(i);
		}
	} else if (m_parallel_thread_number == 32) {
#pragma omp parallel for num_threads(32)
		for (i32 i = 0; i < m_parallel_thread_number; ++i) {
			recognize_mser_parallel_worker(i);
		}
	}

	//basiclog_info2(sys_strcombine()<<"bad_variance_number "<<bad_variance_number);

	//sys_timer nms("nms");
	//nms.begin();

	mser_region* parent_region;

	memset(m_region_level_size, 0, sizeof(u32) * 257);

	for (i32 i = 0; i < (int)m_pinfo.size(); ++i) {

		mt_block_memory<mser_region>::block_pos bp;
		for (;;) {
			basicmath_block_memory_visit(m_pinfo[i].m_mser_regions, bp);

			if (bp.m_data->m_region_flag == mser_region::Flag_Merged) {
				continue;
			}

			parent_region = bp.m_data->m_parent;

			if (bp.m_data->m_region_flag == mser_region::Flag_Unknow) {
				basiclog_assert2(bp.m_data->m_calculated_var == 0);
				m_region_level_size[bp.m_data->m_gray_level]++;
				totalUnkonwSize++;
				bp.m_data->m_calculated_var = 1;
			} else if (parent_region == NULL || parent_region->m_region_flag == mser_region::Flag_Invalid) {
				continue;
			}

			basiclog_assert2(parent_region->m_region_flag != mser_region::Flag_Merged);

			if (m_nms_similarity >= 0 && bp.m_data->m_var >= 0 && parent_region->m_var >= 0 && parent_region->m_gray_level == bp.m_data->m_gray_level + 1) {
				double subValue = parent_region->m_var - bp.m_data->m_var;
				if (subValue > m_nms_similarity) {
					if (mser_region::Flag_Unknow == parent_region->m_region_flag) {
						if (parent_region->m_calculated_var == 1) {
							--m_region_level_size[parent_region->m_gray_level];
							--totalUnkonwSize;
						}

						parent_region->m_region_flag = mser_region::Flag_Invalid;
					}
				} else if (-subValue > m_nms_similarity) {
					if (mser_region::Flag_Unknow == bp.m_data->m_region_flag) {
						if (bp.m_data->m_calculated_var == 1) {
							--m_region_level_size[bp.m_data->m_gray_level];
							--totalUnkonwSize;
						}

						bp.m_data->m_region_flag = mser_region::Flag_Invalid;
					}
				} 
			}
		}
	}

	//order mser region
	mt_helper::integral_array<u32>(m_start_indexes, m_region_level_size, m_region_level_size + 256);

	basiclog_assert2(m_start_indexes[256] == totalUnkonwSize);


	m_gray_order_region_size = totalUnkonwSize;
	m_gray_order_regions = (mser_region**)m_heap;


	for (i32 i = 0; i < (int)m_pinfo.size(); ++i) {
		mt_block_memory<mser_region>::block_pos bp;

		for (;;) {
			basicmath_block_memory_visit(m_pinfo[i].m_mser_regions, bp);

			if (bp.m_data->m_region_flag == mser_region::Flag_Unknow) {
				m_gray_order_regions[m_start_indexes[bp.m_data->m_gray_level]++] = bp.m_data;
			}
		}
	}

	mser_region* cur_region;

	if (m_duplicated_variation > 0) {
		u32 validCount = 0;

		m_remove_duplicated_memory_helper.reserve(100);


		// Remove duplicated regions
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];

			if (cur_region->m_region_flag != mser_region::Flag_Unknow) {
				continue;
			}

			m_remove_duplicated_memory_helper.clear();
			m_remove_duplicated_memory_helper.push_back(cur_region);
			get_duplicated_regions(m_remove_duplicated_memory_helper, cur_region, cur_region);
			//m_remove_duplicated_memory_helper.pop_back();

			i32 middleIndex = (i32)m_remove_duplicated_memory_helper.size() / 2;

			if (middleIndex > 0) {
				i32 oldSize = (i32)m_remove_duplicated_memory_helper.size();
				get_duplicated_regions(m_remove_duplicated_memory_helper, m_remove_duplicated_memory_helper[middleIndex], m_remove_duplicated_memory_helper.back());
			}

			for (i32 j = 0; j < (i32)m_remove_duplicated_memory_helper.size(); ++j) {
				if (j != middleIndex) {
					m_remove_duplicated_memory_helper[j]->m_region_flag = mser_region::Flag_Invalid;
				} else {
					m_remove_duplicated_memory_helper[j]->m_region_flag = mser_region::Flag_Valid;
					++validCount;
				}
			}
		}

		u32 index = 0;
		m_channel_total_pixel_number = 0;

		for (u32 i = 0; i < totalUnkonwSize; ++i) {
			cur_region = m_gray_order_regions[i];
			if (cur_region->m_region_flag == mser_region::Flag_Valid) {
				m_channel_total_pixel_number += cur_region->m_size;
				m_gray_order_regions[index++] = cur_region;
			}
		}

		m_gray_order_region_size = validCount;
		basiclog_assert2(index == validCount);
	} else {
		m_channel_total_pixel_number = 0;
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			m_channel_total_pixel_number += cur_region->m_size;
			cur_region->m_region_flag = mser_region::Flag_Valid;
		}
	}

	basiclog_info2(sys_strcombine()<<"m_gray_order_region_size: "<<m_gray_order_region_size);
}

void img_fast_mser_v1::recognize_mser_parallel_worker(i32 parallel_index) {
	mser_region* parent_region;
	mser_region* start_region;
	mser_region* temp_region;

	mt_block_memory<mser_region>::block_pos bp;

	i32 gray_level_threshold;
	//total_er_number += m_pinfo[i].m_er_number;

	for (;;) {

		basicmath_block_memory_visit(m_pinfo[parallel_index].m_mser_regions, bp);

		if (bp.m_data->m_region_flag != mser_region::Flag_Merged) {
			basiclog_assert2(bp.m_data->m_calculated_var == 0);

			gray_level_threshold = bp.m_data->m_gray_level + m_delta;
			start_region = bp.m_data;
			get_set_real_parent_for_merged(start_region, parent_region);

			while (parent_region != NULL && (i32)parent_region->m_gray_level <= gray_level_threshold) {
				basiclog_assert2(parent_region->m_region_flag != mser_region::Flag_Merged);

				start_region = parent_region;
				if (parent_region->m_patch_index == parallel_index) {
					get_set_real_parent_for_merged(parent_region, temp_region);
				} else {
					get_real_parent_for_merged(parent_region, temp_region);
				}

				parent_region = temp_region;
			}

			if (parent_region != NULL || start_region->m_gray_level == gray_level_threshold) {
				bp.m_data->m_var = (start_region->m_size - bp.m_data->m_size) / (f32)bp.m_data->m_size;
			} else {
				bp.m_data->m_var = -1;
			}

			if (bp.m_data->m_var > m_stable_variation) {
				bp.m_data->m_region_flag = mser_region::Flag_Invalid;
				//bad_variance_number++;
			} else if (bp.m_data->m_size < m_min_point || bp.m_data->m_size > m_max_point || NULL == bp.m_data->m_parent) {
				bp.m_data->m_region_flag = mser_region::Flag_Invalid;
			}
		}
	}
}

void img_fast_mser_v1::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	if (m_gray_order_region_size <= 0) {
		return;
	}

	//extract_pixel_parallel_4_simple_parallel_impl(msers, gray_mask);
	//extract_pixel_parallel_4_simple_parallel_impl2(msers, gray_mask);
	//collect_mser_parallel_4_parallel_impl(msers, gray_mask);
	//extract_pixel_parallel_4_parallel_impl(msers, gray_mask);
	
	if (m_parallel_thread_number == 1) {
		extract_pixel_parallel_1_fast(msers, gray_mask);
	} else {
		extract_pixel_parallel_k_fast(msers, gray_mask);
	}
}

void img_fast_mser_v1::extract_pixel_parallel_4_simple_impl(img_multi_msers& msers, u8 gray_mask) {
	sys_timer t("collect_mser_parallel_4_simple_parallel_impl");
	t.begin();

	//basiclog_assert2(m_recursive_point_threshold == -1 || m_recursive_point_threshold == INT_MAX);
	m_recursive_point_threshold = INT_MAX;
	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];

	t_msers.resize(m_gray_order_region_size);
	region_memory_size = 0;

	mt_point* memory_offset;

	if (m_recursive_point_threshold == INT_MAX) {
		region_memory_size = m_channel_total_pixel_number;
		memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);

		memory_offset = memory;
	}

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		mser_region* cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;

		if (m_recursive_point_threshold == -1) {
			cur_mser.m_memory_type = img_mser::Memory_Type_Self;
			cur_mser.m_points = new mt_point[cur_region->m_size];
		} else {
			cur_mser.m_memory_type = img_mser::Memory_Type_Share;
			cur_mser.m_points = memory_offset;
			memory_offset += cur_mser.m_size;
		}

		cur_mser.m_size = cur_region->m_size;
	}

#pragma omp parallel for num_threads(4)
	for (i32 k = 0; k < 4; ++k) {
		for (u32 i = k; i < m_gray_order_region_size; i += 4) {
			mser_region* cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			i32 pt_index = cur_region->m_head;

			i32 rowMin;
			i32 colMin;
			i32 rowMax;
			i32 colMax;

			for ( i32 j = 0; j < cur_region->m_size; j++ )
			{
				linked_point& lpt = m_linked_points[pt_index];
				mt_point& pts = cur_mser.m_points[j];

				pts.m_y = lpt.m_y;
				pts.m_x = lpt.m_x;

				if (j == 0) {
					rowMin = pts.m_y;
					rowMax = pts.m_y;
					colMin = pts.m_x;
					colMax = pts.m_x;
				} else {
					if (pts.m_x > colMax) {
						colMax = pts.m_x;
					} else if (pts.m_x < colMin) {
						colMin = pts.m_x;
					}

					if (pts.m_y > rowMax) {
						rowMax = pts.m_y;
					} else if (pts.m_y < rowMin) {
						rowMin = pts.m_y;
					}
				}

				pt_index = lpt.m_next;
			}

			cur_mser.m_rect.set_rect(mt_point(colMin, rowMin), mt_point(colMax, rowMax));
		}
	}

	t.end();
}

void img_fast_mser_v1::extract_pixel_parallel_4_simple_impl2(img_multi_msers& msers, u8 gray_mask) {
	sys_timer t("collect_mser_parallel_4_simple_parallel_impl2");
	t.begin();

	//basiclog_assert2(m_recursive_point_threshold == -1 || m_recursive_point_threshold == INT_MAX);
	m_recursive_point_threshold = INT_MAX;

	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];

	t_msers.resize(m_gray_order_region_size);
	region_memory_size = 0;



	if (m_recursive_point_threshold == INT_MAX) {
		region_memory_size = m_channel_total_pixel_number;
		memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);



		i32 block_pixel_sizes[4] = {0, 0, 0, 0};

		for (i32 k = 0; k < 4; ++k) {
			for (u32 i = k; i < m_gray_order_region_size; i += 4) {
				mser_region* cur_region = m_gray_order_regions[i];
				img_mser& cur_mser = t_msers[i];

				cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;

				cur_mser.m_size = cur_region->m_size;

				block_pixel_sizes[k] += cur_mser.m_size;
			}
		}

		mt_point* memory_offset[4] = {memory, memory + block_pixel_sizes[0], memory + block_pixel_sizes[1] + block_pixel_sizes[2], memory + block_pixel_sizes[0] + block_pixel_sizes[1] + block_pixel_sizes[2]};

		for (i32 k = 0; k < 4; ++k) {
			for (u32 i = k; i < m_gray_order_region_size; i += 4) {
				img_mser& cur_mser = t_msers[i];
				cur_mser.m_memory_type = img_mser::Memory_Type_Share;
				cur_mser.m_points = memory_offset[k];
				memory_offset[k] += cur_mser.m_size;
			}
		}

	} else {
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			mser_region* cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;

			cur_mser.m_memory_type = img_mser::Memory_Type_Self;
			cur_mser.m_points = new mt_point[cur_region->m_size];

			cur_mser.m_size = cur_region->m_size;
		}
	}

#pragma omp parallel for num_threads(4)
	for (i32 k = 0; k < 4; ++k) {
		for (u32 i = k; i < m_gray_order_region_size; i += 4) {
			mser_region* cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			i32 pt_index = cur_region->m_head;

			i32 rowMin;
			i32 colMin;
			i32 rowMax;
			i32 colMax;

			for ( i32 j = 0; j < cur_region->m_size; j++ )
			{
				linked_point& lpt = m_linked_points[pt_index];
				mt_point& pts = cur_mser.m_points[j];

				pts.m_y = lpt.m_y;
				pts.m_x = lpt.m_x;

				if (j == 0) {
					rowMin = pts.m_y;
					rowMax = pts.m_y;
					colMin = pts.m_x;
					colMax = pts.m_x;
				} else {
					if (pts.m_x > colMax) {
						colMax = pts.m_x;
					} else if (pts.m_x < colMin) {
						colMin = pts.m_x;
					}

					if (pts.m_y > rowMax) {
						rowMax = pts.m_y;
					} else if (pts.m_y < rowMin) {
						rowMin = pts.m_y;
					}
				}

				pt_index = lpt.m_next;
			}

			cur_mser.m_rect.set_rect(mt_point(colMin, rowMin), mt_point(colMax, rowMax));
		}
	}

	t.end();
}


void img_fast_mser_v1::extract_pixel_parallel_1_fast(img_multi_msers& msers, u8 gray_mask) {
	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);

	mser_region** top_regions = (mser_region**)(m_heap + m_gray_order_region_size);
	i32 top_region_size = m_heap_size - m_gray_order_region_size;
	i32 top_index = 0;
	region_memory_size = 0;

	vector<mser_region*> top_backup;
	mser_region* cur_region;
	mser_region* real_region;

	sys_timer t("get top");
	t.begin();

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];

		for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
		}

		if (real_region == NULL) {
			if (top_index == top_region_size) {
				if (top_backup.capacity() == 0) {
					top_backup.reserve(100);
				}

				top_backup.push_back(cur_region);
			} else {
				top_regions[top_index] = cur_region;
			}

			++top_index;
			region_memory_size += cur_region->m_size;
		}
	}

	t.end();

	sys_timer e("extraction");
	e.begin();

	memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);
	mt_point* memory_offset = memory;

	//10000, 200, 1
	//500
	for (i32 i = 0; i < top_index; ++i) {
		if (i < top_region_size) {
			cur_region = top_regions[i];
		} else {
			cur_region = top_backup[i - top_region_size];

		}

		i32 pt_index = cur_region->m_head;

		for (i32 j = 0; j < cur_region->m_size; ++j) {
			linked_point& lpt = m_linked_points[pt_index];
			//basiclog_assert2(cur_mem < end_mem && cur_mem >= memory + memory_offsets[k]);

			memory_offset->m_x = lpt.m_x;
			memory_offset->m_y = lpt.m_y;

			lpt.m_ref = (i32)(memory_offset - memory);
			++memory_offset;


			pt_index = lpt.m_next;
		}
	}

	/*for (i32 i = 0; i < top_index; ++i) {
	if (i < top_region_size) {
	cur_region = top_regions[i];
	} else {
	cur_region = top_backup[i - top_region_size];
	}

	i32 pt_index = cur_region->m_head;

	for (i32 j = 0; j < cur_region->m_size; ++j) {
	linked_point& lpt = m_linked_points[pt_index];

	memory_offset->m_x = lpt.m_x;
	memory_offset->m_y = lpt.m_y;

	lpt.m_ref = (i32)(memory_offset - memory);
	++memory_offset;
	pt_index = lpt.m_next;
	}
	}*/

	e.end();


	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		img_mser& cur_mser = t_msers[i];
		cur_region = m_gray_order_regions[i];

		cur_mser.m_size = cur_region->m_size;
		cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
		cur_mser.m_memory_type = img_mser::Memory_Type_Range;
		
		i32* pt_range = (i32*)&cur_mser.m_points;
		pt_range[0] = m_linked_points[cur_region->m_head].m_ref;
		pt_range[1] = m_linked_points[cur_region->m_tail].m_ref;
	}
}

void img_fast_mser_v1::extract_pixel_parallel_k_fast(img_multi_msers& msers, u8 gray_mask) {
	if (m_gray_order_region_size <= 0) {
		return;
	}

	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);

	mser_region** top_regions = (mser_region**)(m_heap + m_gray_order_region_size);
	i32 top_region_size = m_heap_size - m_gray_order_region_size;
	i32 top_index = 0;
	region_memory_size = 0;

	vector<mser_region*> top_backup;
	mser_region* cur_region;
	mser_region* real_region;

	sys_timer t("get top");
	t.begin();

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];

		for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
		}

		cur_region->m_parent = real_region;

		if (real_region == NULL) {
			if (top_index == top_region_size) {
				if (top_backup.capacity() == 0) {
					top_backup.reserve(100);
				}

				top_backup.push_back(cur_region);
			} else {
				top_regions[top_index] = cur_region;
			}

			++top_index;
			region_memory_size += cur_region->m_size;
		}
	}

	t.end();

	sys_timer e("extraction");
	e.begin();

	memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);
	mt_point* memory_offset = memory;

	i32* parallel_stop_indexes = NULL;
	i32* memory_offsets = NULL;

	if (m_parallel_thread_number == 2) {
		parallel_stop_indexes = (i32*)m_start_indexes;
		memory_offsets = parallel_stop_indexes + 2;

		for (i32 i = 0; i < 4; ++i) {
			m_start_indexes[i] = 0;
		}
	} else if (m_parallel_thread_number == 4) {
		parallel_stop_indexes = (i32*)m_start_indexes;
		memory_offsets = parallel_stop_indexes + 4;

		for (i32 i = 0; i < 8; ++i) {
			m_start_indexes[i] = 0;
		}
	} else if (m_parallel_thread_number == 8) {
		parallel_stop_indexes = (i32*)m_start_indexes;
		memory_offsets = parallel_stop_indexes + 8;

		for (i32 i = 0; i < 16; ++i) {
			m_start_indexes[i] = 0;
		}
	} else if (m_parallel_thread_number == 16) {
		parallel_stop_indexes = (i32*)m_start_indexes;
		memory_offsets = parallel_stop_indexes + 16;

		for (i32 i = 0; i < 32; ++i) {
			m_start_indexes[i] = 0;
		}
	} else if (m_parallel_thread_number == 32) {
		parallel_stop_indexes = (i32*)m_start_indexes;
		memory_offsets = parallel_stop_indexes + 32;

		for (i32 i = 0; i < 64; ++i) {
			m_start_indexes[i] = 0;
		}
	}

	i32 quarter_size = region_memory_size / m_parallel_thread_number;
	i32 parallel_size = quarter_size;
	i32 cur_size = 0;
	i32 index = 0;

	//10000, 200, 1
	//500
	for (i32 i = 0; i < top_index; ++i) {
		if (i < top_region_size) {
			cur_region = top_regions[i];
		} else {
			cur_region = top_backup[i - top_region_size];

		}

		cur_size += cur_region->m_size;
		parallel_stop_indexes[index] = i;

		if (cur_size > parallel_size) {
			++index;

			if (index == m_parallel_thread_number) {
				break;
			}

			memory_offsets[index] = cur_size;			
			parallel_size += quarter_size;
		}
	}

	//parallel_size += quarter_size;

	if (m_parallel_thread_number == 2) {
#pragma omp parallel for num_threads(2)
		for (i32 k = 0; k < m_parallel_thread_number; ++k) {
			extract_top_pixel_parallel(k, parallel_stop_indexes, memory_offsets, memory, region_memory_size, top_regions, top_region_size, top_backup);
		}
	} else if (m_parallel_thread_number == 4) {
#pragma omp parallel for num_threads(4)
		for (i32 k = 0; k < m_parallel_thread_number; ++k) {
			extract_top_pixel_parallel(k, parallel_stop_indexes, memory_offsets, memory, region_memory_size, top_regions, top_region_size, top_backup);
		}
	} else if (m_parallel_thread_number == 8) {
#pragma omp parallel for num_threads(8)
		for (i32 k = 0; k < m_parallel_thread_number; ++k) {
			extract_top_pixel_parallel(k, parallel_stop_indexes, memory_offsets, memory, region_memory_size, top_regions, top_region_size, top_backup);
		}
	} else if (m_parallel_thread_number == 16) {
#pragma omp parallel for num_threads(16)
		for (i32 k = 0; k < m_parallel_thread_number; ++k) {
			extract_top_pixel_parallel(k, parallel_stop_indexes, memory_offsets, memory, region_memory_size, top_regions, top_region_size, top_backup);
		}
	} else if (m_parallel_thread_number == 32) {
#pragma omp parallel for num_threads(32)
		for (i32 k = 0; k < m_parallel_thread_number; ++k) {
			extract_top_pixel_parallel(k, parallel_stop_indexes, memory_offsets, memory, region_memory_size, top_regions, top_region_size, top_backup);
		}
	}



	/*for (i32 i = 0; i < top_index; ++i) {
	if (i < top_region_size) {
	cur_region = top_regions[i];
	} else {
	cur_region = top_backup[i - top_region_size];
	}

	i32 pt_index = cur_region->m_head;

	for (i32 j = 0; j < cur_region->m_size; ++j) {
	linked_point& lpt = m_linked_points[pt_index];

	memory_offset->m_x = lpt.m_x;
	memory_offset->m_y = lpt.m_y;

	lpt.m_ref = (i32)(memory_offset - memory);
	++memory_offset;
	pt_index = lpt.m_next;
	}
	}*/

	e.end();

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		img_mser& cur_mser = t_msers[i];
		cur_region = m_gray_order_regions[i];

		cur_mser.m_size = cur_region->m_size;
		cur_mser.m_rect.set_rect(cur_region->m_left, cur_region->m_top, i32(cur_region->m_right - cur_region->m_left + 1), i32(cur_region->m_bottom - cur_region->m_top + 1));
		cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
		cur_mser.m_memory_type = img_mser::Memory_Type_Range;
		
		// store start memory index
		cur_mser.m_points = memory + m_linked_points[cur_region->m_head].m_ref;
	}
}

void img_fast_mser_v1::extract_top_pixel_parallel(i32 k, i32* parallel_stop_indexes, i32* memory_offsets, mt_point* memory, i32 region_memory_size, mser_region** top_regions, i32 top_region_size, vector<mser_region*>& top_backup) {
	i32 start = k == 0 ? 0 : parallel_stop_indexes[k - 1] + 1;
	i32 stop = parallel_stop_indexes[k];
	mt_point* cur_mem = memory + memory_offsets[k];
	mt_point* end_mem = k == (m_parallel_thread_number - 1) ? memory + region_memory_size : memory + memory_offsets[k + 1];
	mser_region* cur_region;

	for (i32 i = start; i <= stop; ++i) {
		if (i < top_region_size) {
			cur_region = top_regions[i];
		} else {
			cur_region = top_backup[i - top_region_size];
		}

		i32 pt_index = cur_region->m_head;

		for (i32 j = 0; j < cur_region->m_size; ++j) {
			linked_point& lpt = m_linked_points[pt_index];
			//basiclog_assert2(cur_mem < end_mem && cur_mem >= memory + memory_offsets[k]);

			cur_mem->m_x = lpt.m_x;
			cur_mem->m_y = lpt.m_y;

			lpt.m_ref = (i32)(cur_mem - memory);
			++cur_mem;


			pt_index = lpt.m_next;
		}
	}
}

void img_fast_mser_v1::extract_pixel_parallel_4_self_child_impl(img_multi_msers& msers, u8 gray_mask) {
	sys_timer t("collect_mser_parallel_4_parallel_impl");
	t.begin();
	
	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);

	mser_region* cur_region;
	mser_region* real_region;
	mser_region* parent_region;

	basiclog_assert2(m_recursive_point_threshold != -1);

	i32 region_memory_sizes[4] = {0, 0, 0, 0};

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		cur_mser.m_rect.m_left = -1;
		cur_mser.m_size = cur_region->m_size;
		cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
		cur_region->m_child_pixel_size = 0;
		cur_region->m_child_memory_number = 0;
	}

	struct dt_size {
		union {
			mt_point* pt_offset;

			struct {
				i32 self_size;
				i32 cur_memery_size;
			};
		};
	};

	sys_timer size_computing("size computing");
	size_computing.begin();

	i32 free_memory_size = m_heap_size - m_gray_order_region_size;
	i32 need_memory_size = m_gray_order_region_size * sizeof(dt_size) / sizeof(i16*);
	dt_size* dt_sizes;
	
	if (need_memory_size <= free_memory_size) {
		dt_sizes = (dt_size*)(m_heap + m_gray_order_region_size);
	} else {
		basiclog_info2(sys_strcombine()<<"needs to allocate dt_size: "<<m_gray_order_region_size);
		dt_sizes = (dt_size*)malloc(sizeof(dt_size) * m_gray_order_region_size);
	}

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];
		i32 cur_memory_size;

		if (cur_region->m_child_memory_number == 0 || cur_mser.m_size <= m_recursive_point_threshold) {
			cur_mser.m_memory_type = img_mser::Memory_Type_Share;
			cur_memory_size = cur_mser.m_size;
		} else {
			cur_mser.m_memory_type = img_mser::Memory_Type_Recursive;
			cur_memory_size = 1 + cur_mser.m_size - cur_region->m_child_pixel_size + (1 + cur_region->m_child_memory_number) / 2;
		}

		region_memory_sizes[i % 4] += cur_memory_size;

		for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
		}

		if (real_region != NULL) {
			real_region->m_child_memory_number += 1;
			real_region->m_child_pixel_size += cur_mser.m_size;
		}

		cur_region->m_parent = real_region;
		cur_region->m_mser_index = i;

		dt_sizes[i].self_size = cur_mser.m_size - cur_region->m_child_pixel_size;
		dt_sizes[i].cur_memery_size = cur_memory_size;
	}

	size_computing.end();

	sys_timer memory_t("memory allocating");
	memory_t.begin();

	region_memory_size = region_memory_sizes[0] + region_memory_sizes[1] + region_memory_sizes[2] + region_memory_sizes[3];
	memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);

	mt_point* memory_offset[4] = {memory, memory + region_memory_sizes[0], memory + region_memory_sizes[0] + region_memory_sizes[1], memory + region_memory_sizes[0] + region_memory_sizes[1] + region_memory_sizes[2]};

	for (i32 k = 0; k < 4; ++k) {
		for (u32 i = k; i < m_gray_order_region_size; i += 4) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];
			parent_region = cur_region->m_parent;

			if (parent_region != NULL && t_msers[parent_region->m_mser_index].m_memory_type == img_mser::Memory_Type_Recursive) {
				if (m_linked_points[cur_region->m_head].m_prev != -1) {
					m_linked_points[m_linked_points[cur_region->m_head].m_prev].m_next = m_linked_points[cur_region->m_tail].m_next;
				}

				if (m_linked_points[cur_region->m_tail].m_next != NULL) {
					m_linked_points[m_linked_points[cur_region->m_tail].m_next].m_prev = m_linked_points[cur_region->m_head].m_prev;
				}
			}

			cur_mser.m_points = memory_offset[k];
			memory_offset[k] += dt_sizes[i].cur_memery_size;

			

			if (cur_mser.m_memory_type == img_mser::Memory_Type_Recursive) {
				cur_region->m_self_size = dt_sizes[i].self_size;

				cur_mser.m_points[0].m_x = dt_sizes[i].self_size;
				cur_mser.m_points[0].m_y = 0;

				dt_sizes[i].pt_offset = cur_mser.m_points;
				cur_mser.m_points += 1;
			} else {
				cur_region->m_self_size = cur_mser.m_size;
				dt_sizes[i].pt_offset = cur_mser.m_points;
			}
		}
	}

	memory_t.end();

	sys_timer self_t("self_pixel");
	self_t.begin();
	// extract self pixels
	#pragma omp parallel for num_threads(4)
	for (i32 k = 0; k < 4; ++k) {
		for (u32 i = k; i < m_gray_order_region_size; i += 4) {
			mser_region* cur_region = m_gray_order_regions[i];
			img_mser& mser = t_msers[i];
			mt_point*& pts = mser.m_points;
			i32 self_size = cur_region->m_self_size;

			i32 pt_index = cur_region->m_head;

			for (i32 j = 0; j < self_size; ++j) {
				linked_point& lpt = m_linked_points[pt_index];

				pts->m_y = lpt.m_y;
				pts->m_x = lpt.m_x;
				
				if (mser.m_rect.m_left == -1) {
					mser.m_rect.m_left = pts->m_x;
					mser.m_rect.m_width = pts->m_x;
					mser.m_rect.m_top = pts->m_y;
					mser.m_rect.m_height = pts->m_y;
				} else {
					if (pts->m_x < mser.m_rect.m_left) {
						mser.m_rect.m_left = pts->m_x;
					} else if (pts->m_x > mser.m_rect.m_width) {
						mser.m_rect.m_width = pts->m_x;
					}

					if (pts->m_y < mser.m_rect.m_top) {
						mser.m_rect.m_top = pts->m_y;
					} else if (pts->m_y > mser.m_rect.m_height) {
						mser.m_rect.m_height = pts->m_y;
					}
				}

				++pts;
				pt_index = lpt.m_next;
			}
		}
	}

	self_t.end();

	sys_timer g_t("global pixel");
	g_t.begin();

	// extract shared pixels
	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		img_mser& cur_mser = t_msers[i];
		cur_region = m_gray_order_regions[i];

		cur_mser.m_rect.m_width = cur_mser.m_rect.m_width - cur_mser.m_rect.m_left + 1;
		cur_mser.m_rect.m_height = cur_mser.m_rect.m_height - cur_mser.m_rect.m_top + 1;

		cur_mser.m_points = dt_sizes[i].pt_offset;
	}

	i32 memcpy_number = 0;
	img_mser* parent_mser;

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];	
		parent_region = cur_region->m_parent;

		img_mser& cur_mser = t_msers[i];

		parent_mser = NULL;
		if (parent_region != NULL && parent_region->m_region_flag == mser_region::Flag_Valid) {
			parent_mser = &t_msers[parent_region->m_mser_index];

			if (parent_mser->m_memory_type == img_mser::Memory_Type_Recursive) {
				parent_mser->m_rect.adjust_by_append_rect(cur_mser.m_rect);

				mt_point* child_start = parent_mser->m_points + 1 + parent_mser->m_points[0].m_x;
				i32 child_index = parent_mser->m_points[0].m_y;

				i32* child_data = (i32*)child_start;
				child_data[parent_mser->m_points[0].m_y++] = (i32)(&cur_mser - parent_mser);
			}
		}
	}

	g_t.end();

	if (need_memory_size > free_memory_size) {
		free(dt_sizes);
	}

	t.end();
}

void img_fast_mser_v1::get_duplicated_regions(vector<mser_region*>& duplicated_regions, mser_region* stable_region, mser_region* begin_region) {
	mser_region* parentRegion = begin_region->m_parent;

	while (true) {
		if (NULL == parentRegion) {
			break;
		}

		//basiclog_assert2(parentRegion->m_region_flag != mser_region::Flag_Merged);

		if (parentRegion->m_size > m_max_point) {
			// If the size of parent is too large, we do not need to find duplicated parent regions (parent regions will be delete absolutely). 
			break;
		}

		double variationFromBegin = (parentRegion->m_size - stable_region->m_size) / (double)stable_region->m_size;

		if (variationFromBegin > m_duplicated_variation) {
			break;
		}

		if (mser_region::Flag_Valid == parentRegion->m_region_flag) {
			basiclog_warning2("Too big mDuplicateVariantion");
			//parentRegion = parentRegion->m_parent;
			//continue;
		}

		if (mser_region::Flag_Invalid == parentRegion->m_region_flag) {
			parentRegion = parentRegion->m_parent;
			continue;
		}

		duplicated_regions.push_back(parentRegion);
		parentRegion = parentRegion->m_parent;
	}

	//duplicated_regions.push_back(parentRegion);
}

void img_fast_mser_v1::merge_comp(connected_comp* comp1, connected_comp* comp2) {	
	comp1->m_region->m_parent = comp2->m_region;
	comp2->m_region->m_boundary_region |= comp1->m_region->m_boundary_region;

	if (comp2->m_size > 0) {
		m_linked_points[comp2->m_tail].m_next = comp1->m_head;
		m_linked_points[comp1->m_head].m_prev = comp2->m_tail;

		comp2->m_tail = comp1->m_tail;	

		bound_pointer_adjust(comp2, comp1);
	} else {
		comp2->m_head = comp1->m_head;
		comp2->m_tail = comp1->m_tail;	

		bound_pointer_assgin(comp2, comp1);
	}

	
	comp2->m_size += comp1->m_size;
}