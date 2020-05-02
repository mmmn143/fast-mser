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



/**

* Evaluate an MSER algorithm. We can evaluate CV-MSER, CV-MSER+, VF-MSER, ID-MSER, Fast MSER V1 and Fast MSER V2.

*/

static void eval(const mt_mat& src, img_mser_alg_factory* factory) {

	// We would like to compare channel parallel MSER algorithms on our computer with 4 CPU cores, i.e. executing a thread on each core.

	// Thus, we need 4 channels for channel parallel MSER algorithms.

	mt_mat srcs[4];



	if (src.channel() == 1) {

		srcs[0] = src;

	} else {

		for (i32 i = 0; i < src.channel(); ++i) {

			srcs[i] = src.channel_at(i).clone();

		}



		// Add a gray-scale channel

		srcs[3] = img_img::cvt_color(src, img_img::Color_Covert_Type_BGR2Gray);

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



	sys_alg_analyzer::add("mser cost", sys_timer::get_tick_cout() - tick);



	delete mser;

}



/**

* Evaluate an channel parallel MSER algorithm. We can evaluate CPCV-MSER, CPCV-MSER+, CPVF-MSER, CPID-MSER.

* As Fast MSER V1 and V2 are partition parallel algorithms, we can not evaluate them using this method.

*/

static void eval_cp(const mt_mat& src, img_mser_alg_factory* factory) {

	// We would like to compare channel parallel MSER algorithms on our computer with 4 CPU cores, i.e. executing a thread on each core.

	// Thus, we need 4 channels for channel parallel MSER algorithms.

	mt_mat srcs[4];



	if (src.channel() == 1) {

		srcs[0] = src;

	} else {

		for (i32 i = 0; i < src.channel(); ++i) {

			srcs[i] = src.channel_at(i).clone();

		}



		// Add a gray-scale channel

		srcs[3] = img_img::cvt_color(src, img_img::Color_Covert_Type_BGR2Gray);

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



	sys_alg_analyzer::add("mser cost", sys_timer::get_tick_cout() - tick);

	delete mser;

}



void img_mser_test::run(vector<string>& argvs) {

	mt_helper::enable_omp_mkl(sys_true);

	sys_alg_analyzer::init();



	// Load a gray-scale image from a given path

	mt_mat gray_test_image = img_img::load("G:/study_project/matel_resource/mser dataset/icdar_whole/10077696/img_19.png", img_img::Load_Grayscale);





	log_logger::get_logger()->enable_info(false);

	log_logger::get_logger()->enable_debug(false);

	log_logger::get_logger()->enable_warning(false);



	// Create a designated MSER algorithm factory. Here is an example of creating a Fast MSER V2 algorithm factory.

	img_mser_alg_factory* factory = new img_mser_alg_factory(typeid(img_fast_mser_v2).name());



	// Evaluate an MSER algorithm.

	eval(gray_test_image, factory);



	// Evaluate channel parallel algorithm. Note that Fast MSER V1 and V2 can not evaluated by this method because they are partition parallel algorithms.

	//eval_cp(gray_test_image, factory);



	// Delete factory

	delete factory;



	sys_alg_analyzer::release();

}