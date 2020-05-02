#include "stdafx.h"
#include "img_mser_helper.h"

void img_mser_helper::shallow_copy_msers(vector<const img_mser*>& mser_pointers, const img_multi_msers& msers) {
	mser_pointers.reserve(msers.m_msers[0].size() + msers.m_msers[1].size());

	for (i32 i = 0; i < (i32)msers.m_msers[0].size(); ++i) {
		mser_pointers.push_back(&msers.m_msers[0][i]);
	}

	for (i32 i = 0; i < (i32)msers.m_msers[1].size(); ++i) {
		mser_pointers.push_back(&msers.m_msers[1][i]);
	}
}

void img_mser_helper::save_msers(const img_multi_msers& msers, const string& save_dir, const mt_size& dst_size, const mt_scalar& forground_color /* = img_Color_Black */, const mt_scalar& background_color /* = img_Color_White */, b8 gray /* = sys_true */) {
	sys_path::create_multi_dir(save_dir);
	vector<i32> stack_helper;

	mt_mat image;
	
	for (i32 i = 0; i < (i32)msers.m_msers[0].size(); ++i) {
		image_from_mser(image, msers.m_msers[0][i], dst_size, forground_color, background_color, gray, &stack_helper);

		img_img::save(sys_strcombine()<<save_dir<<"from_min_"<<i<<".png", image);
	}

	for (i32 i = 0; i < (i32)msers.m_msers[1].size(); ++i) {
		image_from_mser(image, msers.m_msers[1][i], dst_size, forground_color, background_color, gray, &stack_helper);

		img_img::save(sys_strcombine()<<save_dir<<"from_max_"<<i<<".png", image);
	}
}

