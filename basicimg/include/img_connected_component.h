#pragma once

#include "img_types.h"

#define basicimg_cc_n_begin()	\
	normal_connected_info preInfo;\
	normal_extract_start(preInfo, image, maskInfo, connect_type, 2000);\
	image_pixel_type* start_image_data = preInfo.m_temp_image.ptr<image_pixel_type>(1, 1);	\
	image_pixel_type* image_data = start_image_data;	\
	image_pixel_type* top_image_data;	\
	image_pixel_type* other_image_data;	\
	b8 connect_condition = false;	\
	mt_point startPoint;	\
	mt_point* currentRegion = NULL;\
	i32 offset;\
	i32 offset_x;\
	i32 offset_y;\
	for (i32 row = 1; row <= preInfo.m_activate_rect.m_height; ++row) {	\
		for (i32 col = 1; col <= preInfo.m_activate_rect.m_width; ++col) {	\
			if (0 == image_data->m_visit_flag) {	\
				startPoint.m_x = col - 1 + preInfo.m_activate_rect.m_left;	\
				startPoint.m_y = row - 1 + preInfo.m_activate_rect.m_top;	\
				currentRegion = &preInfo.m_regions.back();	\
				image_data->m_visit_flag = 1;	\
				currentRegion->m_x = preInfo.m_cache_point_index;	\
				preInfo.m_cache_points[preInfo.m_cache_point_index++] = startPoint;	\
				preInfo.m_stack_helper[++preInfo.m_stack_top_index] = image_data;		\
				while (preInfo.m_stack_top_index != -1) {	\
					top_image_data = preInfo.m_stack_helper[preInfo.m_stack_top_index--];	\
					for (i32 i = 1; i < preInfo.m_window_size; ++i) {	\
						other_image_data = top_image_data + preInfo.m_neibour_pixel_offsets[i];	\
						if (1 == other_image_data->m_visit_flag) {	\
							continue;	\
						}	\


#define basicimg_cc_n_end()	\
						if (connect_condition) {\
							preInfo.m_stack_helper[++preInfo.m_stack_top_index] = other_image_data;\
							other_image_data->m_visit_flag = 1;\
							offset = (i32)(other_image_data - start_image_data);\
							offset_y = offset / preInfo.m_temp_image.size()[1];	\
							offset_x = offset - offset_y * preInfo.m_temp_image.size()[1];	\
							preInfo.m_cache_points[preInfo.m_cache_point_index++] = mt_point(offset_x + preInfo.m_activate_rect.m_left, offset_y + preInfo.m_activate_rect.m_top);	\
						}\
					}\
				}\
				currentRegion->m_y = preInfo.m_cache_point_index;\
				preInfo.m_regions.push_back(mt_point(0, 0));\
			}\
			++image_data;\
		}\
		image_data += preInfo.m_row_step;\
	}\
	normal_extract_stop(ccs, preInfo);


