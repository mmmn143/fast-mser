#include "stdafx.h"

#include "img_ms_mser.h"

img_ms_mser::img_ms_mser() 
	: img_mser_base() {

		m_extended_image = NULL;
		m_extended_image_size = 0;

		m_heap = NULL;
		m_heap_size = 0;

		m_points = NULL;
		m_point_size = 0;

		m_mser_regions = NULL;
		m_mser_region_size = 0;

		m_gray_order_regions = NULL;
		m_gray_order_region_size = 0;
		m_gray_order_regions_memory_size = 0;

		m_exceed_times = 0;
}

img_ms_mser::~img_ms_mser() {
	basicsys_free(m_heap);
	basicsys_free(m_points);
	basicsys_free(m_mser_regions);
	basicsys_free(m_extended_image);
	basicsys_free(m_gray_order_regions);
}

void img_ms_mser::clear_memory_cache() {
	basicsys_free(m_extended_image);
	basicsys_free(m_points);
	basicsys_free(m_mser_regions);
	basicsys_free(m_heap);
	basicsys_free(m_gray_order_regions);

	m_heap_size = 0;
	m_extended_image_size = 0;
	m_point_size = 0;
	m_mser_region_size = 0;
	m_gray_order_regions_memory_size = 0;
	m_exceed_times = 0;

	vector<mser_region*> temp;
	m_remove_duplicated_memory_helper.swap(temp);
}

