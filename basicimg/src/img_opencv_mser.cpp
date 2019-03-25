#include "stdafx.h"

#include "img_opencv_mser.h"

img_opencv_mser::~img_opencv_mser() {

}

// clear the connected component in stack
void img_opencv_mser::initComp(ConnectedComp* comp) {
	comp->mSize = 0;
	comp->mCurrentVar = 0;
	comp->mLastVar = 1;
	comp->mHistory = NULL;
}

void img_opencv_mser::newHistory(ConnectedComp* comp, GrowHistory* history) {
	history->mChild = history;
	if (NULL == comp->mHistory) {
		history->mShortCut = history;
		history->mStable = 0;
	} else {
		comp->mHistory->mChild = history;
		history->mShortCut = comp->mHistory->mShortCut;
		history->mStable = comp->mHistory->mStable;
	}
	history->mGrayLevel = comp->mGrayLevel;
	history->mSize = comp->mSize;
	comp->mHistory = history;
}

void img_opencv_mser::mergeComp(ConnectedComp* comp1, ConnectedComp* comp2, ConnectedComp* comp, GrowHistory* history) {
	LinkedPoint* head;
	LinkedPoint* tail;
	comp->mGrayLevel = comp2->mGrayLevel;
	history->mChild = history;
	// select the winner by size
	if ( comp1->mSize >= comp2->mSize ) {
		if ( NULL == comp1->mHistory ) {
			history->mShortCut = history;
			history->mStable = 0;
		} else {
			comp1->mHistory->mChild = history;
			history->mShortCut = comp1->mHistory->mShortCut;
			history->mStable = comp1->mHistory->mStable;
		}
		if (NULL != comp2->mHistory && comp2->mHistory->mStable > history->mStable) {
			history->mStable = comp2->mHistory->mStable;
		}
			
		history->mGrayLevel = comp1->mGrayLevel;
		history->mSize = comp1->mSize;
		// put comp1 to history
		comp->mCurrentVar = comp1->mCurrentVar;
		comp->mLastVar = comp1->mLastVar;
		if (comp1->mSize > 0 && comp2->mSize > 0) {
			comp1->mTail->mNext = comp2->mHead;
			comp2->mHead->mPrev = comp1->mTail;
		}

		head = ( comp1->mSize > 0 ) ? comp1->mHead : comp2->mHead;
		tail = ( comp2->mSize > 0 ) ? comp2->mTail : comp1->mTail;
		// always made the newly added in the last of the pixel list (comp1 ... comp2)
	} else {
		if (NULL == comp2->mHistory) {
			history->mShortCut = history;
			history->mStable = 0;
		} else {
			comp2->mHistory->mChild = history;
			history->mShortCut = comp2->mHistory->mShortCut;
			history->mStable = comp2->mHistory->mStable;
		}
		if (NULL != comp1->mHistory && comp1->mHistory->mStable > history->mStable) {
			history->mStable = comp1->mHistory->mStable;
		}
			
		history->mGrayLevel = comp2->mGrayLevel;
		history->mSize = comp2->mSize;
		// put comp2 to history
		comp->mCurrentVar = comp2->mCurrentVar;
		comp->mLastVar = comp2->mLastVar;
		if (comp1->mSize > 0 && comp2->mSize > 0) {
			comp2->mTail->mNext = comp1->mHead;
			comp1->mHead->mPrev = comp2->mTail;
		}
		head = ( comp2->mSize > 0 ) ? comp2->mHead : comp1->mHead;
		tail = ( comp1->mSize > 0 ) ? comp1->mTail : comp2->mTail;
		// always made the newly added in the last of the pixel list (comp2 ... comp1)
	}
	comp->mHead = head;
	comp->mTail = tail;
	comp->mHistory = history;
	comp->mSize = comp1->mSize + comp2->mSize;
}

