#include "test.h"

void sys_strconvert_test::run(vector<string>& argvs) {
	string s("你好");

	for (i32 i = 0; i < s.size(); ++i) {
		printf("%d\n", (i32)s[i]);
	}

	string utf = sys_strconvert::utf8_from_local(s);

	sys_test_equal((i32)utf.size(), 6);
	sys_test_equal((i32)utf[0], -28);
	sys_test_equal((i32)utf[1], -67);
	sys_test_equal((i32)utf[2], -96);
	sys_test_equal((i32)utf[3], -27);
	sys_test_equal((i32)utf[4], -91);
	sys_test_equal((i32)utf[5], -67);
}