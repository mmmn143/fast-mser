#include "test.h"
#include "basicsys.h"
#include "basicmath.h"

using namespace basicmath;

void sys_openmp_test::run(vector<wstring>& argvs) {
	sys_multi_process::enable_omp_mkl(sys_true);

	for (i32 t = 0; t < 10; ++t) {
		sys_timer c(L"omp combine");

		//1 1204ms
		//4 387ms 377ms 368ms out loop
		//3 474ms  out loop
		//3 483ms 500ms 497ms inner loop
		//4 386ms 391ms 383ms 390ms 383ms inner loop

		c.begin();
#pragma omp parallel for num_threads(4)
		for (i32 k = 0; k < 12; ++k) {

			for (int i = 0; i < 200000; ++i) {
				f64 v = mt_random::gaussian(10, 20);

				v += 1;
			}
		}


		c.end();

		sys_timer c1(L"omp splited");

		//1 1204ms
		//4 387ms 377ms 368ms out loop
		//3 474ms  out loop
		//3 483ms 500ms 497ms inner loop
		//4 386ms 391ms 383ms 390ms 383ms inner loop

		c1.begin();

		for (i32 k = 0; k < 12; ++k) {
#pragma omp parallel for num_threads(4)
			for (int i = 0; i < 200000; ++i) {
				f64 v = mt_random::gaussian(10, 20);

				v += 1;
			}
		}


		c1.end();
	}

	
}