#include "stdafx.h"

#include "img_mser_base.h"

#include "img_linked_linear_mser.h"
#include "img_origin_mser.h"
#include "img_opencv_mser.h"
#include "img_opencv3_mser.h"
#include "img_idiap_mser.h"
#include "img_vlfeat_mser.h"
#include "img_ppms_mser.h"
#include "img_fast_mser.h"
#include "img_ms_mser.h"

img_mser::~img_mser() {
	if (m_memory_type == Memory_Type_Self) {
		basicsys_delete_array(m_points);
	}
}

img_mser::img_mser(const img_mser& other) {
	m_points = NULL;
	*this = other;
}

void img_mser::operator=(const img_mser& other) {
	m_gray_level = other.m_gray_level;
	m_memory_type = other.m_memory_type;
	m_size = other.m_size;
	m_rect = other.m_rect;

	if (m_memory_type == Memory_Type_Self) {
		basicsys_delete_array(m_points);

		if (m_size > 0) {
			m_points = new mt_point[m_size];
			memcpy(m_points, other.m_points, sizeof(mt_point) * m_size);
		} else {
			m_points = NULL;
		}
	} else {
		m_points = other.m_points;
	}
}

img_multi_msers::img_multi_msers(const img_multi_msers& other) {
	m_memory[0] = NULL;
	m_memory[1] = NULL;

	*this = other;
}

void img_multi_msers::operator=(const img_multi_msers& other) {
	m_msers[0] = other.m_msers[0];
	m_msers[1] = other.m_msers[1];

	m_memory_size[0] = other.m_memory_size[0];
	m_memory_size[1] = other.m_memory_size[1];

	basicsys_delete_array(m_memory[0]);
	basicsys_delete_array(m_memory[1]);

	if (m_memory_size[0] > 0) {
		m_memory[0] = new mt_point[m_memory_size[0]];
		memcpy(m_memory[0], other.m_memory[0], sizeof(mt_point) * m_memory_size[0]);
	}

	if (m_memory_size[1] > 0) {
		m_memory[1] = new mt_point[m_memory_size[1]];
		memcpy(m_memory[1], other.m_memory[1], sizeof(mt_point) * m_memory_size[1]);
	}
}

void img_mser_base::extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask /* = img_mask_info<u8>() */) {
	m_max_point = (i32)(m_max_point_ratio * gray_src.size()[0] * gray_src.size()[1]);
	
	basiclog_info2(m_max_point);

	basiclog_assert2(gray_src.channel() == 1);
	basiclog_assert2(gray_src.depth() == mt_U8 || gray_src.depth() == mt_I8);

	res = img_multi_msers();

	i64 tick = sys_timer::get_tick_cout();
	allocate_memory(gray_src, mask);
	sys_alg_analyzer::add(L"initialization", sys_timer::get_tick_cout() - tick);

	for (i32 i = 0; i < 2; ++i) {
		if (m_from_min_max[i]) {
			u8 gray_mask = (i == 0) ? 0 : 255;

			tick = sys_timer::get_tick_cout();
			build_tree(gray_src, mask, gray_mask);
			sys_alg_analyzer::add(L"building_tree", sys_timer::get_tick_cout() - tick);

			tick = sys_timer::get_tick_cout();
			recognize_mser();
			sys_alg_analyzer::add(L"recognition", sys_timer::get_tick_cout() - tick);

			tick = sys_timer::get_tick_cout();
			extract_pixel(res, gray_mask);
			sys_alg_analyzer::add(L"extraction", sys_timer::get_tick_cout() - tick);
			sys_alg_analyzer::add(L"pixel_number", m_channel_total_pixel_number);
		}
	}

	sys_alg_analyzer::add(L"mser_number", res.m_msers[0].size() + res.m_msers[1].size());
	sys_alg_analyzer::add(L"mser_memory", res.m_memory_size[0] + res.m_memory_size[1]);
}

img_mser_base* img_mser_alg_factory::new_instance() {
	if (typeid(img_linked_linear_mser).name() == m_class_name) {
		return new img_linked_linear_mser();
	} else if (typeid(img_opencv3_mser).name() == m_class_name) {
		return new img_opencv3_mser();
	} else if (typeid(img_opencv_mser).name() == m_class_name) {
		return new img_opencv_mser();
	} else if (typeid(img_original_mser).name() == m_class_name) {
		return new img_original_mser();
	} else if (typeid(img_idiap_mser).name() == m_class_name) {
		return new img_idiap_mser();
	} else if (typeid(img_vlfeat_mser).name() == m_class_name) {
		return new img_vlfeat_mser();
	} else if (typeid(img_fast_mser).name() == m_class_name) {
		return new img_fast_mser();
	} else if (typeid(img_ppms_mser).name() == m_class_name) {
		return new img_ppms_mser();
	} else if (typeid(img_ms_mser).name() == m_class_name) {
		return new img_ms_mser();
	}

	return NULL;
}