#include "stdafx.h"

#include "img_mser_result.h"

void img_mser_result::shallow_copy_cc(vector<img_connected_component*>& ccs, Result_Type type) {
	ccs.reserve(m_msers[0].size() + m_msers[1].size());

	for (int i = 0; i < 2; ++i) {
		if (i == 0) {
			if (type == img_mser_result::Result_Type_From_Max) {
				continue;
			}
		} else if (i == 1) {
			if (type == img_mser_result::Result_Type_From_Min) {
				continue;
			}
		}

		for (int j = 0; j < (int)m_msers[i].size(); ++j) {
			ccs.push_back(&m_msers[i][j].m_cc);
		}
	}
}

void img_mser_result::shallow_copy_cc(vector<img_mser_component*>& ccs, Result_Type type) {
	ccs.reserve(m_msers[0].size() + m_msers[1].size());

	for (int i = 0; i < 2; ++i) {
		if (i == 0) {
			if (type == img_mser_result::Result_Type_From_Max) {
				continue;
			}
		} else if (i == 1) {
			if (type == img_mser_result::Result_Type_From_Min) {
				continue;
			}
		}

		for (int j = 0; j < (int)m_msers[i].size(); ++j) {
			ccs.push_back(&m_msers[i][j]);
		}
	}
}

mt_mat img_mser_result::generate_mser_result_image(const mt_mat& src, Result_Type type) {
	vector<img_connected_component*> ccs;
	shallow_copy_cc(ccs, type);

	mt_mat mser_image = img_connected_component_helper::image_from_connected_component(ccs, mt_rect(0, 0, src.size()[1], src.size()[0]), img_Color_Black, img_Color_White, sys_false);

	img_draw draw(mser_image);

	for (int i = 0; i < (int)ccs.size(); ++i) {
		draw.draw_rectangle(ccs[i]->m_rect, img_Color_Green);
	}

	return mser_image;
}

img_mser_channel_image img_mser_result::generate_mser_channel_image(const mt_mat& src, bool from_min) {
	img_mser_channel_image channel_image;

	channel_image.m_level_image = mt_mat(src.size()[0], src.size()[1], mt_U8C1);
	channel_image.m_mser_id_image = mt_mat(src.size()[0], src.size()[1], mt_I32C1, mt_scalar(-1));

	if (from_min) {
		channel_image.m_level_image.set(255);

		for (int j = 0; j < (int)m_msers[0].size(); ++j) {
			for (int k = 0; k < (int)m_msers[0][j].m_cc.m_size; ++k) {
				const mt_point& pt = m_msers[0][j].m_cc.m_points[k];

				if (m_msers[0][j].m_level < channel_image.m_level_image.at<u8>(pt.m_y, pt.m_x, 0)) {
					channel_image.m_level_image.at<u8>(pt.m_y, pt.m_x, 0) = m_msers[0][j].m_level;
					channel_image.m_mser_id_image.at<i32>(pt.m_y, pt.m_x, 0) = j;
				}
			}
		}
	} else {
		channel_image.m_level_image.set(0);

		for (int j = 0; j < (int)m_msers[1].size(); ++j) {
			for (int k = 0; k < (int)m_msers[1][j].m_cc.m_size; ++k) {
				const mt_point& pt = m_msers[1][j].m_cc.m_points[k];

				if (m_msers[1][j].m_level > channel_image.m_level_image.at<u8>(pt.m_y, pt.m_x, 0)) {
					channel_image.m_level_image.at<u8>(pt.m_y, pt.m_x, 0) = m_msers[0][j].m_level;
					channel_image.m_mser_id_image.at<i32>(pt.m_y, pt.m_x, 0) = j;
				}
			}
		}
	}

	return channel_image;
}
