#include "stdafx.h"
#include "img_origin_mser.h"

img_original_mser::~img_original_mser() {
	basicsys_delete_array(m_stack_helper);
}

//void img_original_mser::inner_extract(img_multi_msers& res, const mt_mat& gray_src, const img_mask_info<u8>& mask) {
//	res = img_multi_msers();
//	
//	sys_timer timer("ml_original_mser::extract");
//	timer.begin();
//
//	preProcessImage(gray_src, mask);
//
//	if (m_from_min_max[0]) {	
//		extractPass(sys_true);
//		collect(res.m_msers[0], m_regions, sys_true);
//
//		if (m_from_min_max[1]) {
//			resetPreInfo();
//		}
//	}
//
//	if (m_from_min_max[1]) {
//		reversePixelValue();
//		extractPass(sys_false);
//		collect(res.m_msers[1], m_regions, sys_false);
//	}
//
//	if (!m_enable_memory_cache) {
//		clear_memory_cache();
//	}
//
//	timer.end();
//}

void img_original_mser::recognize_mser() {
	i32 invalidCount = 0;
	i32 nmsCount = 0;
	i32 duplicatedCount = 0;

	sys_timer timer1("StableTimer");
	timer1.begin();

	for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
		m_regions[i].mShortCut = &m_regions[i];

		if (m_regions[i].m_merged) {
			m_regions[i].mFlags  = Flag_Invalid;
			++invalidCount;
			continue;
		}

		// Find real parent regions for all regions
		mser_region* parentRegion = m_regions[i].mParent;	

		while (NULL != parentRegion && parentRegion->m_merged) {
			parentRegion = parentRegion->mParent;
		}

		m_regions[i].mParent = parentRegion;

		if (NULL != parentRegion) {
			mser_region* parentParentRegion = parentRegion->mParent;	

			while (NULL != parentParentRegion && parentParentRegion->m_merged) {
				parentParentRegion = parentParentRegion->mParent;
			}

			parentRegion->mParent = parentParentRegion;
		}
		//if (NULL == regions[i].mParent) {
		//	regions[i].mFlags  = Flag_Invalid;
		//	++invalidCount;
		//	continue;
		//}
	}

	for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
		if (m_regions[i].mFlags != Flag_Invalid) {

			if (m_delta > 0) {
				calculateStableVariantion(&m_regions[i]);

				if (m_regions[i].mStableVariantion > m_stable_variation) {
					if (m_regions[i].mFlags != Flag_Invalid) {
						m_regions[i].mFlags = Flag_Invalid;
						++invalidCount;	

						continue;
					}
				}
			}
			
			if (m_regions[i].mSize < m_min_point || m_regions[i].mSize > m_max_point) {
				m_regions[i].mFlags  = Flag_Invalid;
				++invalidCount;
			}
		}
	}

	if (m_delta > 0 && m_nms_similarity >= 0) {
		for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
			mser_region& currentRegion = m_regions[i]; 
			mser_region* parentRegion = currentRegion.mParent;
			if (currentRegion.mStableVariantion >= 0 && parentRegion != NULL && parentRegion->mStableVariantion >= 0 && parentRegion->mGrayLevel == currentRegion.mGrayLevel + 1) {
				f64 subValue = parentRegion->mStableVariantion - currentRegion.mStableVariantion;
				if (subValue > m_nms_similarity) {
					if (Flag_Invalid != parentRegion->mFlags) {
						parentRegion->mFlags = Flag_Invalid;
						++invalidCount;	
						++nmsCount;
					}
				} else if (-subValue > m_nms_similarity) {
					if (Flag_Invalid != currentRegion.mFlags) {
						currentRegion.mFlags = Flag_Invalid;
						++invalidCount;	
						++nmsCount;
					}
				} 
			}
		}
	}

	timer1.end();

	sys_timer timer2("DuplicatedTimer");
	timer2.begin();

	vector<mser_region*> memoryHelper;
	memoryHelper.reserve(100);

	// Remove duplicated regions
	for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
		if (Flag_Unknow != m_regions[i].mFlags) {
			continue;
		}

		memoryHelper.clear();
		memoryHelper.push_back(&m_regions[i]);
		getDuplicatedRegions(memoryHelper, &m_regions[i], &m_regions[i]);

		i32 middleIndex = (i32)memoryHelper.size() / 2;

		if (middleIndex > 0) {
			i32 oldSize = (i32)memoryHelper.size();
			getDuplicatedRegions(memoryHelper, memoryHelper[middleIndex], memoryHelper.back());
		}

		for (i32 j = 0; j < (i32)memoryHelper.size(); ++j) {
			if (j != middleIndex) {
				memoryHelper[j]->mFlags = Flag_Invalid;
				++invalidCount;
				++duplicatedCount;
			} else {
				//We consider the middle one is the "maximally" stable extremal region
				memoryHelper[j]->mFlags = Flag_Valid;
			}	
		}
	}

	i32 erCount = 0;
	i32 validER = 0;
	m_channel_total_pixel_number = 0;

	for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
		//basiclog_assert2(Flag_Unknow != regions[i].mFlags);

		if (!m_regions[i].m_merged) {
			if (Flag_Valid == m_regions[i].mFlags) {
				++validER;
				m_channel_total_pixel_number += (u32)m_regions[i].mSize;
			}

			++erCount;
		}
	}

	//basiclog_assert2(validER == (i32)regions.size() - invalidCount);

	timer2.end();
	basiclog_info2(sys_strcombine()<<"er number "<<erCount << " valid mser number " << validER << " nms number " << nmsCount << " duplicated number "<<duplicatedCount);
	m_invalid_er_number = invalidCount;
}

