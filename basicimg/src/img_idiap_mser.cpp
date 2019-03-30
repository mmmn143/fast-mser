// Please see https://github.com/idiap/mser for its original implementation.


#include "stdafx.h"
#include "img_idiap_mser.h"

/// The img_idiap_mser class extracts maximally stable extremal regions from a grayscale (8 bits) image.

/// @note The img_idiap_mser class is not reentrant, so if you want to extract regions in parallel, each

/// thread needs to have its own img_idiap_mser class instance.

img_idiap_mser::~img_idiap_mser() {
	clear_memory_cache();
}

void img_idiap_mser::clear_memory_cache() {
	m_src = mt_mat();
	pool_.swap(vector<mser_region>());
	m_accessible.swap(vector<b8>());
}

img_idiap_mser::mser_region::mser_region(u16 level, i32 pixel) : level_(level), pixel_(pixel), area_(0),
	variation_(-1.f), stable_(sys_false), parent_(0), child_(0), next_(0)

{
	fill_n(moments_, 5, 0.0f);
}



void img_idiap_mser::mser_region::accumulate(int x, int y) {

	++area_;

	moments_[0] += x;

	moments_[1] += y;

	moments_[2] += x * x;

	moments_[3] += x * y;

	moments_[4] += y * y;

}



void img_idiap_mser::mser_region::merge(mser_region * child)

{

	basiclog_assert2(!child->parent_);

	basiclog_assert2(!child->next_);



	// Add the moments together

	area_ += child->area_;

	moments_[0] += child->moments_[0];

	moments_[1] += child->moments_[1];

	moments_[2] += child->moments_[2];

	moments_[3] += child->moments_[3];

	moments_[4] += child->moments_[4];



	child->next_ = child_;

	child_ = child;

	child->parent_ = this;

}



void img_idiap_mser::mser_region::process(i32 delta, i32 minArea, i32 maxArea, f32 maxVariation, f32 nms_similarity) {
	// Find the last parent with level not higher than level + delta

	if (delta > 0) {
		const mser_region * parent = this;



		while (parent->parent_ && (parent->parent_->level_ <= (level_ + delta)))

			parent = parent->parent_;



		// Calculate variation
		if (parent->parent_ != NULL || parent->level_ == (level_ + delta)) {
			variation_ = static_cast<f32>(parent->area_ - area_) / area_;
		} else {
			variation_ = -1.0f;
		}
	} else {
		variation_ = -1.f;
	}

	// Whether or not the region *could* be stable

	stable_ = (!parent_ || (nms_similarity < 0 || variation_ < 0 || parent_->variation_ < 0 || level_ + 1 != parent_->level_ || variation_ - parent_->variation_ <= -nms_similarity)) &&

		(area_ >= minArea) && (area_ <= maxArea) && (variation_ <= maxVariation);



	// Process all the children

	for (mser_region * child = child_; child; child = child->next_) {

		child->process(delta, minArea, maxArea, maxVariation, nms_similarity);

		if (stable_) {
			if ((nms_similarity < 0 || variation_ < 0 || child->variation_ < 0 || level_ != child->level_ + 1 || variation_ - child->variation_ <= -nms_similarity)) {				
				
			} else {
				stable_ = sys_false;
			}
		}
	}
}



b8 img_idiap_mser::mser_region::check(f32 variation, i32 area) const

{

	if (area_ <= area)

		return sys_true;



	if (stable_ && (variation_ < variation))

		return sys_false;



	for (mser_region * child = child_; child; child = child->next_)

		if (!child->check(variation, area))

			return sys_false;



	return sys_true;

}



void img_idiap_mser::mser_region::save(f32 minDiversity, f32 nms_similarity, vector<mser_region> & regions)

{

	if (stable_) {

		int minParentArea = minDiversity < 0 ? area_ : (i32)(area_ / (1.0 - minDiversity) + 0.5);

		const mser_region * parent = this;

		while (parent->parent_ && (parent->parent_->area_ < minParentArea)) {

			parent = parent->parent_;

			if (parent->stable_ && (nms_similarity >= 0 && variation_ >= 0 && parent_->variation_ >= 0 && parent_->level_ == level_ + 1 && parent->variation_ - variation_ < -nms_similarity)) {

				stable_ = sys_false;

				break;

			}

		}



		if (stable_) {

			int maxChildArea = minDiversity < 0 ? area_ : (i32)(area_ * (1.0 - minDiversity) + 0.5);



			if (!check(variation_, maxChildArea))

				stable_ = sys_false;

		}



		if (stable_) {

			regions.push_back(*this);

			regions.back().parent_ = 0;

			regions.back().child_ = 0;

			regions.back().next_ = 0;

		}

	}



	for (mser_region * child = child_; child; child = child->next_)

		child->save(minDiversity, nms_similarity, regions);

}



void img_idiap_mser::mser_region::detect(i32 delta, i32 minArea, i32 maxArea, f32 maxVariation, f32 nms_similarity, f32 minDiversity, vector<mser_region> & regions) {

	process(delta, minArea, maxArea, maxVariation, nms_similarity);

	save(minDiversity, nms_similarity, regions);

}

