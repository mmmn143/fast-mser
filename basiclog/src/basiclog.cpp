#include <eh.h>
#include <iostream>
#include <time.h>
#include <basicsys.h>

using namespace std;

#include "../include/basiclog.h"
#include "helper.h"

using namespace basicsys;
using namespace basiclog;


namespace basiclog {
	log_logger* log_logger_instance = new log_console_logger();
}

void log_logger::enable_debug(bool enable) {
	sys_thread_lock lock(m_mutex);
	m_enable_debug = enable;
}

bool log_logger::is_enable_debug() const {
	sys_thread_lock lock(m_mutex);
	return m_enable_debug;
}

void log_logger::enable_info(bool enable) {
	sys_thread_lock lock(m_mutex);
	m_enable_info = enable;
}

bool log_logger::is_enable_info() const {
	sys_thread_lock lock(m_mutex);
	return m_enable_info;
}

void log_logger::enable_warning(bool enable) {
	sys_thread_lock lock(m_mutex);
	m_enable_warning = enable;
}

bool log_logger::is_enable_warning() const {
	sys_thread_lock lock(m_mutex);
	return m_enable_warning;
}

void log_logger::enable_error(bool enable) {
	sys_thread_lock lock(m_mutex);
	m_enable_error = enable;
}

bool log_logger::is_enable_error() const {
	sys_thread_lock lock(m_mutex);
	return m_enable_error;
}

void log_logger::set_filter(const std::vector<wstring>& filters) {
	sys_thread_lock lock(m_mutex);
	m_filters = filters;
}

void log_logger::get_filter(vector<wstring>& filters) {
	sys_thread_lock lock(m_mutex);
	filters = m_filters;
}

void log_logger::init_logger(log_logger* logger) {
	if (log_logger_instance != NULL) {
		delete log_logger_instance;
	}

	log_logger_instance = logger;
}

log_logger* log_logger::get_logger() {
	return log_logger_instance;
}

void log_logger::release() {
	if (log_logger_instance != NULL) {
		delete log_logger_instance;
		log_logger_instance = NULL;
	}
}

log_logger::~log_logger() {
	sys_thread_lock::release_mutex(m_mutex);
}

log_logger::log_logger(const wstring& timestamp /* = L"%Y/%m/%d %H:%M:%S" */, bool is_pos_contain_dir /* = false */) {
	m_timestamp = timestamp;
	m_is_pos_contain_dir = is_pos_contain_dir;

	m_enable_debug = true;
	m_enable_info = true;
	m_enable_warning = true;
	m_enable_error = true;
	
	m_mutex = sys_thread_lock::create_mutex();
}

void log_logger::print(const wstring& level, const wstring& tag, const wstring& msg, const string& file_path, int line) {
	sys_thread_lock lock(m_mutex);
	
	for (int i = 0; i < (int)m_filters.size(); ++i) {
		if (m_filters[i] == tag) {
			return;
		}
	}
	
	wstring show_msg = msg;

	if (tag == basiclog_performance_warning) {
		show_msg += L" If you don't want to see performance warning, you can add_filter 'basiclog_performance_warning'!";
	}

	wstring w_file_path = sys_strconvert::utf16_from_ansi(file_path);
	wstring pos = m_is_pos_contain_dir ? w_file_path : sys_path::get_file_name(w_file_path);

	wstring log_info = sys_strhelper::combine(L"[%s] [%s] <%s> in line %d of <%s>, ", 
		level.c_str(), 
		format_timestamp().c_str(), 
		tag.c_str(), 
		line,
		pos.c_str());

	log_info.append(show_msg);
	log_info.append(L"\n");


	i32 big_str_size = 5000;

	if (log_info.size() > big_str_size) {

		i32 start_index = 0;
		while (start_index < (i32)log_info.size()) {
			i32 number = min(big_str_size, (i32)log_info.size() - start_index);
			print(log_info.substr(start_index, number));

			start_index += number;
		}

	} else {
		print(log_info);
	}

	if (level.compare(L"error") == 0) {
		error_message_box(show_msg, w_file_path, line);
	}
}

