#include "test.h"


static void test_odd_even_sort() {
	vector<i32> data;

	data.push_back(1);
	data.push_back(2);
	data.push_back(3);
	data.push_back(4);
	data.push_back(5);
	data.push_back(6);

	vector<i32> data2 = data;

	mt_helper::odd_even_sort(data2, sys_true, sys_true);

	vector<i32> gt_data;
	gt_data.push_back(1);
	gt_data.push_back(3);
	gt_data.push_back(5);
	gt_data.push_back(2);
	gt_data.push_back(4);
	gt_data.push_back(6);

	basiclog_info2(data2);

	//test equal odd even with original order
	sys_test_equal(data2, gt_data);

	data2 = data;
	mt_helper::odd_even_sort(data2, sys_false, sys_true);

	gt_data.clear();
	gt_data.push_back(2);
	gt_data.push_back(4);
	gt_data.push_back(6);
	gt_data.push_back(1);
	gt_data.push_back(3);
	gt_data.push_back(5);

	//test equal even odd with original order
	sys_test_equal(data2, gt_data);

	data2 = data;
	mt_helper::odd_even_sort(data2, sys_true, sys_false);

	basiclog_info2(data2);

	i32 low = 0;
	i32 high = (i32)data2.size() - 1;

	while (low < high && data2[low] % 2 == 1) {
		++low;
	}

	while (low < high && data2[high] % 2 == 0) {
		--high;
	}

	//test equal even odd
	sys_test_equal(low, high);

	data2 = data;
	mt_helper::odd_even_sort(data2, sys_false, sys_false);

	basiclog_info2(data2);

	low = 0;
	high = (i32)data2.size() - 1;

	while (low < high && data2[low] % 2 == 0) {
		++low;
	}

	while (low < high && data2[high] % 2 == 1) {
		--high;
	}

	//test equal odd even
	sys_test_equal(low, high);
}

static void test_one_number() {
	sys_test_equal(1, mt_helper::one_number(1));
	sys_test_equal(6, mt_helper::one_number(13));
}

void mt_helper_test::run(vector<wstring>& argvs) {
	test_odd_even_sort();
	test_one_number();
}