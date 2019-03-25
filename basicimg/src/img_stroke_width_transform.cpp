#include "stdafx.h"
#include "img_stroke_width_transform.h"

static const i32 Block_Size = 1000000;

mt_mat img_stroke_width_transform::swt(__in mt_mat& edge, __in mt_mat& gradient_x, __in mt_mat& gradient_y, const params& pars) {	
	point_memory memory;

	vector<swt_ray> rays;
	rays.reserve(100000);

	mt_point next_pts[2];
	vector<mt_point> ray_pts;
	ray_pts.reserve(100);

	f32 cur_x;
	f32 cur_y;
	i32 cur_pix_x;
	i32 cur_pix_y;
	f32 start_g_x;
	f32 start_g_y;
	f32 start_g_norm;
	b8 first;
	b8 has_non_edge_pixel;
	i32 next_pt_count;
	b8 break_search;
	b8 all_pt_out_of_image;
	i32 next_x;
	i32 next_y;
	i32 surround_x;
	i32 surround_y;
	f32 stop_g_x;
	f32 stop_g_y;
	f32 gradient_value;

	i32 nextPointIndex;
	b8 isNextIndex0PushGoodRay;

	mt_mat swtImage(edge.size()[0], edge.size()[1], mt_I16C1, mt_scalar(pars.m_invalid_stroke_width));

	for( i32 row = 0; row < edge.size()[0]; row++ ){
		for ( i32 col = 0; col < edge.size()[1]; col++ ){
			if (edge.at<u8>(row, col, 0) != 255) {
				continue;
			} // end if

			ray_pts.clear();
			ray_pts.push_back(mt_point(col, row));

			cur_x = (f32)col;
			cur_y = (f32)row;
			cur_pix_x = col;
			cur_pix_y = row;
			start_g_x = gradient_x.at<f32>(row, col, 0);
			start_g_y = gradient_y.at<f32>(row, col, 0);

			start_g_norm = start_g_x * start_g_x + start_g_y * start_g_y;
			start_g_norm = sqrt(start_g_norm);

			start_g_x /= start_g_norm;
			start_g_y /= start_g_norm;

			if (pars.m_dark_on_light) {
				start_g_x = -start_g_x;
				start_g_y = -start_g_y;
			}

			//表示第一次向梯度方向移动
			first = sys_true;
			has_non_edge_pixel = sys_false;

			for (;;) {
				cur_x += start_g_x;
				cur_y += start_g_y;

				next_pt_count = get_next_point(next_pts, (i32)mt_helper::neibour_float(cur_x), (i32)mt_helper::neibour_float(cur_y), cur_pix_x, cur_pix_y, start_g_x, start_g_y);

				if (0 == next_pt_count) {
					continue;
				}

				cur_pix_x = next_pts[0].m_x;
				cur_pix_y = next_pts[0].m_y;

				break_search = sys_false;
				all_pt_out_of_image = sys_true;
				isNextIndex0PushGoodRay = sys_false;

				for (nextPointIndex = 0; nextPointIndex < next_pt_count; ++nextPointIndex) {
					next_x = next_pts[nextPointIndex].m_x;
					next_y = next_pts[nextPointIndex].m_y;

					// check if pixel is outside boundary of image
					if (img_img::point_in_image(next_pts[nextPointIndex], edge)) {
						all_pt_out_of_image = sys_false;
					} else {
						continue;
					}

					if (first && 255 == edge.at<uchar>(next_y, next_x)) {
						// 从射线起始点出发，第一次搜寻下一点，如果下一点是边缘，则不做考虑
						continue;
					} 

					if (255 == edge.at<uchar>(next_y, next_x)) {	
						if (has_non_edge_pixel) {
							//必须有至少一个非边缘点,才能组成笔划
							// 尝试搜寻领域的9各点，第一个点为当前点
							for (i32 surroundIndex = 0; surroundIndex < 9; ++surroundIndex) {
								surround_x = next_x + img_DX_3_9[surroundIndex];
								surround_y = next_y + img_DY_3_9[surroundIndex];

								if (0 != surroundIndex) {
									// 验证不是当前点的合法性
									if (!img_img::point_in_image(mt_point(surround_x, surround_y), edge) 
										|| (abs(surround_x - ray_pts.front().m_x) <= 1 && abs(surround_y - ray_pts.front().m_y) <= 1) 
										|| 255 != edge.at<u8>(surround_y, surround_x, 0)) {
											continue;
									}
								}

								// dot product
								stop_g_x = gradient_x.at<f32>(surround_y,surround_x, 0);
								stop_g_y = gradient_y.at<f32>(surround_y,surround_x, 0);
								if (pars.m_dark_on_light) {
									stop_g_x = -stop_g_x;
									stop_g_y = -stop_g_y;
								}

								gradient_value = start_g_x * -stop_g_x + start_g_y * -stop_g_y;

								if (gradient_value >= mt_trigonometric_function::angle_cos(pars.m_pair_intersect_angle)) {
									if (0 == nextPointIndex) {
										ray_pts.push_back(mt_point(surround_x, surround_y));
									} else if (isNextIndex0PushGoodRay) {
										ray_pts.back() = mt_point(surround_x, surround_y);
									}

									if (ray_pts.size() > 2) {
										rays.push_back(swt_ray());
										swt_ray& backRay = rays.back();

										if (Block_Size - memory.m_current_block_size >= (i32)ray_pts.size()) {
											backRay.m_start = memory.m_blocks.back() + memory.m_current_block_size;
											memory.m_current_block_size += (i32)ray_pts.size();
										} else {
											backRay.m_start = (mt_point*)malloc(sizeof(mt_point) * Block_Size);
											memory.m_blocks.push_back(backRay.m_start);
											memory.m_current_block_size = (i32)ray_pts.size();
										}

										mt_point* pointPtr = backRay.m_start;

										for (i32 iter = 0; iter < (i32)ray_pts.size(); ++iter) {
											*pointPtr++ = ray_pts[iter];
										}

										backRay.m_stop = pointPtr;
										backRay.m_width = (i32)mt_helper::neibour_float(ray_pts.front().distance_to(ray_pts.back())) - 1;
										backRay.m_width = max(backRay.m_width, 1);
									}
									break; // break for (surroundIndex)
								}
							} // end for (surroundIndex)
						}

						break_search = sys_true;
						break; // break for (nextPointIndex)
					}  else {
						has_non_edge_pixel = sys_true;

						if (0 == nextPointIndex) {
							isNextIndex0PushGoodRay = sys_true;
						}

						ray_pts.push_back(mt_point(next_x, next_y));
					} // end else

				} // end for (nextPointIndex)

				first = sys_false;

				if (all_pt_out_of_image || break_search) {
					break;
				}
			} // end while (sys_true)
		} // end for (col)
	} // end for (row)

	median_stroke_width(swtImage, rays, pars);

	return swtImage;
}

