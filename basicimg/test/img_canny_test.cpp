#include "test.h"



void img_canny_test::run(vector<string>& argvs) {
	mt_mat gray_test_image = img_img::load(img_helper::get_resource_dir() + "test2.jpg", img_img::Load_Grayscale);

	img_canny edge_detector;
	mt_mat edge_image = edge_detector.edge_detection(gray_test_image);

	img_img::save(img_helper::get_resource_dir() + "test_canny_edge.jpg", edge_image);
}