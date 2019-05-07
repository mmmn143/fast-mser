#include "stdafx.h"
#include "img_ppms_mser.h"

#define get_real_for_merged(region)	\
	for (;region != NULL && region->m_region_flag == mser_region::Flag_Merged; region = region->m_parent){}	\

#define get_real_parent_for_merged(region, region_parent)	\
	for (region_parent = region->m_parent; region_parent != NULL && region_parent->m_region_flag == mser_region::Flag_Merged; region_parent = region_parent->m_parent){}	\

#define get_set_real_parent_for_merged(region, region_parent)	\
	for (region_parent = region->m_parent; region_parent != NULL && region_parent->m_region_flag == mser_region::Flag_Merged; region_parent = region_parent->m_parent){}	\
	region->m_parent = region_parent;

img_ppms_mser::img_ppms_mser()
: img_mser_base() {

	m_points = NULL;
	m_extended_image = NULL;
	m_heap = NULL;
	m_mser_regions = NULL;

	m_point_size = 0;
	m_extended_image_size = 0;
	m_heap_size = 0;
	m_mser_region_size = 0;

	m_gray_order_regions = NULL;
	m_gray_order_region_size = 0;
	m_gray_order_regions_memory_size = 0;
}

img_ppms_mser::~img_ppms_mser() {
	basicsys_free(m_heap);
	basicsys_free(m_points);
	basicsys_free(m_mser_regions);
	basicsys_free(m_extended_image);
	basicsys_free(m_gray_order_regions);
}

void img_ppms_mser::clear_memory_cache() {
	basicsys_free(m_heap);
	basicsys_free(m_points);
	basicsys_free(m_mser_regions);
	basicsys_free(m_extended_image);
	basicsys_free(m_gray_order_regions);

	m_heap_size = 0;
	m_extended_image_size = 0;
	m_point_size = 0;
	m_mser_region_size = 0;
	m_gray_order_regions_memory_size = 0;
	m_exceed_times = 0;

	m_pinfo.swap(vector<parallel_info>());
}

void img_ppms_mser::allocate_memory(const mt_mat& img, const img_mask_info<u8>& mask) {
	allocate_memory_parallel_4(img, mask);
}

void img_ppms_mser::allocate_memory_parallel_4(const mt_mat& img, const img_mask_info<u8>& mask) {
	m_pinfo.resize(4);

	u32 left_width = (u32)img.size()[1] / 2;
	u32 top_height = (u32)img.size()[0] / 2;

	u32 right_width = (u32)img.size()[1] - left_width;
	u32 bottom_height = (u32)img.size()[0] - top_height;

	u32 horizontal_index[2] = {0, left_width};
	u32 vertical_index[2] = {0, top_height};
	u32 widths[2] = {left_width, right_width};
	u32 heights[2] = {top_height, bottom_height};

	u32 pt_size = (left_width + 2) * (top_height + 2) + (left_width + 2) * (bottom_height + 2) + (right_width + 2) * (top_height + 2) + (right_width + 2) * (bottom_height + 2); 
	
	if (pt_size != m_point_size) {
		if (m_points != NULL) {
			free(m_points);
		}

		m_point_size = pt_size;
		m_points = (u32*)malloc(sizeof(u32) * m_point_size);
	}

	if (pt_size != m_extended_image_size) {
		if (m_extended_image != NULL) {
			free(m_extended_image);
		}

		m_extended_image_size = pt_size;
		m_extended_image = (u8*)malloc(sizeof(u8) * m_extended_image_size);
	}
	
	u32 rg_size = img.size()[0] * img.size()[1];
	
	if (rg_size != m_mser_region_size) {
		if (m_mser_regions != NULL) {
			free(m_mser_regions);
		}

		m_mser_region_size = rg_size;
		m_mser_regions = (mser_region*)malloc(sizeof(mser_region) * m_mser_region_size);
	}

	u32 h_size = rg_size + 257 * 4;

	if (h_size != m_heap_size) {
		if (m_heap != NULL) {
			free(m_heap);
		}

		m_heap_size = h_size;
		m_heap = (u32**)malloc(sizeof(u32*) * m_heap_size);
	}

	i32 algorithm_memory_size = pt_size * sizeof(u32) + pt_size * sizeof(u8) + rg_size * sizeof(mser_region) + h_size * sizeof(u32*)
		+ sizeof(u32) * 257 * 2 + sizeof(parallel_info) * 4;
	basiclog_info2(sys_strcombine()<<L"linear memory cost: "<< algorithm_memory_size / 1024.0 / 1024.0 <<L"MB");

	u32 i = 0;
	i32 memory_offset_for_region = 0;
	i32 memory_offset_for_heap = 0;
	i32 memory_offset_with_boundary = 0;

	for (u32 v = 0; v < 2; ++v) {
		for (u32 h = 0; h < 2; ++h) {
			parallel_info& pinfo = m_pinfo[i];
			pinfo.m_left = horizontal_index[h];
			pinfo.m_top = vertical_index[v];
			pinfo.m_width = widths[h];
			pinfo.m_height = heights[v];
			pinfo.m_width_with_boundary = widths[h] + 2;
			pinfo.m_height_with_boundary = heights[v] + 2;
			pinfo.m_extended_image = m_extended_image + memory_offset_with_boundary;
			pinfo.m_points = m_points + memory_offset_with_boundary;

			pinfo.m_heap = m_heap + memory_offset_for_heap;
			pinfo.m_heap_start[0] = &pinfo.m_heap[0];
			
			pinfo.m_mser_regions = m_mser_regions + memory_offset_for_region;

			i32 row_step = widths[h] + 2;

			if (img_Connected_Type_8_Neibour == m_connected_type) {
				pinfo.m_dir[1] = 1;
				pinfo.m_dir[2] = 1 - row_step;
				pinfo.m_dir[3] = -row_step;
				pinfo.m_dir[4] = -1 - row_step;
				pinfo.m_dir[5] = -1;
				pinfo.m_dir[6] = -1 + row_step;
				pinfo.m_dir[7] = row_step;
				pinfo.m_dir[8] = 1 + row_step;


			} else {
				pinfo.m_dir[1] = 1;
				pinfo.m_dir[2] = -row_step;
				pinfo.m_dir[3] = -1;
				pinfo.m_dir[4] = row_step;
			}

			memory_offset_with_boundary += pinfo.m_width_with_boundary * pinfo.m_height_with_boundary;
			
			i32 patch_size = pinfo.m_width * pinfo.m_height;
			memory_offset_for_region += patch_size;
			memory_offset_for_heap += patch_size + 257;
			
			++i;
		}
	}

	m_mask_mask = 0;

	if (img_Connected_Type_8_Neibour == m_connected_type) {
		m_dir_mask = 0xF0000000;		
		m_dir_shift = 28;
		m_boundary_pixel = 9 << m_dir_shift;
	} else {
		m_dir_mask = 0xe0000000;
		m_dir_shift = 29;
		m_boundary_pixel = 5 << m_dir_shift;
	}
}

void img_ppms_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	build_tree_parallel_4(src, mask, gray_mask);
}

void img_ppms_mser::build_tree_parallel_4(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 4; ++i) {
		make_tree_patch(m_pinfo[i], src, mask, gray_mask, i);
	}

	//parallel.end();

	merge_tree_parallel_4();
}

