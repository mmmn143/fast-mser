#include "test.h"


static void test_split() {
	string str = "";
	vector<string> split_res;

	sys_strhelper::split(split_res, str, ",");
	sys_test(split_res.empty());

	str = ",";
	sys_strhelper::split(split_res, str, ",");
	sys_test(2 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);

	str = ",,";
	sys_strhelper::split(split_res, str, ",");
	sys_test(3 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);

	str = "hello,world,";
	sys_strhelper::split(split_res, str, ",");
	sys_test(3 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);
}

void sys_strhelper_test::run(vector<string>& argvs) {
	test_split();

}