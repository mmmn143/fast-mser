#include "test.h"



void mt_max_sum_for_continuous_element_test::run(vector<string>& argvs) {
	vector<i32> values;

	values.push_back(1);
	values.push_back(-2);
	values.push_back(3);
	values.push_back(10);
	values.push_back(-4);
	values.push_back(7);
	values.push_back(2);
	values.push_back(-5);

	mt_max_sum_for_continuous_element<i32> max_sum_instance(values);

	basiclog_info2(max_sum_instance.max_sum());
	basiclog_info2(max_sum_instance.begin_index());
	basiclog_info2(max_sum_instance.length());
}