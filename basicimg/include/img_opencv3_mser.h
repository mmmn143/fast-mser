#pragma once


#include "img_mser_base.h"

namespace basicimg  {

	class img_opencv3_mser : public img_mser_base{
	public:



	protected:

		void inner_extract(img_multi_msers& result, const mt_mat& src, const img_mask_info<u8>& mask);

		void clear_memory_cache();

		enum { DIR_SHIFT = 29, NEXT_MASK = ((1<<DIR_SHIFT)-1)  };

		struct Pixel
		{
			Pixel() : val(0) {}
			Pixel(int _val) : val(_val) {}

			int getGray(const Pixel* ptr0, const uchar* imgptr0, int mask) const
			{
				// for simply get pixel, pixel data and image need to be same size. Otherwise, we need first compute row and col, 
				// then multiply image step and add col
				return imgptr0[this - ptr0] ^ mask;
			}
			int getNext() const { return (val & NEXT_MASK); }
			void setNext(int next) { val = (val & ~NEXT_MASK) | next; }

			int getDir() const { return (int)((unsigned)val >> DIR_SHIFT); }
			void setDir(int dir) { val = (val & NEXT_MASK) | (dir << DIR_SHIFT); }
			bool isVisited() const { return (val & ~NEXT_MASK) != 0; }

			int val;
		};
		typedef int PPixel;

		struct WParams
		{
			i32 m_min_point;
			i32 m_max_point;
			double m_stable_variation;
			f32 m_nms_similarity;
			i32 m_delta;
			vector<img_mser>* msers;
			u32* m_total_pixel_number;
			Pixel* pix0;
			int step;
		};

		struct CompHistory
		{
			CompHistory()
			{
				parent_ = child_ = next_ = 0;
				val = size = 0;
				var = -1.f;
				head = 0;
				checked = false;
			}
			void updateTree( WParams& wp, CompHistory** _h0, CompHistory** _h1, bool f );

			void checkAndCapture( WParams& wp );

			CompHistory* child_;
			CompHistory* parent_;
			CompHistory* next_;
			int val;
			int size;
			float var;
			PPixel head;
			bool checked;
		};

		struct ConnectedComp
		{
			ConnectedComp()
			{
				init(0);
			}

			void init(int gray)
			{
				head = tail = 0;
				history = 0;
				size = 0;
				gray_level = gray;
			}

			// add history chunk to a connected component
			void growHistory(CompHistory*& hptr, WParams& wp, int new_gray_level, bool final);

			// merging two connected components
			void merge( ConnectedComp* comp1, ConnectedComp* comp2,
				CompHistory*& hptr, WParams& wp );

			PPixel head;
			PPixel tail;
			CompHistory* history;
			int gray_level;
			int size;
		};

		void allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask);
		void build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask);
		void recognize_mser();
		void extract_pixel(img_multi_msers& msers, u8 gray_mask);

		void preprocess( const mt_mat& img, int* level_size, u8 mask);

		void pass( const mt_mat& img, vector<img_mser>& mserInfos, const int* level_size, u8 mask );

		mt_mat m_tempsrc;
		vector<Pixel> m_pixbuf;
		vector<Pixel*> m_heapbuf;
		vector<CompHistory> m_histbuf;
		int m_level_size[256];

		Pixel** m_heap_start[256];
		

	};



}