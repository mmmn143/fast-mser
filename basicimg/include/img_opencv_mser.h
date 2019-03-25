#pragma once

#include "img_mser_base.h"

namespace basicimg {
	//for uchar type (if the type is char, you need to convert it to uchar)
	class img_opencv_mser : public img_mser_base {
	public:

		~img_opencv_mser();



	private:

		void inner_extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask);

		void clear_memory_cache();

		class LinkedPoint {
		public:

			LinkedPoint* mPrev;
			LinkedPoint* mNext;
			mt_point mPoint;
		};

		class GrowHistory {
		public:

			GrowHistory* mShortCut;
			GrowHistory* mChild;
			int mStable;	// when it ever stabled before, record the size
			u16 mGrayLevel;
			int mSize;
		};

		class ConnectedComp {
		public:

			LinkedPoint* mHead;
			LinkedPoint* mTail;
			GrowHistory* mHistory;
			u16 mGrayLevel;	//to save value 256 we need the ushort type
			int mSize;
			int mLastVar;	// the derivative of last var
			double mCurrentVar;// the current variation (most time is the variation of one-step back)
		};

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);
		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void recognize_mser();
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);

		void initComp(ConnectedComp* comp);

		// add history of size to a connected component
		void newHistory(ConnectedComp* comp, GrowHistory* history);

		void mergeComp(ConnectedComp* comp1, ConnectedComp* comp2, ConnectedComp* comp, GrowHistory* history);

		double calculateVariation(ConnectedComp* comp, int delta);

		void toMSERInfo(img_mser& mserInfo, ConnectedComp* comp, u8 gray_mask);

		bool isStable(ConnectedComp* comp);

		void accumulateComp(ConnectedComp* comp, LinkedPoint* point);

		void preProcessImage(const mt_mat& src, u8 gray_mask);

		void extractPass(vector<img_mser>& mserInfos, i16* ioptr, u8 gray_mask);
	
		mt_mat m_temp_image;

		vector<i16*> m_heap;

		i16** m_heap_start[256];

		vector<LinkedPoint> m_points;
		vector<GrowHistory> m_history;
		ConnectedComp m_comp[257];

		i32 m_step_mask;
		i32 m_step_gap;
		i32 m_dir[4];
	};
}