void img_original_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	vector<img_mser>& ers = msers.m_msers[(gray_mask == 0) ? 0 : 1];

	ers.resize(m_regions.size() - m_invalid_er_number);
	i32 index = 0;

	for (i32 i = 0; i < (i32)m_regions.size(); ++i) {
		if (Flag_Valid == m_regions[i].mFlags) {
			ers[index].m_gray_level = (gray_mask == 0) ? m_regions[i].mGrayLevel : 255 - m_regions[i].mGrayLevel;

			ers[index].m_size = m_regions[i].mSize;
			ers[index].m_memory_type = img_mser::Memory_Type_Self;
			ers[index].m_points = new mt_point[ers[index].m_size];

			linked_point* itLinkPt = m_regions[i].mHead;

			for (i32 it = 0; it < m_regions[i].mSize; ++it, itLinkPt = itLinkPt->m_next) {
				ers[index].m_points[it] = itLinkPt->m_point;
				ers[index].m_rect.adjust_by_append_point(itLinkPt->m_point);
			}

			++index;
		}
	}
}

b8 img_original_mser::assertHeadNotMerged(vector<mser_region>& nodes, vector<vector<linked_point>>& pts) {
	for (i32 row = 1; row < (i32)pts.size() - 1; ++row) {
		for (i32 col = 1; col < (i32)pts[0].size() - 1; ++col) {
			if (NULL != pts[row][col].m_according_region) {
				linked_point* head = pts[row][col].m_according_region->mHead;

				while (sys_true) {
					if (head == head->m_according_region->mHead) {
						break;
					}

					head = head->m_according_region->mHead;
				}

				if (head->m_according_region->m_merged) {
					return sys_false;
				}
			}
		}
	}

	return sys_true;
}

b8 img_original_mser::assertRegionPtCount(vector<mser_region>& nodes, vector<vector<linked_point>>& pts) {
	for (i32 i = 0; i < (i32)nodes.size(); ++i) {
		i32 ptCount = 0;

		for (linked_point* it = nodes[i].mHead; it != nodes[i].mTail; it = it->m_next) {
			++ptCount;
		}

		++ptCount;

		if (ptCount != nodes[i].mSize) {
			return sys_false;
		}
	}

	return sys_true;
}

