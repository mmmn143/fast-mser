#include "stdafx.h"

#include "img_opencv3_mser.h"

void img_opencv3_mser::CompHistory::updateTree( WParams& wp, CompHistory** _h0, CompHistory** _h1, bool final )
{
	if( var >= 0.f )
		return;
	int delta = wp.m_delta;

	CompHistory* h0_ = 0, *h1_ = 0;
	CompHistory* c = child_;
	if( size >= wp.m_min_point)
	{
		for( ; c != 0; c = c->next_ )
		{
			if( c->var < 0.f )
				c->updateTree(wp, c == child_ ? &h0_ : 0, c == child_ ? &h1_ : 0, final);
			if( c->var < 0.f )
				return;
		}
	}

	// find h0 and h1 such that:
	//    h0->val >= h->val - delta and (h0->parent == 0 or h0->parent->val < h->val - delta)
	//    h1->val <= h->val + delta and (h1->child == 0 or h1->child->val < h->val + delta)
	// then we will adjust h0 and h1 as h moves towards latest
	CompHistory* h0 = this, *h1 = h1_ && h1_->size > size ? h1_ : this;
	if( h0_ )
	{
		for( h0 = h0_; h0 != this && h0->val < val - delta; h0 = h0->parent_ )
			;
	}
	else
	{
		for( ; h0->child_ && h0->child_->val >= val - delta; h0 = h0->child_ )
			;
	}

	for( ; h1->parent_ && h1->parent_->val <= val + delta; h1 = h1->parent_ )
		;

	if( _h0 ) *_h0 = h0;
	if( _h1 ) *_h1 = h1;

	// when we do not well-defined ER(h->val + delta), we stop
	// the process of computing variances unless we are at the final step
	if( !final && !h1->parent_ && h1->val < val + delta )
		return;

	var = (float)(h1->size - h0->size)/size;

	c = child_;
	for( ; c != 0; c = c->next_ )
		c->checkAndCapture(wp);
	if( final && !parent_ )
		checkAndCapture(wp);
}

void img_opencv3_mser::CompHistory::checkAndCapture(WParams& wp )
{
	if( checked )
		return;
	checked = true;

	if( size < wp.m_min_point || size > wp.m_max_point || var < 0.f || var > wp.m_stable_variation )
		return;

	if (wp.m_nms_similarity >= 0) {
		if( var >= 0.f && child_ )
		{
			CompHistory* c = child_;
			for( ; c != 0; c = c->next_ )
			{
				if( c->var >= 0.f && val == c->val + 1 && var - c->var > wp.m_nms_similarity)
					return;
			}
		}
		if( var >= 0.f && parent_ && parent_->var >= 0.f && val + 1 == parent_->val && var - parent_->var > wp.m_nms_similarity)
			return;
	}

	int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN, j = 0;
	wp.msers->push_back(img_mser());
	img_mser& region = wp.msers->back();
	*wp.m_total_pixel_number += (u32)size;
	region.m_size = size;

	region.m_points = new mt_point[region.m_size];
	region.m_memory_type = img_mser::Memory_Type_Self;

	const Pixel* pix0 = wp.pix0;
	int step = wp.step;

	for( PPixel pix = head; j < size; j++, pix = pix0[pix].getNext() )
	{
		int y = pix/step;
		int x = pix - y*step;

		xmin = std::min(xmin, x);
		xmax = std::max(xmax, x);
		ymin = std::min(ymin, y);
		ymax = std::max(ymax, y);

		region.m_points[j] = mt_point(x, y);
	}

	region.m_rect = mt_rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
}

void img_opencv3_mser::ConnectedComp::growHistory(CompHistory*& hptr, WParams& wp, int new_gray_level, bool final)
{
	if (new_gray_level < gray_level)
		new_gray_level = gray_level;

	CompHistory *h;
	if (history && history->val == gray_level)
	{
		h = history;
	}
	else
	{
		h = hptr++;
		h->parent_ = 0;
		h->child_ = history;
		h->next_ = 0;

		if (history)
		{
			history->parent_ = h;
		}
	}
	basiclog_assert2(h != NULL);
	h->val = gray_level;
	h->size = size;
	h->head = head;
	h->var = FLT_MAX;
	h->checked = true;
	if (h->size >= wp.m_min_point)
	{
		h->var = -1.f;
		h->checked = false;
	}

	gray_level = new_gray_level;
	history = h;
	if (history && history->val != gray_level)
	{
		history->updateTree(wp, 0, 0, final);
	}
}

