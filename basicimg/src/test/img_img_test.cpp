#include "test.h"

static void test_load_save() {
	mt_mat img = img_img::load(img_helper::get_resource_dir_path() + L"test.jpg");
	img_img::save(img_helper::get_resource_dir_path() + L"test_save.jpg", img);
}

static void test_resize() {
	mt_mat img = img_img::load(img_helper::get_resource_dir_path() + L"test.jpg");
	mt_mat small_img;
	img_img::resize(small_img, img, mt_size(600, 600), img_img::Inter_Type_Cubic);
	img_img::resize(small_img, img, mt_size(600, 600), img_img::Inter_Type_Cubic);

	img_draw draw(small_img);

	draw.draw_line(mt_point(0, 0), mt_point(100, 100), cv_pen(mt_scalar(0, 0, 255), cv_pen::Dash_Type_Solid, 3));

	img_img::save(img_helper::get_resource_dir_path() + L"test_resize.jpg", small_img);
}

class img_test_cvt {
public:

	static void test() {
		test_cvt_color_to_gray();
	}

protected:

	static void test_cvt_color_to_gray() {
		mt_mat img = img_img::load(img_helper::get_resource_dir_path() + L"text.png");
		//mt_mat gray_img = img_img::cvt_color(img, img_img::Color_Covert_Type_BGR2Gray);
		mt_mat gray_img = img.channel_at(0);
		wstring image_name(L"text_b");

		img_img::save(img_helper::get_resource_dir_path() + image_name + L".png", gray_img);

		img_img::save(img_helper::get_resource_dir_path() + (wstring)(sys_strcombine()<<image_name<<L"_tl.png"), gray_img.sub(mt_rect(0, 0, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir_path() + (wstring)(sys_strcombine()<<image_name<<L"_tr.png"), gray_img.sub(mt_rect(gray_img.size()[1] / 2, 0, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir_path() + (wstring)(sys_strcombine()<<image_name<<L"_bl.png"), gray_img.sub(mt_rect(0, gray_img.size()[0] / 2, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
		img_img::save(img_helper::get_resource_dir_path() + (wstring)(sys_strcombine()<<image_name<<L"_br.png"), gray_img.sub(mt_rect(gray_img.size()[1] / 2, gray_img.size()[0] / 2, gray_img.size()[1] / 2, gray_img.size()[0] / 2)));
	}
};

void img_img_test::run(vector<wstring>& argvs) {

	test_load_save();
	test_resize();

	img_test_cvt::test();
}