void img_ppms_mser::make_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask, u8 patch_index) {
	//sys_timer timer(L"make_tree_patch");
	//timer.begin();

	process_tree_patch(pinfo, img, mask, gray_mask);
	
#define get_gray(ptr)	((m_mask_mask == 0 || (*ptr & m_mask_mask) == 0) ? (*(pinfo.m_extended_image + (ptr - pinfo.m_points)) ^ gray_mask) : 256)

	u32*** heap_cur = pinfo.m_heap_start;
	mser_region* regions = &pinfo.m_mser_regions[0];
	connected_comp* comptr = pinfo.m_comp; 
	u32* points = pinfo.m_points + 1 + pinfo.m_width_with_boundary;
	mser_region* startRegion = regions;
	i32* dir = pinfo.m_dir;

	i16 curr_gray = get_gray(points);

	comptr->m_gray_level = 257;	//process mask
	comptr++;
	*points = 1 << m_dir_shift;
	comptr->m_gray_level = curr_gray;
	init_comp(comptr, regions, patch_index);
	heap_cur += curr_gray;

	int mser_number = 0;
	i32 er_number = 0;

	i16 nbr_gray;
	i32 nbr_index;
	u32* nbr_points;

	for ( ; ; ) {
		nbr_index = *points >> m_dir_shift;

		// take tour of all the 4 or 8 directions
		while (nbr_index <= 4) {
			// get the neighbor
			nbr_points = points + dir[nbr_index];

			// if the neighbor is not visited yet
			if ((*nbr_points & m_dir_mask) == 0) {
				nbr_gray = get_gray(nbr_points);
				*nbr_points = 1 << m_dir_shift;

				if (nbr_gray < curr_gray) {
					// when the value of neighbor smaller than current
					// push current to boundary heap and make the neighbor to be the current one
					// create an empty comp
					*++(*heap_cur) = points;
					*points = (nbr_index + 1) << m_dir_shift;
					heap_cur += (nbr_gray - curr_gray);

					points = nbr_points;
					comptr++;
					comptr->m_gray_level = nbr_gray;
					init_comp(comptr, regions, patch_index);
					curr_gray = nbr_gray;
					nbr_index = 1;
					continue;
				} 

				// otherwise, push the neighbor to boundary heap	
				*(++heap_cur[nbr_gray - curr_gray]) = nbr_points;
			}

			++nbr_index;
		}

		// get the current location
		*points = (nbr_index << m_dir_shift) | (i32)(comptr->m_region - m_mser_regions);
		++comptr->m_size;

		// get the next pixel from boundary heap
		if ( **heap_cur ) {
			points = **heap_cur;
			(*heap_cur)--;
		} else {
			++er_number;
			comptr->m_region->m_size = comptr->m_size;
			comptr->m_region->m_unmerged_size = comptr->m_size;

			heap_cur++;
			i16 pixel_val = 0;
			for ( i32 i = curr_gray+1; i < 257; i++ ) {
				if ( **heap_cur ) {
					pixel_val = i;
					break;
				}

				heap_cur++;
			}

			if ( pixel_val ) {
				curr_gray = pixel_val;
				points = **heap_cur;
				(*heap_cur)--;

				if (pixel_val < comptr[-1].m_gray_level) {
					// check the stablity and push a new history, increase the gray level
					comptr->m_region->m_parent = regions;
					comptr[0].m_gray_level = pixel_val;
					new_region(comptr, regions, patch_index);


					mser_number += 1;
				} else {
					//basiclog_assert2(pixel_val == comptr[-1].m_gray_level);

					// keep merging top two comp in stack until the grey level >= pixel_val
					for ( ; ; ) {
						//BASICML_ASSERT(pixel_val <= comptr[-1].m_gray_level);


						comptr--;
						(comptr+1)->m_region->m_parent = comptr->m_region;
						comptr->m_size += (comptr+1)->m_size;

						if ( pixel_val <= comptr[0].m_gray_level) {
							break;
						}

						//Notice that it seems here never can be reached!
						//BASICML_ASSERT(pixel_val <= comptr[-1].m_gray_level);

						if (pixel_val < comptr[-1].m_gray_level) {
							// check the stablity here otherwise it wouldn't be an ER
							comptr->m_region->m_parent = regions;
							comptr[0].m_gray_level = pixel_val;
							new_region(comptr, regions, patch_index);

							mser_number += 1;
							break;
						}
					}
				}
			} else
				break;
		}
	}

	pinfo.m_er_number = (i32)(regions - startRegion);
	pinfo.m_mser_regions_end = regions;

	basiclog_assert2(pinfo.m_er_number == er_number);

	//timer.end();

	//i32 nullParentCount = 0;

	//for (i32 i = 0; i < erCount; ++i) {
	//	i32 m_size = startRegion[i].m_size;
	//	if (NULL == startRegion[i].m_parent)	{
	//		++nullParentCount;
	//	} 
	//}

	//BASICML_ASSERT(1 == nullParentCount);
	//BASICML_ASSERT(erCount == region - startRegion);

	basiclog_info2(sys_strcombine()<<L"er number "<<er_number<< L" mser number " << mser_number);
}

