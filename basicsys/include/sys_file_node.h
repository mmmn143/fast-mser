#pragma once



namespace basicsys {
	class sys_bxml_file_node_iterator;	

	class sys_file_node {
	public:

		enum Type
		{
			NONE      = 0, //!< empty node
			INT       = 1, //!< an integer
			REAL      = 2, //!< floating-point number
			STR       = 3, //!< text string in UTF-8 encoding
			SEQ       = 5, //!< sequence
			MAP       = 6, //!< mapping
			USER      = 7, //!< a registered object (e.g. a matrix)
		};

		sys_file_node()
			: m_file(NULL)
			, m_node_data_offset(0)
			, m_node_data_size(0)
			, m_data_type(NONE)
			, m_loaded(sys_false) {

		}

		b8 empty() const {
			return m_data_type == NONE;
		}

		b8 is_seq() const {
			return m_data_type == SEQ;
		}

		b8 is_map() const {
			return m_data_type == MAP;
		}

		i32 size() const;
		wstring name() const;

		operator i32() const;
		operator u32() const;
		operator i8() const;
		operator u8() const;
		operator i16() const;
		operator u16() const;
		operator f32() const;
		operator f64() const;
		operator wstring() const;
		operator b8() const;

		template<class T>
		operator vector<T>() const {
			vector<T> value;
			*this>>value;
			return value;
		}

		sys_file_node operator[] (const wchar_t* node_name) const;
		sys_file_node operator[] (const wstring& node_name) const;
		sys_file_node operator[] (int index) const;

		sys_bxml_file_node_iterator begin() const;
		sys_bxml_file_node_iterator end() const;

		FILE* m_file;
		int m_data_type;
		__int64 m_node_data_offset;
		int m_node_data_size;

	protected:

		void load_struct_info() const;

		wstring m_node_name;
		mutable b8 m_loaded;
		mutable map<wstring, sys_file_node> m_map_nodes;
		mutable vector<sys_file_node> m_seq_nodes;
	};

	class sys_bxml_file_node_iterator {
	public:

		enum {
			Binary_Map_ITER, 
			Binary_Seq_ITER,
			CV_ITER,
		};

		sys_bxml_file_node_iterator(i32 type) {
			m_type = type;
		}

		sys_file_node operator*() const;
		bool operator==(const sys_bxml_file_node_iterator& other) const;
		bool operator!=(const sys_bxml_file_node_iterator& other) const;

		sys_bxml_file_node_iterator& operator ++ ();
		sys_bxml_file_node_iterator operator ++ (i32);
		sys_bxml_file_node_iterator& operator -- ();
		sys_bxml_file_node_iterator operator -- (i32);

		sys_bxml_file_node_iterator& operator += (i32 ofs);
		sys_bxml_file_node_iterator& operator -= (i32 ofs);

		vector<sys_file_node>::iterator m_binary_seq_iter;
		map<wstring, sys_file_node>::iterator m_binary_map_iter;
		i32 m_type;
	};

		typedef map<wstring, sys_file_node> ml_map_file_node;

	void sys_read(const sys_file_node& fs, b8& value);
	void sys_read(const sys_file_node& fs, i32& value);
	void sys_read(const sys_file_node& fs, u32& value);
	void sys_read(const sys_file_node& fs, u8& value);
	void sys_read(const sys_file_node& fs, i8& value);
	void sys_read(const sys_file_node& fs, i16& value);
	void sys_read(const sys_file_node& fs, u16& value);
	void sys_read(const sys_file_node& fs, f32& value);
	void sys_read(const sys_file_node& fs, f64& value);
	void sys_read(const sys_file_node& fs, wstring& value);

	static void sys_read(sys_bxml_file_node_iterator& iter, i32& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, u32& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, u8& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, i8& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, i16& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, u16& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, f32& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, f64& value) {
		sys_read(*iter++, value);
	}

	static void sys_read(sys_bxml_file_node_iterator& iter, wstring& value) {
		sys_read(*iter++, value);
	}

	template<class T>
	static void sys_read(sys_bxml_file_node_iterator& iter, vector<T>& value) {
		T temp;
		sys_read(iter, temp);
		value.push_back(temp);
	}

	void sys_read(const sys_file_node& node, vector<wstring>& value);

	template<class T>
	static void sys_read(const sys_file_node& node, vector<T>& value) {
		if (node.m_data_type == sys_file_node::SEQ) {
			sys_bxml_file_node_iterator iter = node.begin();
			value.reserve(node.size());
			while (iter != node.end()) {
				value.push_back(T());
				sys_read(iter, value.back());
			}
		} else {
			T temp_v;
			sys_read(node, temp_v);
			value.push_back(temp_v);
		}
			
	}


	template<class T>
	void sys_read(const sys_file_node& node, T& value);

	template<class T>
	static void operator>>(const sys_file_node& node, T& value) {
		if (!node.empty()) {
			sys_read(node, value);
		}	
	}

}