void img_mser_helper::image_from_mser(mt_mat& res, const img_mser& mser, const mt_size& dst_size, const mt_scalar& forground_color /* = img_Color_Black */, const mt_scalar& background_color /* = img_Color_White */, b8 gray /* = sys_true */, vector<i32>* p_stack_helper) {
	vector<i32> temp;

	if (p_stack_helper == NULL) {
		p_stack_helper = &temp;
	}

	vector<i32>& stack_helper = *p_stack_helper;
	
	int expend_height = 0;
	int expend_width = 0;
	if (mser.m_rect.m_width > mser.m_rect.m_height) {
		expend_height = (mser.m_rect.m_width - mser.m_rect.m_height) / 2;
	} else if (mser.m_rect.m_height > mser.m_rect.m_width) {
		expend_width = (mser.m_rect.m_height - mser.m_rect.m_width) / 2;
	}

	res.create(mser.m_rect.m_height + expend_height * 2, mser.m_rect.m_width + expend_width * 2, gray ? mt_U8C1 : mt_U8C3);
	res.set(background_color);
	mt_point current_pt;

	if (mser.m_memory_type == img_mser::Memory_Type_Self || mser.m_memory_type == img_mser::Memory_Type_Share || mser.m_memory_type == img_mser::Memory_Type_Range) {
		for (int i = 0; i < mser.m_size; ++i) {
			current_pt.m_x = mser.m_points[i].m_x - mser.m_rect.m_left + expend_width;
			current_pt.m_y = mser.m_points[i].m_y - mser.m_rect.m_top + expend_height;

			//basiclog_info2(mser.m_points[i]);

			if (img_img::point_in_image(current_pt, res)) {
				res.set(forground_color, current_pt.m_y, current_pt.m_x);			
			}
		}
	} else if (mser.m_memory_type == img_mser::Memory_Type_Share_Parallel_4 || mser.m_memory_type == img_mser::Memory_Type_Recursive_Parallel_4) {
		stack_helper.push_back(0);

		while (!stack_helper.empty()) {
			i32 mser_index = stack_helper.back();
			stack_helper.pop_back();

			const img_mser* temp_mser = &mser + mser_index;

			if (temp_mser->m_memory_type == img_mser::Memory_Type_Share_Parallel_4) {
				i32 total_pixel_size = 0;

				for (i32 k = 0; k < 4; ++k) {
					mt_point* begin = temp_mser->m_points[k].m_x + temp_mser->m_points;
					mt_point* end = temp_mser->m_points[k].m_y + temp_mser->m_points;

					if (begin == end) {
						continue;
					}

					for (mt_point* pt = begin; pt < end; ++pt) {
						current_pt.m_x = pt->m_x - mser.m_rect.m_left + expend_width;
						current_pt.m_y = pt->m_y - mser.m_rect.m_top + expend_height;
						if (img_img::point_in_image(current_pt, res)) {
							res.set(forground_color, current_pt.m_y, current_pt.m_x);			
						}
					}
				}

			} else {
				for (i32 k = 0; k < 4; ++k) {
					mt_point* begin = temp_mser->m_points[k].m_x + temp_mser->m_points;
					mt_point* end = temp_mser->m_points[k].m_y + temp_mser->m_points;

					if (begin == end) {
						continue;
					}

					for (mt_point* pt = begin; pt < end; ++pt) {
						current_pt.m_x = pt->m_x - mser.m_rect.m_left + expend_width;
						current_pt.m_y = pt->m_y - mser.m_rect.m_top + expend_height;
						if (img_img::point_in_image(current_pt, res)) {
							res.set(forground_color, current_pt.m_y, current_pt.m_x);			
						}
					}
				}

				i32* child_info = (i32*)(temp_mser->m_points + 4);

				for (i32 k = 0; k < child_info[0]; ++k) {
					stack_helper.push_back((i32)(child_info[k + 1] + temp_mser - &mser));
				}
			}
		}
	} else if (mser.m_memory_type == img_mser::Memory_Type_Recursive) {
		stack_helper.push_back(0);

		while (!stack_helper.empty()) {
			i32 mser_index = stack_helper.back();
			stack_helper.pop_back();

			const img_mser* temp_mser = &mser + mser_index;

			if (temp_mser->m_memory_type == img_mser::Memory_Type_Share) {
				mt_point* pt = temp_mser->m_points;
				mt_point* end = pt + temp_mser->m_size;
				while (pt != end) {
					current_pt.m_x = pt->m_x - mser.m_rect.m_left + expend_width;
					current_pt.m_y = pt->m_y - mser.m_rect.m_top + expend_height;
					if (img_img::point_in_image(current_pt, res)) {
						res.set(forground_color, current_pt.m_y, current_pt.m_x);			
					}

					++pt;
				}
			} else {
				mt_point* pt = temp_mser->m_points + 1;
				mt_point* end = pt + temp_mser->m_points[0].m_x;
				while (pt != end) {
					current_pt.m_x = pt->m_x - mser.m_rect.m_left + expend_width;
					current_pt.m_y = pt->m_y - mser.m_rect.m_top + expend_height;
					if (img_img::point_in_image(current_pt, res)) {
						res.set(forground_color, current_pt.m_y, current_pt.m_x);			
					}

					++pt;
				}

				i32* child_info = (i32*)end;

				for (i32 k = 0; k < temp_mser->m_points[0].m_y; ++k) {
					stack_helper.push_back((i32)(child_info[k] + temp_mser - &mser));
				}
			}
		}
	}
	
	if (dst_size.m_width > 0 && dst_size.m_height > 0) {
		img_img::resize(res, res, dst_size, img_img::Inter_Type_Nearest);
	}
}

void img_mser_helper::generate_mser_result_image(mt_mat& res, const mt_mat& src, const img_multi_msers& msers, b8 include_from_min, b8 include_from_max) {

}

