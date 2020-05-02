#include "test.h"

void img_stroke_width_transform_test::run(vector<string>& argvs) {
	mt_mat gray_test_image = img_img::load(img_helper::get_resource_dir() + "test2.jpg", img_img::Load_Grayscale);
	
	img_canny edge_detector;
	edge_detector.set_high_threshold(120);
	edge_detector.set_low_threshold(60);
	mt_mat edge = edge_detector.edge_detection(gray_test_image);

	img_stroke_width_transform::params swt_params;
	swt_params.m_dark_on_light = sys_false;

	mt_mat swt_image = img_stroke_width_transform::swt(edge, edge_detector.gradient_x(), edge_detector.gradient_y(), swt_params);
	mt_mat normlized_swt_image = img_stroke_width_transform::normalize_swt_image(swt_image, 255);
	img_img::save(img_helper::get_resource_dir() + "test_swt.jpg", normlized_swt_image);
}