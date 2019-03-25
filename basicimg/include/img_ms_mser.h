//Author:
//		mmmn143
//Describe:
//		The implementation of the linear maximally stable extremal region algorithm as described in ECCV2008 paper. 
//Summary:
//		The cost time over about 10% than the OpenCV mser due to this algorithm will generate more msers.
#pragma once

#include "img_mser_base.h"

namespace basicimg {
	
	class img_ms_mser : public img_mser_base {
	public:

		img_ms_mser();

		~img_ms_mser();

	private:

		void inner_extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask);

		void clear_memory_cache();

		class link_point {
		public:

			mt_point m_point;
			link_point* m_next;
			link_point* m_prev;
		};

		class mser_region {
		public:

			enum {
				Flag_Unknow = 0,
				Flag_Invalid = 1,
				Flag_Valid = 2,
				Flag_Merged = 3,
				Merged_Mask = 0x0f000000,
				Merged_Mask_Step = 0x01000000,
				Gray_Mask = 0x000003ff,
			};

			u32 m_gray_level : 8;
			u32 m_region_flag : 2;
			u32 m_calculated_var : 1;
			u32 m_assigned_pointer : 1;
			
			union {
				i32 m_size;
				i32 m_child_pixel_size;
			};
					
			union 
			{
				f32 m_var;
				i32 m_child_memory_number;
				i32 m_mser_index;
			};
			
			mser_region* m_parent;
		};

		class connected_comp {
		public:
			mser_region* m_region;
			i16 m_gray_level;	//to save value 256 we need the i16 type
			i32 m_size;
		};

		u32 m_level_size[257];
		u32 m_start_indexes[257];
		u32 m_region_level_size[257];

		i32 m_dir[9];
		u32 m_dir_mask;
		u32 m_boundary_pixel;

		u32 m_mask_mask;
		u32 m_dir_shift;

		u8* m_extended_image;
		u32 m_extended_image_size;

		i32 m_image_width;
		i32 m_image_height;

		u32* m_points;
		u32 m_point_size;

		mser_region* m_mser_regions;
		mser_region* m_mser_regions_end;
		u32 m_mser_region_size;
		u32 m_er_number;

		u32** m_heap;
		u32 m_heap_size;
		u32** m_heap_start[257];

		connected_comp m_comp[256];

		mser_region** m_gray_order_regions;
		u32 m_gray_order_regions_memory_size;
		u32 m_gray_order_region_size;
		u32 m_exceed_times;

		vector<mser_region*> m_remove_duplicated_memory_helper;
		vector<mser_region*> m_merged_regions;

		void allocate_memory(const mt_mat& img, const img_mask_info<u8>& mask);

		void pre_process_image(const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask);

		void init_comp(connected_comp* comp, mser_region*& region);
		void new_region(connected_comp* comp, mser_region*& region);

		void build_tree(const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask);

		/** Determine msers
		*/
		void recognize_mser();

		void recognize_mser_normal();
		void recognize_mser_with_parent_child_cache();

		void remove_duplicated();

		void calculate_variation(mser_region& region);
		
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);

		void get_duplicated_regions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion);
	};
}