double img_opencv_mser::calculateVariation(ConnectedComp* comp, i32 delta) {
	GrowHistory* history = comp->mHistory;
	int val = comp->mGrayLevel;
	if (NULL != history) {
		GrowHistory* shortcut = history->mShortCut;
		while (shortcut != shortcut->mShortCut && shortcut->mGrayLevel + delta > val) {
			shortcut = shortcut->mShortCut;
		}
			
		GrowHistory* child = shortcut->mChild;
		while (child != child->mChild && child->mGrayLevel + delta <= val) {
			shortcut = child;
			child = child->mChild;
		}
		// get the position of history where the shortcut->val <= delta+val and shortcut->child->val >= delta+val
		history->mShortCut = shortcut;
		return (double)(comp->mSize - shortcut->mSize)/(double)shortcut->mSize;
		// here is a small modification of MSER where cal ||R_{i}-R_{i-delta}||/||R_{i-delta}||
		// in standard MSER, cal ||R_{i+delta}-R_{i-delta}||/||R_{i}||
		// my calculation is simpler and much easier to implement
	}

	return 1.;
}

bool img_opencv_mser::isStable(ConnectedComp* comp) {
	// tricky part: it actually check the stablity of one-step back
	if (comp->mHistory == NULL || comp->mHistory->mSize < m_min_point || comp->mHistory->mSize > m_max_point) {
		return 0;
	}
		
	double div = (double)(comp->mHistory->mSize - comp->mHistory->mStable)/(double)comp->mHistory->mSize;
	double var = calculateVariation(comp, m_delta);
	int dvar = (comp->mCurrentVar < var || comp->mHistory->mGrayLevel + 1 < comp->mGrayLevel);
	int stable = ( dvar && !comp->mLastVar && comp->mCurrentVar < m_stable_variation && div > m_duplicated_variation);
	comp->mCurrentVar = var;
	comp->mLastVar = dvar;
	
	if (stable) {
		comp->mHistory->mStable = comp->mHistory->mSize;
	}
		
	return stable != 0;
}

void img_opencv_mser::accumulateComp(ConnectedComp* comp, LinkedPoint* point) {
	if ( comp->mSize > 0 )
	{
		point->mPrev = comp->mTail;
		comp->mTail->mNext = point;
		point->mNext = NULL;
	} else {
		point->mPrev = NULL;
		point->mNext = NULL;
		comp->mHead = point;
	}
	comp->mTail = point;
	++comp->mSize;
}

void img_opencv_mser::toMSERInfo(img_mser& mserInfo, ConnectedComp* comp, u8 gray_mask) {
	mserInfo.m_size = comp->mHistory->mSize;
	m_channel_total_pixel_number += (u32)comp->mHistory->mSize;
	mserInfo.m_points = new mt_point[mserInfo.m_size];
	mserInfo.m_memory_type = img_mser::Memory_Type_Self;
	
	int rowMin = INT_MAX;
	int colMin = INT_MAX;
	int rowMax = 0;
	int colMax = 0;

	LinkedPoint* lpt = comp->mHead;
	for ( int i = 0; i < comp->mHistory->mSize; i++ )
	{
		mserInfo.m_points[i] = lpt->mPoint;

		rowMin = min(rowMin, lpt->mPoint.m_y);
		rowMax = max(rowMax, lpt->mPoint.m_y);
		colMin = min(colMin, lpt->mPoint.m_x);
		colMax = max(colMax, lpt->mPoint.m_x);
		lpt = lpt->mNext;
	}

	mserInfo.m_rect.set_rect(mt_point(colMin, rowMin), mt_point(colMax, rowMax));
	mserInfo.m_gray_level = (gray_mask == 0) ? (comp->mGrayLevel & 0x00ff) : (0xff - (comp->mGrayLevel & 0x00ff));
}

