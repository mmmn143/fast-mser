#include "stdafx.h"

#include "sys_json.h"

 sys_json_writer::sys_json_writer(sys_buffer_writer* writer, b8 in_map_context) {
	 m_writer = writer;

	 if (in_map_context) {
		 m_json_expected = sys_Json_Expected_Name;
		 m_envs.push_back(sys_Json_Envrionment_Map);
	 } else {
		 m_json_expected = sys_Json_Expected_Value;
		 m_envs.push_back(sys_Json_Envrionment_Seq);
	 }

	 m_prev_seq_begin = sys_false;
	 m_prev_seq_element = sys_false;
 }

sys_json_writer& sys_json_writer::operator<<(const i8* str) {
	string text(str);
	return (*this)<<text;
}

sys_json_writer& sys_json_writer::operator<<(const string& str) {
	if (m_writer->is_string_buffer()) {
		text_write_str(str);
	} else {
		binary_write_str(str);
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const b8& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(b8));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const u8& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(u8));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const i8& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(i8));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const u16& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(u16));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const i16& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(i16));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const u32& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(u32));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const i32& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(i32));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const u64& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(u64));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const i64& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Int);
		m_writer->write((i32)sizeof(i64));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const f32& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Real);
		m_writer->write((i32)sizeof(f32));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

sys_json_writer& sys_json_writer::operator<<(const f64& val) {
	basiclog_assert2(m_json_expected == sys_Json_Expected_Value);

	if (m_writer->is_string_buffer()) {
		text_write_str(sys_strcombine()<<val);
	} else {
		m_writer->write(sys_Node_Type_Real);
		m_writer->write((i32)sizeof(f64));
		m_writer->write(val);

		if (m_envs.back() == sys_Node_Type_Map) {
			m_json_expected = sys_Json_Expected_Name;
		}
	}

	return *this;
}

void sys_json_writer::text_write_str(const string& str) {
	if (str == "{") {
		if (m_envs.back() == sys_Json_Envrionment_Seq) {
			m_writer->write(m_tabs);
		}

		m_writer->write(str + "\n");
		
		m_envs.push_back(sys_Json_Envrionment_Map);
		m_tabs += "\t";
		m_json_expected = sys_Json_Expected_Name;

		m_prev_seq_element = sys_false;
		m_prev_seq_begin = sys_false;

	} else if (str == "[") {
		if (m_envs.back() == sys_Json_Envrionment_Seq) {
			m_writer->write(m_tabs);
		}

		m_writer->write(str + "\n");
		
		m_envs.push_back(sys_Json_Envrionment_Seq);
		m_tabs += "\t";		
		m_json_expected = sys_Json_Expected_Value;
		m_prev_seq_element = sys_false;
		m_prev_seq_begin = sys_true;

	} else if (str == "}" || str == "]") {
		m_tabs.erase(m_tabs.begin());
		m_envs.pop_back();

		if (m_prev_seq_element) {
			m_writer->write("\n");
		}

		m_writer->write(m_tabs + str + "\n");

		if (m_envs.back() == sys_Json_Envrionment_Map) {
			m_json_expected = sys_Json_Expected_Name;
		} else if (m_envs.back() == sys_Json_Envrionment_Seq) {
			m_json_expected = sys_Json_Expected_Value;
		}

		m_prev_seq_element = sys_false;
		m_prev_seq_begin = sys_false;
	} else {
		check_str(str);

		if (m_json_expected == sys_Json_Expected_Name) {
			m_writer->write(m_tabs + str + ":");
			m_json_expected = sys_Json_Expected_Value;
			m_prev_seq_element = sys_false;
			m_prev_seq_begin = sys_false;
		} else if (m_json_expected == sys_Json_Expected_Value) {
			if (m_envs.back() == sys_Json_Envrionment_Seq) {
				if (m_prev_seq_begin) {
					m_writer->write(m_tabs.substr(0, (i32)m_tabs.size() - 1));
				}

				m_writer->write("\t" + str);
				m_prev_seq_element = sys_true;
			} else {
				m_writer->write(str + "\n");
				m_json_expected = sys_Json_Expected_Name;
				m_prev_seq_element = sys_false;
			}
		}

		m_prev_seq_begin = sys_false;
	}
}

void sys_json_writer::pre_process_map_seq(u8 type) {
	//type
	//data size
	//data

	//fwrite(&key_memory_size, sizeof(int), 1, m_file);
	//fwrite(node_name.c_str(), sizeof(wchar_t), (int)node_name.size(), m_file);

	//fwrite(&type, sizeof(int), 1, m_file);
	m_writer->write(type);

	//i64 data_size_position;
	//fgetpos(m_file, &data_size_position);
	m_data_size_positions.push_back(m_writer->position());

	//fseek(m_file, sizeof(int), SEEK_CUR);
	m_writer->write((i32)0);
}

