#include "stdafx.h"

#include "img_connected_component.h"

namespace basicimg {

	class private_img_connected_component_helper {
	public:

		template<class T>
		static void connected_component(img_multi_connected_components& ccs, const mt_mat& image, const mt_scalar& background, img_Connected_Type connected_type) {
			if (image.channel() == 1) {
				ml_connected_component_base<sys_array_type<T, 1>> cc;
				cc.ture_normal_extract(ccs, image, img_mask_info<sys_array_type<T, 1>>(sys_array_type<T, 1>(background.value)), connected_type);
			} else if (image.channel() == 3) {
				ml_connected_component_base<sys_array_type<T, 3>> cc;
				cc.ture_normal_extract(ccs, image, img_mask_info<sys_array_type<T, 3>>(sys_array_type<T, 3>(background.value)), connected_type);
			} else if (image.channel() == 4) {
				ml_connected_component_base<sys_array_type<T, 4>> cc;
				cc.ture_normal_extract(ccs, image, img_mask_info<sys_array_type<T, 4>>(sys_array_type<T, 4>(background.value)), connected_type);
			}

			
		}
	};

}

img_connected_component::img_connected_component()
	: m_points(NULL)
	, m_size(0)
	, m_share_memory(sys_false) {

}

img_connected_component::~img_connected_component() {
	release();
}

void img_connected_component::operator = (const img_connected_component& other) {
	if (this == &other) {
		return;
	}

	release();
	m_share_memory = other.m_share_memory;

	if (other.m_share_memory) {
		m_points = other.m_points;
		m_size = other.m_size;

	} else {
		m_size = other.m_size;
		m_points = new mt_point[m_size];

		for (i32 i = 0; i < m_size; ++i) {
			m_points[i] = other.m_points[i];
		}
	}
}

void img_connected_component::release() {
	if (!m_share_memory && m_points != NULL) {
		delete[] m_points;
	}
}

void img_connected_component::init(i32 size) {
	release();

	m_size = size;
	m_points = new mt_point[m_size];
	m_share_memory = sys_false;
}

void img_connected_component::init(mt_point* memory_start, i32 size) {
	release();

	m_points = memory_start;
	m_size = size;
	m_share_memory = sys_true;
}

void img_connected_component_helper::connect_component(img_multi_connected_components& ccs, const mt_mat& image, const mt_scalar& background, img_Connected_Type connected_type) {
	switch (image.depth()) {
	case mt_U8:
		private_img_connected_component_helper::connected_component<u8>(ccs, image, background, connected_type);
		
		break;
	}
}

mt_mat img_connected_component_helper::image_from_connected_component(const vector<img_connected_component>& ccs, const mt_rect& roi, const mt_scalar& forground_color /* = img_Color_Black */, const mt_scalar& background_color /* = img_Color_White */, b8 gray) {
	vector<img_connected_component*> cc_pointers;
	mt_helper::pointer_from_instance(cc_pointers, ccs);

	return image_from_connected_component(cc_pointers, roi, forground_color, background_color);
}

mt_mat img_connected_component_helper::image_from_connected_component(const vector<img_connected_component*>& ccs, const mt_rect& roi, const mt_scalar& forground_color /* = img_Color_Black */, const mt_scalar& background_color /* = img_Color_White */, b8 gray) {
	mt_mat res(roi.m_height, roi.m_width, gray ? mt_U8C1 : mt_U8C3, background_color);

	for (int i = 0; i < (int)ccs.size(); ++i) {
		const mt_point* pts = ccs[i]->m_points;

		for (int j = 0; j < ccs[i]->m_size; ++j) {
			res.set(forground_color, pts[j].m_y - roi.m_top, pts[j].m_x - roi.m_left);
		}
	}

	return res;
}

mt_mat img_connected_component_helper::image_from_connected_component(const img_connected_component& cc, const mt_size& dst_size, const mt_scalar& forground_color /* = img_Color_Black */, const mt_scalar& background_color /* = img_Color_White */, b8 gray /* = sys_true */) {
	int expend_height = 0;
	int expend_width = 0;
	if (cc.m_rect.m_width > cc.m_rect.m_height) {
		expend_height = (cc.m_rect.m_width - cc.m_rect.m_height) / 2;
	} else if (cc.m_rect.m_height > cc.m_rect.m_width) {
		expend_width = (cc.m_rect.m_height - cc.m_rect.m_width) / 2;
	}

	mt_mat image(cc.m_rect.m_height + expend_height * 2, cc.m_rect.m_width + expend_width * 2, gray ? mt_U8C1 : mt_U8C3, background_color);

	for (int i = 0; i < cc.m_size; ++i) {
		mt_point current_pt(cc.m_points[i].m_x - cc.m_rect.m_left + expend_width, cc.m_points[i].m_y - cc.m_rect.m_top + expend_height);
		if (img_img::point_in_image(current_pt, image)) {
			image.set(forground_color, current_pt.m_y, current_pt.m_x);			
		}
	}

	return img_img::resize(image, dst_size, img_img::Inter_Type_Nearest);
}