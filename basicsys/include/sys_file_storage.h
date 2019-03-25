#pragma once

#include "sys_file_node.h"

namespace basicsys {

	class sys_file_storage {
	public:

		enum Open_Flag {
			Read,
			Write,
		};

		enum File_Format {
			Text,
			Binary,
		};

		sys_file_storage(const wstring& file_path, Open_Flag flag, File_Format format = Text);
		~sys_file_storage();

		sys_file_node operator[](const wstring& node_name);

		b8 is_opened() const {
			return m_file != NULL;
		}

		void post_process();
		void pre_process(const wstring& node_name, i32 type);

		void release();

		sys_file_node root() const;

		enum {
			Envrionment_Map,
			Envrionment_Seq,
		};

		enum
		{
			UNDEFINED      = 0,
			VALUE_EXPECTED = 1,
			NAME_EXPECTED  = 2,
			INSIDE_MAP     = 4
		};

		FILE* m_file;
		i32 m_state;

		vector<i32> m_envs;
		vector<i64> m_data_size_positions;

		sys_file_node m_root;
		wstring m_cur_node_name;
		File_Format m_format;
	};

	/** Only write data itself, do not consider the structure of the file.
	*/
	void sys_write_node_data(sys_file_storage& fs, const wchar_t* value);
	void sys_write_node_data(sys_file_storage& fs, const wstring& value);
	void sys_write_node_data(sys_file_storage& fs, i32 value);
	void sys_write_node_data(sys_file_storage& fs, f32 value);
	void sys_write_node_data(sys_file_storage& fs, f64 value);

	template<class T>
	static void sys_write_node_data(sys_file_storage& fs, const vector<T>& value) {
		for (int i = 0; i < (int)value.size(); ++i) {
			sys_write_node_data(fs, value[i]);
		}
	}

	static void sys_inner_write(sys_file_storage& fs, const wstring& value) {
		sys_write_node_data(fs, value);
	}

	/** Write the data and the structure information.
	*/
	static void sys_inner_write(sys_file_storage& fs, const wchar_t* value) {
		sys_inner_write(fs, wstring(value));
	}

	static void sys_inner_write(sys_file_storage& fs, i32 value) {
		sys_write_node_data(fs, value);
	}

	static void sys_inner_write(sys_file_storage& fs, f32 value)  {
		sys_write_node_data(fs, value);
	}

	static void sys_inner_write(sys_file_storage& fs, f64 value)  {
		sys_write_node_data(fs, value);
	}

	sys_file_storage& operator<<(sys_file_storage& fs, const sys_strcombine& value);
	sys_file_storage& operator<<(sys_file_storage& fs, const wchar_t* value);
	sys_file_storage& operator<<(sys_file_storage& fs, const wstring& value);



	/** OpenCV do not support wstring, hence we handle the wstring type specifically.
	*/
	void sys_inner_write(sys_file_storage& fs, const vector<wstring>& value);

	template<class T>
	static void sys_inner_write(sys_file_storage& fs, const vector<T>& value) {
		if ((int)value.size() == 1) {
			sys_inner_write(fs, value[0]);
		} else {
			fs<<L"[";
			sys_write_node_data(fs, value);
			fs<<L"]";
		}		
	}

	template<class T>
	static sys_file_storage& operator<<(sys_file_storage& fs, const T& value) {
		sys_inner_write(fs, value);

		return fs;
	}

	template<class T>
	void sys_write(sys_file_storage& fs, const wstring& node_name, const T& value) {
		fs<<node_name<<value;
	}

}