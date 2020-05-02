#include "test.h"


void mt_top_k_test::run(vector<string>& argvs) {
	vector<i32> datas;
	mt_random::randperm(datas, 100);

	basiclog_info2(datas);

	mt_top_k<i32> top_k(10, sys_true);

	for (i32 i = 0; i < (i32)datas.size(); ++i) {
		top_k.traverse(datas[i]);
	}

	basiclog_info2(top_k.get_top_k());
	vector<i32> sorted_top_k;
	top_k.get_sorted_top_k(sorted_top_k);

	basiclog_info2(sorted_top_k);

	vector<i32> gt_top_k(10);
	for (i32 i = 0; i < 10; ++i) {
		gt_top_k[i] = 99 - i;
	}

	sys_test_equal(gt_top_k, sorted_top_k);

	mt_top_k<i32> bottom_k(10, sys_false);

	for (i32 i = 0; i < (i32)datas.size(); ++i) {
		bottom_k.traverse(datas[i]);
	}

	basiclog_info2(bottom_k.get_top_k());
	bottom_k.get_sorted_top_k(sorted_top_k);

	basiclog_info2(sorted_top_k);

	for (i32 i = 0; i < 10; ++i) {
		gt_top_k[i] = i;
	}

	sys_test_equal(gt_top_k, sorted_top_k);
}