void img_ppms_mser::process_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask) {
	//sys_timer t(L"process_tree_patch");
	//t.begin();

	u32* level_size = pinfo.m_level_size;
	u32* points = pinfo.m_points;
	i32 masked_image_width = pinfo.m_width_with_boundary;
	i32 row;
	i32 col;

	if (gray_mask == 0 || !m_from_min_max[0]) {
		u8* extended_image_data = pinfo.m_extended_image;
		const u8* img_data = img.ptr<u8>(pinfo.m_top, pinfo.m_left, 0);
		u32 img_cpt = img.step()[0] - pinfo.m_width;
		memset(level_size, 0, sizeof(u32) * 257);

		//set the first row to be -1
		for (col = 0; col < masked_image_width; ++col) {
			*points++ = m_boundary_pixel;
		}

		extended_image_data += masked_image_width + 1;

		if (mask.m_mask_type == img_Mask_Type_Null) {
			//set middle row
			for (row = 0; row < pinfo.m_height; ++row) {
				*points++ = m_boundary_pixel;

				for (col = 0; col < pinfo.m_width; ++col) {
					level_size[*img_data]++;
					*extended_image_data++ = *img_data++;
					*points++ = 0;
				}

				*points++ = m_boundary_pixel;
				img_data += img_cpt;
				extended_image_data += 2;
			}
		}

		//set the last row to be -1
		for (col = 0; col < masked_image_width; ++col) {
			*points++ = m_boundary_pixel;
		}
	} else {
		points += masked_image_width + 1;
		u8 gray = 0;
		//set the first row to be -1
		if (mask.m_mask_type == img_Mask_Type_Null) {
			//set middle row
			for (row = 0; row < pinfo.m_height; ++row) {
				for (col = 0; col < pinfo.m_width; ++col) {
					*points++ = 0;
				}

				points += 2;
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

	//t.end();
}

void img_ppms_mser::init_comp(connected_comp* comptr, mser_region*& region, u8 patch_index) {
	comptr->m_size = 0;
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_size = 0;
	region->m_parent = NULL;
	region->m_assigned_pointer = 0;
	region->m_calculated_var = 0;
	comptr->m_region = region;
	++region;
}

void img_ppms_mser::new_region(connected_comp* comptr, mser_region*& region, u8 patch_index) {
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_size = 0;
	region->m_parent = NULL;
	region->m_assigned_pointer = 0;
	region->m_calculated_var = 0;
	comptr->m_region = region;
	++region;
}

void img_ppms_mser::merge_tree_parallel_4() {
	merge_tree_parallel_4_step(1);
	merge_tree_parallel_4_step(2);
	merge_tree_parallel_4_step(3);
}

void img_ppms_mser::merge_tree_parallel_4_step(u8 merged_flag) {
	if (merged_flag == 1 || merged_flag == 2) {
		u32 left_parallel_index = 0;
		u32 right_parallel_index = 1;

		if (merged_flag == 2) {
			left_parallel_index = 2;
			right_parallel_index = 3;
		}

		//top left and top right
		u32 left_step = m_pinfo[left_parallel_index].m_width_with_boundary;
		u32 right_step = m_pinfo[right_parallel_index].m_width_with_boundary;
		u32* left_link_points_base = m_pinfo[left_parallel_index].m_points + m_pinfo[left_parallel_index].m_width + left_step;
		u32* right_link_points_base = m_pinfo[right_parallel_index].m_points + 1 + right_step;
		mser_region* a;
		mser_region* b;
		u32* left_link_points = left_link_points_base;
		u32* right_link_points = right_link_points_base;

		for (i32 i = 0; i < m_pinfo[left_parallel_index].m_height; ++i) {
			a = m_mser_regions + (*left_link_points - m_boundary_pixel);
			b = m_mser_regions + (*right_link_points - m_boundary_pixel);

			get_real_for_merged(a);
			get_real_for_merged(b);
			
			connect(a, b);

			left_link_points += left_step;
			right_link_points += right_step;
		}
	} else {
		//whole
		u32 top_parallel_index = 0;
		u32 bottom_parallel_index = 2;

		u32 top_step;
		u32 bottom_step;
		u32* top_link_points_base;
		u32* bottom_link_points_base;
		u32* top_link_points;
		u32* bottom_link_points;
		mser_region* a;
		mser_region* b;

		for (u32 p = 0; p < 2; ++p) {
			top_parallel_index += p;
			bottom_parallel_index += p;

			top_step = 1;
			bottom_step = 1;
			top_link_points_base = m_pinfo[top_parallel_index].m_points + m_pinfo[top_parallel_index].m_height * m_pinfo[top_parallel_index].m_width_with_boundary + 1;
			bottom_link_points_base = m_pinfo[bottom_parallel_index].m_points + m_pinfo[bottom_parallel_index].m_width_with_boundary + 1;

			top_link_points = top_link_points_base;
			bottom_link_points = bottom_link_points_base;

			for (i32 i = 0; i < m_pinfo[top_parallel_index].m_width; ++i) {
				a = m_mser_regions + (*top_link_points - m_boundary_pixel);
				b = m_mser_regions + (*bottom_link_points - m_boundary_pixel);

				get_real_for_merged(a);
				get_real_for_merged(b);

				connect(a, b);

				top_link_points += top_step;
				bottom_link_points += bottom_step;
			}
		}
	}
}

void img_ppms_mser::connect(mser_region* bigger, mser_region* smaller) {
	b8 first = sys_true;

	mser_region* swap_temp;
	mser_region* smaller_parent;

	i32 pixel_size = 0;
	i32 temp_pixel_size = 0;

	for (;;) {
		if (smaller == NULL || bigger == NULL || smaller == bigger) {
			break;
		}

		if (smaller->m_gray_level > bigger->m_gray_level) {
			swap_temp = bigger;
			bigger = smaller;
			smaller = swap_temp;
		}

		get_set_real_parent_for_merged(smaller, smaller_parent);

		if (smaller_parent != NULL && smaller_parent->m_gray_level < bigger->m_gray_level) {
			smaller->m_size += pixel_size;
			smaller = smaller_parent;
		} else {
			if (bigger->m_gray_level == smaller->m_gray_level) {
				smaller->m_region_flag = mser_region::Flag_Merged;
				m_merged_number += 1;
			}

			temp_pixel_size = smaller->m_size + pixel_size;
			pixel_size = smaller->m_size;
			smaller->m_size = temp_pixel_size;

			smaller->m_parent = bigger;
			smaller = bigger;
			bigger = smaller_parent;
		}
	}

	if (bigger == NULL) {
		while (smaller != NULL) {
			smaller->m_size += pixel_size;
			get_set_real_parent_for_merged(smaller, smaller_parent);
			smaller = smaller_parent;
		}
	}
}

void img_ppms_mser::recognize_mser() {
	recognize_mser_parallel_4();
}

void img_ppms_mser::recognize_mser_parallel_4() {
	//determine_mser_parallel_4_normal(from_min);
	recognize_mser_parallel_4_parallel();
}

void img_ppms_mser::recognize_mser_parallel_4_normal() {
	u32 totalUnkonwSize = 0;
	u32 nmsCount = 0;

	mser_region* cur_region;
	mser_region* end_region;
	mser_region* parent_region;

	memset(m_region_level_size, 0, sizeof(u32) * 257);

	i32 total_er_number = 0;
	i32 bad_variance_number = 0;

	//compute variance and get real parent for each mser_region
	for (i32 i = 0; i < 4; ++i) {

		cur_region = m_pinfo[i].m_mser_regions;
		end_region = m_pinfo[i].m_mser_regions_end;
		total_er_number += m_pinfo[i].m_er_number;

		for (; cur_region != end_region; ++cur_region) {
			//basiclog_assert2(cur_region->m_short_cut_offset == 0);

			if (cur_region->m_region_flag == mser_region::Flag_Unknow) {


				if (m_delta > 0) {
					//this will compute the real parent for each region
					calculate_variation(cur_region);

					if (cur_region->m_var > m_stable_variation) {
						cur_region->m_region_flag = mser_region::Flag_Invalid;
						bad_variance_number++;
						continue;
					}
				}

				if (cur_region->m_size < m_min_point || cur_region->m_size > m_max_point || NULL == cur_region->m_parent) {
					cur_region->m_region_flag = mser_region::Flag_Invalid;
					continue;
				}

				if (m_delta <= 0) {
					get_set_real_parent_for_merged(cur_region, parent_region);
				}

				++totalUnkonwSize;
				++m_region_level_size[cur_region->m_gray_level];
			} else {
				basiclog_assert2(cur_region->m_region_flag == mser_region::Flag_Merged);
			}
		}
	}

	//basiclog_info2(sys_strcombine()<<L"bad_variance_number "<<bad_variance_number);
	i32 beforeUnkonwSize = totalUnkonwSize;

	if (m_delta > 0 && m_nms_similarity >= 0) {
		for (i32 i = 0; i < 4; ++i) {

			cur_region = m_pinfo[i].m_mser_regions;
			end_region = m_pinfo[i].m_mser_regions_end;

			for (; cur_region != end_region; ++cur_region) {

				if (cur_region->m_region_flag == mser_region::Flag_Merged) {
					continue;
				}

				parent_region = cur_region->m_parent;
				//basiclog_assert2(parent_region == NULL || parent_region->m_region_flag != mser_region::Flag_Merged);
				if (cur_region->m_var >= 0 && parent_region != NULL && parent_region->m_var >= 0 && parent_region->m_gray_level == cur_region->m_gray_level + 1) {
					double subValue = parent_region->m_var - cur_region->m_var;
					if (subValue > m_nms_similarity) {
						if (mser_region::Flag_Invalid != parent_region->m_region_flag) {
							parent_region->m_region_flag = mser_region::Flag_Invalid;
							--m_region_level_size[parent_region->m_gray_level];
							--totalUnkonwSize;
						}
					} else if (-subValue > m_nms_similarity) {
						if (mser_region::Flag_Invalid != cur_region->m_region_flag) {
							cur_region->m_region_flag = mser_region::Flag_Invalid;
							--m_region_level_size[cur_region->m_gray_level];
							--totalUnkonwSize;
						}
					} 
				}
			}
		}
	}

	nmsCount = beforeUnkonwSize - totalUnkonwSize;
	//basiclog_info2(sys_strcombine()<<L"nms count "<<nmsCount);

	//order mser region
	mt_helper::integral_array<u32>(m_start_indexes, m_region_level_size, m_region_level_size + 256);

	basiclog_assert2(m_start_indexes[256] == totalUnkonwSize);

	m_gray_order_region_size = totalUnkonwSize;

	if (m_gray_order_regions_memory_size > totalUnkonwSize * 5) {
		++m_exceed_times;

		if (m_exceed_times > 10) {
			m_gray_order_regions_memory_size = m_gray_order_region_size;
			free(m_gray_order_regions);
			m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
		}
	} else {
		m_exceed_times = 0;
		m_gray_order_regions_memory_size = totalUnkonwSize;
		m_gray_order_region_size = totalUnkonwSize;

		if (m_gray_order_regions != NULL) {
			free(m_gray_order_regions);
		}

		m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
	}

	for (i32 i = 0; i < 4; ++i) {
		cur_region = m_pinfo[i].m_mser_regions;
		end_region = m_pinfo[i].m_mser_regions_end;

		for (; cur_region != end_region; ++cur_region) {
			if (cur_region->m_region_flag == mser_region::Flag_Unknow) {
				m_gray_order_regions[m_start_indexes[cur_region->m_gray_level]++] = cur_region;
			}
		}
	}

	u32 validCount = 0;

	m_remove_duplicated_memory_helper.reserve(100);

	//去重
	for (u32 i = 0; i < totalUnkonwSize; ++i) {
		cur_region = m_gray_order_regions[i];

		if (cur_region->m_region_flag != mser_region::Flag_Unknow) {
			continue;
		}

		m_remove_duplicated_memory_helper.clear();
		m_remove_duplicated_memory_helper.push_back(cur_region);
		get_duplicated_regions(m_remove_duplicated_memory_helper, cur_region, cur_region);

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
		if (m_gray_order_regions[i]->m_region_flag == mser_region::Flag_Valid) {
			m_channel_total_pixel_number += m_gray_order_regions[i]->m_size;
			m_gray_order_regions[index++] = m_gray_order_regions[i];
		}
	}

	m_gray_order_region_size = validCount;
	basiclog_assert2(index == validCount);

	basiclog_info2(sys_strcombine()<<L"m_gray_order_region_size: "<<m_gray_order_region_size);
}

void img_ppms_mser::recognize_mser_parallel_4_parallel() {
	u32 totalUnkonwSize = 0;
	i32 bad_variance_number = 0;

	i32 tn = 0;
	i32 nms_count = 0;
	
	#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 4; ++i) {
		mser_region* parent_region;
		mser_region* start_region;
		mser_region* temp_region;
		mser_region* begin_region = m_pinfo[i].m_mser_regions;
		mser_region* cur_region = begin_region;
		mser_region* end_region = m_pinfo[i].m_mser_regions_end;
		u32 gray_level_threshold;

		for (; cur_region != end_region; ++cur_region) {
			
			if (cur_region->m_region_flag != mser_region::Flag_Merged) {
				basiclog_assert2(cur_region->m_calculated_var == 0);

				gray_level_threshold = cur_region->m_gray_level + m_delta;
				start_region = cur_region;
				get_set_real_parent_for_merged(start_region, parent_region);
				
				while (parent_region != NULL && parent_region->m_gray_level <= gray_level_threshold) {
					basiclog_assert2(parent_region->m_region_flag != mser_region::Flag_Merged);

					start_region = parent_region;
					if (parent_region >= begin_region && parent_region < end_region) {
						get_set_real_parent_for_merged(parent_region, temp_region);
					} else {
						get_real_parent_for_merged(parent_region, temp_region);
					}

					parent_region = temp_region;
				}

				if (parent_region != NULL || start_region->m_gray_level == gray_level_threshold) {
					cur_region->m_var = (start_region->m_size - cur_region->m_size) / (f32)cur_region->m_size;
				} else {
					cur_region->m_var = -1;
				}

				if (cur_region->m_var > m_stable_variation) {
					cur_region->m_region_flag = mser_region::Flag_Invalid;
					//bad_variance_number++;
				} else if (cur_region->m_size < m_min_point || cur_region->m_size > m_max_point || NULL == cur_region->m_parent) {
					cur_region->m_region_flag = mser_region::Flag_Invalid;
				}
			} else {
			}
		}
	}

	mser_region* cur_region;
	mser_region* end_region;
	mser_region* parent_region;

	memset(m_region_level_size, 0, sizeof(u32) * 257);

	for (i32 i = 0; i < 4; ++i) {

		cur_region = m_pinfo[i].m_mser_regions;
		end_region = m_pinfo[i].m_mser_regions_end;
		for (; cur_region != end_region; ++cur_region) {
			if (cur_region->m_region_flag == mser_region::Flag_Merged) {
				continue;
			}

			parent_region = cur_region->m_parent;

			if (cur_region->m_region_flag == mser_region::Flag_Unknow) {
				basiclog_assert2(cur_region->m_calculated_var == 0);
				m_region_level_size[cur_region->m_gray_level]++;
				totalUnkonwSize++;
				cur_region->m_calculated_var = 1;
			} else if (parent_region == NULL || parent_region->m_region_flag == mser_region::Flag_Invalid) {
				continue;
			}

			basiclog_assert2(parent_region->m_region_flag != mser_region::Flag_Merged);

			if (m_nms_similarity >= 0 && cur_region->m_var >= 0 && parent_region->m_var >= 0 && parent_region->m_gray_level == cur_region->m_gray_level + 1) {
				double subValue = parent_region->m_var - cur_region->m_var;
				if (subValue > m_nms_similarity) {
					if (mser_region::Flag_Unknow == parent_region->m_region_flag) {
						if (parent_region->m_calculated_var == 1) {
							--m_region_level_size[parent_region->m_gray_level];
							--totalUnkonwSize;
						}

						parent_region->m_region_flag = mser_region::Flag_Invalid;
					}
				} else if (-subValue > m_nms_similarity) {
					if (mser_region::Flag_Unknow == cur_region->m_region_flag) {
						if (cur_region->m_calculated_var == 1) {
							--m_region_level_size[cur_region->m_gray_level];
							--totalUnkonwSize;
						}

						cur_region->m_region_flag = mser_region::Flag_Invalid;
					}
				} 
			}
		}
	}

	//order mser region
	mt_helper::integral_array<u32>(m_start_indexes, m_region_level_size, m_region_level_size + 256);

	basiclog_assert2(m_start_indexes[256] == totalUnkonwSize);

	m_gray_order_region_size = totalUnkonwSize;

	if (m_gray_order_regions_memory_size > totalUnkonwSize * 5) {
		++m_exceed_times;

		if (m_exceed_times > 10) {
			m_gray_order_regions_memory_size = m_gray_order_region_size;
			free(m_gray_order_regions);
			m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
		}
	} else {
		m_exceed_times = 0;
		m_gray_order_regions_memory_size = totalUnkonwSize;
		m_gray_order_region_size = totalUnkonwSize;

		if (m_gray_order_regions != NULL) {
			free(m_gray_order_regions);
		}

		m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
	}

	for (i32 i = 0; i < 4; ++i) {
		cur_region = m_pinfo[i].m_mser_regions;
		end_region = m_pinfo[i].m_mser_regions_end;

		for (; cur_region != end_region; ++cur_region) {
			if (cur_region->m_region_flag == mser_region::Flag_Unknow) {
				m_gray_order_regions[m_start_indexes[cur_region->m_gray_level]++] = cur_region;
			}
		}
	}

	sys_timer duplicated_t(L"duplicated");
	duplicated_t.begin();

	if (m_duplicated_variation > 0) {
		u32 validCount = 0;

		m_remove_duplicated_memory_helper.reserve(100);

		//去重
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];

			if (cur_region->m_region_flag != mser_region::Flag_Unknow) {
				continue;
			}

			m_remove_duplicated_memory_helper.clear();
			m_remove_duplicated_memory_helper.push_back(cur_region);
			get_duplicated_regions(m_remove_duplicated_memory_helper, cur_region, cur_region);

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
			if (m_gray_order_regions[i]->m_region_flag == mser_region::Flag_Valid) {
				m_channel_total_pixel_number += m_gray_order_regions[i]->m_size;
				m_gray_order_regions[index++] = m_gray_order_regions[i];
			}
		}

		m_gray_order_region_size = validCount;
		basiclog_assert2(index == validCount);
	} else {
		m_channel_total_pixel_number = 0;
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			m_channel_total_pixel_number += m_gray_order_regions[i]->m_size;
			m_gray_order_regions[i]->m_region_flag = mser_region::Flag_Valid;
		}
	}

	basiclog_info2(sys_strcombine()<<L"m_gray_order_region_size: "<<m_gray_order_region_size);
}

