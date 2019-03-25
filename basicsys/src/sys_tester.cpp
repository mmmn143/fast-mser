#include "stdafx.h"

#include "sys_tester.h"
#include "sys_strcombine.h"

namespace basicsys {
	static sys_tester* mt_tester_instance;
}

sys_tester::sys_tester() {
	m_test_type = sys_tester::Test_Type_Slience;
	m_correct_case_number = 0;
	m_total_case_number = 0;
}

sys_tester::~sys_tester() {
	if (m_test_type == sys_tester::Test_Type_Slience) {
		if (m_total_case_number > 0) {
			double correct_ratio = m_correct_case_number / (double)m_total_case_number;

			basiclog_info(L"sys_tester", sys_strcombine()<<L"total case: "<<m_total_case_number<<L" correct case: "<<m_correct_case_number<<L" correct ratio: "<<correct_ratio);
		} else {
			basiclog_info(L"sys_tester", L"no test case");
		}
	}
}

void sys_tester::init(Test_Type type /* = Test_Type_Slience */) {
	mt_tester_instance = new sys_tester();
	mt_tester_instance->m_test_type = type;
	basiclog_info(L"sys_tester", L"init sys_tester");
}

sys_tester* sys_tester::get_instance() {
	return mt_tester_instance;
}

void sys_tester::release() {
	basicsys_delete(mt_tester_instance);
}

void sys_tester::add_case_condition(bool condition) {
	if (condition) {
		++m_correct_case_number;
	}

	++m_total_case_number;
}