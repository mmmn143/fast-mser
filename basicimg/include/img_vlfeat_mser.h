#pragma once

#include "img_mser_base.h"


namespace basicimg {

	class img_vlfeat_mser : public img_mser_base {
	public:
		img_vlfeat_mser()
		: img_mser_base()
		, m_vl_feat_instance(NULL) {

		}

	protected:

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);
		void clear_memory_cache();
		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void recognize_mser();
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);

		void* m_vl_feat_instance;
		mt_mat m_src;

	};

}