void img_ppms_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	extract_pixel_parallel_4(msers, gray_mask);
}

void img_ppms_mser::extract_pixel_parallel_4(img_multi_msers& msers, u8 gray_mask) {
	if (m_gray_order_region_size <= 0) {
		return;
	}

	//extract_pixel_parallel_4_serial_impl(msers, gray_mask);
	extract_pixel_parallel_4_parallel_impl(msers, gray_mask);
}

void img_ppms_mser::extract_pixel_parallel_4_serial_impl(img_multi_msers& msers, u8 gray_mask) {
	sys_timer t(L"collect_mser_parallel_4_share_serial_impl");
	t.begin();

	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);
	region_memory_size = 0;
	mt_point* memory_offset;

	img_mser** mser_heap = (img_mser**)m_heap;
	mser_region* cur_region;
	mser_region* end_region;
	mser_region* real_region;
	mser_region* parent_region;
	
	if (m_recursive_point_threshold == -1 || m_recursive_point_threshold == INT_MAX) {
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
			cur_region->m_mser_index = i;

			cur_mser.m_rect.m_left = -1;

			mser_heap[cur_region - m_mser_regions] = &cur_mser;
		}

		int number = 0;
		int total = 0;

		for (i32 i = 0; i < 4; ++i) {
			cur_region = m_pinfo[i].m_mser_regions;
			end_region = m_pinfo[i].m_mser_regions_end;

			for (; cur_region != end_region; ++cur_region) {
				++total;
				if (cur_region->m_region_flag == mser_region::Flag_Valid) {
					//find real parent
					for (parent_region = cur_region->m_parent; parent_region != NULL && parent_region->m_region_flag != mser_region::Flag_Valid; parent_region = parent_region->m_parent) {
						++number;
					}

					cur_region->m_parent = parent_region;
				} else {
					//find real region
					if (cur_region->m_assigned_pointer == 1) {
						continue;
					}

					for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid && real_region->m_assigned_pointer == 0; real_region = real_region->m_parent) {
						++number;
					}

					//set real region
					img_mser* real = NULL;

					if (real_region != NULL) {
						real = mser_heap[real_region - m_mser_regions];
					}

					for (parent_region = cur_region; parent_region != NULL && parent_region != real_region; parent_region = parent_region->m_parent) {				
						mser_heap[parent_region - m_mser_regions] = real;
						parent_region->m_assigned_pointer = 1;
					}
				}
			}
		}

	} else {
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			cur_mser.m_rect.m_left = -1;
			cur_mser.m_size = cur_region->m_size;
			cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
			cur_region->m_child_pixel_size = 0;
			cur_region->m_child_memory_number = 0;
		}

		sys_timer mser_id(L"mser_id");
		mser_id.begin();

		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			if (cur_region->m_child_memory_number == 0 || cur_mser.m_size <= m_recursive_point_threshold) {
				cur_mser.m_memory_type = img_mser::Memory_Type_Share;
				region_memory_size += cur_mser.m_size;
			} else {
				cur_mser.m_memory_type = img_mser::Memory_Type_Recursive;
				region_memory_size += 1 + cur_mser.m_size - cur_region->m_child_pixel_size + (1 + cur_region->m_child_memory_number) / 2;
			}

			for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
			}

			parent_region = cur_region->m_parent;

			if (real_region != NULL) {
				real_region->m_child_memory_number += 1;
				real_region->m_child_pixel_size += cur_mser.m_size;
			}

			cur_region->m_parent = real_region;

			while (parent_region != NULL && parent_region != real_region) {
				cur_region = parent_region->m_parent;

				parent_region->m_parent = real_region;
				parent_region = cur_region;
			}
		}

		memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);
		memory_offset = memory;

		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			cur_mser.m_points = memory_offset;

			if (cur_mser.m_memory_type == img_mser::Memory_Type_Share) {
				memory_offset += cur_mser.m_size;
			} else {
				cur_mser.m_points[0].m_x = cur_mser.m_size - cur_region->m_child_pixel_size;
				cur_mser.m_points[0].m_y = 0;
				memory_offset += 1 + cur_mser.m_size - cur_region->m_child_pixel_size + (1 + cur_region->m_child_memory_number) / 2;
				cur_mser.m_points += 1;
			}

			mser_heap[cur_region - m_mser_regions] = &cur_mser;

			cur_region->m_mser_index = i;
		}

		mser_id.end();

		for (i32 i = 0; i < 4; ++i) {
			cur_region = m_pinfo[i].m_mser_regions;
			end_region = m_pinfo[i].m_mser_regions_end;

			for (; cur_region != end_region; ++cur_region) {
				if (cur_region->m_region_flag == mser_region::Flag_Valid) {
				} else {
					//find real region
					if (cur_region->m_assigned_pointer == 1) {
						continue;
					}

					for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid && real_region->m_assigned_pointer == 0; real_region = real_region->m_parent) {
					}

					img_mser* real = NULL;

					if (real_region != NULL) {
						real = mser_heap[real_region - m_mser_regions];
					}

					for (parent_region = cur_region; parent_region != NULL && parent_region != real_region; parent_region = parent_region->m_parent) {				
						mser_heap[parent_region - m_mser_regions] = real;
						parent_region->m_assigned_pointer = 1;
					}
				}
			}
		}

		
	}

	int u = 0;
	//second, for each link point, we assign point value for its corresponding MSER
	for (i32 i = 0; i < 4; ++i) {
		u32* link_points = m_pinfo[i].m_points + 1 + m_pinfo[i].m_width_with_boundary;
		i32 left_offset = m_pinfo[i].m_left;
		i32 top_pffset = m_pinfo[i].m_top;

		for (i32 row = 0; row < m_pinfo[i].m_height; ++row) {
			for (i32 col = 0; col < m_pinfo[i].m_width; ++col) {
				img_mser* mser = mser_heap[*link_points - m_boundary_pixel];
				if (mser != NULL) {
					mt_point& cpt = *mser->m_points;
					cpt.m_x = col + left_offset;
					cpt.m_y = row + top_pffset;

					if (mser->m_rect.m_left == -1) {
						mser->m_rect.m_left = cpt.m_x;
						mser->m_rect.m_width = cpt.m_x;
						mser->m_rect.m_top = cpt.m_y;
						mser->m_rect.m_height = cpt.m_y;
					} else {
						if (cpt.m_x < mser->m_rect.m_left) {
							mser->m_rect.m_left = cpt.m_x;
						} else if (cpt.m_x > mser->m_rect.m_width) {
							mser->m_rect.m_width = cpt.m_x;
						}

						if (cpt.m_y < mser->m_rect.m_top) {
							mser->m_rect.m_top = cpt.m_y;
						} else if (cpt.m_y > mser->m_rect.m_height) {
							mser->m_rect.m_height = cpt.m_y;
						}
					}

					++mser->m_points;
					++u;
				}

				++link_points;
			}

			link_points += 2;
		}
	}

	mt_point** pt_offset = (mt_point**)m_heap;

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		img_mser& cur_mser = t_msers[i];
		cur_region = m_gray_order_regions[i];

		cur_mser.m_rect.m_width = cur_mser.m_rect.m_width - cur_mser.m_rect.m_left + 1;
		cur_mser.m_rect.m_height = cur_mser.m_rect.m_height - cur_mser.m_rect.m_top + 1;

		if (cur_mser.m_memory_type == img_mser::Memory_Type_Share) {
			//cur_mser.m_points -= cur_mser.m_size;	//? cur_mser.m_size - cur_region->m_child_pixel_size
			pt_offset[i] = cur_mser.m_points - cur_mser.m_size + cur_region->m_child_pixel_size;
		} else {
			pt_offset[i] = cur_mser.m_points - 1 - cur_mser.m_size + cur_region->m_child_pixel_size;
			cur_mser.m_points = pt_offset[i];
		}
	}

	i32 memcpy_number = 0;
	img_mser* parent_mser;

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];	
		parent_region = cur_region->m_parent;

		img_mser& cur_mser = t_msers[i];
		cur_mser.m_points = pt_offset[i];

		parent_mser = NULL;
		if (parent_region != NULL && parent_region->m_region_flag == mser_region::Flag_Valid) {
			parent_mser = &t_msers[parent_region->m_mser_index];
			parent_mser->m_rect.adjust_by_append_rect(cur_mser.m_rect);
		}

		if (parent_mser != NULL) {
			if (cur_mser.m_memory_type == img_mser::Memory_Type_Share) {

				if (parent_mser->m_memory_type == img_mser::Memory_Type_Share) {
					memcpy(parent_mser->m_points, cur_mser.m_points, sizeof(mt_point) * cur_mser.m_size);
					parent_mser->m_points += cur_mser.m_size;
					memcpy_number += 1;
				} else {
					mt_point* child_start = parent_mser->m_points + 1 + parent_mser->m_points[0].m_x;
					i32 child_index = parent_mser->m_points[0].m_y;

					i32* child_data = (i32*)child_start;
					child_data[parent_mser->m_points[0].m_y++] = (i32)(&cur_mser - parent_mser);
				}
			} else {
				mt_point* child_start = parent_mser->m_points + 1 + parent_mser->m_points[0].m_x;
				i32 child_index = parent_mser->m_points[0].m_y;

				i32* child_data = (i32*)child_start;
				child_data[parent_mser->m_points[0].m_y++] = (i32)(&cur_mser - parent_mser);
			}
		}
	}

	//basiclog_info2(u);
	t.end();
}