mt_mat img_stroke_width_transform::normalize_swt_image(mt_mat& swt_image, i16 invalid_stroke_width) {
	mt_mat ret(swt_image.size()[0], swt_image.size()[1], mt_U8C1);
	i16 maxVal = 0;
	i16 minVal = mt_I16_Max;
	for( i32 row = 0; row < swt_image.size()[0]; row++ ){
		const i16* ptr = swt_image.ptr<i16>(row, 0);
		for ( i32 col = 0; col < swt_image.size()[1]; col++ ){
			if (*ptr != invalid_stroke_width) {
				maxVal = max(*ptr, maxVal);
				minVal = min(*ptr, minVal);
			}

			ptr++;
		}
	}

	i32 difference = maxVal - minVal;

	if (difference < 200) {
		difference = 200;
	}

	for( i32 row = 0; row < swt_image.size()[0]; row++ ){
		const i16* ptrin = swt_image.ptr<i16>(row, 0);
		uchar* ptrout = ret.ptr<uchar>(row, 0);
		for ( i32 col = 0; col < swt_image.size()[1]; col++ ){
			if (*ptrin == invalid_stroke_width) {
				*ptrout = 255;
			} else {
				*ptrout = (uchar)(255 * ((*ptrin) - minVal)/(f32)difference);
			}
			ptrout++;
			ptrin++;
		}
	}

	return ret;
}

void img_stroke_width_transform::set_ray_width(__in mt_mat& swt_image, const swt_ray& ray) {
	for (mt_point* iter = ray.m_start; iter < ray.m_stop; ++iter) {
		if (ray.m_width < swt_image.at<i16>(iter->m_y, iter->m_x)) {
			swt_image.at<i16>(iter->m_y, iter->m_x) = ray.m_width;
		}
	}
}

void img_stroke_width_transform::median_stroke_width(mt_mat& swt_image, vector<swt_ray>& rays, const params& pars) {
	for (i32 i = 0; i < (i32)rays.size(); ++i) {
		set_ray_width(swt_image, rays[i]);
	}

	vector<i32> widths;
	i32 raySize = (i32)rays.size();

	for (i32 i = 0; i < raySize; ++i) {
		i32 rayPtSize = (i32)(rays[i].m_stop - rays[i].m_start);
		widths.resize(rayPtSize);		
		i32 index = 0;

		for (mt_point* iter = rays[i].m_start; iter < rays[i].m_stop; ++iter) {
			widths[index++] = swt_image.at<i16>(iter->m_y, iter->m_x);
		}

		rays[i].m_width = mt_helper::median_with_changed_order(widths);
	}

	for (i32 i = 0; i < (i32)rays.size(); ++i) {
		set_ray_width(swt_image, rays[i]);
	}
}

i32 img_stroke_width_transform::get_next_point(mt_point* next_pts, i32 cur_x, i32 cur_y, i32 last_x, i32 last_y, f32 gradient_x, f32 gradient_y) {
	i32	nextPtCount = 0;
	if (cur_x != last_x && cur_y != last_y) {
		basiclog_assert2(1 == abs(cur_x - last_x) && 1 == abs(cur_y - last_y));
		next_pts[nextPtCount].m_x = cur_x;
		next_pts[nextPtCount].m_y = cur_y;

		++nextPtCount;

		if (abs(gradient_y) <= abs(gradient_x)) {
			if (gradient_x > 0) {
				next_pts[nextPtCount].m_x = cur_x;
				next_pts[nextPtCount].m_y = last_y;
			} else {
				next_pts[nextPtCount].m_x = last_x;
				next_pts[nextPtCount].m_y = cur_y;
			}	
		} else {
			if (gradient_y > 0) {
				next_pts[nextPtCount].m_x = last_x;
				next_pts[nextPtCount].m_y = cur_y;
			} else {
				next_pts[nextPtCount].m_x = cur_x;
				next_pts[nextPtCount].m_y = last_y;
			}
		}

		++nextPtCount;
	} else if (cur_x != last_x || cur_y != last_y) {
		next_pts[nextPtCount].m_x = cur_x;
		next_pts[nextPtCount].m_y = cur_y;
		++nextPtCount;
	}

	return nextPtCount;
}