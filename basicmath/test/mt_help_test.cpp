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

static void test_add_five() {
	//sys_test_equal(54, mt_helper::add_five(4));
	//sys_test_equal(75452, mt_helper::add_five(7452));
	//sys_test_equal(-10598, mt_helper::add_five(-1098));

	//sys_test_equal(5268, mt_helper::add_five(268));
	//sys_test_equal(6750, mt_helper::add_five(670));
	sys_test_equal(50, mt_helper::add_five(0));
	//sys_test_equal(-5999, mt_helper::add_five(-999));

}

static void test_max_lexi_order() {
	//sys_test_equal("dca", mt_helper::max_lexi_order("dbca"));
	//sys_test_equal("bcd", mt_helper::max_lexi_order("abcd"));

	sys_test_equal("ab", mt_helper::max_lexi_order("acb"));
	//sys_test_equal("ho", mt_helper::max_lexi_order("hot"));
	//sys_test_equal("cdility", mt_helper::max_lexi_order("codility"));
	//sys_test_equal("aaa", mt_helper::max_lexi_order("aaaa"));
}

static void test_battle_ship() {
	sys_test_equal("1,0", mt_helper::battle_ship("1A,2B", "1A,1B,2A,2B"));
	sys_test_equal("1,1", mt_helper::battle_ship("1A,1B;2A,2B", "1A,1B,2A"));
}

static void test_split_array() {
	vector<i32> a;
	vector<i32> b;

	vector<i32> data;
	data.push_back(1);
	data.push_back(10);
	//data.push_back(-1);
	data.push_back(-100);
	//data.push_back(1000);

	mt_helper::split_array_to_max_mean_sub_arrays(a, b, data);

	// 103

	// 

	basiclog_info2(a);
	basiclog_info2(b);
}

void mt_helper_test::run(vector<string>& argvs) {
	//test_odd_even_sort();
	//test_one_number();
	//test_add_five();
	//test_max_lexi_order();
	//test_battle_ship();

	test_split_array();
}