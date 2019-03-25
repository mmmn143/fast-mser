#include "test.h"
#include <basicsys.h>

using namespace basicsys;

static void set_config(img_mser_base* alg) {
	//alg->set_stable_delta(0);
	//alg->set_duplicated_variation(-1);
	//alg->set_min_max_point(0, 1.0f);
	//alg->set_nms_similarity(-1);
	//alg->set_duplicated_variation(-1);
	//alg->set_stable_max_variation(FLT_MAX);
	//alg->set_stable_delta(5);
	
	//alg->set_duplicated_variation(0.2f);
	//alg->set_stable_max_variation(0.25f);
	//alg->set_stable_delta(5);

	//alg->set_recursive_point_threshold(20);
}


static void eval_bp(const mt_mat& src, img_mser_alg_factory* factory) {
	mt_mat srcs[4];

	if (src.channel() == 1) {
		srcs[0] = src;
	} else {
		for (i32 i = 0; i < src.channel(); ++i) {
			srcs[i] = src.channel_at(i).clone();
		}

		srcs[3] = srcs[0];
	}

	img_mser_base* mser = factory->new_instance();
	set_config(mser);

	i64 tick = sys_timer::get_tick_cout();

	{
		if (src.channel() == 1) {
			img_multi_msers mser_res;
			mser->extract(mser_res, src);

			//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/mser/", mt_size(32, 32));
		} else {
			for (i32 i = 0; i < 4; ++i) {
				img_multi_msers mser_res;
				mser->extract(mser_res, srcs[i]);

				//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/mser/", mt_size(32, 32));
			}
		}
	}

	sys_alg_analyzer::add(L"mser cost", sys_timer::get_tick_cout() - tick);

	delete mser;
}

static void eval_cp(const mt_mat& src, img_mser_alg_factory* factory) {
	mt_mat srcs[4];

	if (src.channel() == 1) {
		srcs[0] = src;
	} else {
		for (i32 i = 0; i < src.channel(); ++i) {
			srcs[i] = src.channel_at(i).clone();
		}

		srcs[3] = srcs[0];
	}

	img_mser_base* mser = factory->new_instance();
	set_config(mser);

	i64 tick = sys_timer::get_tick_cout();
	{
		if (src.channel() == 1) {
			img_multi_msers mser_res;
			mser->extract(mser_res, src);

			//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/mser/", mt_size(32, 32));
		} else {
#pragma omp parallel for num_threads(4)
			for (i32 i = 0; i < 4; ++i) {
				img_multi_msers mser_res;
				mser->extract(mser_res, srcs[i]);

				//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/mser/", mt_size(32, 32));
			}
		}
	}

	sys_alg_analyzer::add(L"mser cost", sys_timer::get_tick_cout() - tick);
	delete mser;
}

static void random_test(img_mser_alg_factory* factory)  {
	for (i32 k = 0; k < 10000; ++k) {
		mt_mat gray_test_image(100, 100, mt_U8C1);

		for (i32 row = 0; row < gray_test_image.size()[0]; ++row) {
			for (i32 col = 0; col < gray_test_image.size()[1]; ++col) {
				i32 v = mt_random::random_next_i32(0, 100);

				gray_test_image.at<u8>(row, col, 0) = v;
			}
		}

		//basiclog_info2(gray_test_image);

		eval_bp(gray_test_image, factory);
	}
}

static void generate_random_image() {
	i32 sizes[] = {2000, 3000};

	for (i32 i = 0; i < 10; ++i) {
		mt_mat image = mt_random::random_uniform_iid(2, sizes, mt_U8C3, 0, sys_false, 255, sys_false);

		img_img::save(sys_strcombine()<<L"G:/study_project/matel_resource/mser dataset/"<<i<<L".png", image);
	}

	
}

