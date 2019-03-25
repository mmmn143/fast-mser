#include "stdafx.h"

#include "sys_file_storage.h"

sys_file_storage::sys_file_storage(const wstring& file_path, Open_Flag flag, File_Format format) {
	wstring extend_name = sys_path::get_file_extend_name(file_path);

	if (flag == Read) {
		_wfopen_s(&m_file, file_path.c_str(), L"rb");

		if (NULL == m_file) {
			basiclog_error2(sys_strcombine()<<L"file "<<file_path<<L" not exist");
		} else {
			m_root.m_data_type = sys_file_node::MAP;
			m_root.m_file = m_file;
			m_root.m_node_data_offset = 0;

			fseek(m_file, 0, SEEK_END);
			m_root.m_node_data_size = ftell(m_file);
		}		

	} else if (flag == Write) {
		_wfopen_s(&m_file, file_path.c_str(), L"wb");
		m_state = NAME_EXPECTED;
		m_envs.push_back(Envrionment_Map);
	}

	m_format = format;
}

sys_file_storage::~sys_file_storage() {
	release();
}

sys_file_node sys_file_storage::operator [](const wstring& node_name) {
	return m_root[node_name];
}

void sys_file_storage::post_process() {
	__int64 data_size_position;
	fgetpos(m_file, &data_size_position);

	fseek(m_file, (long)m_data_size_positions.back(), SEEK_SET);

	__int64 data_size = data_size_position - m_data_size_positions.back() - sizeof(int);
	fwrite(&data_size, sizeof(int), 1, m_file);
	fseek(m_file, (long)data_size_position, SEEK_SET);

	m_data_size_positions.pop_back();

	if (m_envs.back() == Envrionment_Map) {
		m_state = NAME_EXPECTED;
	} else if (m_envs.back() == Envrionment_Seq) {
		m_state = VALUE_EXPECTED;
	}
}

sys_file_node sys_file_storage::root() const {
	return m_root;
}

void sys_file_storage::pre_process(const wstring& node_name, int type) {
	//name size
	//name
	//type
	//data size
	//data

	int key_memory_size = (int)node_name.size() * sizeof(wchar_t);

	fwrite(&key_memory_size, sizeof(int), 1, m_file);
	fwrite(node_name.c_str(), sizeof(wchar_t), (int)node_name.size(), m_file);

	fwrite(&type, sizeof(int), 1, m_file);

	__int64 data_size_position;
	fgetpos(m_file, &data_size_position);
	m_data_size_positions.push_back(data_size_position);

	fseek(m_file, sizeof(int), SEEK_CUR);
}


void sys_file_storage::release() {
	if (m_file != NULL) {
		fclose(m_file);
		m_file = NULL;
		m_state = NAME_EXPECTED;
		m_envs.clear();
		m_data_size_positions.clear();
	}
}


namespace basicsys {