void sys_json_writer::post_process_map_seq() {
	i64 data_size_position = m_writer->position();
	//fgetpos(m_file, &data_size_position);

	m_writer->seek(m_data_size_positions.back(), sys_File_Seek_Set);
	//fseek(m_file, (long)m_data_size_positions.back(), SEEK_SET);

	i64 data_size = data_size_position - m_data_size_positions.back() - sizeof(i32);
	//fwrite(&data_size, sizeof(int), 1, m_file);
	//fseek(m_file, (long)data_size_position, SEEK_SET);

	m_writer->write((i32)data_size);
	m_writer->seek(data_size_position, sys_File_Seek_Set);

	m_data_size_positions.pop_back();

	if (m_envs.back() == sys_Json_Envrionment_Map) {
		m_json_expected = sys_Json_Expected_Name;
	} else if (m_envs.back() == sys_Json_Envrionment_Seq) {
		m_json_expected = sys_Json_Expected_Value;
	}
}

b8 sys_json_writer::check_str(const string& str) {
	i8 invalid_charcaters[] = {'\n', '\t', '{', '}', '[', ']'};

	for (int i = 0; i < sizeof(invalid_charcaters) / sizeof(i8); ++i) {
		if (int(str.find(invalid_charcaters[i])) != -1) {
			string str;

			if (invalid_charcaters[i] == '\n') {
				str = "str must not include \\n";
			} else if (invalid_charcaters[i] == L'\t') {
				str = "str must not include \\t";
			} else {
				str = sys_strcombine()<<"str must not include "<<invalid_charcaters[i];
			}

			basiclog_error2(str);
			return sys_false;
		}
	}

	return sys_true;
}

void sys_json_writer::binary_write_str(const string& str) {
	if (str == "{") {
		m_envs.push_back(sys_Json_Envrionment_Map);
		m_json_expected = sys_Json_Expected_Name;
		pre_process_map_seq(sys_Node_Type_Map);

	} else if (str == "[") {
		m_envs.push_back(sys_Json_Envrionment_Seq);

		//The node in Seq has no name
		m_json_expected = sys_Json_Expected_Value;
		pre_process_map_seq(sys_Node_Type_Seq);

	} else if (str == "}" || str == "]") {
		if (str == "}" && m_envs.back() != sys_Json_Envrionment_Map) {
			basiclog_assert2(false);
		}

		if (str == "]" && m_envs.back() != sys_Json_Envrionment_Seq) {
			basiclog_assert2(false);
		}

		m_envs.pop_back();
		post_process_map_seq();
	} else {
		check_str(str);

		if (m_json_expected == sys_Json_Expected_Name) {
			basiclog_assert2(!str.empty());

			m_writer->write((i32)(str.size() * sizeof(i8)));
			m_writer->write(str);

			m_json_expected = sys_Json_Expected_Value;

		} else if (m_json_expected = sys_Json_Expected_Value) {
			m_writer->write(sys_Node_Type_Text);
			m_writer->write((i32)(str.size() * sizeof(i8)));
			m_writer->write(str);

			if (m_envs.back() == sys_Node_Type_Map) {
				m_json_expected = sys_Json_Expected_Name;
			}
		}
	}
}

sys_json_reader::sys_json_reader(sys_buffer_reader* reader, const string& node_name, i64 offset /* = 0 */, i32 size /* = -1 */, u8 node_type /* = sys_Node_Type_Map */) {
	m_reader = reader;
	m_node_name = node_name;
	m_offset = offset;
	m_size = size;
	m_node_type = node_type;

	if (m_size == -1) {
		m_size = (i32)reader->remain_size();
	}
}

void sys_json_reader::operator>>(string& text) const {
	(const_cast<sys_json_reader*>(this))->load();

	if (m_node_type == sys_Node_Type_Text) {
		m_reader->seek(m_offset, sys_File_Seek_Set);
		m_reader->read_str(text, m_size);

		basiclog_assert2(text[0] != '\t');
	} else if (m_node_type == sys_Node_Type_Seq && m_seq_nodes.size() == 1) {
		vector<string> texts;
		(*this)>>texts;

		text = texts[0];
	}
}

void sys_json_reader::operator>>(b8& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		if (text == "true") {
			val = sys_true;
		} else if (text == "false") {
			val = sys_false;
		} else {
			basiclog_assert2(sys_false);
		}

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = b8((i32)tmp);
	}
}