void img_opencv_mser::preProcessImage(const mt_mat& src, u8 gray_mask) {
	int srcByteOffset = src.step()[0] / sizeof(u8) - src.size()[1];
	int cpt_1 = m_temp_image.step()[0] / sizeof(i16) - src.size()[1] - 2;
	i16* imgptr = (i16*)m_temp_image.data();

	int level_size[256];
	for ( int i = 0; i < 256; i++ ) {
		level_size[i] = 0;
	}
		
	//第一行设置为已访问
	for (int col = 0; col < src.size()[1] + 2; ++col) {
		*imgptr++ = -1;
	}

	imgptr += cpt_1;
	u8* srcptr = (u8*)src.data();
	
	if (gray_mask) {
		for (int row = 0; row < src.size()[0]; ++row) {
			*imgptr++ = -1;
			for (int col = 0; col < src.size()[1]; ++col) {
				level_size[*srcptr]++;
				*imgptr++ = *srcptr++;
			}
			*imgptr++ = -1;
			imgptr += cpt_1;
			srcptr += srcByteOffset;
		}
	} else {
		uchar srcValue;
		for (int row = 0; row < src.size()[0]; ++row) {
			*imgptr++ = -1;
			for (int col = 0; col < src.size()[1]; ++col) {
				srcValue = 0xff - *srcptr++;
				level_size[srcValue]++;
				*imgptr++ = srcValue;
			}
			*imgptr++ = -1;
			imgptr += cpt_1;
			srcptr += srcByteOffset;
		}
	}	

	//最后一行设置为已访问
	for ( int col = 0; col < src.size()[1] + 2; col++ ) {
		*imgptr++ = -1;
	}

	m_heap_start[0][0] = 0;
	for ( int i = 1; i < 256; i++ ) {
		m_heap_start[i] = m_heap_start[i-1]+level_size[i-1]+1;
		m_heap_start[i][0] = 0;
	}
}

void img_opencv_mser::extractPass(vector<img_mser>& mserInfos, i16* ioptr, u8 gray_mask) {
	sys_timer time(L"extractPass");
	time.begin();

	GrowHistory* begin_histptr = &m_history[0];
	ConnectedComp* comptr = m_comp;
	i16*** heap_cur = m_heap_start;
	LinkedPoint* ptsptr = &m_points[0];
	GrowHistory* histptr = &m_history[0];

	short* imgptr = ioptr; 
	comptr->mGrayLevel = 256;
	comptr++;
	comptr->mGrayLevel = (*imgptr)&0xff;
	initComp( comptr );
	*imgptr |= 0x8000;
	heap_cur += (*imgptr)&0xff;
	
	i32 offset = 0;

	for ( ; ; ) {
		// take tour of all the 4 directions
		while ( ((*imgptr)&0x0700) < 0x400 ) {
			// get the neighbor
			short* imgptr_nbr = imgptr+m_dir[((*imgptr)&0x0700)>>8];
			// if the neighbor is not visited yet
			if ( *imgptr_nbr >= 0 ) {

				*imgptr_nbr |= 0x8000; // mark it as visited
				offset = ((*imgptr_nbr)&0xff)-((*imgptr)&0xff);

				if ( ((*imgptr_nbr)&0xff) < ((*imgptr)&0xff) ) {
					// when the value of neighbor smaller than current
					// push current to boundary heap and make the neighbor to be the current one
					// create an empty comp
					(*heap_cur)++;
					**heap_cur = imgptr;
					*imgptr += 0x0100;
					heap_cur += offset;

					imgptr = imgptr_nbr;
					comptr++;
					initComp( comptr );
					comptr->mGrayLevel = (*imgptr)&0xff;
					continue;
				} else {
					// otherwise, push the neighbor to boundary heap
					heap_cur[offset]++;
					*heap_cur[offset] = imgptr_nbr;
				}
			}

			*imgptr += 0x100;
		}
		int imsk = (int)(imgptr-ioptr);
		ptsptr->mPoint.m_x = imsk&m_step_mask;
		ptsptr->mPoint.m_y = imsk>>m_step_gap;
		// get the current location
		accumulateComp( comptr, ptsptr );
		ptsptr++;
		// get the next pixel from boundary heap
		if ( **heap_cur ) {
			imgptr = **heap_cur;
			(*heap_cur)--;
		} else {
			heap_cur++;
			ushort pixel_val = 0;
			for ( int i = ((*imgptr)&0xff)+1; i < 256; i++ ) {
				if ( **heap_cur ) {
					pixel_val = i;
					break;
				}
				heap_cur++;
			}

			if ( pixel_val ) {
				imgptr = **heap_cur;
				(*heap_cur)--;
				
				if (pixel_val < comptr[-1].mGrayLevel) {
					// check the stablity and push a new history, increase the gray level
					if ( isStable( comptr) ) {
						mserInfos.push_back(img_mser());
						toMSERInfo(mserInfos.back(), comptr, gray_mask);
					}
					newHistory( comptr, histptr );
					comptr[0].mGrayLevel = pixel_val;
					histptr++;
				} else {
					// keep merging top two comp in stack until the grey level >= pixel_val
					for ( ; ; ) {
						comptr--;
						mergeComp( comptr+1, comptr, comptr, histptr );
						histptr++;

						if ( pixel_val <= comptr[0].mGrayLevel) {
							break;
						}
							
						if (pixel_val < comptr[-1].mGrayLevel) {
							// check the stablity here otherwise it wouldn't be an ER
							if (isStable(comptr)) {
								mserInfos.push_back(img_mser());
								toMSERInfo(mserInfos.back(), comptr, gray_mask);
							}

							newHistory( comptr, histptr );
							comptr[0].mGrayLevel = pixel_val;
							histptr++;
							break;
						}
					}
				}
			} else
				break;
		}
	}

	time.end();

	basiclog_info2(sys_strcombine()<<L"er number "<<(int)(histptr - begin_histptr));
}

