#pragma once

#include "sys_tester.h"

namespace basicsys {

	class sys_exe_config {
	public:

		sys_exe_config();
		sys_exe_config(const wstring& config_path);
		sys_exe_config(void* log_instance, sys_tester::Test_Type test_type);
		~sys_exe_config();

	protected:

		void* m_log;
		sys_tester::Test_Type m_test_type;
	};

}