#define basicimg_cc_s_begin()	\
	normal_connected_info preInfo;	\
	normal_extract_start(preInfo, image, maskInfo, connect_type, 1);	\
	image_pixel_type* start_image_data = preInfo.m_temp_image.ptr<image_pixel_type>(1, 1);	\
	image_pixel_type* image_data = preInfo.m_temp_image.ptr<image_pixel_type>(startPoint.m_y - preInfo.m_activate_rect.m_top + 1, startPoint.m_x - preInfo.m_activate_rect.m_left + 1);	\
	image_pixel_type* top_image_data;	\
	image_pixel_type* other_image_data;	\
	b8 connect_condition = false;	\
	mt_point* currentRegion = NULL;	\
	i32 offset;	\
	i32 offset_x;	\
	i32 offset_y;	\
	currentRegion = &preInfo.m_regions.back();	\
	image_data->m_visit_flag = 1;	\
	currentRegion->m_x = preInfo.m_cache_point_index;	\
	preInfo.m_cache_points[preInfo.m_cache_point_index++] = startPoint;	\
	preInfo.m_stack_helper[++preInfo.m_stack_top_index] = image_data;	\
	while (preInfo.m_stack_top_index != -1) { \
		top_image_data = preInfo.m_stack_helper[preInfo.m_stack_top_index--];	\
		for (i32 i = 1; i < preInfo.m_window_size; ++i) { \
			other_image_data = top_image_data + preInfo.m_neibour_pixel_offsets[i];	\
			if (1 == other_image_data->m_visit_flag) { \
				continue;	\
			}	

#define basicimg_cc_s_end()	\
			if (connect_condition) { \
				preInfo.m_stack_helper[++preInfo.m_stack_top_index] = other_image_data;	\
				other_image_data->m_visit_flag = 1;	\
				offset = (i32)(other_image_data - start_image_data);	\
				offset_y = offset / preInfo.m_temp_image.size()[1];		\
				offset_x = offset - offset_y * preInfo.m_temp_image.size()[1];		\
				preInfo.m_cache_points[preInfo.m_cache_point_index++] = mt_point(offset_x + preInfo.m_activate_rect.m_left, offset_y + preInfo.m_activate_rect.m_top);		\
			}	\
		}	\
	}	\
	currentRegion->m_y = preInfo.m_cache_point_index;	\
	single_extract_stop(cc, preInfo);\
	delete preInfo.m_cache_points;

namespace basicimg {
	class img_connected_component {
	public:

		img_connected_component();

		~img_connected_component();

		void operator = (const img_connected_component& other);

		void release();

		void init(i32 size);

		void init(mt_point* memory_start, i32 size);

		b8 m_share_memory;
		i32 m_size;

		mt_point* m_points;
		mt_rect m_rect;
	};

	class img_multi_connected_components {
	public:
		img_multi_connected_components() {
			m_cache_points = NULL;
		}

		~img_multi_connected_components() {
			basicsys_delete_array(m_cache_points);
		}

		vector<img_connected_component> m_ccs;
		mt_point* m_cache_points;
	};

	enum img_Mask_Type {
		img_Mask_Type_Null,
		img_Mask_Type_Image,
		img_Mask_Type_Rect,
		img_Mask_Type_Value,
	};

	enum img_Connected_Type {
		img_Connected_Type_4_Neibour,
		img_Connected_Type_8_Neibour,
	};

	template<class T>
	class img_mask_info {
	public:
		img_mask_info()
			: m_mask_type(img_Mask_Type_Null) {

		}

		img_mask_info(const mt_mat& mask_mat) 
			: m_mask_type(img_Mask_Type_Image) 
			, m_mask_image(mask_mat) {
		}

		img_mask_info(const mt_rect& maskRect) 			
			: m_mask_type(img_Mask_Type_Rect) 
			, m_mask_rect(maskRect) {

		}

		img_mask_info(const T& maskValue) 			
			: m_mask_type(img_Mask_Type_Value) 
			, m_mask_value(maskValue) {

		}

		img_Mask_Type m_mask_type;
		mt_mat m_mask_image;//0表示不考虑，1表示考虑
		mt_rect m_mask_rect;	//仅提取maskRect内部的组件
		T m_mask_value;	//不考虑像素值为maskValue
	};

	/**
	@note When the data type is multi-channel, such as CV_8UC3, we must use custom defined type without memory byte alignment.
	*/
	template<class T>
	class ml_connected_component_base {
	public:

		virtual void normal_extract(img_multi_connected_components& ccs, const mt_mat& image, const img_mask_info<T>& maskInfo = img_mask_info<T>(), img_Connected_Type connect_type = img_Connected_Type_8_Neibour) {
			normal_connected_info preInfo;
			normal_extract_start(preInfo, image, maskInfo, connect_type, 2000);
			image_pixel_type* start_image_data = preInfo.m_temp_image.ptr<image_pixel_type>(1, 1);
			image_pixel_type* image_data = start_image_data;
			image_pixel_type* top_image_data;
			image_pixel_type* other_image_data;
			b8 connect_condition = false;
			mt_point startPoint; 
			mt_point* currentRegion = NULL;
			i32 offset;
			i32 offset_x;
			i32 offset_y;

			for (i32 row = 1; row <= preInfo.m_activate_rect.m_height; ++row) {
				for (i32 col = 1; col <= preInfo.m_activate_rect.m_width; ++col) {
					basiclog_assert2(image_data == preInfo.m_temp_image.ptr<image_pixel_type>(row, col));

					if (0 == image_data->m_visit_flag) {
						startPoint.m_x = col - 1 + preInfo.m_activate_rect.m_left;
						startPoint.m_y = row - 1 + preInfo.m_activate_rect.m_top;

						currentRegion = &preInfo.m_regions.back();
						image_data->m_visit_flag = 1;
						currentRegion->m_x = preInfo.m_cache_point_index;
						preInfo.m_cache_points[preInfo.m_cache_point_index++] = startPoint;

						preInfo.m_stack_helper[++preInfo.m_stack_top_index] = image_data;

						while (preInfo.m_stack_top_index != -1) {
							top_image_data = preInfo.m_stack_helper[preInfo.m_stack_top_index--];

							for (i32 i = 1; i < preInfo.m_window_size; ++i) {
								other_image_data = top_image_data + preInfo.m_neibour_pixel_offsets[i];

								if (1 == other_image_data->m_visit_flag) {
									continue;
								}

								//Add your self code

								if (connect_condition) {
									preInfo.m_stack_helper[++preInfo.m_stack_top_index] = other_image_data;
									other_image_data->m_visit_flag = 1;
									offset = (i32)(other_image_data - start_image_data);
									offset_y = offset / preInfo.m_temp_image.size()[1];	
									offset_x = offset - offset_y * preInfo.m_temp_image.size()[1];	
									preInfo.m_cache_points[preInfo.m_cache_point_index++] = mt_point(offset_x + preInfo.m_activate_rect.m_left, offset_y + preInfo.m_activate_rect.m_top);	
								}
							}
						}

						currentRegion->m_y = preInfo.m_cache_point_index;
						preInfo.m_regions.push_back(mt_point(0, 0));
					}

					++image_data;
				}

				image_data += preInfo.m_row_step;
			}

			normal_extract_stop(ccs, preInfo);
		}

		virtual void single_extract(img_connected_component& cc, const mt_point& startPoint, mt_mat& image, const img_mask_info<T>& maskInfo = img_mask_info<T>(), img_Connected_Type connect_type = img_Connected_Type_8_Neibour) {
			normal_connected_info preInfo;
			normal_extract_start(preInfo, image, maskInfo, connect_type, 1);
			image_pixel_type* start_image_data = preInfo.m_temp_image.ptr<image_pixel_type>(1, 1);

			image_pixel_type* image_data = preInfo.m_temp_image.ptr<image_pixel_type>(startPoint.m_y - preInfo.m_activate_rect.m_top + 1, startPoint.m_x - preInfo.m_activate_rect.m_left + 1);
			image_pixel_type* top_image_data;
			image_pixel_type* other_image_data;
			b8 connect_condition = false;
			mt_point* currentRegion = NULL;
			i32 offset;
			i32 offset_x;
			i32 offset_y;

			currentRegion = &preInfo.m_regions.back();
			image_data->m_visit_flag = 1;
			currentRegion->m_x = preInfo.m_cache_point_index;
			*preInfo.m_cache_points++ = startPoint;

			preInfo.m_stack_helper[++preInfo.m_stack_top_index] = image_data;

			while (preInfo.m_stack_top_index != -1) {
				top_image_data = preInfo.m_stack_helper[preInfo.m_stack_top_index--];

				for (i32 i = 1; i < preInfo.m_window_size; ++i) {
					other_image_data = top_image_data + preInfo.m_neibour_pixel_offsets[i];

					if (1 == other_image_data->m_visit_flag) {
						continue;
					}

					//Add your self code

					if (connect_condition) {
						preInfo.m_stack_helper[++preInfo.m_stack_top_index] = other_image_data;
						other_image_data->m_visit_flag = 1;
						offset = (i32)(other_image_data - start_image_data);
						offset_y = offset / preInfo.m_temp_image.size()[1];	
						offset_x = offset - offset_y * preInfo.m_temp_image.size()[1];	
						*preInfo.m_cache_points++ = mt_point(offset_x + preInfo.m_activate_rect.m_left, offset_y + preInfo.m_activate_rect.m_top);	
					}
				}
			}

			currentRegion->m_y = preInfo.m_cache_point_index;
			single_extract_stop(cc, preInfo);
			delete preInfo.m_cache_points;
		}