void img_ppms_mser::extract_pixel_parallel_4_parallel_impl(img_multi_msers& msers, u8 gray_mask) {
	//total block self pixel size, mser block self pixel size, self rect,
	i32 one_block_mser_info_size = m_gray_order_region_size * 3 + 1;
	i32 total_block_mser_info_size = one_block_mser_info_size * 4;

	i32* region_heap = (i32*)m_heap;
	i32 region_heap_size = (i32)(m_pinfo[3].m_mser_regions_end - m_mser_regions);
	i32 free_memory_size = m_heap_size * (sizeof(i16*)) / sizeof(i32) - region_heap_size;
	i32* mser_block_indexes = NULL;

	if (free_memory_size >= total_block_mser_info_size) {
		mser_block_indexes = (i32*)(m_heap) + region_heap_size;
	} else {
		basiclog_info2(sys_strcombine()<<L"needs to allocate i32: "<<total_block_mser_info_size);
		mser_block_indexes = (i32*)malloc(sizeof(i32) * total_block_mser_info_size);
	}

	memset(mser_block_indexes, 0, sizeof(i32) * total_block_mser_info_size);

	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);
	region_memory_size = 0;

	mser_region* cur_region;
	mser_region* real_region;
	mser_region* parent_region;
	
	basiclog_assert2(m_recursive_point_threshold != -1);

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		cur_mser.m_size = cur_region->m_size;
		cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
		cur_region->m_child_pixel_size = 0;
		cur_region->m_child_memory_number = 0;
	}

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		if (cur_region->m_child_memory_number == 0 || cur_mser.m_size <= m_recursive_point_threshold) {
			cur_mser.m_memory_type = img_mser::Memory_Type_Share_Parallel_4;
			region_memory_size += 4 + cur_mser.m_size;
		} else {
			cur_mser.m_memory_type = img_mser::Memory_Type_Recursive_Parallel_4;
			region_memory_size += 4 + cur_mser.m_size - cur_region->m_child_pixel_size + (2 + cur_region->m_child_memory_number) / 2;
		}

		region_heap[cur_region - m_mser_regions] = i;

		for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
		}

		parent_region = cur_region->m_parent;

		if (real_region != NULL) {
			real_region->m_child_memory_number += 1;
			real_region->m_child_pixel_size += cur_mser.m_size;
		}

		cur_region->m_parent = real_region;

		while (parent_region != NULL && parent_region != real_region) {
			cur_region = parent_region->m_parent;

			parent_region->m_parent = real_region;
			parent_region = cur_region;
		}
	}

	mser_region* end_region;
	for (i32 i = 0; i < 4; ++i) {

		cur_region = m_pinfo[i].m_mser_regions;
		end_region = m_pinfo[i].m_mser_regions_end;

		for (; cur_region != end_region; ++cur_region) {
			if (cur_region->m_region_flag == mser_region::Flag_Valid) {
			} else {
				//find real region
				if (cur_region->m_assigned_pointer == 1) {
					continue;
				}

				for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid && real_region->m_assigned_pointer == 0; real_region = real_region->m_parent) {
				}

				i32 real = -1;

				if (real_region != NULL) {
					real = region_heap[real_region - m_mser_regions];
				}

				for (parent_region = cur_region; parent_region != NULL && parent_region != real_region; parent_region = parent_region->m_parent) {				
					region_heap[parent_region - m_mser_regions] = real;
					parent_region->m_assigned_pointer = 1;
				}
			}
		}
	}

	struct mser_block_info {
		i32 block_self_pixel_size;
		i16 top, left, bottom, right;
	};
	
	
	#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 4; ++i) {
		mser_block_info* mser_block_infos = (mser_block_info*)(mser_block_indexes + one_block_mser_info_size * i + 1);

		i32 cur_memory_size = 0;
		//sys_timer ct(sys_strcombine()<<L"ct "<<i);
		//ct.begin();

		u32* link_points = m_pinfo[i].m_points + 1 + m_pinfo[i].m_width_with_boundary;
		i32 width = m_pinfo[i].m_width;
		i32 height = m_pinfo[i].m_height;

		for (i32 row = 0; row < height; ++row) {
			for (i32 col = 0; col < width; ++col) {
				i32 mser_index = region_heap[*link_points - m_boundary_pixel];				
				*link_points = mser_index;

				if (mser_index != -1) {
					cur_memory_size += 1;
					mser_block_infos[mser_index].block_self_pixel_size += 1;
				}

				++link_points;	
			}

			link_points += 2;
		}

		mser_block_indexes[one_block_mser_info_size * i] = cur_memory_size;
	}

	i32 self_memory_size_index[4] = {0, 0, 0, 0};
	self_memory_size_index[1] = one_block_mser_info_size;
	self_memory_size_index[2] = self_memory_size_index[1] + one_block_mser_info_size;
	self_memory_size_index[3] = self_memory_size_index[2] + one_block_mser_info_size;

	i32 self_memory_size[4] = {mser_block_indexes[self_memory_size_index[0]], mser_block_indexes[self_memory_size_index[1]], mser_block_indexes[self_memory_size_index[2]], mser_block_indexes[self_memory_size_index[3]]};

	i32 block_memory_start_indexes[4] = {0, 0, 0, 0};
	block_memory_start_indexes[1] = region_memory_size - self_memory_size[3] - self_memory_size[2] - self_memory_size[1];
	block_memory_start_indexes[2]= region_memory_size - self_memory_size[3] - self_memory_size[2];
	block_memory_start_indexes[3]= region_memory_size - self_memory_size[3];

	//basiclog_info2(sys_strcombine()<<block_memory_start_indexes[0]<<L" "<<block_memory_start_indexes[1]<<L" "<<block_memory_start_indexes[2]<<L" "<<block_memory_start_indexes[3]);

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);

	//set points
	mser_block_info* mser_block_info_pointer[4] = {(mser_block_info*)(mser_block_indexes + self_memory_size_index[0] + 1),
		(mser_block_info*)(mser_block_indexes + self_memory_size_index[1] + 1),
		(mser_block_info*)(mser_block_indexes + self_memory_size_index[2] + 1),
		(mser_block_info*)(mser_block_indexes + self_memory_size_index[3] + 1)};

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];
		
		if (cur_mser.m_memory_type == img_mser::Memory_Type_Share_Parallel_4) {
			cur_mser.m_points = memory + block_memory_start_indexes[0];

			block_memory_start_indexes[0] += 4;

			cur_mser.m_points[0].m_x = block_memory_start_indexes[0];
			cur_mser.m_points[1].m_x = block_memory_start_indexes[1];
			cur_mser.m_points[2].m_x = block_memory_start_indexes[2];
			cur_mser.m_points[3].m_x = block_memory_start_indexes[3];

			cur_mser.m_points[0].m_y = block_memory_start_indexes[0] + mser_block_info_pointer[0]->block_self_pixel_size;

			block_memory_start_indexes[0] += cur_region->m_child_pixel_size + mser_block_info_pointer[0]->block_self_pixel_size;
			block_memory_start_indexes[1] += mser_block_info_pointer[1]->block_self_pixel_size;
			block_memory_start_indexes[2] += mser_block_info_pointer[2]->block_self_pixel_size;
			block_memory_start_indexes[3] += mser_block_info_pointer[3]->block_self_pixel_size;

			cur_mser.m_points[1].m_y = block_memory_start_indexes[1];
			cur_mser.m_points[2].m_y = block_memory_start_indexes[2];
			cur_mser.m_points[3].m_y = block_memory_start_indexes[3];
			
			mser_block_info_pointer[0]->block_self_pixel_size = cur_mser.m_points[0].m_x;
			mser_block_info_pointer[1]->block_self_pixel_size = cur_mser.m_points[1].m_x;
			mser_block_info_pointer[2]->block_self_pixel_size = cur_mser.m_points[2].m_x;
			mser_block_info_pointer[3]->block_self_pixel_size = cur_mser.m_points[3].m_x;
		} else {
			cur_mser.m_points = memory + block_memory_start_indexes[0];
			i32 child_index_size = (2 + cur_region->m_child_memory_number) / 2;

			block_memory_start_indexes[0] += 4 + child_index_size;

			cur_mser.m_points[4].m_x = 0;	//current child number

			cur_mser.m_points[0].m_x = block_memory_start_indexes[0];
			cur_mser.m_points[1].m_x = block_memory_start_indexes[1];
			cur_mser.m_points[2].m_x = block_memory_start_indexes[2];
			cur_mser.m_points[3].m_x = block_memory_start_indexes[3];

			block_memory_start_indexes[0] += mser_block_info_pointer[0]->block_self_pixel_size;
			block_memory_start_indexes[1] += mser_block_info_pointer[1]->block_self_pixel_size;
			block_memory_start_indexes[2] += mser_block_info_pointer[2]->block_self_pixel_size;
			block_memory_start_indexes[3] += mser_block_info_pointer[3]->block_self_pixel_size;

			cur_mser.m_points[0].m_y = block_memory_start_indexes[0];
			cur_mser.m_points[1].m_y = block_memory_start_indexes[1];
			cur_mser.m_points[2].m_y = block_memory_start_indexes[2];
			cur_mser.m_points[3].m_y = block_memory_start_indexes[3];

			mser_block_info_pointer[0]->block_self_pixel_size = cur_mser.m_points[0].m_x;
			mser_block_info_pointer[1]->block_self_pixel_size = cur_mser.m_points[1].m_x;
			mser_block_info_pointer[2]->block_self_pixel_size = cur_mser.m_points[2].m_x;
			mser_block_info_pointer[3]->block_self_pixel_size = cur_mser.m_points[3].m_x;
		}

		cur_mser.m_gray_level = cur_region->m_gray_level;
		cur_region->m_mser_index = i;

		mser_block_info_pointer[0]->left = -1;
		mser_block_info_pointer[1]->left = -1;
		mser_block_info_pointer[2]->left = -1;
		mser_block_info_pointer[3]->left = -1;

		mser_block_info_pointer[0] += 1;
		mser_block_info_pointer[1] += 1;
		mser_block_info_pointer[2] += 1;
		mser_block_info_pointer[3] += 1;
	}

