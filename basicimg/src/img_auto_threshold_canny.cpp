#include "stdafx.h"
#include "img_canny.h"
#include "img_auto_threshold_canny.h"

mt_mat img_auto_threshold_canny::edge_detection(const mt_mat& src, const mt_mat& gradient_x /* = mt_mat() */, const mt_mat& gradient_y /* = mt_mat() */, const mt_mat& gradient_mag /* = mt_mat() */) {
	mt_mat edge;
	edge_detection(edge, src, gradient_x, gradient_y, gradient_mag);

	return edge;
}

void img_auto_threshold_canny::edge_detection(mt_mat& edge, const mt_mat& src, const mt_mat& gradient_x /* = mt_mat() */, const mt_mat& gradient_y /* = mt_mat() */, const mt_mat& gradient_mag /* = mt_mat() */) {
	//取出所有大于lowThreshold的边缘阈值
	m_canny.set_high_threshold(m_auto_threshold_min);
	m_canny.set_low_threshold((i32)(m_auto_threshold_min * m_low_to_high_ratio));

	m_canny.edge_detection(m_temp_edge, src, gradient_x, gradient_y, gradient_mag);

	i32 rows = src.size()[0];
	i32 cols = src.size()[1];

	//计算积分图像
	m_edge_gradient_integral_sum_image.create(rows, cols, mt_F32C1);
	m_edge_gradient_integral_sum_image.set(0);
	m_edge_gradient_integral_number_image.create(rows, cols, mt_I32C1);
	m_edge_gradient_integral_number_image.set(0);

	if (255 == m_temp_edge.at<u8>(0, 0, 0)) {
		m_edge_gradient_integral_sum_image.at<f32>(0, 0, 0) = m_canny.gradient_mag().at<f32>(0, 0, 0);
		m_edge_gradient_integral_number_image.at<i32>(0, 0, 0) = 1;
	}

	for (i32 col = 1; col < cols; ++col) {
		if (255 == m_temp_edge.at<u8>(0, col, 0)) {
			m_edge_gradient_integral_sum_image.at<f32>(0, col, 0) = m_edge_gradient_integral_sum_image.at<f32>(0, col - 1, 0) + m_canny.gradient_mag().at<f32>(0, col, 0);
			m_edge_gradient_integral_number_image.at<i32>(0, col, 0) = m_edge_gradient_integral_number_image.at<i32>(0, col - 1, 0) + 1;
		} else {
			m_edge_gradient_integral_sum_image.at<f32>(0, col) = m_edge_gradient_integral_sum_image.at<f32>(0, col - 1, 0);
			m_edge_gradient_integral_number_image.at<i32>(0, col, 0) = m_edge_gradient_integral_number_image.at<i32>(0, col - 1, 0);
		}
	}

	for (i32 row = 1; row < rows; ++row) {
		if (255 == m_temp_edge.at<u8>(row, 0, 0)) {
			m_edge_gradient_integral_sum_image.at<f32>(row, 0, 0) = m_edge_gradient_integral_sum_image.at<f32>(row - 1, 0, 0) + m_canny.gradient_mag().at<f32>(row, 0, 0);
			m_edge_gradient_integral_number_image.at<i32>(row, 0, 0) = m_edge_gradient_integral_number_image.at<i32>(row - 1, 0, 0) + 1;
		} else {
			m_edge_gradient_integral_sum_image.at<f32>(row, 0, 0) = m_edge_gradient_integral_sum_image.at<f32>(row - 1, 0, 0);
			m_edge_gradient_integral_number_image.at<i32>(row, 0, 0) = m_edge_gradient_integral_number_image.at<i32>(row - 1, 0, 0);
		}
	}

	for (i32 row = 1; row < rows; ++row) {
		for (i32 col = 1; col < cols; ++col) {
			if (255 == m_temp_edge.at<u8>(row, col, 0)) {
				m_edge_gradient_integral_sum_image.at<f32>(row, col, 0) = m_edge_gradient_integral_sum_image.at<f32>(row, col - 1, 0) + m_edge_gradient_integral_sum_image.at<f32>(row - 1, col, 0) - m_edge_gradient_integral_sum_image.at<f32>(row - 1, col - 1, 0) + m_canny.gradient_mag().at<f32>(row, col, 0);
				m_edge_gradient_integral_number_image.at<i32>(row, col, 0) = m_edge_gradient_integral_number_image.at<i32>(row, col - 1, 0) + m_edge_gradient_integral_number_image.at<i32>(row - 1, col, 0) - m_edge_gradient_integral_number_image.at<i32>(row - 1, col - 1, 0) + 1;
			} else {
				m_edge_gradient_integral_sum_image.at<f32>(row, col, 0) = m_edge_gradient_integral_sum_image.at<f32>(row, col - 1, 0) + m_edge_gradient_integral_sum_image.at<f32>(row - 1, col, 0) - m_edge_gradient_integral_sum_image.at<f32>(row - 1, col - 1, 0);
				m_edge_gradient_integral_number_image.at<i32>(row, col, 0) = m_edge_gradient_integral_number_image.at<i32>(row, col - 1, 0) + m_edge_gradient_integral_number_image.at<i32>(row - 1, col, 0) - m_edge_gradient_integral_number_image.at<i32>(row - 1, col - 1, 0);
			}
		}
	}

	i32 window_destiny_flag_number_threshold = i32(m_auto_threshold_window_size * m_auto_threshold_window_size * m_density_flag_number_to_window_ratio);

	//计算梯度均值
	i32 window_step_number = 4;
	i32 window_step = m_auto_threshold_window_size / window_step_number;
	i32 half_window_size = m_auto_threshold_window_size / 2;

	i32 image_window_row_number = rows / window_step + ((rows % window_step) > half_window_size ? 1 : 0);
	i32 image_window_col_number = cols / window_step + ((rows % window_step) > half_window_size ? 1 : 0);

#define REAL_ROW(row)	\
	(min(((row) / window_step), rowStepSub1))
#define REAL_COL(col)	\
	(min(((col) / window_step), colStepSub1))

#define INTEGRAL_ELEMENT_VALUE(image, datatype, row, col)	\
	((-1 == row || -1 == col) ? 0 : image.at<datatype>(row, col, 0))


#define INTEGRAL_VALUE(image, datatype, startRow, startCol, stopRow, stopCol)	\
	(image.at<datatype>(stopRow, stopCol, 0) - INTEGRAL_ELEMENT_VALUE(image, datatype, startRow, stopCol) - INTEGRAL_ELEMENT_VALUE(image, datatype, stopRow, startCol) + INTEGRAL_ELEMENT_VALUE(image, datatype,  startRow, startCol))

	mt_size size(image_window_col_number, image_window_row_number);

	mt_mat gradient_mean(size.m_height, size.m_width, mt_F32C1, mt_scalar(0));
	mt_mat gradient_count(size.m_height, size.m_width, mt_F32C1, mt_scalar(0));
	mt_mat destiny_flags(size.m_height, size.m_width, mt_U8C1, mt_scalar(0));

	i32 count;
	i32 oldCount;
	f32 sumValue;

	i32 row = 0;
	i32 rowStep = 0;

	i32 rowStepSub1 = image_window_row_number - 1;
	i32 colStepSub1 = image_window_col_number - 1; 
	i32 rowStepMax = image_window_row_number - window_step_number;
	i32 colStepMax = image_window_col_number - window_step_number;
	i32 rowMax = rows - 1;
	i32 colMax = cols - 1;


	while (rowStep <= rowStepMax) {
		i32 stopRow =  row + m_auto_threshold_window_size - 1;

		if (rowStep == rowStepMax) {
			stopRow = rowMax;
		}

		i32 col = 0;
		i32 colStep = 0;

		while (colStep <= colStepMax) {
			i32 stopCol = col + m_auto_threshold_window_size - 1;

			if (colStep == colStepMax) {
				stopCol = colMax;
			}

			i32 startRow = row - 1;
			i32 startCol = col - 1;


			// calculate the window edge pixel number
			count = INTEGRAL_VALUE(m_edge_gradient_integral_number_image, i32, startRow, startCol, stopRow, stopCol);
			if (count > 0) {

				// calculate the gradient sum of the edge pixels in the window
				sumValue = INTEGRAL_VALUE(m_edge_gradient_integral_sum_image, f32, startRow, startCol, stopRow, stopCol);

				i32 realRow = REAL_ROW(row);
				i32 realCol = REAL_COL(col);
				i32 realRowEnd = realRow + window_step_number;
				i32 realColEnd = realCol + window_step_number;
				for (i32 i = realRow; i < realRowEnd; ++i) {
					for (i32 j = realCol; j < realColEnd; ++j) {
						oldCount = gradient_count.at<i32>(i, j, 0);
						gradient_mean.at<f32>(i, j, 0) = (gradient_mean.at<f32>(i, j, 0) * oldCount + sumValue) / (f32)(oldCount + count);
						gradient_count.at<i32>(i, j, 0) = oldCount + count;

						if (count >= window_destiny_flag_number_threshold) {
							destiny_flags.at<u8>(i, j, 0) = destiny_flags.at<u8>(i, j, 0) + 1;
						}
					}
				}
			}

			col += window_step;
			++colStep;
		}

		row += window_step;
		++rowStep;
	}

	// 提取主边缘
	edge.create(rows, cols, mt_U8C1);
	edge.set(0);
	mt_mat autoHighThreshold = m_edge_gradient_integral_sum_image;
	f64 auto_high_threshold;

	for (i32 row = 0; row < rows; ++row) {
		i32 realRow = REAL_ROW(row);
		for (i32 col = 0; col < cols; ++col) {		
			if (m_temp_edge.at<u8>(row,col, 0) == 255) {
				i32 realCol = REAL_COL(col);

				auto_high_threshold = gradient_mean.at<f32>(realRow, realCol, 0) * m_auto_threshold_to_mean_ratio;

				if (destiny_flags.at<u8>(realRow, realCol) > m_density_window_flag_overlap_number_threshold) {
					auto_high_threshold = m_auto_threshold_density;
				}

				if (auto_high_threshold < m_auto_threshold_min) {
					auto_high_threshold = m_auto_threshold_min;
				} else if (auto_high_threshold > m_auto_threshold_max){
					auto_high_threshold = m_auto_threshold_max;
				}

				autoHighThreshold.at<f32>(row, col, 0) = (f32)auto_high_threshold;

				if (m_canny.gradient_mag().at<f32>(row, col, 0) > auto_high_threshold) {
					edge.at<u8>(row, col, 0) = 255;
				}
			}
		}
	}

	mt_mat flagImage = m_edge_gradient_integral_number_image;
	flagImage.set(0);
	for( i32 row = 1; row < rowMax; row++ ){
		for (i32 col = 1; col < colMax; col++ ){			
			if ((255 == edge.at<u8>(row, col, 0)) 
				&& m_canny.gradient_mag().at<f32>(row, col, 0) < m_can_not_be_removed_threshold
				&& 0 == flagImage.at<i32>(row, col, 0)) {
					remove_small_edge(edge, flagImage, mt_point(col, row));
			}
		}
	}

	for (i32 row = 0; row < rows; ++row) {
		for (i32 col = 0; col < cols; ++col) {
			if (255 == edge.at<u8>(row, col, 0)
				&& 255 == m_temp_edge.at<u8>(row, col, 0)) {
					trace_edge(edge, autoHighThreshold.at<f32>(row, col, 0) * m_low_to_high_ratio, mt_point(col, row));
			}
		}
	}
}

