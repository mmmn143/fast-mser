#include "stdafx.h"

#include "sys_file_node.h"


sys_bxml_file_node_iterator sys_file_node::begin() const {
	if (m_file != NULL) {
		load_struct_info();

		if (m_data_type == MAP) {
			sys_bxml_file_node_iterator iter(sys_bxml_file_node_iterator::Binary_Map_ITER);
			iter.m_binary_map_iter = m_map_nodes.begin();

			return iter;
		} else if (m_data_type == SEQ) {
			sys_bxml_file_node_iterator iter(sys_bxml_file_node_iterator::Binary_Seq_ITER);
			iter.m_binary_seq_iter = m_seq_nodes.begin();

			return iter;
		} else {
			basiclog_assert2(false);

			return sys_bxml_file_node_iterator(0);
		}
	} else {
		basiclog_assert2(false);
	}
}

sys_bxml_file_node_iterator sys_file_node::end() const {
	if (m_file != NULL) {
		load_struct_info();

		if (m_data_type == MAP) {
			sys_bxml_file_node_iterator iter(sys_bxml_file_node_iterator::Binary_Map_ITER);
			iter.m_binary_map_iter = m_map_nodes.end();

			return iter;
		} else if (m_data_type == SEQ) {
			sys_bxml_file_node_iterator iter(sys_bxml_file_node_iterator::Binary_Seq_ITER);
			iter.m_binary_seq_iter = m_seq_nodes.end();

			return iter;
		} else {
			basiclog_assert2(false);

			return sys_bxml_file_node_iterator(0);
		}
	} else {
		basiclog_assert2(false);
		return sys_bxml_file_node_iterator(0);
	}
}

void sys_file_node::load_struct_info() const {
	if (m_data_type != SEQ && m_data_type != MAP) {
		return;
	}

	if (m_loaded) {
		return;
	}

	m_loaded = true;

	fseek(m_file, (long)m_node_data_offset, SEEK_SET);

	if (m_data_type == SEQ) {
		int data_size = 0;

		while (data_size < m_node_data_size) {
			sys_file_node node;
			int name_size;
			fread(&name_size, sizeof(int), 1, m_file);

			if (name_size > 0) {
				wchar_t* name = new wchar_t[name_size / 2 + 1];
				name[name_size / 2] = 0;

				fread(name, 1, name_size, m_file);
				node.m_node_name = name;
			}

			int item_data_type;
			fread(&item_data_type, sizeof(int), 1, m_file);
			int item_data_size;
			fread(&item_data_size, sizeof(int), 1, m_file);

			node.m_data_type = item_data_type;
			node.m_file = m_file;
			node.m_node_data_size = item_data_size;
			fgetpos(m_file, &node.m_node_data_offset);

			m_seq_nodes.push_back(node);

			fseek(m_file, item_data_size, SEEK_CUR);
			data_size += sizeof(int) + name_size + sizeof(int) * 2 + item_data_size;
		}
	} else {
		int data_size = 0;

		while (data_size < m_node_data_size) {
			sys_file_node node;
			int name_size;
			fread(&name_size, sizeof(int), 1, m_file);
			basiclog_assert2(name_size > 0);

			wchar_t* name = new wchar_t[name_size / 2 + 1];
			name[name_size / 2] = 0;

			fread(name, 1, name_size, m_file);
			node.m_node_name = name;

			int item_data_type;
			fread(&item_data_type, sizeof(int), 1, m_file);
			int item_data_size;
			fread(&item_data_size, sizeof(int), 1, m_file);

			node.m_data_type = item_data_type;
			node.m_file = m_file;
			node.m_node_data_size = item_data_size;
			fgetpos(m_file, &node.m_node_data_offset);

			m_map_nodes.insert(make_pair(node.m_node_name, node));
			fseek(m_file, item_data_size, SEEK_CUR);
			data_size += sizeof(int) + name_size + sizeof(int) * 2 + item_data_size;
		}
	}
}

int sys_file_node::size() const {
		if (m_data_type == SEQ) {
			load_struct_info();

			return (int)m_seq_nodes.size();
		} else if (m_data_type == MAP) {
			load_struct_info();

			return (int)m_map_nodes.size();
		} else {
			return 0;
		}
}

wstring sys_file_node::name() const {
	basiclog_assert2(m_file != NULL);

	return m_node_name;
}

