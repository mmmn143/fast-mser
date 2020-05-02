#include "test.h"





void sys_strcombine_test::run(vector<string>& argvs) {
	sys_test_equal((string)(sys_strcombine()<<2), "2");
	sys_test_equal((string)(sys_strcombine()<<2.0f), "2.0000000");

	sys_test_equal((string)(sys_strcombine()<<2.0), "2.0000000000000000");
	sys_test_equal((string)(sys_strcombine()<<'a'), "a");
	sys_test_equal((string)(sys_strcombine()<<u8(255)), "255");

	sys_test_equal((string)(sys_strcombine()<<sys_false), "false");
	sys_test_equal((string)(sys_strcombine()<<!sys_false), "true");
}