	protected:

		class image_pixel_type {
		public:

			u8 m_visit_flag;
			T m_image_value;
		};

		class normal_connected_info {
		public:
			mt_point* m_cache_points;
			i32 m_cache_point_index;
			vector<mt_point> m_regions;

			i32 m_window_size;
			const i32* mDX;
			const i32* mDY;

			mt_rect m_activate_rect;
			i32 m_row_step;

			image_pixel_type** m_stack_helper;
			i32 m_stack_top_index;

			mt_mat m_temp_image;//For unit/unit64/float/double image

			i32 m_image_element_size;
			i32 m_neibour_pixel_offsets[9];
			i32 m_mask_image_element_size;
			i32 m_mask_image_step[9];	
		};

		class single_connect_info {
		public:

			vector<mt_point> m_stack_helper;

			mt_mat m_temp_mask_image;

			i32 m_window_size;
			const i32* mDX;
			const i32* mDY;

			i32 m_image_element_size;
			i32 m_neibour_pixel_offsets[9];
			i32 m_mask_image_element_size;
			i32 m_mask_image_step[9];

			u8* mMaskImageDataStartPointer;
			u8* mMaskImageDataStopPointer;
		};

		void normal_extract_start(normal_connected_info& preInfo, const mt_mat& src, const img_mask_info<T>& maskInfo, img_Connected_Type connect_type, i32 suggestRegionCount) {		
			preInfo.m_window_size = (img_Connected_Type_8_Neibour == connect_type ? 9 : 5);
			preInfo.mDX = (img_Connected_Type_8_Neibour == connect_type ? img_DX_3_9 : img_DX_3_5);
			preInfo.mDY = (img_Connected_Type_8_Neibour == connect_type ? img_DY_3_9 : img_DY_3_5);
			preInfo.m_image_element_size = sizeof(image_pixel_type);

			preInfo.m_regions.reserve(suggestRegionCount);
			preInfo.m_regions.push_back(mt_point(0, 0));

			i32 area = 0;
			i32 temp_image_append_offset;

			if (img_Mask_Type_Null == maskInfo.m_mask_type || img_Mask_Type_Rect == maskInfo.m_mask_type) {
				if (img_Mask_Type_Null == maskInfo.m_mask_type) {
					preInfo.m_activate_rect.set_rect(mt_point(0, 0), mt_size(src.size()[1], src.size()[0]));
				} else {
					preInfo.m_activate_rect = maskInfo.m_mask_rect;
				}

				i32 srcByteOffset = (i32)src.step()[0] / sizeof(T) - preInfo.m_activate_rect.m_width;

				preInfo.m_temp_image = mt_mat((preInfo.m_activate_rect.m_height + 2) * sizeof(image_pixel_type), (preInfo.m_activate_rect.m_width + 2) * sizeof(image_pixel_type), mt_U8C1);
				preInfo.m_temp_image.size()[1] = preInfo.m_activate_rect.m_width + 2;
				preInfo.m_temp_image.size()[0] = preInfo.m_activate_rect.m_height + 2;
				preInfo.m_temp_image.step()[1] = sizeof(image_pixel_type);
				temp_image_append_offset = (i32)preInfo.m_temp_image.step()[0] / sizeof(image_pixel_type) - preInfo.m_temp_image.size()[1];

				image_pixel_type* data = (image_pixel_type*)preInfo.m_temp_image.data();
				const T* rawData = src.ptr<T>(preInfo.m_activate_rect.m_top, preInfo.m_activate_rect.m_left);

				for (i32 col = 0; col < preInfo.m_temp_image.size()[1]; ++col) {
					data->m_visit_flag = 1;
					++data;
				}

				data += temp_image_append_offset;

				for (i32 row = 1; row <= preInfo.m_activate_rect.m_height; ++row) {
					data->m_visit_flag = 1;
					++data;

					i32 col = 1;
					for (; col <= preInfo.m_activate_rect.m_width; ++col) {
						data->m_visit_flag = 0;
						data->m_image_value = *rawData++;
						++data;
					}

					data->m_visit_flag = 1;
					++data;
					data += temp_image_append_offset;
					rawData += srcByteOffset;
				}

				for (i32 col = 0; col < preInfo.m_temp_image.size()[1]; ++col) {
					data->m_visit_flag = 1;
					++data;
				}

				area = preInfo.m_activate_rect.getArea();
			} else {
				if (img_Mask_Type_Image == maskInfo.m_mask_type) {
					//First we statistic the maskImage
					i32 rowMax = 0;
					i32 colMax = 0;
					i32 rowMin = src.size()[0];
					i32 colMin = src.size()[1];

					i32 maskOffset = (i32)maskInfo.m_mask_image.step()[0] - maskInfo.m_mask_image.size()[1];
					const u8* maskData = (u8*)maskInfo.m_mask_image.data();

					for (i32 row = 0; row < maskInfo.m_mask_image.size()[0]; ++row) {
						for (i32 col = 0; col < maskInfo.m_mask_image.size()[1]; ++col) {

							if (1 == *maskData++) {
								rowMax = max(rowMax, row);
								colMax = max(colMax, col);
								rowMin = min(rowMin, row);
								colMin = min(colMin, col);
							}
						}

						maskData += maskOffset;
					}

					mt_size targetImageSize(colMax - colMin + 1, rowMax - rowMin + 1);
					preInfo.m_activate_rect.set_rect(mt_point(colMin, rowMin), targetImageSize);

					preInfo.m_temp_image = mt_mat((targetImageSize.m_height + 2) * sizeof(image_pixel_type), (targetImageSize.m_width + 2) * sizeof(image_pixel_type), mt_U8C1);
					preInfo.m_temp_image.size()[1] = targetImageSize.m_width + 2;
					preInfo.m_temp_image.size()[0] = targetImageSize.m_height + 2;
					preInfo.m_temp_image.step()[1] = sizeof(image_pixel_type);
					temp_image_append_offset = (i32)preInfo.m_temp_image.step()[0] / sizeof(image_pixel_type) - preInfo.m_temp_image.size()[1];

					i32 rawImageAppendOffset = (i32)src.step()[0] / sizeof(T) - targetImageSize.m_width;
					image_pixel_type* data = (image_pixel_type*)preInfo.m_temp_image.data();

					//定位有效矩形区域的第一个像素
					const T* rawData = src.ptr<T>(rowMin, colMin);
					maskData = maskInfo.m_mask_image.ptr<u8>(rowMin, colMin);

					for (i32 col = 0; col < preInfo.m_temp_image.size()[1]; ++col) {
						data->m_visit_flag = 1;
						++data;
					}

					data += temp_image_append_offset;

					for (i32 row = 1; row <= preInfo.m_activate_rect.m_height; ++row) {
						data->m_visit_flag = 1;
						++data;

						i32 col = 1;
						for (; col <= preInfo.m_activate_rect.m_width; ++col) {

							//basiclog_assert2(maskData == CV_IMAGE_DATA(maskInfo.m_mask_image, u8, row - 1 + rowMin, col - 1 + colMin));
							//basiclog_assert2(rawData == CV_IMAGE_DATA(image, T, row - 1 + rowMin, col - 1 + colMin));
							//basiclog_assert2(data == CV_IMAGE_DATA(preInfo.m_temp_image, image_pixel_type, row, col));

							if (1 == *maskData++) {
								data->m_visit_flag = 0;
								data->m_image_value = *rawData;
								++area;
							} else {
								data->m_visit_flag = 1;
							}

							++data;
							++rawData;
						}

						data->m_visit_flag = 1;
						++data;
						data += temp_image_append_offset;
						maskData += rawImageAppendOffset;
						rawData += rawImageAppendOffset;
					}
				} else if (img_Mask_Type_Value == maskInfo.m_mask_type) {
					i32 srcByteOffset = (i32)src.step()[0] / sizeof(T) - src.size()[1];

					i32 rowMax = 0;
					i32 colMax = 0;
					i32 rowMin = src.size()[0];
					i32 colMin = src.size()[1];

					T* image_data = (T*)src.data();

					for (i32 row = 0; row < src.size()[0]; ++row) {
						for (i32 col = 0; col < src.size()[1]; ++col) {

							basiclog_assert2(image_data == src.ptr<T>(row, col));	

							if (*image_data++ != maskInfo.m_mask_value) {
								rowMax = max(rowMax, row);
								colMax = max(colMax, col);
								rowMin = min(rowMin, row);
								colMin = min(colMin, col);
							}
						}

						image_data += srcByteOffset;
					}

					mt_size targetImageSize(colMax - colMin + 1, rowMax - rowMin + 1);
					preInfo.m_activate_rect.set_rect(mt_point(colMin, rowMin), targetImageSize);

					preInfo.m_temp_image = mt_mat((targetImageSize.m_height + 2) * sizeof(image_pixel_type), (targetImageSize.m_width + 2) * sizeof(image_pixel_type), mt_U8C1);
					preInfo.m_temp_image.size()[1] = targetImageSize.m_width + 2;
					preInfo.m_temp_image.size()[0] = targetImageSize.m_height + 2;
					preInfo.m_temp_image.step()[1] = sizeof(image_pixel_type);
					temp_image_append_offset = (i32)preInfo.m_temp_image.step()[0] / sizeof(image_pixel_type) - preInfo.m_temp_image.size()[1];

					i32 rawImageAppendOffset = (i32)src.step()[0] / sizeof(T) - targetImageSize.m_width;

					image_pixel_type* data = (image_pixel_type*)preInfo.m_temp_image.data();

					//定位有效矩形区域的第一个像素
					const T* rawData = src.ptr<T>(rowMin, colMin);

					for (i32 col = 0; col < preInfo.m_temp_image.size()[1]; ++col) {
						data->m_visit_flag = 1;
						++data;
					}

					data += temp_image_append_offset;

					for (i32 row = 1; row <= preInfo.m_activate_rect.m_height; ++row) {
						basiclog_assert2(data == preInfo.m_temp_image.ptr<image_pixel_type>(row, 0));

						data->m_visit_flag = 1;
						++data;

						i32 col = 1;
						for (; col <= preInfo.m_activate_rect.m_width; ++col) {

							basiclog_assert2(rawData == src.ptr<T>(row - 1 + rowMin, col - 1 + colMin));
							basiclog_assert2(data == preInfo.m_temp_image.ptr<image_pixel_type>(row, col));

							if (*rawData != maskInfo.m_mask_value) {
								data->m_visit_flag = 0;
								data->m_image_value = *rawData;	
								++area;
							} else {
								data->m_visit_flag = 1;
							}

							++data;
							++rawData;
						}

						data->m_visit_flag = 1;
						++data;
						data += temp_image_append_offset;

						rawData += rawImageAppendOffset;
					}

					for (i32 col = 0; col < preInfo.m_temp_image.size()[1]; ++col) {
						//basiclog_assert2(data == CV_IMAGE_DATA(preInfo.m_temp_image, image_pixel_type, preInfo.mRowMax + 1, col));

						data->m_visit_flag = 1;
						++data;
					}
				}
			}

			for (i32 i = 1; i < preInfo.m_window_size; ++i) {
				preInfo.m_neibour_pixel_offsets[i] = img_img::image_pixel_offset(preInfo.m_temp_image.step()[0], preInfo.m_image_element_size, preInfo.mDX[i], preInfo.mDY[i]);
			}

			preInfo.m_row_step = (i32)preInfo.m_temp_image.step()[0] / sizeof(image_pixel_type) - preInfo.m_activate_rect.m_width;
			preInfo.m_stack_helper = new image_pixel_type*[area];
			preInfo.m_stack_top_index = -1;
			preInfo.m_cache_points = new mt_point[area];
		}