void img_ms_mser::allocate_memory(const mt_mat& img, const img_mask_info<u8>& mask) {
	m_image_height = img.size()[0];
	m_image_width = img.size()[1];

	u32 pt_size = (img.size()[0] + 2) * (img.size()[1] + 2);

	if (pt_size != m_point_size) {
		if (m_points != NULL) {
			free(m_points);
		}

		m_point_size = pt_size;
		m_points = (u32*)malloc(sizeof(u32) * m_point_size);
	}

	u32 extended_image_size = pt_size;

	if (extended_image_size != m_extended_image_size) {
		if (m_extended_image != NULL) {
			free(m_extended_image);
		}

		m_extended_image_size = extended_image_size;
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

	m_heap_start[0] = &m_heap[0];

	struct temp_mser_region {
		u8 m_gray_level;
		u8 m_flags;

		i32 m_size;

		union 
		{
			f32 m_var;
			u32 m_unmerged_size;
			i32 m_child_memory_number;
			i32 m_mser_index;
		};

		temp_mser_region* m_parent;
	};

	m_channel_total_running_memory += pt_size * sizeof(u32) + pt_size * sizeof(u8) + rg_size * sizeof(temp_mser_region) + h_size * sizeof(u32*)
		+ sizeof(u32) * 257 * 3;

	i32 row_step = img.size()[1] + 2;

	m_mask_mask = 0;

	if (img_Connected_Type_8_Neibour == m_connected_type) {
		m_dir[1] = 1;
		m_dir[2] = 1 - row_step;
		m_dir[3] = -row_step;
		m_dir[4] = -1 - row_step;
		m_dir[5] = -1;
		m_dir[6] = -1 + row_step;
		m_dir[7] = row_step;
		m_dir[8] = 1 + row_step;

		m_dir_mask = 0xF0000000;		
		m_dir_shift = 28;
		m_boundary_pixel = 9 << m_dir_shift;
	} else {
		m_dir[1] = 1;
		m_dir[2] = -1;
		m_dir[3] = row_step;
		m_dir[4] = -row_step;
		m_dir_mask = 0xe0000000;
		m_dir_shift = 29;
		m_boundary_pixel = 5 << m_dir_shift;
	}
}

void img_ms_mser::pre_process_image(const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask) {
	i32 extended_image_width = img.size()[1] + 2;
	u32* points = m_points;
	i32 row;
	i32 col;
	i32 width = img.size()[1];
	i32 height = img.size()[0];

	if (gray_mask == 0 || !m_from_min_max[0]) {
		u8* extended_image_data = m_extended_image;
		const u8* img_data = img.ptr<u8>(0, 0, 0);
		memset(m_level_size, 0, sizeof(u32) * 257);

		//set the first row to be -1
		for (col = 0; col < extended_image_width; ++col) {
			*points++ = m_boundary_pixel;
		}

		extended_image_data += extended_image_width + 1;

		if (mask.m_mask_type == img_Mask_Type_Null) {
			//set middle row
			for (row = 0; row < height; ++row) {
				*points++ = m_boundary_pixel;

				for (col = 0; col < width; ++col) {
					m_level_size[*img_data]++;
					*extended_image_data++ = *img_data++;
					*points++ = 0;
				}

				*points++ = m_boundary_pixel;
				extended_image_data += 2;
			}			
		}

		//set the last row to be -1
		for (col = 0; col < extended_image_width; ++col) {
			*points++ = m_boundary_pixel;
		}
	} else {
		points += extended_image_width + 1;
		//set the first row to be -1
		if (mask.m_mask_type == img_Mask_Type_Null) {
			//set middle row
			for (row = 0; row < height; ++row) {
				for (col = 0; col < width; ++col) {
					*points++ = 0;
				}

				points += 2;
			}
		}

		i32 temp;
		for(i32 i = 0; i < 128; ++i) {
			temp = m_level_size[i];
			m_level_size[i] = m_level_size[255 - i];
			m_level_size[255 - i] = temp;
		}
	}

	m_heap_start[0][0] = 0;
	for ( i32 i = 1; i < 257; i++ ) {
		m_heap_start[i] = m_heap_start[i-1] + m_level_size[i-1]+1;
		m_heap_start[i][0] = 0;
	}
}

void img_ms_mser::build_tree(const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask) {
	pre_process_image(img, mask, gray_mask);

#define get_gray(ptr)	((m_mask_mask == 0 || (*ptr & m_mask_mask) == 0) ? (*(m_extended_image + (ptr - m_points)) ^ gray_mask) : 256)

	i32 erCount = 0;

	u32*** heap_cur = m_heap_start;
	mser_region* regions = m_mser_regions;
	connected_comp* comptr = m_comp; 
	u32* points = m_points + 1 + img.size()[1] + 2;
	mser_region* startRegion = regions;
	i32* dir = m_dir;
	i32 cols = img.size()[1];

	i16 curr_gray = get_gray(points);
	
	comptr->m_gray_level = 257;	//process mask
	comptr++;
	comptr->m_gray_level = curr_gray;
	init_comp(comptr, regions);
	*points = 1 << m_dir_shift;
	heap_cur += curr_gray;

	int mser_number = 0;
	
	i16 nbr_gray;
	i32 nbr_index;
	u32* nbr_points;

	for ( ; ; ) {
		nbr_index = *points >> m_dir_shift;

		// take tour of all the 4 or 8 directions
		while (nbr_index <= 4) {
			// get the neighbor
			nbr_points = points + m_dir[nbr_index];

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
					init_comp(comptr, regions);
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
			++erCount;
			comptr->m_region->m_size = comptr->m_size;

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
					new_region(comptr, regions);


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
							new_region(comptr, regions);

							mser_number += 1;
							break;
						}
					}
				}
			} else
				break;
		}
	}

	m_er_number = (i32)(regions - startRegion);
	m_mser_regions_end = regions;

	basiclog_info2(sys_strcombine()<<"er number "<<erCount << " region number " << (i64)(regions - startRegion) << " mser number " << mser_number);
}

void img_ms_mser::init_comp(connected_comp* comptr, mser_region*& region) {
	comptr->m_size = 0;
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;
	region->m_assigned_pointer = 0;
	region->m_size = 0;
	region->m_parent = NULL;
	region->m_calculated_var = 0;
	comptr->m_region = region;
	++region;
}

