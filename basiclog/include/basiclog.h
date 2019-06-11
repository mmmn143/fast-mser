#pragma once

#define basiclog_debug(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_debug()) basiclog::log_logger::get_logger()->print(L"debug", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_info(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_info()) basiclog::log_logger::get_logger()->print(L"info", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_warning(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_warning()) basiclog::log_logger::get_logger()->print(L"warning", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_error(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print(L"error", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_assert(tag, condition)	\
	if (!(condition) && basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print(L"error", tag, L"", __FILE__, __LINE__)

#define basiclog_assert_message(tag, condition, msg)	\
	if (!(condition) && basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print(L"error", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_unsupport(tag)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print(L"error", tag, L"unsupport operation", __FILE__, __LINE__)



#define basiclog_debug2(msg)	\
	basiclog_debug(L"", msg)

#define basiclog_info2(msg)	\
	basiclog_info(L"", msg)

#define basiclog_warning2(msg)	\
	basiclog_warning(L"", msg)

#define basiclog_error2(msg)	\
	basiclog_error(L"", msg)

#define basiclog_assert2(condition)	\
	basiclog_assert(L"", condition)

#define basiclog_assert_message2(condition, msg)	\
	basiclog_assert_message(L"", condition, msg)

#define basiclog_unsupport2()	\
	basiclog_unsupport(L"")

#define basiclog_performance_warning L"performance"

#include <stdio.h>
#include <vector>
#include <list>
#include <string>
#include <sys_basic_define.h>

namespace basiclog {
	class log_logger {
	public:

		static void init_logger(log_logger* logger);
		static log_logger* get_logger();
		static void release();

		virtual ~log_logger();

		log_logger(const std::wstring& timestamp = L"%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false);

		void enable_debug(bool enable);

		bool is_enable_debug() const;

		void enable_info(bool enable);

		bool is_enable_info() const;

		void enable_warning(bool enable);

		bool is_enable_warning() const;

		void enable_error(bool enable);

		bool is_enable_error() const;

		void set_filter(const std::vector<std::wstring>& filters);
		void add_filter(const std::wstring& filter);

		void get_filter(std::vector<std::wstring>& filters);

		void print(const std::wstring& level, const std::wstring& tag, const std::wstring& msg, const std::string& file_path, int line);

	protected:

		std::wstring format_timestamp() const;

		virtual void print(const std::wstring& log_info) = 0;

		std::wstring m_timestamp;
		bool m_is_pos_contain_dir;

		bool m_enable_debug;
		bool m_enable_info;
		bool m_enable_warning;
		bool m_enable_error;

		std::vector<std::wstring> m_filters;
		void* m_mutex;
	};

	class log_console_logger : public log_logger {
	public:

		log_console_logger(const std::wstring& timestamp = L"%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false)
		: log_logger(timestamp, is_pos_contain_dir) {
		}

	protected:

		virtual void print(const std::wstring& log_info);
	};

	class log_ide_console_logger : public log_logger {
	public:

		log_ide_console_logger(const std::wstring& timestamp = L"%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false)
		: log_logger(timestamp, is_pos_contain_dir) {
		}

	protected:

		virtual void print(const std::wstring& log_info);
	};

	class log_file_logger : public log_logger {
	public:

		log_file_logger(const std::wstring& dir, int file_max_log_number = 50000, int max_file_number = 10000, const std::wstring& timestamp = L"%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false);

		~log_file_logger();

		void set_dir(const std::wstring& dir);

		void set_file_max_log_number(int max_number);

		void set_max_file_number(int max_number);

	protected:

		virtual void print(const std::wstring& log_info);
		std::wstring generate_new_file_title() const;

		std::wstring m_save_dir;

		FILE* m_cur_file;
		int m_cur_file_log_number;

		int m_file_max_log_number;
		int m_max_file_number;


		std::list<std::wstring> m_list_files;
	};
}