// merging two connected components
void img_opencv3_mser::ConnectedComp::merge( ConnectedComp* comp1, ConnectedComp* comp2,
		   CompHistory*& hptr, WParams& wp )
{
	if (comp1->gray_level < comp2->gray_level)
		std::swap(comp1, comp2);

	gray_level = comp1->gray_level;
	comp1->growHistory(hptr, wp, gray_level, false);
	comp2->growHistory(hptr, wp, gray_level, false);

	if (comp1->size == 0)
	{
		head = comp2->head;
		tail = comp2->tail;
	}
	else
	{
		head = comp1->head;
		wp.pix0[comp1->tail].setNext(comp2->head);
		tail = comp2->tail;
	}

	size = comp1->size + comp2->size;
	history = comp1->history;

	CompHistory *h1 = history->child_;
	CompHistory *h2 = comp2->history;
	// the child_'s size should be the large one
	if (h1 && h1->size > h2->size)
	{
		// add h2 as a child only if its size is large enough
		if(h2->size >= wp.m_min_point)
		{
			h2->next_ = h1->next_;
			h1->next_ = h2;
			h2->parent_ = history;
		}
	}
	else
	{
		history->child_ = h2;
		h2->parent_ = history;
		// reserve h1 as a child only if its size is large enough
		if (h1 && h1->size >= wp.m_min_point)
		{
			h2->next_ = h1;
		}
	}
}

void img_opencv3_mser::clear_memory_cache() {
	m_tempsrc = mt_mat();

	vector<Pixel> temp;
	m_pixbuf.swap(temp);

	vector<Pixel*> temp2;
	m_heapbuf.swap(temp2);

	vector<CompHistory> temp3;
	m_histbuf.swap(temp3);
}

void img_opencv3_mser::allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask) {
	mt_mat temp = src;

	if (!temp.is_continuous()) {
		m_tempsrc.copy_from(temp);
		temp = m_tempsrc;
	} else {
		m_tempsrc = temp;
	}

	int cols = m_tempsrc.size()[1], rows = m_tempsrc.size()[0];

	int area = rows * cols;
	m_pixbuf.resize(area);
	m_heapbuf.resize(area + 256);
	m_histbuf.resize(area);

	m_channel_total_running_memory += sizeof(Pixel) * area + sizeof(Pixel*) * (area + 256) + sizeof(CompHistory) * area + sizeof(Pixel**) * 256 + sizeof(ConnectedComp) * 257 + sizeof(i32) * 256;
}

void img_opencv3_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	
}

void img_opencv3_mser::recognize_mser() {

}

