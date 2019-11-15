/** @file img_fast_mser_v1.h

The Fast MSER V1 algorithm.
We propose a novel sub-tree merging algorithm and a novel pixel extraction algorithm to accelerate MSER detection. See details in our paper.
*/

#pragma once

#include "img_mser_base.h"

namespace basicimg {

	class img_fast_mser_v1 : public img_mser_base {
	public:

		img_fast_mser_v1();

		~img_fast_mser_v1();

	protected:

		void clear_memory_cache();

		enum {
			Boundary_Yes_Mask = 0x4000,
		};

		class linked_point {
		public:

			u16 m_x;
			u16 m_y;
			i32 m_next;
			i32 m_prev;
			i32 m_ref;
		};

		class mser_region {
		public:

			enum {
				Flag_Unknow = 0,
				Flag_Invalid = 1,
				Flag_Valid = 2,
				Flag_Merged = 3,
				Gray_Mask = 0x000003ff,
			};

			u32 m_region_flag : 2;
			u32 m_gray_level : 8;
			u32 m_calculated_var : 1;
			u32 m_boundary_region : 1;
			u32 m_patch_index : 8;

			union
			{
				i32 m_size;
				i32 m_child_pixel_size;
				i32 m_self_size;
			};
			
			union 
			{
				u32 m_unmerged_size;
				f32 m_var;
				i32 m_child_memory_number;
				i32 m_mser_index;
			};
			
			mser_region* m_parent;
			
			i32 m_head;
			i32 m_tail;

			//img_mser* m_mser;
			//mt_point* m_points;	//for debug
			
			
			//i32 m_memory_size;
		};

		class connected_comp {
		public:
			i32 m_head;
			i32 m_tail;
			mser_region* m_region;
			i16 m_gray_level;	//to save value 256 we need the i16 type
			i32 m_size;
		};

		class parallel_info {
		public:

			//4 for direction, 9 for pixel, 1 for boundary flag, 1 for visited flag
			i16* m_masked_image_data;
			linked_point* m_link_points;
			linked_point* m_linked_points_end;

			i32 m_top;
			i32 m_left;
			i32 m_width;
			i32 m_height;
			i32 m_width_with_boundary;
			i32 m_height_with_boundary;

			i32 m_er_number;
			mt_block_memory<mser_region> m_mser_regions;

			i16** m_heap;
			connected_comp m_comp[256];
			i16** m_heap_start[257];

			mser_region** m_boundary_regions[4];	//left, right, top, bottom
			i32 m_dir[8];
			u32 m_level_size[257];
		};

		vector<parallel_info> m_pinfo;
		u32 m_start_indexes[257];
		u32 m_region_level_size[257];
		
		u32 m_dir_mask;
		u32 m_dir_max;

		i16* m_masked_image;
		u32 m_masked_image_size;

		linked_point* m_linked_points;
		u32 m_linked_point_size;

		i16** m_heap;
		u32 m_heap_size;

		mser_region** m_boundary_regions;
		u32 m_boundary_region_size;

		mser_region** m_gray_order_regions;
		u32 m_gray_order_region_size;



		

		i32 m_merged_number;	//for debug
		b8 m_need_patch_calculate_variance;

		vector<mser_region*> m_remove_duplicated_memory_helper;
		i32* m_merged_points;
		i32 m_merged_point_size;
		i32 m_merged_point_index;


		vector<i32*> m_orderd_indexes;

		void allocate_memory(const mt_mat& img, const img_mask_info<u8>& mask);
		void allocate_memory_parallel_4(const mt_mat& img, const img_mask_info<u8>& mask);

		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void build_tree_parallel_4(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);

		void make_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask, u8 patch_index);
		void process_tree_patch(parallel_info& pinfo, const mt_mat& img, const img_mask_info<u8>& mask, u8 gray_mask);

		void init_comp(connected_comp* comp, mser_region* region, u8 patch_index);
		void new_region(connected_comp* comp, mser_region* region, u8 patch_index);

		/** Our novel sub-tree merging algorithm
		*/
		void merge_tree_parallel_4(const mt_mat& src, u8 gray_mask);

		void merge_tree_parallel_4_step_with_order(u8 merged_flag, i32 rows, i32 cols);

		void merge_tree_parallel_4_step(u8 merged_flag);

		void connect(mser_region* a, mser_region* b, u32 split_patch_index);

		/** Determine msers
		*/

		void recognize_mser();

		void recognize_mser_parallel_4();
		void recognize_mser_parallel_4_parallel();
		
		void disconnect(mser_region* r);

		void extract_pixel(img_multi_msers& msers, u8 gray_mask);
		
		void extract_pixel_parallel_4(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_parallel_4_simple_parallel_impl(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_parallel_4_simple_parallel_impl2(img_multi_msers& msers, u8 gray_mask);
		
		void extract_pixel_parallel_4_parallel_impl(img_multi_msers& msers, u8 gray_mask);
		void extract_pixel_parallel_4_fast(img_multi_msers& msers, u8 gray_mask);

		void get_duplicated_regions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion);

		void accumulate_comp(connected_comp* comp, linked_point*& point);

		void merge_comp(connected_comp* comp1, connected_comp* comp2);
	};


}