#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 4; ++i) {
		mt_point* pt;
		mser_block_info* mser_block_infos = (mser_block_info*)(mser_block_indexes + one_block_mser_info_size * i + 1);
		mser_block_info* cur_mser_block_info;
		u32* link_points = m_pinfo[i].m_points + 1 + m_pinfo[i].m_width_with_boundary;
		i32 width = m_pinfo[i].m_width;
		i32 height = m_pinfo[i].m_height;
		i32 left_offset = m_pinfo[i].m_left;
		i32 top_pffset = m_pinfo[i].m_top;

		for (i32 row = 0; row < height; ++row) {
			for (i32 col = 0; col < width; ++col) {
				i32 mser_index = *link_points;

				if (mser_index != -1) {
					cur_mser_block_info = mser_block_infos + mser_index;
					pt = memory + cur_mser_block_info->block_self_pixel_size;
					pt->m_x = col + left_offset;
					pt->m_y = row + top_pffset;

					cur_mser_block_info->block_self_pixel_size += 1;

					if (cur_mser_block_info->left == -1) {
						cur_mser_block_info->left = pt->m_x;
						cur_mser_block_info->right = pt->m_x;
						cur_mser_block_info->top = pt->m_y;
						cur_mser_block_info->bottom = pt->m_y;
					} else {
						if (pt->m_x < cur_mser_block_info->left) {
							cur_mser_block_info->left = pt->m_x;
						} else if (pt->m_x > cur_mser_block_info->right) {
							cur_mser_block_info->right = pt->m_x;
						}

						if (pt->m_y < cur_mser_block_info->top) {
							cur_mser_block_info->top = pt->m_y;
						} else if (pt->m_y > cur_mser_block_info->bottom) {
							cur_mser_block_info->bottom = pt->m_y;
						}
					}
				}

				++link_points;	
			}

			link_points += 2;
		}
	}


	mser_block_info_pointer[0] = (mser_block_info*)(mser_block_indexes + self_memory_size_index[0] + 1);
	mser_block_info_pointer[1] = (mser_block_info*)(mser_block_indexes + self_memory_size_index[1] + 1);
	mser_block_info_pointer[2] = (mser_block_info*)(mser_block_indexes + self_memory_size_index[2] + 1);
	mser_block_info_pointer[3] = (mser_block_info*)(mser_block_indexes + self_memory_size_index[3] + 1);


	//extract global
	i32 block_self_pixel_size;
	i32 left, top, right, bottom;
	mt_rect temp_rect;
	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		parent_region = cur_region->m_parent;
		img_mser* parent_mser = NULL;

		if (parent_region != NULL && parent_region->m_region_flag == mser_region::Flag_Valid) {
			parent_mser = &t_msers[parent_region->m_mser_index];
		}

		i32 memory_offset = (i32)(cur_mser.m_points - memory);

		//rect
		left = -1;

		for (i32 k = 0; k < 4; ++k) {
			if (mser_block_info_pointer[k]->left == -1) {
				continue;
			}

			if (left == -1) {
				left = mser_block_info_pointer[k]->left;
				right = mser_block_info_pointer[k]->right;
				top = mser_block_info_pointer[k]->top;
				bottom = mser_block_info_pointer[k]->bottom;
			} else {
				if (mser_block_info_pointer[k]->left < left) {
					left = mser_block_info_pointer[k]->left;
				} else if (mser_block_info_pointer[k]->right > right) {
					right = mser_block_info_pointer[k]->right;
				}

				if (mser_block_info_pointer[k]->top < top) {
					top = mser_block_info_pointer[k]->top;
				} else if (mser_block_info_pointer[k]->bottom > bottom) {
					bottom = mser_block_info_pointer[k]->bottom;
				}
			}
		}

		if (cur_mser.m_memory_type == img_mser::Memory_Type_Share_Parallel_4) {



			if (parent_mser != NULL) {
				if (parent_mser->m_memory_type == img_mser::Memory_Type_Share_Parallel_4) {
					for (i32 k = 0; k < 4; ++k) {
						block_self_pixel_size = cur_mser.m_points[k].m_y - cur_mser.m_points[k].m_x;
						
						if (block_self_pixel_size > 0) {
							//basiclog_info2((cur_mser.m_points[k].m_x + memory)->m_y);
							memcpy(parent_mser->m_points[0].m_y + memory, cur_mser.m_points[k].m_x + memory, sizeof(mt_point) * block_self_pixel_size);
							parent_mser->m_points[0].m_y += block_self_pixel_size;
						}
						
					}

				} else {
					i32* child_indexes = (i32*)(parent_mser->m_points + 4);
					child_indexes[++child_indexes[0]] = (i32)(&cur_mser - parent_mser);
				}
			}
		} else {
			if (parent_mser != NULL) {
				i32* child_indexes = (i32*)(parent_mser->m_points + 4);
				child_indexes[++child_indexes[0]] = (i32)(&cur_mser - parent_mser);
			}
		}

		temp_rect.set_rect(left, top, right - left + 1, bottom - top + 1);
		cur_mser.m_rect.adjust_by_append_rect(temp_rect);

		if (parent_mser != NULL) {
			parent_mser->m_rect.adjust_by_append_rect(cur_mser.m_rect);
		}

		cur_mser.m_points[0].m_x -= memory_offset;
		cur_mser.m_points[0].m_y -= memory_offset;
		cur_mser.m_points[1].m_x -= memory_offset;
		cur_mser.m_points[1].m_y -= memory_offset;
		cur_mser.m_points[2].m_x -= memory_offset;
		cur_mser.m_points[2].m_y -= memory_offset;
		cur_mser.m_points[3].m_x -= memory_offset;
		cur_mser.m_points[3].m_y -= memory_offset;

		mser_block_info_pointer[0] += 1;
		mser_block_info_pointer[1] += 1;
		mser_block_info_pointer[2] += 1;
		mser_block_info_pointer[3] += 1;
	}

	if (free_memory_size < total_block_mser_info_size) {
		free(mser_block_indexes);
	}
}

