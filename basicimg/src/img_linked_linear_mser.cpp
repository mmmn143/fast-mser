#include "stdafx.h"

#include "img_linked_linear_mser.h"

// clear the connected component in stack
void img_linked_linear_mser::init_comp(connected_comp* comptr, mser_region*& region) {
	comptr->m_size = 0;
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_calculated_var = 0;
	region->m_short_cut_offset = 0;
	region->m_parent = NULL;
	comptr->m_region = region;
	++region;
}

void img_linked_linear_mser::new_region(connected_comp* comptr, mser_region*& region) {
	region->m_gray_level = (u8)comptr->m_gray_level;
	region->m_region_flag = mser_region::Flag_Unknow;

	region->m_calculated_var = 0;
	region->m_short_cut_offset = 0;
	region->m_parent = NULL;
	comptr->m_region = region;
	++region;
}

void img_linked_linear_mser::merge_comp(connected_comp* comp1, connected_comp* comp2) {	
	comp1->m_region->m_parent = comp2->m_region;

	if (comp2->m_size > 0) {
		basiclog_assert2(m_points[comp2->m_tail].m_next == -1);
		basiclog_assert2(m_points[comp1->m_tail].m_next == -1);

		m_points[comp2->m_tail].m_next = comp1->m_head;
		comp2->m_tail = comp1->m_tail;	
	} else {
		comp2->m_head = comp1->m_head;
		comp2->m_tail = comp1->m_tail;	
	}

	comp2->m_tail = comp1->m_tail;	
	comp2->m_size += comp1->m_size;
}

void img_linked_linear_mser::accumulate_comp(connected_comp* comp, i32 point) {
	if (comp->m_size > 0) {
		m_points[point].m_next = comp->m_head;
	} else {
		m_points[point].m_next = -1;
		comp->m_tail = point;
	}

	comp->m_head = point;
	++comp->m_size;
}

void img_linked_linear_mser::calculate_variation(mser_region& region) {
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

	//if (temp != &region) {
	//	region.m_var = (temp->m_size - region.m_size) / (f32)region.m_size;
	//} else {
	//	if (parent != NULL || temp->m_gray_level == grayLevelThreshold) {
	//		basiclog_assert2(sys_false);
	//	}

	//	region.m_var = -1;
	//}
}

void img_linked_linear_mser::calculate_variance_with_cache(mser_region& region) {
	u32 grayLevelThreshold = region.m_gray_level + m_delta;

	mser_region* temp = &region + region.m_short_cut_offset;
	mser_region* parent = temp->m_parent;

	while (NULL != parent && parent->m_gray_level <= grayLevelThreshold) {
		temp = parent;
		parent = parent->m_parent;
	}

	if (temp != &region) {
		if (!region.m_parent->m_calculated_var) {
			region.m_parent->m_short_cut_offset = (int)(temp - region.m_parent);
		}

		region.m_var = (temp->m_size - region.m_size) / (f32)region.m_size;
	} else {
		region.m_var = -1;
	}

	region.m_calculated_var = sys_true;
}

