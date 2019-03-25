#include "test.h"


static void test_split() {
	wstring str = L"";
	vector<wstring> split_res;

	sys_strhelper::split(split_res, str, L",");
	sys_test(split_res.empty());

	str = L",";
	sys_strhelper::split(split_res, str, L",");
	sys_test(2 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);

	str = L",,";
	sys_strhelper::split(split_res, str, L",");
	sys_test(3 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);

	str = L"hello,world,";
	sys_strhelper::split(split_res, str, L",");
	sys_test(3 == split_res.size());

	basiclog_info2(sys_strcombine()<<split_res);
}

void sys_strhelper_test::run(vector<wstring>& argvs) {
	test_split();

}