void sys_json_reader::operator>>(i8& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (i8)atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (i8)tmp;
	}
}

void sys_json_reader::operator>>(u8& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (u8)atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (u8)tmp;
	}
}

void sys_json_reader::operator>>(u16& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (u16)atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (u16)tmp;
	}
}

void sys_json_reader::operator>>(i16& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (i16)atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (i16)tmp;
	}
}

void sys_json_reader::operator>>(u32& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (u32)atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (u32)tmp;
	}
}

void sys_json_reader::operator>>(i32& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = atoi(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (i32)tmp;
	}
}

void sys_json_reader::operator>>(u64& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = (u64)_atoi64(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (u64)tmp;
	}
}

void sys_json_reader::operator>>(i64& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = _atoi64(text.c_str());

	} else {
		f64 tmp = 0;
		(*this)>>tmp;

		val = (i64)tmp;
	}
}

void sys_json_reader::operator>>(f32& val) const {
	f64 tmp;
	(*this)>>tmp;

	val = (f32)tmp;
}

void sys_json_reader::operator>>(f64& val) const {
	if (m_reader->is_string_buffer()) {
		string text;
		(*this)>>text;

		basiclog_assert2(!text.empty());

		val = atof(text.c_str());
	} else {
		(const_cast<sys_json_reader*>(this))->load();
		m_reader->seek(m_offset, sys_File_Seek_Set);
		sys_byte_buffer_reader* byte_reader = (sys_byte_buffer_reader*)m_reader;

		if (m_node_type == sys_Node_Type_Int) {
			if (m_size == sizeof(i8)) {
				val = (f64)byte_reader->read_c8();
			} else if (m_size == sizeof(i16)) {
				val = (f64)byte_reader->read_i16();
			} else if (m_size == sizeof(i32)) {
				val = (f64)byte_reader->read_i32();
			} else if (m_size == sizeof(i64)) {
				val = (f64)byte_reader->read_i64();
			} else {
				basiclog_assert2(false);
			}
		} else if (m_node_type == sys_Node_Type_Real) {
			if (m_size == sizeof(i32)) {
				val = (f64)byte_reader->read_f32();
			} else if (m_size == sizeof(i64)) {
				val = byte_reader->read_f64();
			} else {
				basiclog_assert2(false);
			}
		} else if (m_node_type == sys_Node_Type_Seq && (i32)m_seq_nodes.size() == 1) {
			vector<double> values;
			*this>>values;

			if ((int)values.size() == 1) {
				val = values[0];
			} else {
				basiclog_assert2(false);
			}
		} else {
			basiclog_assert2(false);
		}
	}
}

sys_json_reader sys_json_reader::operator[](const string& node_name) const {
	(const_cast<sys_json_reader*>(this))->load();

	basiclog_assert2(m_node_type == sys_Json_Envrionment_Map);

	map<string, sys_json_reader>::const_iterator iter = m_map_nodes.find(node_name);

	basiclog_assert2(iter != m_map_nodes.end());

	return iter->second;
}

b8 sys_json_reader::has_key(const string& key_name) const {
	(const_cast<sys_json_reader*>(this))->load();

	map<string, sys_json_reader>::const_iterator iter = m_map_nodes.find(key_name);

	if (iter != m_map_nodes.end()) {
		return sys_true;
	}

	return sys_false;
}

sys_json_reader sys_json_reader::operator[](const i32 index) const {
	(const_cast<sys_json_reader*>(this))->load();

	basiclog_assert2(m_node_type == sys_Json_Envrionment_Seq);

	return m_seq_nodes[index];
}

sys_json_reader_iteartor sys_json_reader::begin() const {
	basiclog_assert2(m_node_type == sys_Json_Envrionment_Seq || m_node_type == sys_Json_Envrionment_Map);

	(const_cast<sys_json_reader*>(this))->load();

	return sys_json_reader_iteartor(m_node_type, m_map_nodes.begin(), m_seq_nodes.begin());
}

sys_json_reader_iteartor sys_json_reader::end() const {
	basiclog_assert2(m_node_type == sys_Json_Envrionment_Seq || m_node_type == sys_Json_Envrionment_Map);

	(const_cast<sys_json_reader*>(this))->load();

	return sys_json_reader_iteartor(m_node_type, m_map_nodes.end(), m_seq_nodes.end());
}

