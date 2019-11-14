#include "test.h"
#include <basicsys.h>

using namespace basicsys;

/**
* Select a configuration.
*/
static void set_config(img_mser_base* alg) {
	//TextDetection
	alg->set_duplicated_variation(0.1f);
	alg->set_stable_max_variation(0.5f);
	alg->set_stable_delta(1);

	// DetectorEval
	//alg->set_duplicated_variation(0.2f);
	//alg->set_stable_max_variation(0.25f);
	//alg->set_stable_delta(5);
}


static void eval(const mt_mat& src, img_mser_alg_factory* factory) {
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

void img_mser_test::run(vector<wstring>& argvs) {
	mt_helper::enable_omp_mkl(sys_true);
	sys_alg_analyzer::init();

	// Load a gray-scale image from a given path
	mt_mat gray_test_image = img_img::load(L"G:/study_project/matel_resource/mser dataset/realword/10077696/img_19.png", img_img::Load_Grayscale);

	
	log_logger::get_logger()->enable_info(false);
	log_logger::get_logger()->enable_debug(false);
	log_logger::get_logger()->enable_warning(false);

	// Create a designated MSER algorithm factory. Here is an example of creating a Fast MSER V2 algorithm factory.
	img_mser_alg_factory* factory = new img_mser_alg_factory(typeid(img_fast_mser_v2).name());

	// Evaluate an MSER algorithm.
	eval(gray_test_image, factory);
	
	// Evaluate channel parallel algorithm. Note that Fast MSER V1 and V2 can not evaluated by this method because they are partition parallel algorithms.
	//eval_cp(gray_test_image, factory);
	
	sys_alg_analyzer::release();
	
	// Delete factory
	delete factory;
}