static void generate_test_image() {
	vector<wstring> images;
	sys_path::get_file_path(images, L"G:/study_project/matel_resource/mser dataset/realword/10077696/");

	i32 dst_size = 1000000;

	for (i32 i = 0; i < (i32)images.size() && i <= 1; ++i) {
		mt_mat image = img_img::load(images[i]);

		i32 current_size = image.size()[0] * image.size()[1];
		f32 ratio = current_size / (f32)dst_size;
		ratio = sqrt(ratio);

		mt_mat dst_image = img_img::resize(image, mt_size(i32(image.size()[1] / ratio), i32(image.size()[0] / ratio)));


		img_img::save(sys_strcombine()<<L"G:/study_project/matel_resource/mser dataset/"<<sys_path::get_file_name(images[i]), dst_image);
	}
}

static void generate_test_image_image_matching() {
	vector<wstring> images;
	sys_path::get_file_path(images, L"G:/study_project/matel_resource/mser dataset/realword/1280 960/");

	mt_size size(1224, 816);

	i32 dst_size = 1000000;

	for (i32 i = 0; i < (i32)images.size(); ++i) {
		mt_mat image = img_img::load(images[i]);
	
		if (image.is_empty()) {
			continue;
		}

		//i32 current_size = image.size()[0] * image.size()[1];
		//f32 ratio = current_size / (f32)dst_size;
		//ratio = sqrt(ratio);

		mt_mat dst_image = img_img::resize(image, size);


		img_img::save(sys_strcombine()<<L"G:/study_project/matel_resource/mser dataset/"<<sys_path::get_file_title(images[i])<<L".png", dst_image);
	}
}

static void eval_bp(const vector<wstring>& image_paths, img_mser_alg_factory* factory) {
	img_mser_base* mser = factory->new_instance();
	set_config(mser);
	//icdar
	//idiap vf 1
	//other 10

	//image matching
	//idiap vf 1
	// other 4
	for (i32 i = 0; i < (i32)image_paths.size(); ++i) {
		/*if (i != 0 && i !=	10 && i != 24 && i != 37) {
		continue;
		}*/

		mt_mat image = img_img::load(image_paths[i], img_img::Load_Color);
		mt_mat gray_image = img_img::cvt_color(image, img_img::Color_Covert_Type_BGR2Gray);
		
		mt_mat channels[4] = {image.channel_at(0).clone(), image.channel_at(1).clone(), image.channel_at(2).clone(), gray_image};	
		i64 tick = sys_timer::get_tick_cout();

		{

			for (i32 c = 0; c < 4; ++c) {
				img_multi_msers mser_res;
				mser->extract(mser_res, channels[c]);


			
				//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/pixel_parallel_mser/", mt_size(32, 32));
			}

			mser->clear_memory_cache();		
		}

		

		sys_alg_analyzer::add(L"mser cost", sys_timer::get_tick_cout() - tick);
	}

	delete mser;
}

static void eval_cp(const vector<wstring>& image_paths, img_mser_alg_factory* factory) {
	for (i32 i = 0; i < (i32)image_paths.size() && i < 1; ++i) {
		if (i != 0 && i !=	10 && i != 24 && i != 37) {
			continue;
		}

		//basiclog_info2(image_paths[i]);

		mt_mat image = img_img::load(image_paths[i], img_img::Load_Color);
		mt_mat gray_image = img_img::cvt_color(image, img_img::Color_Covert_Type_BGR2Gray);

		mt_mat channels[4] = {image.channel_at(0).clone(), image.channel_at(1).clone(), image.channel_at(2).clone(), gray_image};	

		i64 tick = sys_timer::get_tick_cout();

		{				
			img_multi_msers res[4];

			#pragma omp parallel for num_threads(4)
			for (i32 c = 0; c < 4; ++c) {
				img_mser_base* mser = factory->new_instance();
				set_config(mser);
				mser->extract(res[c], channels[c]);

				delete mser;
			}	
		}

		sys_alg_analyzer::add(L"mser cost", sys_timer::get_tick_cout() - tick);
	}
}