void img_mser_helper::visit_pixel_test(const img_multi_msers& msers) {
	sys_timer t("visit_pixel_test");
	t.begin();

	vector<i32> stack_helper;

	mt_mat image;

	for (i32 i = 0; i < (i32)msers.m_msers[0].size(); ++i) {
		visit_pixel_test(msers.m_msers[0][i], &stack_helper);
	}

	for (i32 i = 0; i < (i32)msers.m_msers[1].size(); ++i) {
		visit_pixel_test(msers.m_msers[1][i], &stack_helper);
	}

	t.end();
}

void img_mser_helper::visit_pixel_test(const img_mser& mser, vector<i32>* p_stack_helper /* = vector<i32>() */) {
	vector<i32> temp;

	if (p_stack_helper == NULL) {
		p_stack_helper = &temp;
	}

	vector<i32>& stack_helper = *p_stack_helper;

	i32 sum_x = 0;
	i32 sum_y = 0;
	mt_point current_pt;

	if (mser.m_memory_type == img_mser::Memory_Type_Self || mser.m_memory_type == img_mser::Memory_Type_Share) {
		for (int i = 0; i < mser.m_size; ++i) {
			sum_x += mser.m_points[i].m_x;
			sum_y += mser.m_points[i].m_y;
		}
	} else if (mser.m_memory_type == img_mser::Memory_Type_Share_Parallel_4 || mser.m_memory_type == img_mser::Memory_Type_Recursive_Parallel_4) {
		stack_helper.push_back(0);

		while (!stack_helper.empty()) {
			i32 mser_index = stack_helper.back();
			stack_helper.pop_back();

			const img_mser* temp_mser = &mser + mser_index;

			if (temp_mser->m_memory_type == img_mser::Memory_Type_Share_Parallel_4) {
				i32 total_pixel_size = 0;

				for (i32 k = 0; k < 4; ++k) {
					mt_point* begin = temp_mser->m_points[k].m_x + temp_mser->m_points;
					mt_point* end = temp_mser->m_points[k].m_y + temp_mser->m_points;

					if (begin == end) {
						continue;
					}

					for (mt_point* pt = begin; pt < end; ++pt) {
						sum_x += pt->m_x;
						sum_y += pt->m_y;
					}
				}

			} else {
				for (i32 k = 0; k < 4; ++k) {
					mt_point* begin = temp_mser->m_points[k].m_x + temp_mser->m_points;
					mt_point* end = temp_mser->m_points[k].m_y + temp_mser->m_points;

					if (begin == end) {
						continue;
					}

					for (mt_point* pt = begin; pt < end; ++pt) {
						sum_x += pt->m_x;
						sum_y += pt->m_y;
					}
				}

				i32* child_info = (i32*)(temp_mser->m_points + 4);

				for (i32 k = 0; k < child_info[0]; ++k) {
					stack_helper.push_back((i32)(child_info[k + 1] + temp_mser - &mser));
				}
			}
		}
	} else if (mser.m_memory_type == img_mser::Memory_Type_Recursive) {
		stack_helper.push_back(0);

		while (!stack_helper.empty()) {
			i32 mser_index = stack_helper.back();
			stack_helper.pop_back();

			const img_mser* temp_mser = &mser + mser_index;

			if (temp_mser->m_memory_type == img_mser::Memory_Type_Share) {
				mt_point* pt = temp_mser->m_points;
				mt_point* end = pt + temp_mser->m_size;
				while (pt != end) {
					sum_x += pt->m_x;
					sum_y += pt->m_y;

					++pt;
				}
			} else {
				mt_point* pt = temp_mser->m_points + 1;
				mt_point* end = pt + temp_mser->m_points[0].m_x;
				while (pt != end) {
					sum_x += pt->m_x;
					sum_y += pt->m_y;

					++pt;
				}

				i32* child_info = (i32*)end;

				for (i32 k = 0; k < temp_mser->m_points[0].m_y; ++k) {
					stack_helper.push_back((i32)(child_info[k] + temp_mser - &mser));
				}
			}
		}
	}

	//basiclog_info2(sys_strcombine()<<"sum_x: "<<sum_x<<", sum_y: "<<sum_y);
}