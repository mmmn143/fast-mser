#include "test.h"
#include <iostream>
#include <limits>
using namespace std;

int main() {
	sys_exe_config exe_config(new log_ide_console_logger(), sys_tester::Test_Type_Slience);


	vector<wstring> argvs;
	//mt_mat_test().run(argvs);
	//mt_helper_test().run(argvs);
	//mt_top_k_test().run(argvs);
	//mt_sort_test().run(argvs);
	//mt_01_bag_test().run(argvs);
	//mt_max_sum_for_continuous_element_test().run(argvs);
	//mt_longest_common_substring_test().run(argvs);
	//mt_continuous_memory_test().run(argvs);
	mt_openmp_test().run(argvs);
	//mt_block_memory_test().run(argvs);

	f32 val = -(f32)mt_Infinity;

	if (val < 0 && mt_helper::is_infinity(val)) {
		basiclog_info2(L"haha");
	}

	mt_mat_cache::output_cache_statistic();

	return 0;
}