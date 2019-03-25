#include "stdafx.h"

#include "sys_multi_process.h"
#include <mkl.h>
#include <omp.h>

/**
#pragma omp parallel for num_threads(omp_get_max_threads()) is equivalent to #pragma omp parallel for.
*/
void sys_multi_process::enable_omp_mkl(b8 enable) {
	if (enable) {
		int sys_suggest_threads = omp_get_num_procs() - 1;
		if (sys_suggest_threads == 0) {
				sys_suggest_threads = 1;
		}
		
		omp_set_num_threads(sys_suggest_threads);
		omp_set_dynamic(1);
		mkl_set_num_threads(sys_suggest_threads);
		mkl_set_dynamic(1);
	} else {
		omp_set_num_threads(1);
		mkl_set_num_threads(1);
	}
}