void img_original_mser::getDuplicatedRegions(vector<mser_region*>& duplicatedRegions, mser_region* stableRegion, mser_region* beginRegion) {
	mser_region* parentRegion = beginRegion->mParent;

	while (sys_true) {
		if (NULL == parentRegion) {
			break;
		}

		if (parentRegion->mSize > m_max_point) {
			break;
		}

		f64 variationFromBegin = (parentRegion->mSize - stableRegion->mSize) / (f64)stableRegion->mSize;

		if (variationFromBegin > m_duplicated_variation) {
			break;
		}

		if (Flag_Invalid == parentRegion->mFlags) {
			parentRegion = parentRegion->mParent;
			continue;
		}

		duplicatedRegions.push_back(parentRegion);
		parentRegion = parentRegion->mParent;
	}
}

void img_original_mser::getStabilityRegions(vector<mser_region*>& duplicatedRegions, mser_region* beginRegion) {
	mser_region* parentRegion = beginRegion->mParent;

	while (sys_true) {
		if (NULL == parentRegion) {
			break;
		}

		f64 variationFromBegin = (parentRegion->mSize - beginRegion->mSize) / (f64)beginRegion->mSize;

		if (variationFromBegin > m_duplicated_variation) {
			break;
		}

		duplicatedRegions.push_back(parentRegion);
		parentRegion = parentRegion->mParent;
	}
}

void img_original_mser::calculateStableVariantion(mser_region* region) {
	//basiclog_assert2(!region->mMerged);
	i32 grayLevelThreshold = region->mGrayLevel + m_delta;

	//Region* parent = region->mParent;
	//Region* temp = region;

	//while (parent != NULL && parent->mGrayLevel <= grayLevelThreshold) {
	//	temp = parent;
	//	parent = parent->mParent;
	//}
	//
	//if (NULL != parent) {
	//	region->mStableVariantion = (temp->mSize - region->mSize) / (f32)region->mSize;
	//}

	mser_region* parent = region->mShortCut->mParent;
	mser_region* temp = region->mShortCut;

	while (parent != NULL && parent->mGrayLevel <= grayLevelThreshold) {
		temp = parent;
		parent = parent->mParent;
	}

	if (parent != NULL || temp->mGrayLevel == grayLevelThreshold) {
		region->mParent->mShortCut = temp;
		region->mStableVariantion = (temp->mSize - region->mSize) / (f32)region->mSize;
	}
}

void img_original_mser::extractPass(b8 fromMin) {
	mt_point top;
	mt_point other;
	i16* topImageData;
	i16* otherImageData;
	b8 connectCondition = sys_false;
	mser_region* currentRegion;
	mser_region* otherRegion;
	linked_point* startLinkPoint;
	linked_point* otherLinkPoint;
	mser_region* tempRegion;

	i32 startIndex = 0;
	i32 stopIndex = (i32)m_pts.size();
	i32 step = 1;

	if (!fromMin) {
		startIndex = (i32)m_pts.size() - 1;
		stopIndex = -1;
		step = -1;
	}

	for (i32 i = startIndex; i != stopIndex; i += step) {
		topImageData = mTempImage.ptr<i16>(m_pts[i].m_y, m_pts[i].m_x);
		startLinkPoint = &m_link_pts[topImageData - (i16*)mTempImage.data()];

		if (NULL == startLinkPoint->m_according_region) {
			m_regions.push_back(mser_region());
			currentRegion = &m_regions.back();

			startLinkPoint->m_according_region = currentRegion;
			currentRegion->mHead = startLinkPoint;
			currentRegion->mTail = currentRegion->mHead;
			currentRegion->mSize = 1;
			currentRegion->mShortCut = currentRegion;
			currentRegion->mGrayLevel = *topImageData;
			m_stack_helper[++m_stack_top_index] = topImageData;

			while (m_stack_top_index != -1) {
				topImageData = m_stack_helper[m_stack_top_index--];

				for (i32 j = 1; j < m_window_size; ++j) {
					otherImageData = topImageData + mImageStep[j];

					if (*otherImageData < 0) {
						continue;
					}

					if (*otherImageData <= *topImageData) {
						otherLinkPoint = &m_link_pts[otherImageData - (i16*)mTempImage.data()];

						if (NULL == otherLinkPoint->m_according_region) {
							//if not connected
							currentRegion->mTail->m_next = otherLinkPoint;
							currentRegion->mTail = currentRegion->mTail->m_next;
							otherLinkPoint->m_according_region = currentRegion;

							m_stack_helper[++m_stack_top_index] = otherImageData;
							++currentRegion->mSize;
						} else {							
							otherRegion = otherLinkPoint->m_according_region->mShortCut;

							while (otherRegion->mShortCut != otherRegion) {
								tempRegion = otherRegion;
								otherRegion = otherRegion->mShortCut;
								tempRegion->mShortCut = currentRegion;
							}

							otherLinkPoint->m_according_region->mShortCut = currentRegion;

							if (otherRegion != currentRegion) {

								otherRegion->m_merged = (otherRegion->mGrayLevel == currentRegion->mGrayLevel);
								otherRegion->mParent = currentRegion;
								currentRegion->mTail->m_next = otherRegion->mHead;
								currentRegion->mTail = otherRegion->mTail;
								currentRegion->mSize += otherRegion->mSize;
								otherRegion->mShortCut = currentRegion;
							}
						}
					}
				}
			}
		}
	}
}