static void eval_sm(const vector<wstring>& image_paths, img_mser_alg_factory* factory) {
	sys_timer t(L"mser");
	sys_timer v(L"visit");
	i32 memory_size = 0;

	for (i32 i = 0; i < (i32)image_paths.size(); ++i) {
		mt_mat image = img_img::load(image_paths[i]);
		mt_mat gray_image = img_img::cvt_color(image, img_img::Color_Covert_Type_BGR2Gray);

		mt_mat channels[4] = {image.channel_at(0).clone(), image.channel_at(1).clone(), image.channel_at(2).clone(), gray_image};	
		t.begin();

		{
			img_mser_base* mser = factory->new_instance();
			set_config(mser);
			mser->set_recursive_point_threshold(50000);
			for (i32 c = 0; c < 4; ++c) {
				img_multi_msers mser_res;
				mser->extract(mser_res, channels[c]);

				memory_size += mser_res.m_memory_size[0] + mser_res.m_memory_size[1];
				//img_mser_helper::save_msers(mser_res, L"G:/study_project/matel_resource/image_resource/pixel_parallel_mser/", mt_size(32, 32));

				v.begin();
				img_mser_helper::visit_pixel_test(mser_res);
				v.end();

			}

			delete mser;
		}


		t.end();
	}

	basiclog_info2(sys_strcombine()<<L"output memory_size: "<<memory_size);
}


