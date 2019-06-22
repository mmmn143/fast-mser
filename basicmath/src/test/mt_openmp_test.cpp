#include "test.h"
#include "basicmath.h"

using namespace basicmath;

static void test_no_parallel() {
	sys_timer t(L"test_no_parallel");
	t.begin();

	for (i32 i = 0; i < 100; ++i) {
		for (i32 j = 0; j < 100; ++j) {
			float sum = 0;

			for (int k = 0; k < 20000; ++k) {
				f64 v = mt_random::gaussian(10, 20);

				sum += v;
			}

			if (sum > 0) {
				sum *= 2;
			}
		}
	}

	t.end();
}

static void test_parallel() {
	sys_timer t(L"test_parallel");
	t.begin();

	for (i32 i = 0; i < 100; ++i) {
#pragma omp parallel for num_threads(4)
		for (i32 j = 0; j < 100; ++j) {
			float sum = 0;

			for (int k = 0; k < 20000; ++k) {
				f64 v = mt_random::gaussian(10, 20);

				sum += v;
			}

			if (sum > 0) {
				sum *= 2;
			}
		}
	}



	t.end();
}

void mt_openmp_test::run(vector<wstring>& argvs) {
	mt_helper::enable_omp_mkl(sys_true);

	test_parallel();
	test_no_parallel();

//	for (i32 t = 0; t < 10; ++t) {
//		sys_timer c(L"omp combine");
//
//		//1 1204ms
//		//4 387ms 377ms 368ms out loop
//		//3 474ms  out loop
//		//3 483ms 500ms 497ms inner loop
//		//4 386ms 391ms 383ms 390ms 383ms inner loop
//
//		c.begin();
//#pragma omp parallel for num_threads(4)
//		for (i32 k = 0; k < 12; ++k) {
//
//			for (int i = 0; i < 200000; ++i) {
//				f64 v = mt_random::gaussian(10, 20);
//
//				v += 1;
//			}
//		}
//
//
//		c.end();
//
//		sys_timer c1(L"omp splited");
//
//		//1 1204ms
//		//4 387ms 377ms 368ms out loop
//		//3 474ms  out loop
//		//3 483ms 500ms 497ms inner loop
//		//4 386ms 391ms 383ms 390ms 383ms inner loop
//
//		c1.begin();
//
//		for (i32 k = 0; k < 12; ++k) {
//#pragma omp parallel for num_threads(4)
//			for (int i = 0; i < 200000; ++i) {
//				f64 v = mt_random::gaussian(10, 20);
//
//				v += 1;
//			}
//		}
//
//
//		c1.end();
//	}
//
	
}