void sys_json_reader::load() {
	if (m_loaded) {
		return;
	}

	if (m_reader == NULL) {
		return;
	}

	if (m_node_type != sys_Json_Envrionment_Seq && m_node_type != sys_Json_Envrionment_Map) {
		return;
	}

	m_loaded = true;

	if (m_size == 0) {
		return;
	}

	if (m_reader->is_string_buffer()) {
		text_load_struct_info();
	} else {
		binary_load_struct_info();
	}
}

void sys_json_reader::text_load_struct_info() {
	m_reader->seek(m_offset, sys_File_Seek_Set);

	string text;
	m_reader->read_str(text, m_size);

	if (m_node_type == sys_Json_Envrionment_Seq) {
		i32 valid_index = (i32)text.find_first_not_of('\t');

		if (valid_index != -1) {
			i8 first_character = text[valid_index];
			if (first_character == '{' || first_character == '[') {
				i32 last_index = 0;

				while (last_index < (i32)text.size()) {
					i32 node_data_start_index = (i32)text.find_first_not_of('\t', last_index);

					if (node_data_start_index == -1) {
						break;
					}

					node_data_start_index += 2;  //need skip { or [, and \n	

					i32 node_data_stop_index = find_match_character(text, first_character, node_data_start_index);
					basiclog_assert2(node_data_stop_index != -1);
					basiclog_assert2(node_data_stop_index == (i32)text.size() - 1 || text[node_data_stop_index + 1] == L'\n');

					//basiclog_info2(text.substr(node_data_start_index, node_data_stop_index - node_data_start_index));

					sys_json_reader node(m_reader, "", m_offset + node_data_start_index * sizeof(i8), (node_data_stop_index - node_data_start_index) * sizeof(i8), first_character == '{' ? sys_Node_Type_Map : sys_Node_Type_Seq);
					m_seq_nodes.push_back(node);

					last_index = node_data_stop_index + 2; //need skip } or ], and \n
				}
			} else {
				//must be element!
				int last_index = valid_index;
				for (i32 i = last_index; i < (i32)text.size(); ++i) {
					if (text.at(i) == '\t' || text.at(i) == '\n') {
						//basiclog_assert2(i > last_index);

						sys_json_reader node(m_reader, "", m_offset + last_index * sizeof(i8), (i32)(i - last_index) * sizeof(i8), sys_Node_Type_Text); //not include the last '\t'
						m_seq_nodes.push_back(node);

						last_index = i + 1; //need skip \t

						if (text[i] == '\n') {
							break;
						}
					}
				}
			}
		}
	} else {
		basiclog_assert2(text[0] != '{' && text[0] != '[' && text[0] != '\n');

		int last_index = 0;
		u8 node_type;
		
		while (last_index < (i32)text.size()) {
			int node_name_end_index = (i32)text.find(L':', last_index);

			if (node_name_end_index == -1) {
				break;
			}

			string node_name = text.substr(last_index, node_name_end_index - last_index);
			sys_strhelper::trim_left(node_name, '\t');

			int node_data_start_index = node_name_end_index + 1;
			int node_data_stop_index;

			i8 first_node_data_character = text[node_data_start_index];

			if (first_node_data_character == '{' || first_node_data_character == '[') {
				node_data_start_index += 2;
				node_data_stop_index = find_match_character(text, first_node_data_character, node_data_start_index); //context between {\n } or [\n ]
				node_type = first_node_data_character == '{' ? sys_Node_Type_Map : sys_Node_Type_Seq;
				last_index = node_data_stop_index + 2; //need skip } or ], and \n		
			} else {
				i32 tmp_data_end_index = (i32)text.find('\n', node_data_start_index);

				if (tmp_data_end_index == -1) {
					node_data_stop_index = m_size / sizeof(i8);
				} else {
					node_data_stop_index = tmp_data_end_index;
				}

				node_type = sys_Node_Type_Text;	//not include the last '\n'

				last_index = node_data_stop_index + 1; //need skip \n		
			}

			sys_json_reader node(m_reader, node_name, m_offset + node_data_start_index * sizeof(i8), (node_data_stop_index - node_data_start_index) * sizeof(i8), node_type);
			m_map_nodes.insert(make_pair(node_name, node));
		}
	}
}