void img_idiap_mser::allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask) {
	if (!src.is_continuous()) {
		m_src.copy_from(src);

		basiclog_assert2(sys_false);
	} else {
		m_src = src;
	}

	pool_.resize(256);

	if (m_accessible.capacity() == 0) {
		m_accessible.resize(m_src.size()[0] * m_src.size()[1], 0);
	} else {
		m_accessible.resize(m_src.size()[0] * m_src.size()[1]);
		memset(&m_accessible[0], 0, sizeof(b8) * m_accessible.size());

	}
	
}

void img_idiap_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {

}

void img_idiap_mser::recognize_mser() {

}

void img_idiap_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	vector<mser_region> regions;

	if (gray_mask != 0 && m_from_min_max[0]) {
		/*for (i32 i = 0; i < (i32)m_accessible.size(); ++i) {
		m_accessible[i] = 0;
		}*/

		memset(&m_accessible[0], 0, sizeof(b8) * m_accessible.size());
	} 

	vector<i32> boundaryPixels[256];

	int priority = 256;

	vector<mser_region *> regionStack;
	regionStack.push_back(new (&pool_[poolIndex_++]) mser_region);

#define get_gray(gray)	((gray) ^ gray_mask)

	int curPixel = 0;
	int curEdge = 0;

	u8* bits = m_src.data();
	u8 curLevel = get_gray(bits[0]);

	i32 width = m_src.size()[1];
	i32 height = m_src.size()[0];

	m_accessible[0] = sys_true;



	// 3. Push an empty component with current level onto the component stack.

step_3:

	regionStack.push_back(new (&pool_[poolIndex_++]) mser_region(curLevel, curPixel));



	if (poolIndex_ == pool_.size())

		doublePool(regionStack);

	for (;;) {

		const int x = curPixel % width;

		const int y = curPixel / width;



		for (; curEdge < (m_connected_type == img_Connected_Type_8_Neibour ? 8 : 4); ++curEdge) {

			int neighborPixel = curPixel;

			if (m_connected_type == img_Connected_Type_8_Neibour) {

				switch (curEdge) {

				case 0: if (x < width - 1) neighborPixel = curPixel + 1; break;

				case 1: if ((x < width - 1) && (y > 0)) neighborPixel = curPixel - width + 1; break;

				case 2: if (y > 0) neighborPixel = curPixel - width; break;

				case 3: if ((x > 0) && (y > 0)) neighborPixel = curPixel - width - 1; break;

				case 4: if (x > 0) neighborPixel = curPixel - 1; break;

				case 5: if ((x > 0) && (y < height - 1)) neighborPixel = curPixel + width - 1; break;

				case 6: if (y < height - 1) neighborPixel = curPixel + width; break;

				default: if ((x < width - 1) && (y < height - 1)) neighborPixel = curPixel + width + 1; break;

				}

			}

			else {

				switch (curEdge) {

				case 0: if (x < width - 1) neighborPixel = curPixel + 1; break;

				case 1: if (y < height - 1) neighborPixel = curPixel + width; break;

				case 2: if (x > 0) neighborPixel = curPixel - 1; break;

				default: if (y > 0) neighborPixel = curPixel - width; break;

				}

			}

			if (neighborPixel >= m_accessible.size()) {
				basiclog_assert2(sys_false);
			}

			if (neighborPixel != curPixel && !m_accessible[neighborPixel]) {

				u8 neighborLevel = get_gray(bits[neighborPixel]);

				m_accessible[neighborPixel] = sys_true;

				if (neighborLevel >= curLevel) {

					boundaryPixels[neighborLevel].push_back(neighborPixel << 4);

					if (neighborLevel < priority)

						priority = neighborLevel;

				}

				else {

					boundaryPixels[curLevel].push_back((curPixel << 4) | (curEdge + 1));



					if (curLevel < priority)

						priority = curLevel;


					curPixel = neighborPixel;

					curEdge = 0;

					curLevel = neighborLevel;



					goto step_3;

				}

			}

		}


		regionStack.back()->accumulate(x, y);

		if (priority == 256) {

			regionStack.back()->detect(m_delta, m_min_point, m_max_point, m_stable_variation, m_nms_similarity, m_duplicated_variation, regions);

			poolIndex_ = 0;

			goto step_4;
		}



		curPixel = boundaryPixels[priority].back() >> 4;

		curEdge = boundaryPixels[priority].back() & 15;



		boundaryPixels[priority].pop_back();



		while (boundaryPixels[priority].empty() && (priority < 256))

			++priority;



		const int newPixelGreyLevel = get_gray(bits[curPixel]);



		if (newPixelGreyLevel != curLevel) {

			curLevel = newPixelGreyLevel;
			processStack(newPixelGreyLevel, curPixel, regionStack);

		}

	}

