#include "test.h"





void sys_strcombine_test::run(vector<wstring>& argvs) {
	sys_test_equal((wstring)(sys_strcombine()<<2), L"2");
	sys_test_equal((wstring)(sys_strcombine()<<2.0f), L"2.0000000");

	sys_test_equal((wstring)(sys_strcombine()<<2.0), L"2.0000000000000000");
	sys_test_equal((wstring)(sys_strcombine()<<L'a'), L"a");
	sys_test_equal((wstring)(sys_strcombine()<<u8(255)), L"255");

	sys_test_equal((wstring)(sys_strcombine()<<sys_false), L"false");
	sys_test_equal((wstring)(sys_strcombine()<<!sys_false), L"true");
}