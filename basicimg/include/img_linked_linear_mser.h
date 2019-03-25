//Author:
//		mmmn143
//Describe:
//		The implementation of the linear maximally stable extremal region algorithm as described in ECCV2008 paper. 
//Summary:
//		The cost time over about 10% than the OpenCV mser due to this algorithm will generate more msers.
#pragma once

#include "img_mser_base.h"

namespace basicimg {
	/** It is a advanced version of img_linear_mser by optimizing the memory saving (including regions, points, grayOrderIndexes, Memory_Share), variance computation.
	*/
	class img_linked_linear_mser : public img_mser_base {
	public:

		img_linked_linear_mser();

		~img_linked_linear_mser();

	private:

		void inner_extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask);

		void clear_memory_cache();

		enum {
			Region_Non_Head,
			Region_Valid_Head,
			Region_Invalid_Head,
		};

		class mser_region;

		class linked_point {
		public:
			linked_point* m_prev;
			linked_point* m_next;
			mt_point m_point;
		};

		class mser_region {
		public:
			enum {
				Flag_Unknow = 0,
				Flag_Invalid = 1,
				Flag_Valid = 2,
			};

			i8 m_region_flag;

			u8 m_calculated_var;
			u8 m_has_child;

			u8 m_gray_level;
			i32 m_size;
			
			union {
				f32 m_var;
				i32 m_short_cut_offset;
			};
			
			mser_region* m_parent;
			linked_point* m_head;
			linked_point* m_tail;
			img_mser* m_mser;
			mt_point* m_points;	//for debug
		};

		class connected_comp {
		public:

			linked_point* m_head;
			linked_point* m_tail;
			mser_region* m_region;
			i16 m_gray_level;	//to save value 256 we need the i16 type
			i32 m_size;
		};

		i16* m_masked_image;
		u32 m_masked_image_size;

		mt_rect m_mask_activate_rect;
		
		i16** m_heap;
		i32 m_heap_size;

		i16** m_heap_start[257];

		mser_region** m_gray_order_regions;
		u32 m_gray_order_region_size;

		linked_point* m_points;
		i32 m_point_size;
		mser_region* m_regions;
		i32 m_region_size;
		i32 m_er_number;

		connected_comp m_comp[258];
		i32 m_dir[8];
		i32 m_dir_mask;
		i32 m_dir_max;

		u32 m_level_size[257];
		u32 m_start_indexes[257];

		vector<mser_region*> m_remove_duplicated_memory_helper;

		void build_tree(const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask);

		void init_comp(connected_comp* comp, mser_region*& region);

		void new_region(connected_comp* comp, mser_region*& region);

		void merge_comp(connected_comp* comp1, connected_comp* comp2);

		void calculate_variation(mser_region& region);

		void calculate_variance_with_cache(mser_region& region);

		

		void accumulate_comp(connected_comp* comp, linked_point* point);

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);



		void pre_process_image(const mt_mat& src, u8 gray_mask, const img_mask_info<u8>& mask);

		void extract_pass(i16* ioptr);

		void recognize_mser();

		void recognize_mser_normal();
		void recognize_mser_with_parent_child_cache();

		void remove_duplicated();

		void extract_pixel(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_self_memory(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_share_memory(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_share_extraction_share_memory(img_multi_msers& msers, u8 gray_mask);

		void get_duplicated_regions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion);
	};
}