step_4:
	;

	//i32 index = (gray_mask == 0) ? 0 : 1;
	//mt_point*& memory = msers.m_memory[index];
	//msers.m_memory_size[index] = (i32)regions.size() * 3;
	//memory = (mt_point*)malloc(msers.m_memory_size[index] * sizeof(mt_point));

	//vector<img_mser>& t_msers = msers.m_msers[index];
	//t_msers.resize(regions.size());

	//m_channel_total_pixel_number = 0;

	//f32* offset = (f32*)memory;

	//for (i32 i = 0; i < (i32)regions.size(); ++i) {
	//	mser_region& cur_region = regions[i];

	//	t_msers[i].m_gray_level = get_gray(cur_region.level_);
	//	t_msers[i].m_size = cur_region.area_;

	//	m_channel_total_pixel_number += cur_region.area_;

	//	//basiclog_info2(sys_strcombine()<<L"gray "<<t_msers[i].m_gray_level<<L" area "<<cur_region.area_);

	//	t_msers[i].m_memory_type = img_mser::Memory_Type_Moments;
	//	t_msers[i].m_points = (mt_point*)offset;
	//	offset[0] = cur_region.moments_[0];
	//	offset[1] = cur_region.moments_[1];
	//	offset[2] = cur_region.moments_[2];
	//	offset[3] = cur_region.moments_[3];
	//	offset[4] = cur_region.moments_[4];

	//	offset += 6;
	//}

	//if (gray_mask == 255 || !m_from_min_max[1]) {
	//	i32 memory_size = (i32)(sizeof(b8) * m_accessible.capacity());
	//	memory_size += (i32)(sizeof(mser_region) * (i32)pool_.capacity());
	//	memory_size += (i32)(sizeof(mser_region*) * regionStack.capacity());
	//	for (i32 i = 0; i < 256; ++i) {
	//		memory_size += (i32)(sizeof(i32) * boundaryPixels[i].size());
	//	}

	//	basiclog_info2(sys_strcombine()<<L"idiap memory cost "<< memory_size / 1024.0 / 1024.0 <<L"MB");
	//}
}

void img_idiap_mser::processStack(int newPixelGreyLevel, int pixel, vector<mser_region *> & regionStack) {

	// 1. Process component on the top of the stack. The next grey-level is the minimum of

	// newPixelGreyLevel and the grey-level for the second component on the stack.

	do {

		mser_region * top = regionStack.back();



		regionStack.pop_back();



		// 2. If newPixelGreyLevel is smaller than the grey-level on the second component on the

		// stack, set the top of stack grey-level to newPixelGreyLevel and return from sub-routine

		// (This occurs when the new pixel is at a grey-level for which there is not yet a component

		// instantiated, so we let the top of stack be that level by just changing its grey-level.

		if (newPixelGreyLevel < regionStack.back()->level_) {

			regionStack.push_back(new (&pool_[poolIndex_++]) mser_region(newPixelGreyLevel, pixel));



			if (poolIndex_ == pool_.size())
				top = reinterpret_cast<mser_region *>(reinterpret_cast<char *>(top) + doublePool(regionStack));

			regionStack.back()->merge(top);



			return;

		}



		// 3. Remove the top of stack and merge it into the second component on stack as follows:

		// Add the first and second moment accumulators together and/or join the pixel lists.

		// Either merge the histories of the components, or take the history from the winner. Note

		// here that the top of stack should be considered one ¡¯time-step¡¯ back, so its current

		// size is part of the history. Therefore the top of stack would be the winner if its

		// current size is larger than the previous size of second on stack.

		regionStack.back()->merge(top);

	}

	// 4. If(newPixelGreyLevel>top of stack grey-level) go to 1.

	while (newPixelGreyLevel > regionStack.back()->level_);

}



i64 img_idiap_mser::doublePool(vector<mser_region *> & regionStack)

{

	assert(!pool_.empty()); // Cannot double the size of an empty pool



	vector<mser_region> newPool(pool_.size() * 2);

	copy(pool_.begin(), pool_.end(), newPool.begin());



	// Cast to char in case the two pointers do not share the same alignment

	const i64 offset = reinterpret_cast<char *>(&newPool[0]) -

		reinterpret_cast<char *>(&pool_[0]);



	for (size_t i = 0; i < pool_.size(); ++i) {

		if (newPool[i].parent_)

			newPool[i].parent_ =

			reinterpret_cast<mser_region *>(reinterpret_cast<char *>(newPool[i].parent_) + offset);



		if (newPool[i].child_)

			newPool[i].child_ =

			reinterpret_cast<mser_region *>(reinterpret_cast<char *>(newPool[i].child_) + offset);



		if (newPool[i].next_)

			newPool[i].next_ =

			reinterpret_cast<mser_region *>(reinterpret_cast<char *>(newPool[i].next_) + offset);

	}



	for (size_t i = 0; i < regionStack.size(); ++i)

		regionStack[i] =

		reinterpret_cast<mser_region *>(reinterpret_cast<char *>(regionStack[i]) + offset);



	pool_.swap(newPool);



	return offset;

}