void img_linked_linear_mser::allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask) {
	i32 area = 0;
	m_mask_activate_rect.set_rect(mt_point(0, 0), mt_size(src.size()[1], src.size()[0]));
	area = m_mask_activate_rect.getArea();
	/*
	if (m_enable_memory_cache) {
	m_mask_activate_rect.set_rect(mt_point(0, 0), mt_size(src.size()[1], src.size()[0]));
	area = m_mask_activate_rect.getArea();
	} else {
	if (img_Mask_Type_Null == mask.m_mask_type || img_Mask_Type_Rect == mask.m_mask_type) {
	if (img_Mask_Type_Null == mask.m_mask_type) {
	m_mask_activate_rect.set_rect(mt_point(0, 0), mt_size(src.size()[1], src.size()[0]));
	} else {
	m_mask_activate_rect = mask.m_mask_rect;
	}

	area = m_mask_activate_rect.getArea();
	} else if (img_Mask_Type_Image == mask.m_mask_type) {
	i32 mask_append_offset = (i32)mask.m_mask_image.step()[0] - mask.m_mask_image.size()[1];

	i32 rowMax = 0;
	i32 colMax = 0;
	i32 rowMin = src.size()[0];
	i32 colMin = src.size()[1];

	const u8* maskData = (u8*)mask.m_mask_image.data();

	for (i32 row = 0; row < mask.m_mask_image.size()[0]; ++row) {
	for (i32 col = 0; col < mask.m_mask_image.size()[1]; ++col) {

	if (1 == *maskData++) {
	rowMax = max(rowMax, row);
	colMax = max(colMax, col);
	rowMin = min(rowMin, row);
	colMin = min(colMin, col);
	++area;
	}
	}

	maskData += mask_append_offset;
	}

	m_mask_activate_rect.set_rect(mt_point(colMin, rowMin), mt_size(colMax - colMin + 1, rowMax - rowMin + 1));
	} else if (img_Mask_Type_Value == mask.m_mask_type) {
	i32 rowMax = 0;
	i32 colMax = 0;
	i32 rowMin = src.size()[0];
	i32 colMin = src.size()[1];

	const u8* imageData = (u8*)src.data();
	i32 src_append_offset = (i32)src.step()[0] - src.size()[1];

	for (i32 row = 0; row < src.size()[0]; ++row) {
	for (i32 col = 0; col < src.size()[1]; ++col) {

	if (*imageData++ != mask.m_mask_value) {
	rowMax = max(rowMax, row);
	colMax = max(colMax, col);
	rowMin = min(rowMin, row);
	colMin = min(colMin, col);
	++area;
	}
	}

	imageData += src_append_offset;
	}

	m_mask_activate_rect.set_rect(mt_point(colMin, rowMin), mt_size(colMax - colMin + 1, rowMax - rowMin + 1));
	} 
	}*/

	i32 masked_image_size = (m_mask_activate_rect.m_height + 2) * (m_mask_activate_rect.m_width + 2);

	if (masked_image_size != m_masked_image_size) {
		if (m_masked_image != NULL) {
			free(m_masked_image);
		}

		m_masked_image_size = masked_image_size;
		m_masked_image = (i16*)malloc(sizeof(i16) * m_masked_image_size);
	}

	// pre-allocate boundary heap
	i32 heap_size = area + 257;
	if (heap_size != m_heap_size) {
		if (m_heap != NULL) {
			free(m_heap);
		}
		
		m_heap_size = heap_size;
		m_heap = (i16**)malloc(sizeof(i16*) * m_heap_size);
	}

	m_heap_start[0] = &m_heap[0];

	if (area != m_point_size) {
		if (m_points != NULL) {
			free(m_points);
		}
		
		m_point_size = area;
		m_points = (linked_point*)malloc(sizeof(linked_point) * area);
	}
	
	if (area != m_region_size) {
		if (m_regions != NULL) {
			free(m_regions);
		}
		
		m_region_size = area;
		m_regions = (mser_region*)malloc(sizeof(mser_region) * area);
	}

	i32 memory_cost = sizeof(i16) * m_masked_image_size + sizeof(i16*) * m_heap_size + sizeof(linked_point) * m_point_size + sizeof(mser_region) * m_region_size
		+ sizeof(i16**) * 257 + sizeof(connected_comp) * 257 + sizeof(u32) * 257 * 2;

	basiclog_info2(sys_strcombine()<<L"linear memory cost "<< memory_cost / 1024.0 / 1024.0 <<L"MB");
	
	i32 row_step = m_mask_activate_rect.m_width + 2;

	if (img_Connected_Type_8_Neibour == m_connected_type) {
		m_dir[0] = 1;
		m_dir[1] = 1 - row_step;
		m_dir[2] = -row_step;
		m_dir[3] = -1 - row_step;
		m_dir[4] = -1;
		m_dir[5] = -1 + row_step;
		m_dir[6] = row_step;
		m_dir[7] = 1 + row_step;

		m_dir_max = 0x1000;
		m_dir_mask = 0x1e00;
	} else {
		m_dir[0] = 1;
		m_dir[1] = -row_step;
		m_dir[2] = -1;
		m_dir[3] = row_step;
		m_dir_max = 0x800;
		m_dir_mask = 0x0e00;
	}
}

