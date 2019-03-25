#pragma once

#include "sys_json.h"

namespace basicsys {
	class sys_json_file_writer {
	public:



		b8 m_is_binary;
		sys_json_binary_writer m_binary_writer;
		sys_json_string_writer m_text_writer;
	};

	class sys_json_file_reader {
	public:

		sys_json_file_reader operator[](const wstring& node_name);

		b8 m_is_binary;
		sys_json_binary_writer m_binary_reader;
		sys_json_string_writer m_text_reader;

		void load_struct_info();
	};

	template<class T>
	sys_file_storage& operator<<(sys_json_file_writer& fs, const T& value) {
		if (fs.m_binary_writer) {
			fs.m_binary_writer<<value;
		}
	}
}