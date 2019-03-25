#include "test.h"


void mt_sort_test::run(vector<wstring>& argvs) {
	vector<i32> datas;
	mt_random::randperm(datas, 100);
	basiclog_info2(datas);

	vector<i32> increment_gt(100);
	for (i32 i = 0; i < 100; ++i) {
		increment_gt[i] = i;
	}

	vector<i32> decrement_gt(100);
	for (i32 i = 0; i < 100; ++i) {
		decrement_gt[i] = 100 - 1 - i;
	}

	vector<i32> temp;

	temp = datas;
	mt_sort::bubble_sort<i32>(temp, sys_true);

	sys_test_equal(temp, increment_gt);

	temp = datas;
	mt_sort::bubble_sort<i32>(temp, sys_false);

	sys_test_equal(decrement_gt, temp);

	temp = datas;
	mt_sort::heap_sort<i32>(temp, sys_true);

	sys_test_equal(temp, increment_gt);

	temp = datas;
	mt_sort::heap_sort<i32>(temp, sys_false);

	sys_test_equal(decrement_gt, temp);
}