sys_file_node sys_file_node::operator [](const wchar_t* node_name)  const {
	return (*this)[wstring(node_name)];
}

sys_file_node sys_file_node::operator [](const wstring& node_name)  const {
	basiclog_assert2(m_file != NULL);

		if (m_data_type == MAP) {
			load_struct_info();
			ml_map_file_node::const_iterator iter = m_map_nodes.find(node_name);

			if (iter != m_map_nodes.end()) {
				return iter->second;
			} else {
				return sys_file_node();
			}
		} else if (m_data_type == SEQ) {
			load_struct_info();
			for (int i = 0; i < (int)m_seq_nodes.size(); ++i) {
				if (m_seq_nodes[i].m_node_name == node_name) {
					return m_seq_nodes[i];
				}
			}

			return sys_file_node();
		} else {
			basiclog_assert2(false);
			return sys_file_node();
		}
}

sys_file_node sys_file_node::operator [](int index) const {
	basiclog_assert2(m_file != NULL);

		load_struct_info();

		if (index >= 0 && index < (int)m_seq_nodes.size()) {
			return m_seq_nodes[index];
		} else {
			return sys_file_node();
		}
}

namespace basicsys {
	void sys_read(const sys_file_node& fs, b8& value) {
		int i_value;
		sys_read(fs, i_value);

		value = i_value == 1 ? sys_true : sys_false;
	}

	void sys_read(const sys_file_node& fs, i32& value) {
		basiclog_assert2(fs.m_file != NULL);

			double d_value;
			sys_read(fs, d_value);
			value = (int)d_value;
	}

	void sys_read(const sys_file_node& fs, u32& value) {
		basiclog_assert2(fs.m_file != NULL);

			double d_value;
			sys_read(fs, d_value);
			value = (int)d_value;
	}


	void sys_read(const sys_file_node& fs, u8& value) {
			double d_value;
			sys_read(fs, d_value);
			value = (u8)d_value;
	}

	void sys_read(const sys_file_node& fs, i8& value) {
			double d_value;
			sys_read(fs, d_value);
			value = (char)d_value;
	}

	void sys_read(const sys_file_node& fs, i16& value) {
			double d_value;
			sys_read(fs, d_value);
			value = (short)d_value;

	}

	void sys_read(const sys_file_node& fs, u16& value) {
			double d_value;
			sys_read(fs, d_value);
			value = (short)d_value;

	}

	void sys_read(const sys_file_node& fs, f32& value) {
			f64 d_value;
			sys_read(fs, d_value);
			value = (f32)d_value;
	}

	void sys_read(const sys_file_node& fs, f64& value) {
			fseek(fs.m_file, (long)fs.m_node_data_offset, SEEK_SET);

			if (fs.m_data_type == sys_file_node::INT) {
				int i_value;
				fread(&i_value, sizeof(int), 1, fs.m_file);
				value = i_value;
			} else if (fs.m_data_type == sys_file_node::REAL) {
				if (fs.m_node_data_size == sizeof(float)) {
					float f_value;
					fread(&f_value, sizeof(float), 1, fs.m_file);
					value = f_value;
				} else {
					fread(&value, sizeof(double), 1, fs.m_file);
				}
			} else if (fs.m_data_type == sys_file_node::SEQ && fs.size() == 1) {
				vector<double> values;
				sys_read(fs, values);

				if ((int)values.size() == 1) {
					value = values[0];
				}
			} else {
				basiclog_assert_message2(false, L"the dat of this node is not numerical");
			}
	}


	void sys_read(const sys_file_node& fs, wstring& value) {
			fseek(fs.m_file, (long)fs.m_node_data_offset, SEEK_SET);

			if (fs.m_data_type == sys_file_node::STR) {
				wchar_t* text = new wchar_t[fs.m_node_data_size / 2 + 1];
				text[fs.m_node_data_size / 2] = 0;
				fread(text, 1, fs.m_node_data_size, fs.m_file);
				value = wstring(text);
			} else if (fs.m_data_type == sys_file_node::REAL) {
				f64 temp_v;
				sys_read(fs, temp_v);

				if (fs.m_node_data_size == sizeof(float)) {
					value = sys_strcombine()<<(float)temp_v;
				} else {
					value = sys_strcombine()<<temp_v;
				}

				basiclog_warning2(L"convert the real data to string");

			} else if (fs.m_data_type == sys_file_node::SEQ && fs.size() == 1) {
				vector<wstring> values;
				sys_read(fs, values);

				if ((int)values.size() == 1) {
					value = values[0];
				}
			}
	}