	sys_file_storage& operator<<(sys_file_storage& fs, const wstring& value) {
		if (fs.m_format == sys_file_storage::Binary) {
			if (value == L"{") {
				fs.m_envs.push_back(sys_file_storage::Envrionment_Map);
				fs.m_state = sys_file_storage::NAME_EXPECTED;
				fs.pre_process(fs.m_cur_node_name, sys_file_node::MAP);
				fs.m_cur_node_name = L"";

			} else if (value == L"[") {
				fs.m_envs.push_back(sys_file_storage::Envrionment_Seq);

				//The node in Seq has no name
				fs.m_state = sys_file_storage::VALUE_EXPECTED;
				fs.pre_process(fs.m_cur_node_name, sys_file_node::SEQ);
				fs.m_cur_node_name = L"";

			} else if (value == L"}" || value == L"]") {
				if (value == L"}" && fs.m_envs.back() != sys_file_storage::Envrionment_Map) {
					basiclog_assert2(false);
				}

				if (value == L"]" && fs.m_envs.back() != sys_file_storage::Envrionment_Seq) {
					basiclog_assert2(false);
				}

				fs.m_envs.pop_back();
				fs.post_process();
			} else {
				if (fs.m_state & sys_file_storage::NAME_EXPECTED) {
					fs.m_cur_node_name = value;
					fs.m_state = sys_file_storage::VALUE_EXPECTED;

				} else if (fs.m_state & sys_file_storage::VALUE_EXPECTED) {
					sys_inner_write(fs, value);
				}
			}		
		} else {
			if (value == L"{") {
				fs.m_envs.push_back(sys_file_storage::Envrionment_Map);
				fs.m_state = sys_file_storage::NAME_EXPECTED;
				fs.pre_process(fs.m_cur_node_name, sys_file_node::MAP);
				fs.m_cur_node_name = L"";

			} else if (value == L"[") {
				fs.m_envs.push_back(sys_file_storage::Envrionment_Seq);

				//The node in Seq has no name
				fs.m_state = sys_file_storage::VALUE_EXPECTED;
				fs.pre_process(fs.m_cur_node_name, sys_file_node::SEQ);
				fs.m_cur_node_name = L"";

			} else if (value == L"}" || value == L"]") {
				if (value == L"}" && fs.m_envs.back() != sys_file_storage::Envrionment_Map) {
					basiclog_assert2(false);
				}

				if (value == L"]" && fs.m_envs.back() != sys_file_storage::Envrionment_Seq) {
					basiclog_assert2(false);
				}

				fs.m_envs.pop_back();
				fs.post_process();
			} else {
				if (fs.m_state & sys_file_storage::NAME_EXPECTED) {
					fs.m_cur_node_name = value;
					fs.m_state = sys_file_storage::VALUE_EXPECTED;

				} else if (fs.m_state & sys_file_storage::VALUE_EXPECTED) {
					sys_inner_write(fs, value);
				}
			}		
		}
		
		return fs;
	}

	sys_file_storage& operator<<(sys_file_storage& fs, const sys_strcombine& value) {
		return fs<<wstring(value);
	}

	sys_file_storage& operator<<(sys_file_storage& fs, const wchar_t* value) {
		return fs<<wstring(value);
	}

	void sys_write_node_data(sys_file_storage& fs, const wchar_t* value) {
		sys_write_node_data(fs, wstring(value));
	}

	void sys_write_node_data(sys_file_storage& fs, const wstring& value) {
		basiclog_assert2(fs.m_state & sys_file_storage::VALUE_EXPECTED);

		fs.pre_process(fs.m_cur_node_name, sys_file_node::STR);
		fwrite(value.c_str(), sizeof(wchar_t), (int)value.size(), fs.m_file);
		fs.post_process();
	}

	void sys_write_node_data(sys_file_storage& fs, int value) {
		basiclog_assert2(fs.m_state & sys_file_storage::VALUE_EXPECTED);

		fs.pre_process(fs.m_cur_node_name, sys_file_node::INT);
		fwrite(&value, sizeof(int), 1, fs.m_file);
		fs.post_process();
	}

	void sys_write_node_data(sys_file_storage& fs, float value) {
		basiclog_assert2(fs.m_state & sys_file_storage::VALUE_EXPECTED);

		fs.pre_process(fs.m_cur_node_name, sys_file_node::REAL);
		fwrite(&value, sizeof(float), 1, fs.m_file);
		fs.post_process();
	}

	void sys_write_node_data(sys_file_storage& fs, double value) {
		basiclog_assert2(fs.m_state & sys_file_storage::VALUE_EXPECTED);

		fs.pre_process(fs.m_cur_node_name, sys_file_node::REAL);
		fwrite(&value, sizeof(double), 1, fs.m_file);
		fs.post_process();
	}

	void sys_inner_write(sys_file_storage& fs, const vector<wstring>& value) {
		if ((int)value.size() == 1) {
			sys_inner_write(fs, value[0]);
		} else {
			fs<<L"[";
			sys_write_node_data(fs, value);
			fs<<L"]";
		}	
	}
}