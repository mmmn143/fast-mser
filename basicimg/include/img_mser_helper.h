#pragma once

#include "img_mser_base.h"

namespace basicimg {

	class img_mser_helper {
	public:

		static void shallow_copy_msers(vector<const img_mser*>& mser_pointers, const img_multi_msers& msers);

		static void save_msers(const img_multi_msers& msers, const wstring& save_path, const mt_size& dst_size, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true);

		static void visit_pixel_test(const img_multi_msers& msers);

		static mt_mat image_from_mser(const img_mser& mser, const mt_size& dst_size, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true, vector<i32>& stack_helper = vector<i32>());
		static void image_from_mser(mt_mat& res, const img_mser& mser, const mt_size& dst_size, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true, vector<i32>& stack_helper = vector<i32>());



		static void generate_mser_result_image(mt_mat& res, const mt_mat& src, const img_multi_msers& msers, b8 include_from_min, b8 include_from_max);



		static void generate_mser_channel_image(mt_mat& level_image, mt_mat& id_image, const mt_mat& src, const img_multi_msers& msers, b8 from_min);

	private:

		static void visit_pixel_test(const img_mser& mser, vector<i32>& stack_helper = vector<i32>());
	};

}