void img_linked_linear_mser::pre_process_image(const mt_mat& src, u8 gray_mask, const img_mask_info<u8>& mask) {
	i32 activate_with_boundary = m_mask_activate_rect.m_width + 2;
	i16* imgptr = m_masked_image;

	i32 level_size[257];
	for ( i32 i = 0; i < 257; i++ ) {
		level_size[i] = 0;
	}

	//第一行设置为已访问
	for (i32 col = 0; col < activate_with_boundary; ++col) {
		*imgptr++ = -1;
	}

	if (img_Mask_Type_Null == mask.m_mask_type || img_Mask_Type_Rect == mask.m_mask_type) {
		i32 srcByteOffset = (i32)src.step()[0] - m_mask_activate_rect.m_width;
		const u8* srcptr = src.ptr<u8>(m_mask_activate_rect.m_top, m_mask_activate_rect.m_left, 0);

		if (gray_mask == 0) {
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr++ = -1;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					//BASICML_ASSERT(imgptr == image.ptr<i16>(row + 1, col + 1));
					//BASICML_ASSERT(srcptr == src.ptr<u8>(row + activate_rect.m_top, col + activate_rect.m_left));

					level_size[*srcptr]++;
					*imgptr++ = *srcptr++;
				}
				*imgptr++ = -1;
				srcptr += srcByteOffset;
			}
		} else {
			i16 srcValue;
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr++ = -1;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					srcValue = 0xff - *srcptr++;
					level_size[srcValue]++;
					*imgptr++ = srcValue;
				}
				*imgptr++ = -1;
				srcptr += srcByteOffset;
			}
		}
	} else if (img_Mask_Type_Image == mask.m_mask_type) {
		i32 srcByteOffset = (i32)src.step()[0] - m_mask_activate_rect.m_width;
		i32 mask_append_offset = (i32)mask.m_mask_image.step()[0] - m_mask_activate_rect.m_width;

		const u8* srcptr = src.ptr<u8>(m_mask_activate_rect.m_top, m_mask_activate_rect.m_left, 0);
		const u8* mask_data = mask.m_mask_image.ptr<u8>(m_mask_activate_rect.m_top, m_mask_activate_rect.m_left, 0);

		if (gray_mask == 0) {
			i16 srcValue;
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr = -1;
				imgptr++;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					if (0 == *mask_data) {
						srcValue = 256;	// gray level 256 for invalid pixel masked with 0
					} else {
						srcValue = *srcptr;
					}

					level_size[srcValue]++;
					*imgptr = srcValue;

					imgptr++;
					srcptr++;
					mask_data++;
				}
				*imgptr = -1;
				imgptr++;
				srcptr += srcByteOffset;
				mask_data += mask_append_offset;
			}
		} else {
			i16 srcValue;
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr = -1;
				imgptr++;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					if (0 == *mask_data) {
						srcValue = 256;	// gray level 256 for invalid pixel masked with 0
					} else {
						srcValue = 0xff - *srcptr;
					}

					level_size[srcValue]++;
					*imgptr = srcValue;
					imgptr++;
					srcptr++;
					mask_data++;
				}
				*imgptr = -1;
				imgptr++;
				srcptr += srcByteOffset;
				mask_data += mask_append_offset;
			}
		}	
	} else if (img_Mask_Type_Value == mask.m_mask_type) {
		i32 srcByteOffset = (i32)src.step()[0] - m_mask_activate_rect.m_width;

		const u8* srcptr = src.ptr<u8>(m_mask_activate_rect.m_top, m_mask_activate_rect.m_left, 0);

		if (gray_mask == 0) {
			i16 srcValue;
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr = -1;
				imgptr++;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					if (mask.m_mask_value == *srcptr) {
						srcValue = 256;	// gray level 256 for invalid pixel masked with 0
					} else {
						srcValue = *srcptr;
					}

					level_size[*srcptr]++;
					*imgptr = *srcptr;

					imgptr++;
					srcptr++;
				}
				*imgptr = -1;
				imgptr++;
				srcptr += srcByteOffset;
			}
		} else {
			i16 srcValue;
			for (i32 row = 0; row < m_mask_activate_rect.m_height; ++row) {
				*imgptr = -1;
				imgptr++;
				for (i32 col = 0; col < m_mask_activate_rect.m_width; ++col) {
					if (mask.m_mask_value == *srcptr) {
						srcValue = 256;	// gray level 256 for invalid pixel masked with 0
					} else {
						srcValue = 0xff - *srcptr;
					}

					level_size[srcValue]++;
					*imgptr = srcValue;
					imgptr++;
					srcptr++;
				}
				*imgptr = -1;
				imgptr++;
				srcptr += srcByteOffset;
			}
		}
	}

	//最后一行设置为已访问
	for (i32 col = 0; col < activate_with_boundary; ++col) {
		*imgptr++ = -1;
	}

	m_heap_start[0][0] = 0;
	for ( i32 i = 1; i < 257; i++ ) {
		m_heap_start[i] = m_heap_start[i-1]+level_size[i-1]+1;
		m_heap_start[i][0] = 0;
	}
}