wstring log_logger::format_timestamp() const {
	time_t time_utc;  
	tm tm_local;  
	time(&time_utc);    
	localtime_s(&tm_local, &time_utc); 

	tm tm_utc;
	gmtime_s(&tm_utc, &time_utc);

	int time_zone = tm_local.tm_hour - tm_utc.tm_hour;  
	if (time_zone < -12) {  
		time_zone += 24;   
	} else if (time_zone > 12) {  
		time_zone -= 24;  
	}  

	wchar_t buffer[1024];
	wcsftime(buffer, 1024, m_timestamp.c_str(), &tm_local);

	return sys_strhelper::combine(L"%s UTC %s%d", buffer, time_zone > 0 ? L"+" : L"", time_zone);
}

void log_console_logger::print(const wstring& log_info) {
	wcout<<log_info.c_str();
}

void log_ide_console_logger::print(const wstring& log_info) {
	ide_console_print(log_info);
}

log_file_logger::~log_file_logger() {
	if (m_cur_file != NULL) {
		fclose(m_cur_file);
	}
}


log_file_logger::log_file_logger(const wstring& dir, int file_max_log_number /* = 50000 */, int max_file_number /* = 10000 */, const wstring& timestamp /* = L"%Y/%m/%d %H:%M:%S" */, bool is_pos_contain_dir /* = false */) 
	: log_logger(timestamp, is_pos_contain_dir) {

		m_file_max_log_number = file_max_log_number;
		m_max_file_number = max_file_number;
		m_cur_file = NULL;
		m_cur_file_log_number = 0;

		sys_path::create_multi_dir(m_save_dir);
}

void log_file_logger::set_dir(const wstring& dir) {
	{
		sys_thread_lock lock(m_mutex);

		m_save_dir = dir;
		sys_path::create_multi_dir(m_save_dir);

		m_list_files.clear();

		if (NULL != m_cur_file) {
			fclose(m_cur_file);
		}

		m_cur_file = NULL;
	}
}

void log_file_logger::set_file_max_log_number(int max_number) {
	{
		sys_thread_lock lock(m_mutex);
		m_file_max_log_number = max_number;
	}
}

void log_file_logger::set_max_file_number(int max_number) {
	{
		sys_thread_lock lock(m_mutex);
		m_max_file_number = max_number;
	}
}

void log_file_logger::print(const wstring& log_info) {
	if (NULL == m_cur_file || m_cur_file_log_number == m_file_max_log_number) {
		if ((int)m_list_files.size() == m_max_file_number) {
			_wremove(m_list_files.front().c_str());
			m_list_files.pop_front();
		}

		m_list_files.push_back(m_save_dir + generate_new_file_title() + L".txt");

		if (m_cur_file != NULL) {
			fclose(m_cur_file);
		}

		_wfopen_s(&m_cur_file, m_list_files.back().c_str(), L"wb");
		m_cur_file_log_number = 0;
	}
	
	fprintf(m_cur_file, "%s", sys_strconvert::utf8_from_utf16(log_info).c_str());
	fflush(m_cur_file);
	++m_cur_file_log_number;
}


wstring log_file_logger::generate_new_file_title() const {
	time_t time_utc;  
	tm tm_local;  
	time(&time_utc);    
	localtime_s(&tm_local, &time_utc); 

	tm tm_utc;
	gmtime_s(&tm_utc, &time_utc);

	int time_zone = tm_local.tm_hour - tm_utc.tm_hour;  
	if (time_zone < -12) {  
		time_zone += 24;   
	} else if (time_zone > 12) {  
		time_zone -= 24;  
	}  

	wchar_t buffer[1024];
	wcsftime(buffer, 1024, L"%Y-%m-%d %H-%M-%S", &tm_local);

	return sys_strhelper::combine(L"%s UTC %s%d", buffer, time_zone > 0 ? L"+" : L"", time_zone);
}