void img_ms_mser::new_region(connected_comp* comptr, mser_region*& region) {
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_size = 0;
	region->m_parent = NULL;
	region->m_assigned_pointer = 0;
	region->m_calculated_var = 0;
	comptr->m_region = region;
	++region;
}

void img_ms_mser::recognize_mser() {
	recognize_mser_normal();
}

void img_ms_mser::recognize_mser_normal() {
	memset(m_region_level_size, 0, sizeof(u32) * 257);

	i32 totalUnkonwSize = 0;
	i32 nmsCount = 0;

	i32 bad_variance_number = 0;

	for (u32 i = 0; i < m_er_number; ++i) {
		mser_region& cur_region = m_mser_regions[i];

		if (m_delta > 0) {
			calculate_variation(cur_region);

			if (cur_region.m_var > m_stable_variation) {
				cur_region.m_region_flag = mser_region::Flag_Invalid;
				bad_variance_number++;
				continue;
			}
		}

		if (cur_region.m_size < m_min_point || cur_region.m_size > m_max_point || NULL == cur_region.m_parent) {
			cur_region.m_region_flag = mser_region::Flag_Invalid;
			continue;
		}

		++totalUnkonwSize;
		++m_region_level_size[cur_region.m_gray_level];
	}




	i32 beforeUnkonwSize = totalUnkonwSize;

	//real image, this is faster, for 10M image, this cost 96, while previous costs 149
	if (m_delta > 0 && m_nms_similarity >= 0) {
		for (u32 i = 0; i < m_er_number; ++i) {
			mser_region* cur_region = &m_mser_regions[i];
			mser_region* parentRegion = cur_region->m_parent;

			if (parentRegion == NULL || (cur_region->m_region_flag == mser_region::Flag_Invalid && parentRegion->m_region_flag == mser_region::Flag_Invalid)) {
				continue;
			}

			if (cur_region->m_var >= 0 && parentRegion != NULL && parentRegion->m_var >= 0 && parentRegion->m_gray_level == cur_region->m_gray_level + 1) {
				double subValue = parentRegion->m_var - cur_region->m_var;
				if (subValue > m_nms_similarity) {
					if (mser_region::Flag_Invalid != parentRegion->m_region_flag) {
						parentRegion->m_region_flag = mser_region::Flag_Invalid;
						--m_region_level_size[parentRegion->m_gray_level];
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

	nmsCount = beforeUnkonwSize - totalUnkonwSize;

	basiclog_info2(sys_strcombine()<<"bad_variance_number "<<bad_variance_number);
	basiclog_info2(sys_strcombine()<<"nms count "<<nmsCount);

	remove_duplicated();	
}


void img_ms_mser::remove_duplicated() {
	mt_helper::integral_array<u32>(m_start_indexes, m_region_level_size, m_region_level_size + 256);

	m_gray_order_region_size = m_start_indexes[256];
	
	if (m_gray_order_regions_memory_size > m_gray_order_region_size * 5) {
		++m_exceed_times;

		if (m_exceed_times > 10) {
			m_gray_order_regions_memory_size = m_gray_order_region_size;
			free(m_gray_order_regions);
			m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
		}
	} else {
		m_exceed_times = 0;
		m_gray_order_regions_memory_size = m_gray_order_region_size;
		m_gray_order_region_size = m_gray_order_region_size;

		if (m_gray_order_regions != NULL) {
			free(m_gray_order_regions);
		}

		m_gray_order_regions = (mser_region**)malloc(sizeof(mser_region*) * m_gray_order_regions_memory_size);
	}

	mser_region* cur_region = m_mser_regions;
	for (u32 i = 0; i < m_er_number; ++i) {
		if (cur_region->m_region_flag != mser_region::Flag_Invalid) {
			m_gray_order_regions[m_start_indexes[cur_region->m_gray_level]++] = cur_region;
		}

		++cur_region;
	}

	if (m_duplicated_variation > 0) {
		i32 validCount = 0;

		m_remove_duplicated_memory_helper.reserve(100);

		// Remove duplicated regions
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			mser_region* currentRegion = m_gray_order_regions[i];

			if (currentRegion->m_region_flag != mser_region::Flag_Unknow) {
				continue;
			}

			m_remove_duplicated_memory_helper.clear();
			m_remove_duplicated_memory_helper.push_back(currentRegion);
			get_duplicated_regions(m_remove_duplicated_memory_helper, currentRegion, currentRegion);

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
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
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
}

void img_ms_mser::calculate_variation(mser_region& region) {
	u32 grayLevelThreshold = region.m_gray_level + m_delta;

	mser_region* parent = region.m_parent;
	mser_region* temp = &region;

	while (parent != NULL && parent->m_gray_level <= grayLevelThreshold) {
		temp = parent;
		parent = parent->m_parent;
	}

	if (parent != NULL || temp->m_gray_level == grayLevelThreshold) {
		region.m_var = (temp->m_size - region.m_size) / (f32)region.m_size;
	} else {
		region.m_var = -1;
	}
}

void img_ms_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	if (m_gray_order_region_size <= 0) {
		return;
	}

	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	t_msers.resize(m_gray_order_region_size);
	region_memory_size = 0;

	mser_region* cur_region;
	mser_region* real_region;
	mser_region* parent_region;


	img_mser** mser_heap = (img_mser**)m_heap;
	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	mt_point* memory_offset;

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
			cur_mser.m_size = cur_region->m_size;

			if (m_recursive_point_threshold == -1) {
				cur_mser.m_memory_type = img_mser::Memory_Type_Self;
				cur_mser.m_points = new mt_point[cur_region->m_size];
			} else {
				cur_mser.m_memory_type = img_mser::Memory_Type_Share;
				cur_mser.m_points = memory_offset;
				memory_offset += cur_mser.m_size;
			}

			cur_region->m_mser_index = i;
			cur_region->m_child_pixel_size = 0;

			cur_mser.m_rect.m_left = -1;

			mser_heap[cur_region - m_mser_regions] = &cur_mser;
		}

		// compute child pixel size
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			mser_region* cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];

			for (real_region = cur_region->m_parent; real_region != NULL && real_region->m_region_flag != mser_region::Flag_Valid; real_region = real_region->m_parent) {
			}

			parent_region = cur_region->m_parent;

			if (real_region != NULL) {
				real_region->m_child_pixel_size += cur_mser.m_size;
			}

			cur_region->m_parent = real_region;

			while (parent_region != NULL && parent_region != real_region) {
				cur_region = parent_region->m_parent;

				parent_region->m_parent = real_region;
				parent_region = cur_region;
			}
		}

		int number = 0;
		int total = 0;

		for (cur_region = m_mser_regions; cur_region != m_mser_regions_end; ++cur_region) {

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
	} else {
		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];
			cur_mser.m_gray_level = cur_region->m_gray_level ^ gray_mask;
			cur_mser.m_size = cur_region->m_size;
			cur_mser.m_rect.m_left = -1;
			cur_region->m_child_pixel_size = 0;
			cur_region->m_child_memory_number = 0;
		}

		for (u32 i = 0; i < m_gray_order_region_size; ++i) {
			cur_region = m_gray_order_regions[i];
			img_mser& cur_mser = t_msers[i];
			mser_heap[cur_region - m_mser_regions] = &cur_mser;

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

			cur_region->m_mser_index = i;
		}

		mser_region* end_region;

		cur_region = m_mser_regions;
		end_region = m_mser_regions_end;

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

	int u = 0;
	//second, for each link point, we assign point value for its corresponding MSER
	u32* link_points = m_points + m_image_width + 3;

	for (i32 row = 0; row < m_image_height; ++row) {
		for (i32 col = 0; col < m_image_width; ++col) {
			img_mser* mser = mser_heap[*link_points - m_boundary_pixel];

			if (mser != NULL) {
				mt_point& cpt = *mser->m_points;

				cpt.m_x = col;
				cpt.m_y = row;

				if (mser->m_rect.m_left == -1) {
					mser->m_rect.m_left = col;
					mser->m_rect.m_width = col;
					mser->m_rect.m_top = row;
					mser->m_rect.m_height = row;
				} else {
					if (col < mser->m_rect.m_left) {
						mser->m_rect.m_left = col;
					} else if (col > mser->m_rect.m_width) {
						mser->m_rect.m_width = col;
					}

					if (row < mser->m_rect.m_top) {
						mser->m_rect.m_top = row;
					} else if (row > mser->m_rect.m_height) {
						mser->m_rect.m_height = row;
					}
				}

				++mser->m_points;
				++u;
			}

			++link_points;
		}

		link_points += 2;
	}

	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		img_mser& cur_mser = t_msers[i];
		cur_region = m_gray_order_regions[i];

		cur_mser.m_rect.m_width = cur_mser.m_rect.m_width - cur_mser.m_rect.m_left + 1;
		cur_mser.m_rect.m_height = cur_mser.m_rect.m_height - cur_mser.m_rect.m_top + 1;

		if (cur_mser.m_memory_type == img_mser::Memory_Type_Recursive) {
			cur_mser.m_points -= 1 + cur_mser.m_size - cur_region->m_child_pixel_size;
		}
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
			parent_mser->m_rect.adjust_by_append_rect(cur_mser.m_rect);
		}

		if (cur_mser.m_memory_type == img_mser::Memory_Type_Share || cur_mser.m_memory_type == img_mser::Memory_Type_Self) {
			cur_mser.m_points -= cur_mser.m_size;
		}

		if (parent_mser != NULL) {
			if (cur_mser.m_memory_type == img_mser::Memory_Type_Share || cur_mser.m_memory_type == img_mser::Memory_Type_Self) {

				if (parent_mser->m_memory_type == img_mser::Memory_Type_Share || cur_mser.m_memory_type == img_mser::Memory_Type_Self) {
					memcpy(parent_mser->m_points, cur_mser.m_points, sizeof(mt_point) * cur_mser.m_size);
					parent_mser->m_points += cur_mser.m_size;
					memcpy_number += 1;
				} else {
					mt_point* child_start = parent_mser->m_points + 1 + parent_mser->m_points[0].m_x;

					i32* child_data = (i32*)child_start;
					child_data[parent_mser->m_points[0].m_y++] = (i32)(&cur_mser - parent_mser);
				}
			} else {
				mt_point* child_start = parent_mser->m_points + 1 + parent_mser->m_points[0].m_x;

				i32* child_data = (i32*)child_start;
				child_data[parent_mser->m_points[0].m_y++] = (i32)(&cur_mser - parent_mser);
			}
		}
	}
}

void img_ms_mser::get_duplicated_regions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion) {
	mser_region* parentRegion = beginRegion->m_parent;

	while (true) {
		if (NULL == parentRegion) {
			break;
		}

		if (parentRegion->m_size > m_max_point) {
			// If the size of parent is too large, we do not need to find duplicated parent regions (parent regions will be delete absolutely). 
			break;
		}

		double variationFromBegin = (parentRegion->m_size - stableRegion->m_size) / (double)stableRegion->m_size;

		if (variationFromBegin > m_duplicated_variation) {
			break;
		}

		if (mser_region::Flag_Valid == parentRegion->m_region_flag) {
			basiclog_warning2("Too big mDuplicateVariantion");

			parentRegion = parentRegion->m_parent;
			continue;
		}

		if (mser_region::Flag_Invalid == parentRegion->m_region_flag) {
			parentRegion = parentRegion->m_parent;
			continue;
		}

		duplicatedRegions.push_back(parentRegion);
		parentRegion = parentRegion->m_parent;
	}
}