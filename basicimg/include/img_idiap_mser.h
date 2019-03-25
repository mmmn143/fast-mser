#pragma once

#include "img_mser_base.h"

namespace basicimg {
	class img_idiap_mser : public img_mser_base {
	public:
		img_idiap_mser()
			: img_mser_base()
			, poolIndex_(0) {
		}

		~img_idiap_mser();

	private:

		struct mser_region {

			u16 level_; ///< Level at which the region is processed.

			i32 pixel_; ///< Index of the initial pixel (y * width + x).

			i32 area_; ///< Area of the region (moment zero).

			f32 moments_[5]; ///< First and second moments of the region (x, y, x^2, xy, y^2).

			f32 variation_; ///< idiap_MSER variation.



			/// Constructor.

			/// @param[in] level Level at which the region is processed.

			/// @param[in] pixel Index of the initial pixel (y * width + x).

			mser_region(u16 level = 256, i32 pixel = 0);

		private:

			b8 stable_; // Flag indicating if the region is stable

			mser_region * parent_; // Pointer to the parent region

			mser_region * child_; // Pointer to the first child

			mser_region * next_; // Pointer to the next (sister) region



			void accumulate(i32 x, i32 y);

			void merge(mser_region * child);

			void detect(i32 delta, i32 minArea, i32 maxArea, f32 maxVariation, f32 nms_similarity, f32 minDiversity, std::vector<mser_region> & regions);

			void process(i32 delta, i32 minArea, i32 maxArea, f32 maxVariation, f32 nms_similarity);

			b8 check(f32 variation, i32 area) const;

			void save(f32 minDiversity, f32 nms_similarity, std::vector<mser_region> & regions);



			friend class img_idiap_mser;

		};

		// Helper method

		void processStack(int newPixelGreyLevel, int pixel, std::vector<mser_region *> & regionStack);



		// Double the size of the memory pool

		i64 doublePool(std::vector<mser_region *> & regionStack);

		// Memory pool of regions for faster allocation
		std::vector<mser_region> pool_;
		std::vector<b8> m_accessible;
		mt_mat m_src;
		std::size_t poolIndex_;

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);

		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void recognize_mser();
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);
		void clear_memory_cache();
	};
}