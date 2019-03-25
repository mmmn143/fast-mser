//Author:
//		mmmn143
//Describe:
//		The implementation of the maximally stable extreme region (MSER) algorithm.
//Summary:
//		This code reference to the VLFeat, however we modified the properties as follows, 
//		(1) When we set the mStableDelta to 0, then the MSER algorithm turns to ER algorithm only with elimination of duplicated regions
//		(2) When the mNMSVariation <= 0, the MSER algorithm turns to SER algorithm without NMS. Usually, the detected regions in NMS type will be less 10% regions
//			to the non-NMS type (Many regions are pruned by removing duplicated regions). However, the remain regions will have high stability

#pragma once

#include "img_mser_base.h"

namespace basicimg {

	class img_original_mser : public img_mser_base
	{
	public:

		img_original_mser()
		: img_mser_base() {
			m_stack_helper = NULL;
			m_stack_helper_size = 0;
			m_stack_top_index = -1;
		}

		~img_original_mser();



	private:

		//virtual void inner_extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask);

		virtual void clear_memory_cache();

		class mser_region;

		class linked_point {
		public:	
			mser_region* m_according_region;

			linked_point* m_next;
			linked_point* m_head;
			mt_point m_point;
		};

		enum {
			Flag_Unknow,
			Flag_Invalid,
			Flag_Valid,
		};

		class mser_region {
		public:
			mser_region()
				: mHead(NULL)
				, mTail(NULL)
				, m_merged(sys_false)
				, mSize(0)
				, mParent(NULL)
				, mShortCut(NULL)
				, mFlags(Flag_Unknow)
				, mStableVariantion(-1.0)
				, mStability(0)
				, mRetention(0) {
			}

			linked_point* mHead;
			linked_point* mTail;

			b8 m_merged;
			u8 mFlags;
			u16 mGrayLevel;
			i32 mSize;
			mser_region* mParent;
			mser_region* mShortCut;

			f32 mStableVariantion;	//在StableDelta内的变化范围
			uchar mStability;			//稳定度，当前区域的稳定程度，等于该区域在连续相似区域（像素满足一定关系）中的最大保持度
			uchar mRetention;			//保持度，当前区域面积增加2倍，所需的灰度个数
		};



		vector<linked_point> m_link_pts;
		vector<mser_region> m_regions;
		vector<mt_point> m_pts;

		i16** m_stack_helper;
		i32 m_stack_helper_size;
		i32 m_stack_top_index;

		i32 m_window_size;
		const i32* mDX;
		const i32* mDY;
		i32 mImageStep[9];

		mt_mat mTempImage;
		i32 m_invalid_er_number;

		//先构造整个MSER树，然后再判断MSER树种各个节点是否是MSER
		void extractPass(b8 fromMin);
		void recognize_mser();
		void getDuplicatedRegions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion);
		void getStabilityRegions(vector<mser_region*>& stabilityRegions, mser_region* beginRegio);
		void collect(vector<img_mser>& ers, vector<mser_region>& regions, b8 fromMin);
		void calculateStableVariantion(mser_region* region);

		b8 assertHeadNotMerged(vector<mser_region>& nodes, vector<vector<linked_point>>& pts);
		b8 assertRegionPtCount(vector<mser_region>& nodes, vector<vector<linked_point>>& pts);

		void resetPreInfo();
		void releasePreInfo();

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);
		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);

		void preProcessImage(const mt_mat& image, const img_mask_info<u8>& mask);
		void reversePixelValue();
	};
}