void img_linear_mser_test::run(vector<wstring>& argvs) {
	//generate_test_image();

	//generate_random_image();
	//return;
	//sys_timer t(L"mem");

	//for (i32 i = 0; i < 10; ++i) {
	//	char* a = (char*)malloc(sizeof(mt_point) * 1455242087 / 10 / 4 / 2);
	//	
	//	t.begin();
	//	free(a);
	//	t.end();
	//}

	//
	//generate_test_image_image_matching();
	//return;


	sys_multi_process::enable_omp_mkl(sys_true);

	sys_alg_analyzer::init();

	i32 c = 0;

	sys_timer t(L"test");
	
	t.begin();






	void* mutex = sys_thread_lock::create_mutex();

	#pragma omp parallel for num_threads(4)
	for (i32 i = 0; i < 10000000; ++i) {
		sys_thread_lock auto_lock(mutex);
		c += 1;
	}

	t.end();
	basiclog_info2(c);

	vector<wstring> image_paths;
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/998784/");
	
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/2999094/");
	
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/4996850/");
	
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/6998400/");
	
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/8997626/");
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/10077696/");
	//

	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/image_matching/1998728/");
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/random/3996768/");
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/image_matching/6000000/");
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/image_matching/7998376/");
	sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/10077696/");
	//sys_path::get_file_path(image_paths, L"G:/study_project/matel_resource/mser dataset/realword/10077696/");

	//eval_bp(image_paths);
	//eval_cp(image_paths);
	//eval_sm(image_paths);
	//mt_mat gray_test_image = img_img::load(img_helper::get_resource_dir_path() + L"test2.jpg", img_img::Load_Color);
	//mt_mat gray_test_image = img_img::load(img_helper::get_resource_dir_path() + L"IMAG0109.jpg", img_img::Load_Color);
	//mt_mat gray_test_image = img_img::load(img_helper::get_resource_dir_path() + L"IMAG0109.jpg", img_img::Load_Grayscale);
	//mt_mat gray_test_image = img_img::load(L"G:/study_project/matel_resource/mser dataset/random/998784/0.png", img_img::Load_Grayscale);
	mt_mat gray_test_image = img_img::load(L"G:/study_project/matel_resource/mser dataset/realword/10077696/img_19.png", img_img::Load_Grayscale);

	//mt_mat gray_test_image = mt_mat_t<u8>(6, 4, 1).read(4, 3, 7, 4, 1, 0, 0, 2, 2, 5, 2, 3, 4, 3, 7, 4, 1, 0, 0, 2, 2, 5, 2, 3);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(4, 8, 1, 5, 4, 8, 1, 5);
	//mt_mat gray_test_image = mt_mat_t<u8>(12, 2, 1).read(6, 4, 1, 5, 2, 8, 4, 5, 3, 8, 4, 5, 6, 4, 1, 5, 2, 8, 4, 5, 3, 8, 4, 5);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(6, 8, 4, 5, 6, 4, 8, 5);
	//mt_mat gray_test_image = mt_mat_t<u8>(6, 2, 1).read(1, 4, 5, 5, 3, 2, 1, 4, 5, 5, 3, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(6, 2, 1).read(13, 7, 5, 3, 0, 1, 4, 1, 1, 10, 4, 1);

	//mt_mat gray_test_image = mt_mat_t<u8>(2, 2, 1).read(6, 5, 3, 9);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(6, 5, 7, 0, 3, 2, 3, 8);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(8, 4, 9, 1, 7, 6, 9, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(1, 4, 1, 1, 10, 4, 1, 0);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(5, 4, 9, 0, 4, 10, 8, 9);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 6, 4, 8, 6, 6, 3, 1);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 10, 10, 10, 3, 5, 4, 9);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 9, 4, 5, 9, 0, 10, 6);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(1, 0, 0, 1, 1, 1, 1, 1);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 7, 1, 5, 4, 5, 8, 5);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 6, 10, 1, 2, 5, 7, 7);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(3, 10, 10, 8, 6, 2, 1, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(9, 8, 1, 9, 7, 5, 3, 0);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(5, 2, 9, 6, 7, 2, 9, 1);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 2, 1).read(13, 1, 0, 0, 0, 2, 0, 9);
	//mt_mat gray_test_image = mt_mat_t<u8>(6, 2, 1).read(1, 2, 1, 5, 1, 2, 1, 2, 1, 5, 1, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 4, 1).read(4, 3, 7, 4, 1, 0, 0, 2, 1, 0, 0, 2, 4, 3, 7, 4);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 4, 1).read(0, 10, 3, 3, 6, 7, 9, 7, 5, 2, 8, 5, 5, 10, 8, 1);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 4, 1).read(1, 2, 1, 3, 4, 8, 5, 7, 6, 4, 3, 1, 9, 6, 6, 10);
	//mt_mat gray_test_image = mt_mat_t<u8>(8, 2, 1).read(1, 1, 0, 0, 0, 0, 2, 2, 1, 1, 0, 0, 0, 0, 2, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(6, 2, 1).read(1, 1, 0, 0, 2, 2, 1, 1, 0, 0, 2, 2);
	//mt_mat gray_test_image = mt_mat_t<u8>(4, 1, 1).read(1, 5, 1, 5);

	//mt_mat gray_test_image = mt_mat_t<u8>(1, 3, 1).read(2, 1, 4);


	//basiclog_info2(gray_test_image);

	log_logger::get_logger()->enable_info(false);
	log_logger::get_logger()->enable_debug(false);
	log_logger::get_logger()->enable_warning(false);

	img_mser_alg_factory* factory = new img_mser_alg_factory(typeid(img_linked_parallel_mser).name());

	//eval_bp(gray_test_image, factory);
	eval_bp(image_paths, factory);
	//eval_cp(image_paths, factory);
	//eval_sm(image_paths, factory);
	
	//random_test();

	
	//
	
	//mt_mat from_min_image = mser_res.generate_mser_result_image(gray_test_image, img_mser_result::Result_Type_From_Min);
	//img_img::save(img_helper::get_resource_dir_path() + L"linear_mser_from_min.png", from_min_image);

	//mt_mat from_max_image = mser_res.generate_mser_result_image(gray_test_image, img_mser_result::Result_Type_From_Max);
	//img_img::save(img_helper::get_resource_dir_path() + L"linear_mser_from_max.png", from_max_image);



	//vector<img_mser_component*> mser_components;
	//mser_res.shallow_copy_cc(mser_components, img_mser_result::Result_Type_Both);

	//for (i32 i = 0; i < (i32)mser_components.size(); ++i) {
	//	mt_mat component_image = img_connected_component_helper::image_from_connected_component(mser_components[i]->m_cc, mt_size(32, 32));

	//	basiclog_assert2(img_img::save(sys_strcombine()<<mser_component_dir<<i<<L".png", component_image));
	//}

	sys_alg_analyzer::release();
}
