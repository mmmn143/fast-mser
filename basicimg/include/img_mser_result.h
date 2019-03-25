/** @file img_mser_result.h
This file define the basic structure of MSER algorithm needed.
*/
#pragma once

#include "img_connected_component.h"

namespace basicimg {
	class img_mser_param {
	public:
	img_mser_param(i32 stable_detla = 3, f64 duplicate_variation = 0.25, f64 stable_max_variation = 0.25, f64 nms_variation = 0.25, f64 nms_smility = 0.05, 
		img_Connected_Type connect_type = img_Connected_Type_4_Neibour, i32 min_point = 30, i32 max_point = 160000) 
		: m_stable_delta(stable_detla)
		, m_duplicated_variantion(duplicate_variation)
		, m_stable_max_variantion(stable_max_variation)
		, m_nms_variation(nms_variation)
		, m_nms_similiarity(nms_smility)
		, m_connected_type(connect_type)
		, m_collected_min_point_number(min_point)
		, m_collected_max_point_number(max_point) {
			m_enable_from_min_and_from_max[0] = sys_true;
			m_enable_from_min_and_from_max[1] = sys_true;
	}


	f64 m_duplicated_variantion;	//重复的阈值，如果父子像素差小于等于于子的mDuplicateVariantion，则为重复

	i32 m_stable_delta;	//组件与周围对比度较低时(灰度值相差小于等于mStableDelta),无法形成MSER		
	f64 m_stable_max_variantion;	//如果父子像素差小于等于于子的mStableMaxVariantion，则为稳定区域
	
	/** Threshold for NMS.
	Only the ER below mNMSVariation can inhibit other ERs. When the mNMSVariation <= 0, the MSER algorithm turns to SER algorithm without NMS.
	@note It must does not exceed mStableMaxVariantion.
	*/
	f64 m_nms_variation;	
	
	/**
	* Threshold for NMS.
	* If the stability of two ERs in range of mNMSSimility, they will not be inhibited.
	*/
	f64 m_nms_similiarity;

	i32 m_collected_min_point_number;	//!< Threshold for the maximal point count of extracted MSER.
	i32 m_collected_max_point_number;	//!< Threshold for the minimal point count of extracted MSER.

	/**
	Flags for the channel of MSER, index 0 is true indicates the algorithm should perform on the gray order from low to high gray level, that is also mean dark on bright channel.
	index 1 is true indicates the algorithm should perform on the gray order from high to low gray level, that is also mean bright on dark channel.
	*/
	b8 m_enable_from_min_and_from_max[2];	
										
	img_mask_info<u8> m_mask_info;	//!< Mask information for algorithm.
	img_Connected_Type m_connected_type;	//!< Connected type, either 8-neighboring or 4-neighboring.
};

class img_mser_component {
public:
	img_mser_component() 
		: m_level(0) {

	}

	img_connected_component m_cc;
	u8 m_level;
};

class img_mser_channel_image {
public:

	img_mser_channel_image() {
	}

	mt_mat m_level_image;
	mt_mat m_mser_id_image;
};


class img_mser_result {
public:

	enum Result_Type {
		Result_Type_From_Min,
		Result_Type_From_Max,
		Result_Type_Both,
	};

	void shallow_copy_cc(vector<img_connected_component*>& ccs, Result_Type type = Result_Type_Both);

	void shallow_copy_cc(vector<img_mser_component*>& ccs, Result_Type type = Result_Type_Both);

	mt_mat generate_mser_result_image(const mt_mat& src, Result_Type type = Result_Type_Both);

	img_mser_channel_image generate_mser_channel_image(const mt_mat& src, bool fromMin);

	vector<img_mser_component> m_msers[2];
};
}