void img_linked_linear_mser::extract_pass(i16* ioptr) {
	//sys_timer timer(L"extract_pass");
	//timer.begin();

	i32 erCount = 0;

	i16*** heap_cur = m_heap_start;
	linked_point* ptsptr = &m_points[0];
	mser_region* regions = &m_regions[0];
	connected_comp* comptr = m_comp; 
	i32 mask_image_width = m_mask_activate_rect.m_width + 2;

	i16* imgptr = ioptr;

	mser_region* startRegion = regions;

	i32 offset = 0;

	comptr->m_gray_level = 257;	//process mask
	comptr++;
	comptr->m_gray_level = (*imgptr)&0x01ff;
	init_comp(comptr, regions);
	*imgptr |= 0x8000;
	heap_cur += (*imgptr)&0x01ff;

	int mser_number = 0;
	u16 dir_masked = 0;
	i32 er_number = 0;

	for ( ; ; ) {
		// take tour of all the 4 or 8 directions
		while ( (dir_masked = ((*imgptr)&m_dir_mask)) < m_dir_max ) {
			// get the neighbor
			i16* imgptr_nbr = imgptr+m_dir[dir_masked>>9];
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
					init_comp(comptr, regions);
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
		i32 imsk = (i32)(imgptr-ioptr);		
		ptsptr->m_pixel_offset = imsk;



		accumulate_comp( comptr, (i32)(ptsptr - m_points));
		ptsptr++;
		// get the next pixel from boundary heap
		if ( **heap_cur ) {
			imgptr = **heap_cur;
			(*heap_cur)--;
		} else {
			++er_number;
			comptr->m_region->m_head = comptr->m_head;
			comptr->m_region->m_size = comptr->m_size;

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
						merge_comp(comptr+1, comptr);

						if ( pixel_val <= comptr[0].m_gray_level) {
							break;
						}

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
	basiclog_assert2(m_er_number == er_number);

	//timer.end();

	//i32 nullParentCount = 0;

	//for (i32 i = 0; i < erCount; ++i) {
	//	i32 m_size = startRegion[i].m_size;
	//	if (NULL == startRegion[i].m_parent)	{
	//		++nullParentCount;
	//	} 
	//}

	//BASICML_ASSERT(1 == nullParentCount);
	//BASICML_ASSERT(erCount == );

	basiclog_info2(sys_strcombine()<<L"er number "<<er_number << L" opencv mser number " << mser_number);
}

void img_linked_linear_mser::recognize_mser() {
	//determine_invalid_with_parent_child_cache(erCount);
	recognize_mser_normal();
}

void img_linked_linear_mser::recognize_mser_normal() {
	//sys_timer time(L"determine_invalid");
	//time.begin();

	//sys_timer variance_t(L"calculate variance and nms");
	//variance_t.begin();

	memset(m_level_size, 0, sizeof(u32) * 257);
	
	i32 totalUnkonwSize = 0;
	i32 nmsCount = 0;

	i32 bad_variance_number = 0;

	for (i32 i = 0; i < m_er_number; ++i) {
		mser_region& cur_region = m_regions[i];

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
		++m_level_size[cur_region.m_gray_level];
	}

	//sys_timer nms(L"nms");
	//nms.begin();

	i32 beforeUnkonwSize = totalUnkonwSize;

	if (m_delta > 0 && m_nms_similarity >= 0) {
		for (i32 i = 0; i < m_er_number; ++i) {
			mser_region* cur_region = &m_regions[i];
			mser_region* parentRegion = cur_region->m_parent;

			if (parentRegion == NULL || (cur_region->m_region_flag == mser_region::Flag_Invalid && parentRegion->m_region_flag == mser_region::Flag_Invalid)) {
				continue;
			}

			if (cur_region->m_var >= 0 && parentRegion != NULL && parentRegion->m_var >= 0 && parentRegion->m_gray_level == cur_region->m_gray_level + 1) {
				double subValue = parentRegion->m_var - cur_region->m_var;
				if (subValue > m_nms_similarity) {
					if (mser_region::Flag_Invalid != parentRegion->m_region_flag) {
						parentRegion->m_region_flag = mser_region::Flag_Invalid;
						--m_level_size[parentRegion->m_gray_level];
						--totalUnkonwSize;
					}
				} else if (-subValue > m_nms_similarity) {
					if (mser_region::Flag_Invalid != cur_region->m_region_flag) {
						cur_region->m_region_flag = mser_region::Flag_Invalid;
						--m_level_size[cur_region->m_gray_level];
						--totalUnkonwSize;
					}
				} 
			}
		}

	}

	//nms.end();

	nmsCount = beforeUnkonwSize - totalUnkonwSize;
	//variance_t.end();

	

	//basiclog_info2(sys_strcombine()<<L"bad_variance_number "<<bad_variance_number);
	//basiclog_info2(sys_strcombine()<<L"nms count "<<nmsCount);

	remove_duplicated();	
	
	//time.end();
}

void img_linked_linear_mser::remove_duplicated() {
	mt_helper::integral_array<u32>(m_start_indexes, m_level_size, m_level_size + 256);

	m_gray_order_region_size = m_start_indexes[256];
	m_gray_order_regions = (mser_region**)m_heap;

	mser_region* cur_region = m_regions;
	for (i32 i = 0; i < m_er_number; ++i) {
		if (cur_region->m_region_flag != mser_region::Flag_Invalid) {
			m_gray_order_regions[m_start_indexes[cur_region->m_gray_level]++] = cur_region;
		}

		++cur_region;
	}

	if (m_duplicated_variation > 0) {
		i32 validCount = 0;

		m_remove_duplicated_memory_helper.reserve(100);

		//去重
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


void img_linked_linear_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	//collect_mser_point_self_memory(msers, from_min);
	extract_pixel_share_memory(msers, gray_mask);
	//extract_pixel_self_memory(msers, gray_mask);
	//collect_mser_point_share_extraction_share_memory(msers, from_min);
}

void img_linked_linear_mser::extract_pixel_self_memory(img_multi_msers& msers, u8 gray_mask) {
	vector<img_mser>& mser_infos = (gray_mask == 0) ? msers.m_msers[0] : msers.m_msers[1];
	mser_infos.resize(m_gray_order_region_size);
	i32 index = 0;
	i32 mask_image_width = m_mask_activate_rect.m_width + 2;

	mt_point pt;

	//提取
	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		mser_region& currentRegion = *m_gray_order_regions[i];

		mser_infos[index].m_size = currentRegion.m_size;
		mser_infos[index].m_points = new mt_point[mser_infos[index].m_size];
		mser_infos[index].m_memory_type = img_mser::Memory_Type_Self;

		linked_point* lpt = &m_points[currentRegion.m_head];

		pt.m_y = lpt->m_pixel_offset / mask_image_width;
		pt.m_x = lpt->m_pixel_offset - (pt.m_y * mask_image_width);

		i32 rowMin = pt.m_y;
		i32 colMin = pt.m_x;
		i32 rowMax = pt.m_y;
		i32 colMax = pt.m_x;

		for ( i32 j = 0; j < currentRegion.m_size; j++ )
		{
			pt.m_y = lpt->m_pixel_offset / mask_image_width;
			pt.m_x = lpt->m_pixel_offset - (pt.m_y * mask_image_width);

			mser_infos[index].m_points[j] = pt;

			if (pt.m_x > colMax) {
				colMax = pt.m_x;
			} else if (pt.m_x < colMin) {
				colMin = pt.m_x;
			}

			if (pt.m_y > rowMax) {
				rowMax = pt.m_y;
			} else if (pt.m_y < rowMin) {
				rowMin = pt.m_y;
			}

			lpt = &m_points[lpt->m_next];
		}

		mser_infos[index].m_rect.set_rect(mt_point(colMin, rowMin), mt_point(colMax, rowMax));
		mser_infos[index].m_gray_level = (gray_mask == 0) ? currentRegion.m_gray_level : (0xff - currentRegion.m_gray_level);
		++index;
	}
}

void img_linked_linear_mser::extract_pixel_share_memory(img_multi_msers& msers, u8 gray_mask) {
	vector<img_mser>& t_msers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	mt_point*& memory = msers.m_memory[(gray_mask == 0) ? 0 : 1];
	i32& region_memory_size = msers.m_memory_size[(gray_mask == 0) ? 0 : 1];
	
	t_msers.resize(m_gray_order_region_size);
	region_memory_size = m_channel_total_pixel_number;

	memory = (mt_point*)malloc(sizeof(mt_point) * region_memory_size);
	mt_point* memory_offset = memory;

	//for (u32 i = 0; i < m_gray_order_region_size; ++i) {
	//	mser_region* cur_region = m_gray_order_regions[i];
	//	img_mser& cur_mser = t_msers[i];

	//	cur_mser.m_memory_type = img_mser::Memory_Type_Share;
	//	cur_mser.m_gray_level = from_min ? cur_region->m_gray_level : (0xff - cur_region->m_gray_level);
	//	cur_mser.m_points = memory_offset;
	//	cur_mser.m_size = cur_region->m_size;
	//	cur_mser.m_gray_level = from_min ? (cur_region->m_gray_level & 0x00ff) : (0xff - (cur_region->m_gray_level & 0x00ff));

	//	memory_offset += cur_mser.m_size;
	//}

	//sys_timer ct(L"collect point");
	//ct.begin();

	i32 memory_number = 0;
	i32 max_size = 0;
	i32 mask_image_width = m_mask_activate_rect.m_width + 2;

	mt_point pt;

	//提取
	for (u32 i = 0; i < m_gray_order_region_size; ++i) {
		mser_region* cur_region = m_gray_order_regions[i];
		img_mser& cur_mser = t_msers[i];

		cur_mser.m_memory_type = img_mser::Memory_Type_Share;
		cur_mser.m_gray_level = (gray_mask == 0) ? cur_region->m_gray_level : (0xff - cur_region->m_gray_level);
		cur_mser.m_points = memory_offset;
		cur_mser.m_size = cur_region->m_size;

		if (cur_mser.m_size > max_size) {
			max_size = cur_mser.m_size;
		}

		memory_offset += cur_mser.m_size;

		linked_point* lpt = &m_points[cur_region->m_head];

		pt.m_y = lpt->m_pixel_offset / mask_image_width;
		pt.m_x = lpt->m_pixel_offset - (pt.m_y * mask_image_width);
		
		i32 rowMin = pt.m_y;
		i32 colMin = pt.m_x;
		i32 rowMax = pt.m_y;
		i32 colMax = pt.m_x;

		for ( i32 j = 0; j < cur_region->m_size; j++ )
		{
			pt.m_y = lpt->m_pixel_offset / mask_image_width;
			pt.m_x = lpt->m_pixel_offset - (pt.m_y * mask_image_width);

			cur_mser.m_points[j] = pt;
			++memory_number;

			if (pt.m_x > colMax) {
				colMax = pt.m_x;
			} else if (pt.m_x < colMin) {
				colMin = pt.m_x;
			}

			if (pt.m_y > rowMax) {
				rowMax = pt.m_y;
			} else if (pt.m_y < rowMin) {
				rowMin = pt.m_y;
			}

			
			lpt = &m_points[lpt->m_next];
		}

		cur_mser.m_rect.set_rect(mt_point(colMin, rowMin), mt_point(colMax, rowMax));
	}

	basiclog_assert2((memory_offset - memory) == region_memory_size);

	//3000621
	basiclog_info2(L"max size: " <<max_size);

	//ct.end();
}

img_linked_linear_mser::img_linked_linear_mser() 
: img_mser_base() {

	m_masked_image = NULL;
	m_masked_image_size = 0;

	m_heap = NULL;
	m_heap_size = 0;

	m_points = NULL;
	m_point_size = 0;

	m_regions = NULL;
	m_region_size = 0;
}

img_linked_linear_mser::~img_linked_linear_mser() {
	basicsys_free(m_heap);
	basicsys_free(m_points);
	basicsys_free(m_regions);
	basicsys_free(m_masked_image);
}

void img_linked_linear_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	i16* ioptr = m_masked_image + m_mask_activate_rect.m_width + 2 + 1;
	pre_process_image(src, gray_mask, mask);
	extract_pass(ioptr);
}

void img_linked_linear_mser::clear_memory_cache() {
	basicsys_free(m_masked_image);
	basicsys_free(m_points);
	basicsys_free(m_regions);
	basicsys_free(m_heap);

	m_masked_image_size = 0;
	m_point_size = 0;
	m_region_size = 0;
	m_heap_size = 0;

	m_remove_duplicated_memory_helper.swap(vector<mser_region*>());
}

void img_linked_linear_mser::get_duplicated_regions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion) {
	mser_region* parentRegion = beginRegion->m_parent;

	while (true) {
		if (NULL == parentRegion) {
			break;
		}

		if (parentRegion->m_size > m_max_point) {
			//如果父亲节点较大,无须在寻找重复的父亲,因为父亲注定会被删掉
			break;
		}

		double variationFromBegin = (parentRegion->m_size - stableRegion->m_size) / (double)stableRegion->m_size;

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

		duplicatedRegions.push_back(parentRegion);
		parentRegion = parentRegion->m_parent;
	}
}