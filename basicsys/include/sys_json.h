#pragma once


#include "sys_buffer.h"

namespace basicsys {



	enum sys_Json_Expected
	{
		sys_Json_Expected_Undefined      = 0,
		sys_Json_Expected_Value = 1,
		sys_Json_Expected_Name  = 2,
	};

	enum sys_Json_Envrionment {
		sys_Json_Envrionment_Map,
		sys_Json_Envrionment_Seq,
	};

	static const u8 sys_Node_Type_Map = 0;
	static const u8 sys_Node_Type_Seq = 1;
	static const u8 sys_Node_Type_Text = 2;
	static const u8 sys_Node_Type_Int = 3;
	static const u8 sys_Node_Type_Real = 4;

	static b8 is_valid_node_type(u8 type) {
		return type >= 0 && type <= 4;
	}

	/**
	@note both key and value can not contain '\t', '\n', '{', '}', '[', ']' characters!
	*/
	class sys_json_writer {
	public:

		sys_json_writer(sys_buffer_writer* writer, b8 in_map_context = sys_true);

		sys_json_writer& operator<<(const wchar_t* str);
		sys_json_writer& operator<<(const wstring& str);
		sys_json_writer& operator<<(const b8& val);
		sys_json_writer& operator<<(const u8& val);
		sys_json_writer& operator<<(const i8& val);
		sys_json_writer& operator<<(const c16& val);
		sys_json_writer& operator<<(const u16& val);
		sys_json_writer& operator<<(const i16& val);
		sys_json_writer& operator<<(const u32& val);
		sys_json_writer& operator<<(const i32& val);
		sys_json_writer& operator<<(const u64& val);
		sys_json_writer& operator<<(const i64& val);
		sys_json_writer& operator<<(const f32& val);
		sys_json_writer& operator<<(const f64& val);

		template<class T>
		sys_json_writer& operator<<(const vector<T>& val) {
			(*this)<<L"[";

			for (i32 i = 0; i < (i32)val.size(); ++i) {
				(*this)<<val[i];
			}

			(*this)<<L"]";

			return *this;
		}

		template<class T>
		sys_json_writer& operator<<(const map<wstring, T>& val) {
			(*this)<<L"{";

			for (map<wstring, T>::const_iterator iter = val.begin(); iter != val.end(); ++iter) {
				(*this)<<iter->first<<iter->second;
			}

			(*this)<<L"}";

			return *this;
		}

		template<class T>
		sys_json_writer& operator<<(const T& val) {
			write(*this, val);
			return *this;
		}

	protected:

		void text_write_str(const wstring& str);
		void binary_write_str(const wstring& str);

		void pre_process_map_seq(u8 type);
		void post_process_map_seq();

		b8 check_str(const wstring& str);

		sys_buffer_writer* m_writer;
		sys_Json_Expected m_json_expected;
		vector<sys_Json_Envrionment> m_envs;
		wstring m_tabs;

		vector<i64> m_data_size_positions;
		b8 m_prev_seq_begin;
		b8 m_prev_seq_element;
	};

	class sys_json_reader_iteartor;

	class sys_json_reader {
	public:

		sys_json_reader(sys_buffer_reader* reader, const wstring& node_name = L"", i64 offset = 0, i32 size = -1, u8 node_type = sys_Node_Type_Map);

		sys_json_reader operator[](const i32 index) const;
		sys_json_reader operator[](const wchar_t* node_name) const {
			return (*this)[wstring(node_name)];
		}

		sys_json_reader operator[](const wstring& node_name) const;

		sys_json_reader_iteartor begin() const;
		sys_json_reader_iteartor end() const;

		b8 is_map() const {
			return m_node_type == sys_Node_Type_Map;
		}

		b8 is_seq() const {
			return m_node_type == sys_Node_Type_Seq;
		}

		b8 has_key(const wstring& key_name) const;

		i32 size() const {
			if (is_map()) {
				return (i32)m_map_nodes.size();
			} else if (is_seq()) {
				return (i32)m_seq_nodes.size();
			} else {
				return 1;
			}
		}

		void operator>>(wstring& text) const;
		void operator>>(b8& val) const;
		void operator>>(u8& val) const;
		void operator>>(i8& val) const;
		void operator>>(c16& val) const;
		void operator>>(u16& val) const;
		void operator>>(i16& val) const;
		void operator>>(u32& val) const;
		void operator>>(i32& val) const;
		void operator>>(u64& val) const;
		void operator>>(i64& val) const;
		void operator>>(f32& val) const;
		void operator>>(f64& val) const;

		template<class T>
		void operator>>(map<wstring, T>& val) const {
			if (is_map()) {
				val.clear();

				sys_json_reader_iteartor iter = begin();

				while (iter != end()) {
					T one;
					(*iter)>>one;

					val.insert(make_pair((*iter).node_name(), one));

					++iter;
				}

			} else {
				basiclog_assert2(sys_false);
			}
		}

		template<class T>
		void operator>>(vector<T>& val) const {
			if (is_seq()) {
				val.clear();

				sys_json_reader_iteartor iter = begin();
				val.reserve(m_seq_nodes.size());

				while (iter != end()) {
					val.push_back(T());
					(*iter)>>val.back();
					++iter;
				}
			} else if (!is_map()) {
				val.push_back(T());
				(*this)>>val.back();
			}
		}

		template<class T>
		void operator>>(T& val) const {
			read(val, *this);
		}

		wstring to_string() const {
			wstring text;
			*this>>text;
			return text;
		}

		i32 to_i32() const {
			i32 val;
			*this>>val;
			return val;
		}

		b8 to_b8() {
			b8 val;
			*this>>val;
			return val;
		}

		const wstring& node_name() const {
			return m_node_name;
		}

	protected:

		sys_buffer_reader* m_reader;
		
		i64 m_offset;
		i32 m_size;

		u8 m_node_type;
		b8 m_loaded;
		wstring m_node_name;

		void load();
		void text_load_struct_info();
		void binary_load_struct_info();

		i32 find_match_character(const wstring& text, c16 need_matched_char, i32 start_index);

		map<wstring, sys_json_reader> m_map_nodes;
		vector<sys_json_reader> m_seq_nodes;
	};

	class sys_json_reader_iteartor {
	public:
		sys_json_reader_iteartor(u8 node_type) {
			m_node_type = node_type;
		}

		sys_json_reader_iteartor(u8 node_type, map<wstring, sys_json_reader>::const_iterator& map_iterator, vector<sys_json_reader>::const_iterator& seq_iterator) {
			m_node_type = node_type;
			m_map_iterator = map_iterator;
			m_seq_iterator = seq_iterator;
		}

		sys_json_reader operator*() const;
		b8 operator==(const sys_json_reader_iteartor& other) const;
		b8 operator!=(const sys_json_reader_iteartor& other) const;

		sys_json_reader_iteartor& operator ++ ();
		sys_json_reader_iteartor operator ++ (int);
		sys_json_reader_iteartor& operator -- ();
		sys_json_reader_iteartor operator -- (int);

		sys_json_reader_iteartor& operator += (int ofs);
		sys_json_reader_iteartor& operator -= (int ofs);

		


	protected:

		u8 m_node_type;
		map<wstring, sys_json_reader>::const_iterator m_map_iterator;
		vector<sys_json_reader>::const_iterator m_seq_iterator;
	};

	template<class T>
	void write(sys_json_writer& writer, const T& val);

	template<class T>
	void read(T& val, const sys_json_reader& writer);
}