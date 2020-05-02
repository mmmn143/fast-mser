#include "test.h"

static void test_case_0() {
	vector<i32> weights;
	weights.push_back(2);
	weights.push_back(2);
	weights.push_back(5);
	weights.push_back(4);
	weights.push_back(6);

	vector<i32> values;
	values.push_back(6);
	values.push_back(3);
	values.push_back(4);
	values.push_back(6);
	values.push_back(5);

	mt_01_bag<i32, i32> bag(weights, values, 10);
	basiclog_info2(bag.max_value());
	basiclog_info2(bag.flag_01());

	sys_test_equal(bag.max_value(), 15);

	vector<b8> gt_flags;
	gt_flags.push_back(sys_true);
	gt_flags.push_back(sys_true);
	gt_flags.push_back(sys_false);
	gt_flags.push_back(sys_true);
	gt_flags.push_back(sys_false);

	sys_test_equal(bag.flag_01(), gt_flags);
}

static void test_case_1() {
	vector<i32> weights;

	weights.push_back(4);
	weights.push_back(6);

	vector<i32> values;

	values.push_back(6);
	values.push_back(5);

	mt_01_bag<i32, i32> bag(weights, values, 8);
	basiclog_info2(bag.max_value());
	basiclog_info2(bag.flag_01());

	sys_test_equal(bag.max_value(), 6);

	vector<b8> gt_flags;
	gt_flags.push_back(sys_true);
	gt_flags.push_back(sys_false);

	sys_test_equal(bag.flag_01(), gt_flags);
}

static void test_case_2() {

}

void mt_01_bag_test::run(vector<string>& argvs) {
	test_case_0();
	test_case_1();
}