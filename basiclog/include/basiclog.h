#pragma once

#define basiclog_debug(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_debug()) basiclog::log_logger::get_logger()->print("debug", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_info(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_info()) basiclog::log_logger::get_logger()->print("info", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_warning(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_warning()) basiclog::log_logger::get_logger()->print("warning", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_error(tag, msg)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print("error", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_assert(tag, condition)	\
	if (!(condition) && basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print("error", tag, "", __FILE__, __LINE__)

#define basiclog_assert_message(tag, condition, msg)	\
	if (!(condition) && basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print("error", tag, basicsys::sys_strcombine()<<msg, __FILE__, __LINE__)

#define basiclog_unsupport(tag)	\
	if (basiclog::log_logger::get_logger() != NULL && basiclog::log_logger::get_logger()->is_enable_error()) basiclog::log_logger::get_logger()->print("error", tag, "unsupport operation", __FILE__, __LINE__)



#define basiclog_debug2(msg)	\
	basiclog_debug("", msg)

#define basiclog_info2(msg)	\
	basiclog_info("", msg)

#define basiclog_warning2(msg)	\
	basiclog_warning("", msg)

#define basiclog_error2(msg)	\
	basiclog_error("", msg)

#define basiclog_assert2(condition)	\
	basiclog_assert("", condition)

#define basiclog_assert_message2(condition, msg)	\
	basiclog_assert_message("", condition, msg)

#define basiclog_unsupport2()	\
	basiclog_unsupport("")

#define basiclog_performance_warning "performance"

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

		log_logger(const std::string& timestamp = "%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false);

		void enable_debug(bool enable);

		bool is_enable_debug() const;

		void enable_info(bool enable);

		bool is_enable_info() const;

		void enable_warning(bool enable);

		bool is_enable_warning() const;

		void enable_error(bool enable);

		bool is_enable_error() const;

		void set_filter(const std::vector<std::string>& filters);
		void add_filter(const std::string& filter);

		void get_filter(std::vector<std::string>& filters);

		void print(const std::string& level, const std::string& tag, const std::string& msg, const std::string& file_path, int line);

	protected:

		std::string format_timestamp() const;

		virtual void print(const std::string& log_info) = 0;

		std::string m_timestamp;
		bool m_is_pos_contain_dir;

		bool m_enable_debug;
		bool m_enable_info;
		bool m_enable_warning;
		bool m_enable_error;

		std::vector<std::string> m_filters;
		void* m_mutex;
	};

	class log_console_logger : public log_logger {
	public:

		log_console_logger(const std::string& timestamp = "%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false)
		: log_logger(timestamp, is_pos_contain_dir) {
		}

	protected:

		virtual void print(const std::string& log_info);
	};

	class log_ide_console_logger : public log_logger {
	public:

		log_ide_console_logger(const std::string& timestamp = "%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false)
		: log_logger(timestamp, is_pos_contain_dir) {
		}

	protected:

		virtual void print(const std::string& log_info);
	};

	class log_file_logger : public log_logger {
	public:

		log_file_logger(const std::string& dir, int file_max_log_number = 50000, int max_file_number = 10000, const std::string& timestamp = "%Y/%m/%d %H:%M:%S", bool is_pos_contain_dir = false);

		~log_file_logger();

		void set_dir(const std::string& dir);

		void set_file_max_log_number(int max_number);

		void set_max_file_number(int max_number);

	protected:

		virtual void print(const std::string& log_info);
		std::string generate_new_file_title() const;

		std::string m_save_dir;

		FILE* m_cur_file;
		int m_cur_file_log_number;

		int m_file_max_log_number;
		int m_max_file_number;


		std::list<std::string> m_list_files;
	};
}