void img_auto_threshold_canny::clear_memory_cache() {
	m_canny.clear_memory_cache();

	m_temp_edge = mt_mat();
	m_edge_gradient_integral_sum_image = mt_mat();
	m_edge_gradient_integral_number_image = mt_mat();
	m_gradient_mean = mt_mat();
	m_gradient_count = mt_mat();
	m_destiny_flags = mt_mat();
}

void img_auto_threshold_canny::remove_small_edge(mt_mat& edgeImage, mt_mat& flagImage, const mt_point& pt) {
	vector<mt_point> stackPts;
	stackPts.reserve(500);
	stackPts.push_back(pt);

	vector<mt_point> pts;
	pts.reserve(500);
	pts.push_back(pt);
	flagImage.at<i32>(pt.m_y, pt.m_x, 0) = 1;

	bool hasHighGradientValue = false;

	while (!stackPts.empty()) {
		mt_point ptTop = stackPts.back();
		stackPts.pop_back();

		for (i32 i = 1; i < 9; ++i) {
			mt_point other(ptTop.m_x + img_DX_3_9[i], ptTop.m_y + img_DY_3_9[i]);
			if (1 == flagImage.at<i32>(other.m_y, other.m_x, 0)) {
				continue;
			}

			if (255 == edgeImage.at<u8>(other.m_y, other.m_x)) {
				if (!hasHighGradientValue && m_canny.gradient_mag().at<f32>(other.m_y, other.m_x, 0) >= m_can_not_be_removed_threshold) {
					hasHighGradientValue = true;
				}

				if (!img_img::point_in_image_boundary(other, edgeImage)) {
					stackPts.push_back(other);
				}

				pts.push_back(other);
				flagImage.at<i32>(other.m_y, other.m_x, 0) = 1;
			}
		}
	}

	if (!hasHighGradientValue) {
		if ((i32)pts.size() <= m_noise_edge_number_threshold) {
			for (i32 i = 0; i < (i32)pts.size(); ++i) {
				edgeImage.at<u8>(pts[i].m_y, pts[i].m_x, 0) = 0;
			}
		}
	}

	pts.clear();
}

void img_auto_threshold_canny::trace_edge(mt_mat& edgeImage, f32 lowThreshold, const mt_point& pt) {
	vector<mt_point> stackPts;
	stackPts.reserve(500);
	stackPts.push_back(pt);
	m_temp_edge.at<u8>(pt.m_y, pt.m_x, 0) = 0;

	while (!stackPts.empty()) {
		mt_point ptTop = stackPts.back();
		stackPts.pop_back();

		for (i32 i = 1; i < 9; ++i) {
			mt_point other(ptTop.m_x + img_DX_3_9[i], ptTop.m_y + img_DY_3_9[i]);

			if (!img_img::point_in_image(other, edgeImage)) {
				continue;
			}

			if ((m_canny.gradient_mag().at<f32>(other.m_y, other.m_x) < lowThreshold)
				|| 255 == edgeImage.at<u8>(other.m_y, other.m_x, 0)
				|| 0 == m_temp_edge.at<u8>(other.m_y, other.m_x, 0)) {
					continue;
			}

			stackPts.push_back(other);
			edgeImage.at<u8>(other.m_y, other.m_x, 0) = 255;
			m_temp_edge.at<u8>(other.m_y, other.m_x, 0) = 0;
		}
	}
}