void img_opencv_mser::clear_memory_cache() {
	m_temp_image = mt_mat();
	m_points.swap(vector<LinkedPoint>());
	m_history.swap(vector<GrowHistory>());
	m_heap.swap(vector<i16*>());
}

void img_opencv_mser::allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask) {
	int width = 8;
	int stepgap = 3;
	while (width < src.size()[1] + 2) {
		width <<= 1;
		stepgap++;
	}

	m_step_mask = width - 1;

	// to speedup the process, make the width to be 2^N
	m_temp_image.create(src.size()[0] + 2, width, mt_I16C1);

	int sourceImageSize = src.size()[0] * src.size()[1];

	// pre-allocate boundary heap
	m_heap.resize(sourceImageSize + 256);

	m_heap_start[0] = &m_heap[0];

	// pre-allocate linked point and grow history
	m_points.resize(sourceImageSize);
	m_history.resize(sourceImageSize);	

	m_dir[0] = 1;
	m_dir[1] = m_temp_image.size()[1];
	m_dir[2] = -1;
	m_dir[3] = -m_temp_image.size()[1];

	i32 memory_cost = sizeof(u16) * m_temp_image.size()[0] * m_temp_image.size()[1] + sizeof(i16*) * (sourceImageSize + 256) + sizeof(LinkedPoint) * sourceImageSize + sizeof(GrowHistory) * sourceImageSize;
	basiclog_info2(sys_strcombine()<<L"opencv memory cost "<< memory_cost / 1024.0 / 1024.0 <<L"MB");
}

void img_opencv_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	basiclog_assert_message2(mask.m_mask_type == img_Mask_Type_Null, L"Not support mask!");
	preProcessImage(src, gray_mask);
}

void img_opencv_mser::recognize_mser() {

}

void img_opencv_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	i32 index = (gray_mask == 0) ? 0 : 1;
	msers.m_msers[index].reserve(4000);

	i16* ioptr = m_temp_image.ptr<i16>(1, 1);
	m_channel_total_pixel_number = 0;
	extractPass(msers.m_msers[index], ioptr, gray_mask);
}