		void normal_extract_stop(img_multi_connected_components& ccs, normal_connected_info& preInfo) {
			if (preInfo.m_regions.back().m_x == preInfo.m_regions.back().m_y) {
				preInfo.m_regions.pop_back();
			}

			if (!preInfo.m_regions.empty()) {
				ccs.m_cache_points = preInfo.m_cache_points;
				ccs.m_ccs.resize(preInfo.m_regions.size());

				for (i32 i = 0; i < (i32)preInfo.m_regions.size(); ++i) {
					ccs.m_ccs[i].m_share_memory = sys_true;
					ccs.m_ccs[i].m_points = ccs.m_cache_points + preInfo.m_regions[i].m_x;
					ccs.m_ccs[i].m_size = preInfo.m_regions[i].m_y - preInfo.m_regions[i].m_x;

					for (i32 k = 0; k < ccs.m_ccs[i].m_size; ++k) {
						ccs.m_ccs[i].m_rect.adjust_by_append_point(ccs.m_ccs[i].m_points[k]);
					}
				}
			}

			if (preInfo.m_stack_helper != NULL) {
				delete[] preInfo.m_stack_helper;
				preInfo.m_stack_helper = NULL;
			}
		}

		void single_extract_stop(img_connected_component& cc, normal_connected_info& preInfo) {
			cc.m_share_memory = sys_false;
			cc.m_size = preInfo.m_regions[0].m_y - preInfo.m_regions[0].m_x;
			cc.m_points = new mt_point[cc.m_size];

			for (i32 j = preInfo.m_regions[0].m_x, k = 0; j < preInfo.m_regions[0].m_y; ++j, ++k) {
				cc.m_points[k] = preInfo.m_cache_points[j];
				cc.m_rect.adjust_by_append_point(cc.m_points[k]);
			}

			if (preInfo.m_stack_helper != NULL) {
				delete[] preInfo.m_stack_helper;
				preInfo.m_stack_helper = NULL;
			}
		}
	};