void img_opencv3_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	//basiclog_assert2(m_delta > 0);

	preprocess(m_tempsrc, m_level_size, gray_mask);

	CompHistory* histptr = &m_histbuf[0];
	int step = m_tempsrc.size()[1];
	Pixel *ptr0 = &m_pixbuf[0], *ptr = &ptr0[step+1];
	const uchar* imgptr0 = m_tempsrc.data();
	ConnectedComp comp[257];
	ConnectedComp* comptr = &comp[0];
	WParams wp;
	wp.m_max_point = m_max_point;
	wp.m_min_point = m_min_point;
	wp.m_stable_variation = m_stable_variation;
	wp.m_nms_similarity = m_nms_similarity;
	wp.m_delta = m_delta;
	wp.msers = &msers.m_msers[(gray_mask == 0) ? 0 : 1];
	wp.pix0 = ptr0;
	wp.step = step;
	m_channel_total_pixel_number = 0;
	wp.m_total_pixel_number = &m_channel_total_pixel_number;

	comptr->gray_level = 256;
	comptr++;
	comptr->gray_level = ptr->getGray(ptr0, imgptr0, gray_mask);
	ptr->setDir(1);
	int dir[] = { 0, 1, step, -1, -step };
	for( ;; )
	{
		//one pixel access gray at least two times
		//can not process boundary pixels
		int curr_gray = ptr->getGray(ptr0, imgptr0, gray_mask);
		int nbr_idx = ptr->getDir();
		// take tour of all the 4 directions
		for( ; nbr_idx <= 4; nbr_idx++ )
		{
			// get the neighbor
			Pixel* ptr_nbr = ptr + dir[nbr_idx];
			if( !ptr_nbr->isVisited() )
			{
				// set dir=1, next=0
				ptr_nbr->val = 1 << DIR_SHIFT;
				int nbr_gray = ptr_nbr->getGray(ptr0, imgptr0, gray_mask);
				if( nbr_gray < curr_gray )
				{
					// when the value of neighbor smaller than current
					// push current to boundary heap and make the neighbor to be the current one
					// create an empty comp
					*(++m_heap_start[curr_gray]) = ptr;
					ptr->val = (nbr_idx+1) << DIR_SHIFT;
					ptr = ptr_nbr;
					comptr++;
					comptr->init(nbr_gray);
					curr_gray = nbr_gray;
					nbr_idx = 0;
					continue;
				}
				// otherwise, push the neighbor to boundary heap
				*(++m_heap_start[nbr_gray]) = ptr_nbr;
			}
		}

		// set dir = nbr_idx, next = 0
		ptr->val = nbr_idx << DIR_SHIFT;
		int ptrofs = (int)(ptr - ptr0);
		basiclog_assert2(ptrofs != 0);

		// add a pixel to the pixel list
		if( comptr->tail )
			ptr0[comptr->tail].setNext(ptrofs);
		else
			comptr->head = ptrofs;
		comptr->tail = ptrofs;
		comptr->size++;
		// get the next pixel from boundary heap
		if( *m_heap_start[curr_gray] )
		{
			ptr = *m_heap_start[curr_gray];
			m_heap_start[curr_gray]--;
		}
		else
		{
			for( curr_gray++; curr_gray < 256; curr_gray++ )
			{
				if( *m_heap_start[curr_gray] )
					break;
			}
			if( curr_gray >= 256 )
				break;

			ptr = *m_heap_start[curr_gray];
			m_heap_start[curr_gray]--;

			if (curr_gray < comptr[-1].gray_level)
			{
				comptr->growHistory(histptr, wp, curr_gray, false);
				basiclog_assert2(comptr->size == comptr->history->size);
			}
			else
			{
				// there must one pixel with the second component's gray level in the heap,
				// so curr_gray is not large than the second component's gray level
				comptr--;
				basiclog_assert2(curr_gray == comptr->gray_level);
				comptr->merge(comptr, comptr + 1, histptr, wp);
				basiclog_assert2(curr_gray == comptr->gray_level);
			}
		}
	}

	for( ; comptr->gray_level != 256; comptr-- )
	{
		comptr->growHistory(histptr, wp, 256, true);
	}

	basiclog_info2(sys_strcombine()<<"er number: "<< (i64)(histptr - &m_histbuf[0]));
}

void img_opencv3_mser::preprocess( const mt_mat& img, int* level_size, u8 mask)
{
	if (mask == 0 || !m_from_min_max[0]) {
		memset(level_size, 0, 256*sizeof(level_size[0]));

		int i, j, cols = img.size()[1], rows = img.size()[0];
		int step = cols;

		Pixel borderpix;
		borderpix.setDir(5);

		sys_timer t0("mask");
		t0.begin();

		for( j = 0; j < step; j++ )
		{
			m_pixbuf[j] = m_pixbuf[j + (rows-1)*step] = borderpix;
		}

		for( i = 1; i < rows-1; i++ )
		{
			const u8* imgptr = img.ptr<u8>(i, 0);
			Pixel* pptr = &m_pixbuf[i*step];
			pptr[0] = pptr[cols-1] = borderpix;
			for( j = 1; j < cols-1; j++ )
			{
				int val = imgptr[j];
				level_size[val]++;
				pptr[j].val = 0;
			}
		}

		t0.end();

		
	} else {
		for(i32 i = 0; i < 128; i++ )
			std::swap(level_size[i], level_size[255-i]);

		int cols = img.size()[1], rows = img.size()[0];
		int step = cols;
		for(i32 i = 1; i < rows-1; i++ )
		{
			Pixel* pptr = &m_pixbuf[i*step];
			for(i32 j = 1; j < cols-1; j++ )
			{
				pptr[j].val = 0;
			}
		}
	}

	m_heap_start[0] = &m_heapbuf[0];
	m_heap_start[0][0] = 0;

	for( int i = 1; i < 256; i++ )
	{
		m_heap_start[i] = m_heap_start[i-1] + level_size[i-1] + 1;
		m_heap_start[i][0] = 0;
	}
}