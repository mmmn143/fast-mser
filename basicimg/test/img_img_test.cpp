#include "test.h"

static void test_load_save() {
	mt_mat img = img_img::load(img_helper::get_resource_dir() + "test.jpg");
	img_img::save(img_helper::get_resource_dir() + "test_save.jpg", img);
}

static void test_resize() {
	mt_mat img = img_img::load(img_helper::get_resource_dir() + "test.jpg");
	mt_mat small_img;
	img_img::resize(small_img, img, mt_size(600, 600), img_img::Inter_Type_Cubic);
	img_img::resize(small_img, img, mt_size(600, 600), img_img::Inter_Type_Cubic);

	img_draw draw(small_img);

	draw.draw_line(mt_point(0, 0), mt_point(100, 100), cv_pen(mt_scalar(0, 0, 255), cv_pen::Dash_Type_Solid, 3));

	img_img::save(img_helper::get_resource_dir() + "test_resize.jpg", small_img);
}

class img_test_cvt {
public:

	static void test() {
		mt_mat img = img_img::load(img_helper::get_resource_dir() + "text.png");
		string image_name("img_test_cvt");
		mt_mat gray_img = img_img::cvt_color(img, img_img::Color_Covert_Type_BGR2Gray);
		img_img::save(img_helper::get_resource_dir() + image_name + ".png", gray_img);


		test_cvt_color_to_gray(gray_img, image_name);
	}

protected:

	static void test_cvt_color_to_gray(const mt_mat& gray_img, const string& image_name) {
		img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<image_name<<"_tl.png"), gray_img.sub(mt_rect(0, 0, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<image_name<<"_tr.png"), gray_img.sub(mt_rect(gray_img.size()[1] / 2, 0, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<image_name<<"_bl.png"), gray_img.sub(mt_rect(0, gray_img.size()[0] / 2, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<image_name<<"_br.png"), gray_img.sub(mt_rect(gray_img.size()[1] / 2, gray_img.size()[0] / 2, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
	}
};

class img_test_gaussian_blur {
public:

	static void test() {
		mt_mat img = img_img::load(img_helper::get_resource_dir() + "text.png");
		string image_name("img_test_gaussian_blur");
		mt_mat gray_img = img_img::cvt_color(img, img_img::Color_Covert_Type_BGR2Gray);
		img_img::save(img_helper::get_resource_dir() + image_name + "_gray.png", gray_img);

		test_gaussian_blur(gray_img, image_name);
	}

protected:

	static void test_gaussian_blur(const mt_mat& gray_img, const string& image_name) {
		mt_mat gaussian_image = img_img::gaussian_blur(gray_img, mt_size(-1, -1), 1.5, 1.5);
		img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<image_name<<"_gaussian.png"), gaussian_image);
	}
};

static void test_chessboard() {
	mt_mat img;
	img_img::genarate_chessboard(img, mt_size(3888, 2592), 1, 3);

	img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<"chessboard_1.png"), img);

	img_img::genarate_chessboard(img, mt_size(3888, 2592), 2, 3);

	img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<"chessboard_2.png"), img);

	img_img::genarate_chessboard(img, mt_size(3888, 2592), 4, 3);

	img_img::save(img_helper::get_resource_dir() + (string)(sys_strcombine()<<"chessboard_4.png"), img);
}

void img_img_test::run(vector<string>& argvs) {

	test_load_save();
	test_resize();

	img_test_cvt::test();
	img_test_gaussian_blur::test();

	test_chessboard();
}