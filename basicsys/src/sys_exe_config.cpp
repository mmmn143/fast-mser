#include "stdafx.h"

#include "sys_exe_config.h"

sys_exe_config::sys_exe_config() {
	log_logger::init_logger(new log_ide_console_logger());
	sys_tester::init(sys_tester::Test_Type_Slience);
}

sys_exe_config::sys_exe_config(const wstring& config_path) {

}

sys_exe_config::sys_exe_config(void* log_instance, sys_tester::Test_Type test_type) {
	log_logger::init_logger((log_logger*)log_instance);
	sys_tester::init(test_type);
}

sys_exe_config::~sys_exe_config() {
	sys_tester::release();
	log_logger::release();
}