void img_original_mser::resetPreInfo() {
	m_regions.clear();

	for (i32 i = 0; i < (i32)m_link_pts.size(); ++i) {
		m_link_pts[i].m_next = NULL;
		m_link_pts[i].m_according_region = NULL;
	}
}

void img_original_mser::clear_memory_cache() {
	basicsys_delete_array(m_stack_helper);
	m_stack_helper_size = 0;

	mTempImage = mt_mat();

	vector<mser_region> temp;
	m_regions.swap(temp);

	vector<linked_point> temp2;
	m_link_pts.swap(temp2);

	vector<mt_point> temp3;
	m_pts.swap(temp3);
}

void img_original_mser::allocate_memory(const mt_mat& image, const img_mask_info<u8>& mask) {
	m_window_size = (img_Connected_Type_8_Neibour == m_connected_type ? 9 : 5);
	mDX = (img_Connected_Type_8_Neibour == m_connected_type ? img_DX_3_9 : img_DX_3_5);
	mDY = (img_Connected_Type_8_Neibour == m_connected_type ? img_DY_3_9 : img_DY_3_5);

	i32 area = 0;
	mt_rect activate_rect;
	i32 expandImageAppendOffset;

	if (img_Mask_Type_Null == mask.m_mask_type || img_Mask_Type_Rect == mask.m_mask_type) {	
		if (img_Mask_Type_Null == mask.m_mask_type) {
			activate_rect.set_rect(mt_point(0, 0), mt_size(image.size()[1], image.size()[0]));
		} else {
			activate_rect = mask.m_mask_rect;
		}

		area = activate_rect.getArea();
		mt_size targetImageSize(mask.m_mask_rect.m_width, mask.m_mask_rect.m_height);

		mTempImage = mt_mat(activate_rect.m_height + 2, activate_rect.m_width + 2, mt_I16C1, mt_scalar(i16(0x8000)));
		expandImageAppendOffset = (i32)mTempImage.step()[0] / sizeof(i16) - activate_rect.m_width;
		i32 rawImageAppendOffset = (i32)image.step()[0] - activate_rect.m_width;

		i16* tempImageData = mTempImage.ptr<i16>(1, 1);

		const uchar* imageData = image.ptr<uchar>(activate_rect.m_top, activate_rect.m_left);

		for (i32 row = 1; row <= activate_rect.m_height; ++row) {
			for (i32 col = 1; col <= activate_rect.m_width; ++col) {

				//basiclog_assert2(imageData == image.ptr<uchar>(row - 1 + activate_rect.m_top, col - 1 + activate_rect.m_left));
				//basiclog_assert2(tempImageData == mTempImage.ptr<i16>(row, col));

				*tempImageData++ = *imageData++;
			}

			tempImageData += expandImageAppendOffset;
			imageData += rawImageAppendOffset;
		}
	} else {
		if (img_Mask_Type_Image == mask.m_mask_type) {
			//First we statistic the maskImage
			i32 rowMax = 0;
			i32 colMax = 0;
			i32 rowMin = image.size()[0];
			i32 colMin = image.size()[1];

			i32 mask_append_offset = (i32)mask.m_mask_image.step()[0] - mask.m_mask_image.size()[1];

			const uchar* maskData = (uchar*)mask.m_mask_image.data();

			for (i32 row = 0; row < mask.m_mask_image.size()[0]; ++row) {
				for (i32 col = 0; col < mask.m_mask_image.size()[1]; ++col) {

					if (1 == *maskData++) {
						rowMax = max(rowMax, row);
						colMax = max(colMax, col);
						rowMin = min(rowMin, row);
						colMin = min(colMin, col);
					}
				}

				maskData += mask_append_offset;
			}

			mt_size targetImageSize(colMax - colMin + 1, rowMax - rowMin + 1);
			activate_rect.set_rect(mt_point(colMin, rowMin), targetImageSize);

			mTempImage = mt_mat(targetImageSize.m_height + 2, targetImageSize.m_width + 2, mt_I16C1, mt_scalar(i16(0x8000)));
			expandImageAppendOffset = (i32)mTempImage.step()[0] / sizeof(i16) - targetImageSize.m_width;
			i32 rawImageAppendOffset = (i32)image.step()[0] - targetImageSize.m_width;
			mask_append_offset = (i32)mask.m_mask_image.step()[0] - targetImageSize.m_width;

			i16* tempImageData = mTempImage.ptr<i16>(1, 1);

			const uchar* imageData = image.ptr<uchar>(rowMin, colMin);
			maskData = mask.m_mask_image.ptr<uchar>(rowMin, colMin);

			for (i32 row = 1; row <= targetImageSize.m_height; ++row) {
				for (i32 col = 1; col <= targetImageSize.m_width; ++col) {

					basiclog_assert2(maskData == mask.m_mask_image.ptr<uchar>(row - 1 + rowMin, col - 1 + colMin));
					basiclog_assert2(imageData == image.ptr<uchar>(row - 1 + rowMin, col - 1 + colMin));
					basiclog_assert2(tempImageData == mTempImage.ptr<i16>(row, col));

					if (1 == *maskData++) {
						*tempImageData = *imageData;	
						++area;
					}

					++tempImageData;
					++imageData;
				}

				tempImageData += expandImageAppendOffset;
				maskData += mask_append_offset;
				imageData += rawImageAppendOffset;
			}
		} else if (img_Mask_Type_Value == mask.m_mask_type) {
			i32 rowMax = 0;
			i32 colMax = 0;
			i32 rowMin = image.size()[0];
			i32 colMin = image.size()[1];

			const uchar* imageData = (uchar*)image.data();

			for (i32 row = 0; row < image.size()[0]; ++row) {
				for (i32 col = 0; col < image.size()[1]; ++col) {

					if (*imageData++ != mask.m_mask_value) {
						rowMax = max(rowMax, row);
						colMax = max(colMax, col);
						rowMin = min(rowMin, row);
						colMin = min(colMin, col);
					}
				}
			}

			mt_size targetImageSize(colMax - colMin + 1, rowMax - rowMin + 1);
			activate_rect.set_rect(mt_point(colMin, rowMin), targetImageSize);

			mTempImage = mt_mat(targetImageSize.m_height + 2, targetImageSize.m_width + 2, mt_I16C1, mt_scalar(i16(0x8000)));
			expandImageAppendOffset = (i32)mTempImage.step()[0] / sizeof(i16) - targetImageSize.m_width;
			i32 rawImageAppendOffset = (i32)image.step()[0] - targetImageSize.m_width;

			i16* tempImageData = mTempImage.ptr<i16>(1, 1);

			imageData = image.ptr<uchar>(rowMin, colMin);

			for (i32 row = 1; row <= targetImageSize.m_height; ++row) {
				for (i32 col = 1; col <= targetImageSize.m_width; ++col) {

					basiclog_assert2(imageData == image.ptr<uchar>(row - 1 + rowMin, col - 1 + colMin));
					basiclog_assert2(tempImageData == mTempImage.ptr<i16>(row, col));

					if (*imageData != mask.m_mask_value) {
						*tempImageData = *imageData;	
						++area;
					}

					++tempImageData;
					++imageData;
				}

				tempImageData += expandImageAppendOffset;
				imageData += rawImageAppendOffset;
			}
		}
	}

	if (m_stack_helper_size != area) {
		m_stack_helper_size = area;
		basicsys_delete_array(m_stack_helper);
		m_stack_helper = new i16*[area];
	} else {
		resetPreInfo();
	}

	m_stack_top_index = -1;

	for (i32 i = 1; i < m_window_size; ++i) {
		mImageStep[i] = img_img::image_pixel_offset(mTempImage.step()[0], sizeof(i16), mDX[i], mDY[i]);
	}

	m_regions.reserve(area);

	m_link_pts.resize(mTempImage.size()[0] * mTempImage.size()[1]);

	i32 row = 0;
	i32 col = 0;

	for (i32 i = 0; i < (i32)m_link_pts.size(); ++i) {
		m_link_pts[i].m_next = NULL;
		m_link_pts[i].m_according_region = NULL;

		m_link_pts[i].m_point.m_y = row + activate_rect.m_top - 1;
		m_link_pts[i].m_point.m_x = col + activate_rect.m_left - 1;

		++col;

		if (col == mTempImage.size()[1]) {
			++row;
			col = 0;
		}
	}

	vector<i32> colorPixelCounts;
	colorPixelCounts.resize(256, 0);

	m_pts.resize(area);

	i16* data = mTempImage.ptr<i16>(1, 1);

	for (i32 row = 1; row <= activate_rect.m_height; ++row) {
		for (i32 col = 1; col <= activate_rect.m_width; ++col) {
			//basiclog_assert2(data == mTempImage.ptr<i16>(row, col));

			if (*data >= 0) {
				++colorPixelCounts[*data];
			}

			++data;
		}

		data += expandImageAppendOffset;
	}

	vector<i32> colorStartIndexs;
	colorStartIndexs.resize(colorPixelCounts.size() + 1);
	mt_helper::integral_array(colorStartIndexs, colorPixelCounts);	

	m_channel_total_running_memory += sizeof(u16) * mTempImage.size()[0] * mTempImage.size()[1] + sizeof(linked_point) * area + sizeof(mser_region) * area + sizeof(mt_point) * area + sizeof(i16*) * area;

	vector<i32>& tempColorStartIndexs = colorPixelCounts;
	tempColorStartIndexs = colorStartIndexs;

	data = mTempImage.ptr<i16>(1, 1);

	for (i32 row = 1; row <= activate_rect.m_height; ++row) {
		for (i32 col = 1; col <= activate_rect.m_width; ++col) {
			if (*data >= 0) {
				m_pts[colorStartIndexs[*data]++] = mt_point(col, row);
			}

			++data;
		}

		data += expandImageAppendOffset;
	}
}

void img_original_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	if (gray_mask == 0) {
		extractPass(sys_true);
	} else {
		if (m_from_min_max[0]) {
			resetPreInfo();
		}

		reversePixelValue();
		extractPass(sys_false);
	}
}

void img_original_mser::reversePixelValue() {
	i16* data = mTempImage.ptr<i16>(1, 1);
	i32 expandImageAppendOffset = (i32)mTempImage.step()[0] / sizeof(i16) - (mTempImage.size()[1] - 2);

	i32 row_max = mTempImage.size()[0] - 2;
	i32 col_max = mTempImage.size()[1] - 2;

	for (i32 row = 1; row <= row_max; ++row) {
		for (i32 col = 1; col <= col_max; ++col) {
			if (*data >= 0) {
				*data = 0xff - *data;
			}
			++data;
		}

		data += expandImageAppendOffset;
	}
}