void img_ppms_mser::calculate_variation(mser_region* region) {
	u32 grayLevelThreshold = region->m_gray_level + m_delta;

	mser_region* temp = region;
	mser_region* parent;

	get_set_real_parent_for_merged(temp, parent);

	//if (parent != NULL) {
	//	basiclog_assert2(parent->m_region_flag != mser_region::Flag_Merged);
	//}

	mser_region* temp_parent;

	while (NULL != parent && parent->m_gray_level <= grayLevelThreshold) {
		temp = parent;

		get_set_real_parent_for_merged(parent, temp_parent);

		parent = temp_parent;

		//if (parent != NULL) {
		//	basiclog_assert2(parent->m_region_flag != mser_region::Flag_Merged);
		//}
	}

	if (parent != NULL || temp->m_gray_level == grayLevelThreshold) {
		//get_set_real_parent_for_merged(region, temp_parent);

		region->m_var = (temp->m_size - region->m_size) / (f32)region->m_size;
		//basiclog_assert2(region->m_var > 0);
	} else {
		region->m_var = -1;
	}

	//region->m_calculated_var = 1;
}

void img_ppms_mser::get_duplicated_regions(vector<mser_region*>& duplicated_regions, mser_region* stable_region, mser_region* begin_region) {
	mser_region* parentRegion = begin_region->m_parent;

	while (true) {
		if (NULL == parentRegion) {
			break;
		}

		//basiclog_assert2(parentRegion->m_region_flag != mser_region::Flag_Merged);

		if (parentRegion->m_size > m_max_point) {
			//如果父亲节点较大,无须在寻找重复的父亲,因为父亲注定会被删掉
			break;
		}

		double variationFromBegin = (parentRegion->m_size - stable_region->m_size) / (double)stable_region->m_size;

		if (variationFromBegin > m_duplicated_variation) {
			break;
		}

		if (mser_region::Flag_Valid == parentRegion->m_region_flag) {
			basiclog_warning2(L"Too big mDuplicateVariantion");

			parentRegion = parentRegion->m_parent;
			continue;
		}

		if (mser_region::Flag_Invalid == parentRegion->m_region_flag) {
			parentRegion = parentRegion->m_parent;
			continue;
		}

		duplicated_regions.push_back(parentRegion);
		parentRegion = parentRegion->m_parent;
	}
}