	template<class T>
	class ml_connected_component_true_condition_imp : public ml_connected_component_base<T> {
	public:

		virtual void normal_extract(img_multi_connected_components& ccs, const mt_mat& image, const img_mask_info<T>& maskInfo = img_mask_info<T>(), img_Connected_Type connect_type = img_Connected_Type_8_Neibour) {
			basicimg_cc_n_begin();
			connect_condition = true;
			basicimg_cc_n_end();
		}

		virtual void single_extract(img_connected_component cc, const mt_point& startPoint, const mt_mat& image, const img_mask_info<T>& maskInfo /* = MaskInfo<T>() */, img_Connected_Type connect_type /* = img_Connected_Type_8_Neibour */) {
			basicimg_cc_s_begin();
			connect_condition = true;
			basicimg_cc_s_end();
		}
	};



	class img_connected_component_helper {
	public:

		static void connect_component(img_multi_connected_components& ccs, const mt_mat& image, const mt_scalar& background, img_Connected_Type connected_type);

		static mt_mat image_from_connected_component(const vector<img_connected_component*>& ccs, const mt_rect& roi, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true);
	
		static mt_mat image_from_connected_component(const vector<img_connected_component>& ccs, const mt_rect& roi, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true);
	
		static mt_mat image_from_connected_component(const img_connected_component& cc, const mt_size& dst_size, const mt_scalar& forground_color = img_Color_Black, const mt_scalar& background_color = img_Color_White, b8 gray = sys_true);
	};
}