void sys_json_reader::binary_load_struct_info() {
	m_reader->seek(m_offset, sys_File_Seek_Set);

	sys_byte_buffer_reader* reader = m_reader->to_byte_buffer_reader();

	i64 data_offset = m_offset;
	i64 data_end = m_size + m_offset;

	if (m_node_type == sys_Json_Envrionment_Seq) {
		while (reader->position() < data_end) {

			u8 node_type = reader->read_u8();

			basiclog_assert2(is_valid_node_type(node_type));

			i32 node_data_size = reader->read_i32();

			//if (node_type != sys_Node_Type_Map && node_type != sys_Node_Type_Seq) {
			//	basiclog_assert2(node_data_size > 0);
			//}
			
			sys_json_reader node(reader, "", reader->position(), node_data_size, node_type);
			m_seq_nodes.push_back(node);

			reader->seek(node_data_size, sys_File_Seek_Cur);
		}
	} else {
		while (reader->position() < data_end) {

			i32 node_name_size = reader->read_i32();

			//basiclog_info2(reader->position());

			string node_name = reader->read_str(node_name_size);

			u8 node_type = reader->read_u8();

			basiclog_assert2(is_valid_node_type(node_type));

			i32 node_data_size = reader->read_i32();

			if (node_type != sys_Node_Type_Map && node_type != sys_Node_Type_Seq) {
				basiclog_assert2(node_data_size > 0);
			}

			sys_json_reader node(reader, node_name, reader->position(), node_data_size, node_type);
			m_map_nodes.insert(make_pair(node_name, node));

			reader->seek(node_data_size, sys_File_Seek_Cur);
		}
	}
}

i32 sys_json_reader::find_match_character(const string& text, i8 need_matched_char, i32 start_index) {
	i32 count = 1;
	i8 other_char = need_matched_char == '{' ? '}' : ']';

	for (i32 i = start_index; i < (i32)text.size(); ++i) {
		if (text[i] == need_matched_char) {
			++count;
		} else if (text[i] == other_char) {
			--count;

			if (count == 0) {
				return i;
			}
		}
	}

	basiclog_assert2(sys_false);
	return -1;
}

sys_json_reader sys_json_reader_iteartor::operator*() const {
	if (m_node_type == sys_Node_Type_Map) {
		return m_map_iterator->second;
	} else if (m_node_type == sys_Node_Type_Seq) {
		return *m_seq_iterator;
	} else {
		basiclog_assert2(false);
		return sys_json_reader(NULL);
	}
}

b8 sys_json_reader_iteartor::operator==(const sys_json_reader_iteartor& other) const {
	return m_node_type == other.m_node_type && m_map_iterator == other.m_map_iterator && m_seq_iterator == other.m_seq_iterator;
}

b8 sys_json_reader_iteartor::operator!=(const sys_json_reader_iteartor& other) const {
	return !(*this == other);
}

sys_json_reader_iteartor& sys_json_reader_iteartor::operator ++ () {
	if (m_node_type == sys_Node_Type_Map) {
		++m_map_iterator;
	} else if (m_node_type == sys_Node_Type_Seq) {
		++m_seq_iterator;
	} else {
		basiclog_assert2(false);
	}

	return *this;
}

sys_json_reader_iteartor sys_json_reader_iteartor::operator ++ (int) {
	sys_json_reader_iteartor other(m_node_type);

	if (m_node_type == sys_Node_Type_Map) {
		other.m_map_iterator = m_map_iterator++;
	} else if (m_node_type == sys_Node_Type_Seq) {
		other.m_seq_iterator = m_seq_iterator++;
	} else {
		basiclog_assert2(false);
	}

	return other;
}

sys_json_reader_iteartor& sys_json_reader_iteartor::operator -- () {
	if (m_node_type == sys_Node_Type_Map) {
		--m_map_iterator;
	} else if (m_node_type == sys_Node_Type_Seq) {
		--m_seq_iterator;
	} else {
		basiclog_assert2(false);
	}

	return *this;
}

sys_json_reader_iteartor sys_json_reader_iteartor::operator -- (int) {
	sys_json_reader_iteartor other(m_node_type);

	if (m_node_type == sys_Node_Type_Map) {
		other.m_map_iterator = m_map_iterator--;
	} else if (m_node_type == sys_Node_Type_Seq) {
		other.m_seq_iterator = m_seq_iterator--;
	} else {
		basiclog_assert2(false);
	}

	return other;
}

sys_json_reader_iteartor& sys_json_reader_iteartor::operator += (int ofs) {
	if (m_node_type == sys_Node_Type_Map) {
		basiclog_assert2(false);
	} else if (m_node_type == sys_Node_Type_Seq) {
		m_seq_iterator += ofs;
	} else {
		basiclog_assert2(false);
	}

	return *this;
}

sys_json_reader_iteartor& sys_json_reader_iteartor::operator -= (int ofs) {
	if (m_node_type == sys_Node_Type_Map) {
		basiclog_assert2(false);
	} else if (m_node_type == sys_Node_Type_Seq) {
		m_seq_iterator -= ofs;
	} else {
		basiclog_assert2(false);
	}

	return *this;
}