#include "test.h"



void mt_longest_common_substring_test::run(vector<string>& argvs) {
	vector<i32> a;

	a.push_back(1);
	a.push_back(-2);
	a.push_back(3);
	a.push_back(10);
	a.push_back(-4);
	a.push_back(7);
	a.push_back(2);
	a.push_back(-5);



	vector<i32> b;
	b.push_back(1);
	b.push_back(2);
	b.push_back(3);
	b.push_back(10);
	b.push_back(-4);
	b.push_back(7);
	b.push_back(2);
	b.push_back(5);

	mt_longest_common_substring<i32> instance(a, b);

	basiclog_info2(instance.a_begin_index());
	basiclog_info2(instance.b_begin_index());
	basiclog_info2(instance.length());
}