	void sys_read(const sys_file_node& node, vector<wstring>& value) {
			if (node.m_data_type == sys_file_node::SEQ) {
				sys_bxml_file_node_iterator iter = node.begin();
				value.reserve(node.size());
				while (iter != node.end()) {
					value.push_back(wstring());
					sys_read(iter, value.back());
				}
			} else if (node.m_data_type == sys_file_node::STR) {
				wstring temp_v;
				sys_read(node, temp_v);
				value.push_back(temp_v);
			} else {
				basiclog_assert_message2(false, L"the node is not a sequence node");
			}
	}

}

sys_file_node::operator i32() const {
	int value;
	*this>>value;
	return value;
}

sys_file_node::operator b8() const {
	bool value;
	*this>>value;
	return value;
}

sys_file_node::operator i8() const {
	char value;
	*this>>value;
	return value;
}

sys_file_node::operator i16() const {
	short value;
	*this>>value;
	return value;
}


sys_file_node::operator u32() const {
	unsigned int value;
	*this>>value;
	return value;
}

sys_file_node::operator u8() const {
	u8 value;
	*this>>value;
	return value;
}

sys_file_node::operator u16() const {
	unsigned short value;
	*this>>value;
	return value;
}

sys_file_node::operator f32() const {
	float value;
	*this>>value;
	return value;
}

sys_file_node::operator f64() const {
	double value;
	*this>>value;
	return value;
}

sys_file_node::operator wstring() const {
	wstring value;
	*this>>value;
	return value;
}

sys_file_node sys_bxml_file_node_iterator::operator*() const {
	if (m_type == Binary_Map_ITER) {
		return m_binary_map_iter->second;
	} else {
		return *m_binary_seq_iter;
	}
}

bool sys_bxml_file_node_iterator::operator==(const sys_bxml_file_node_iterator& other) const {
	if (m_type == Binary_Map_ITER) {
		return m_binary_map_iter == other.m_binary_map_iter;
	} else {
		return m_binary_seq_iter == other.m_binary_seq_iter;
	}
}

bool sys_bxml_file_node_iterator::operator!=(const sys_bxml_file_node_iterator& other) const {
	return !(*this == other);
}

sys_bxml_file_node_iterator& sys_bxml_file_node_iterator::operator++() {
	if (m_type == Binary_Map_ITER) {
		++m_binary_map_iter;
	} else {
		++m_binary_seq_iter;
	}

	return *this;
}

sys_bxml_file_node_iterator& sys_bxml_file_node_iterator::operator--() {
	if (m_type == Binary_Map_ITER) {
		--m_binary_map_iter;
	} else {
		--m_binary_seq_iter;
	}

	return *this;
}


sys_bxml_file_node_iterator sys_bxml_file_node_iterator::operator++(int) {
	sys_bxml_file_node_iterator iter(m_type);
	if (m_type == Binary_Map_ITER) {
		iter.m_binary_map_iter = m_binary_map_iter++;
	} else {
		iter.m_binary_seq_iter = m_binary_seq_iter++;
	}

	return iter;
}

sys_bxml_file_node_iterator sys_bxml_file_node_iterator::operator--(int) {
	sys_bxml_file_node_iterator iter(m_type);
	if (m_type == Binary_Map_ITER) {
		iter.m_binary_map_iter = m_binary_map_iter--;
	} else {
		iter.m_binary_seq_iter = m_binary_seq_iter--;
	}

	return iter;
}

sys_bxml_file_node_iterator& sys_bxml_file_node_iterator::operator+=(int ofs) {
	 if (m_type == Binary_Map_ITER) {
		basiclog_assert2(false);
	} else {
		m_binary_seq_iter += ofs;
	}

	return *this;
}

sys_bxml_file_node_iterator& sys_bxml_file_node_iterator::operator-=(int ofs) {
	if (m_type == Binary_Map_ITER) {
		basiclog_assert2(false);
	} else {
		m_binary_seq_iter -= ofs;
	}

	return *this;
}