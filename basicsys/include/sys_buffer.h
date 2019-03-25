#pragma once




namespace basicsys {
	enum sys_File_Seek {
		sys_File_Seek_Set = 0,
		sys_File_Seek_Cur = 1,
		sys_File_Seek_End = 2,
	};

	enum sys_IO_Status {
		sys_IO_Status_OK,
		sys_IO_Status_Error_Eof,
		sys_IO_Status_Error_File_Operation,
	};

	class sys_buffer_writer {
	public:

		sys_buffer_writer() {
			m_io_status = sys_IO_Status_OK;
		}

		virtual ~sys_buffer_writer() {}

		virtual void close() = 0;
		virtual b8 is_closed() const = 0;

		virtual sys_buffer_writer* write(b8 val) = 0;

		virtual sys_buffer_writer* write(i8 val) = 0;
		virtual sys_buffer_writer* write(c16 val) = 0;
		virtual sys_buffer_writer* write(u8 val) = 0;

		virtual sys_buffer_writer* write(u16 val) = 0;
		virtual sys_buffer_writer* write(i16 val) = 0;
		virtual sys_buffer_writer* write(u32 val) = 0;
		virtual sys_buffer_writer* write(i32 val) = 0;
		virtual sys_buffer_writer* write(u64 val) = 0;
		virtual sys_buffer_writer* write(i64 val) = 0;
		virtual sys_buffer_writer* write(f32 val) = 0;
		virtual sys_buffer_writer* write(f64 val) = 0;

		virtual sys_buffer_writer* write(const wchar_t* val) {
			return write(wstring(val));
		}

		virtual sys_buffer_writer* write(const wstring& val) = 0;

		virtual i64 position() const = 0;
		virtual sys_buffer_writer* seek(i64 offset, sys_File_Seek seek_type) = 0;



		virtual b8 is_string_buffer() const = 0;
		virtual sys_IO_Status get_status() const {return m_io_status;}

	protected:

		mutable sys_IO_Status m_io_status;
	};

	class sys_byte_buffer_reader;
	class sys_string_buffer_reader;

	class sys_buffer_reader {
	public:

		sys_buffer_reader() {
			m_io_status = sys_IO_Status_OK;
		}

		virtual ~sys_buffer_reader() {

		}

		virtual void close() = 0;
		virtual b8 is_closed() const = 0;

		virtual b8 is_string_buffer() const = 0;

		virtual wstring read_str(i32 size) const = 0;
		virtual void read_str(wstring& str, i32 size) const = 0;

		virtual i64 position() const = 0;
		virtual i64 size() const = 0;

		i64 remain_size() const {
			return size() - position();
		}

		virtual sys_buffer_reader* seek(i64 offset, sys_File_Seek seek_type) = 0;

		virtual sys_byte_buffer_reader* to_byte_buffer_reader() {return NULL;}
		const sys_byte_buffer_reader* to_byte_buffer_reader() const {return (const_cast<const sys_buffer_reader*>(this))->to_byte_buffer_reader();}
		virtual sys_string_buffer_reader* to_string_buffer_reader() {return NULL;}
		const sys_string_buffer_reader* to_string_buffer_reader() const {return (const_cast<const sys_buffer_reader*>(this))->to_string_buffer_reader();}

		sys_IO_Status get_status() const {return m_io_status;}

		virtual b8 is_eof() const = 0;

	protected:

		mutable sys_IO_Status m_io_status;
	}; 

	class sys_byte_buffer_writer : public sys_buffer_writer {
	public:
		sys_byte_buffer_writer(vector<basicsys::u8>& buffer) {
			m_buffer = &buffer;
			m_position = 0;
		}

		virtual ~sys_byte_buffer_writer() {
			close();
		}

		virtual void close() {

		}

		virtual b8 is_closed() const {
			return sys_false;
		}

		virtual sys_buffer_writer* write(b8 val);
		virtual sys_buffer_writer* write(i8 val);
		virtual sys_buffer_writer* write(c16 val);
		virtual sys_buffer_writer* write(u8 val);
		virtual sys_buffer_writer* write(u16 val);
		virtual sys_buffer_writer* write(i16 val);
		virtual sys_buffer_writer* write(u32 val);
		virtual sys_buffer_writer* write(i32 val);
		virtual sys_buffer_writer* write(u64 val);
		virtual sys_buffer_writer* write(i64 val);
		virtual sys_buffer_writer* write(f32 val);
		virtual sys_buffer_writer* write(f64 val);

		virtual sys_buffer_writer* write(const wstring& val);

		virtual sys_buffer_writer* write(const vector<u8>& buffer);
		virtual sys_buffer_writer* write(const u8* buffer_start, const u8* buffer_end);

		virtual i64 position() const {
			return m_position;
		}

		virtual sys_buffer_writer* seek(i64 offset, sys_File_Seek seek_type);

		b8 is_string_buffer() const {return sys_false;}

	protected:

		void expand_buffer(i32 size);

		vector<u8>* m_buffer;
		i64 m_position;
	};

	class sys_byte_buffer_reader : public sys_buffer_reader {
	public:

		sys_byte_buffer_reader(const u8* begin, const u8* end) {
			m_begin = begin;
			m_end = end;
			m_position = 0;
		}

		virtual ~sys_byte_buffer_reader() {
			close();
		}

		virtual void close() {

		}

		virtual b8 is_closed() const {
			return sys_false;
		}

		virtual i64 position() const {
			return m_position;
		}

		virtual i64 size() const {
			return m_end - m_begin;
		}

		virtual b8 is_string_buffer() const {
			return sys_false;
		}

		virtual b8 read_b8() const;

		virtual i8 read_c8() const;

		virtual u8 read_u8() const;

		virtual c16 read_c16() const;

		virtual u16 read_u16() const;

		virtual i16 read_i16() const;

		virtual u32 read_u32() const;

		virtual i32 read_i32() const;

		virtual u64 read_u64() const;

		virtual i64 read_i64() const;

		virtual f32 read_f32() const;

		virtual f64 read_f64() const;

		virtual wstring read_str(i32 size) const {
			wstring text;
			read_str(text, size);
			return text;
		}

		virtual void read_str(wstring& str, i32 size) const;
		virtual void read(vector<u8>& data, i32 size) const;
		virtual void read(u8* data, i32 size) const;

		virtual sys_buffer_reader* seek(i64 offset, sys_File_Seek seek_type);

		virtual b8 is_eof() const {
			return remain_size() == 0;
		}

		virtual sys_byte_buffer_reader* to_byte_buffer_reader() {return this;}

	protected:

		const u8* m_begin;
		const u8* m_end;
		mutable i64 m_position;
	};



	class sys_byte_file_buffer_writer : public sys_byte_buffer_writer {
	public:

		sys_byte_file_buffer_writer(const wstring& file_path, b8 is_append = false);
		sys_byte_file_buffer_writer(FILE* file, b8 need_close_file = false);

		virtual ~sys_byte_file_buffer_writer();

		virtual void close();

		virtual b8 is_closed() const {
			return m_file == NULL;
		}

		virtual sys_buffer_writer* write(const u8* buffer_start, const u8* buffer_end);

		virtual i64 position() const;

		virtual sys_buffer_writer* seek(i64 offset, sys_File_Seek seek_type);

		virtual sys_IO_Status get_status() {
			return m_file_io_status;
		}

	protected:

		FILE* m_file;
		b8 m_need_close_file;
		mutable sys_IO_Status m_file_io_status;
	};

	class sys_byte_file_buffer_reader : public sys_byte_buffer_reader {
	public:

		sys_byte_file_buffer_reader(const wstring& file_path);
		sys_byte_file_buffer_reader(FILE* file, b8 need_close_file = false);

		virtual ~sys_byte_file_buffer_reader();

		virtual b8 is_closed() const {
			return m_file == NULL;
		}

		virtual sys_IO_Status get_status() const {
			return m_file_io_status;
		}

		/**

		@note the read operation will move the position of the FILE structure!
		*/
		//virtual c16 read_c16() const;

		//virtual u16 read_u16() const;

		//virtual i16 read_i16() const;

		//virtual u32 read_u32() const;

		//virtual i32 read_i32() const;

		//virtual u64 read_u64() const;

		//virtual i64 read_i64() const;

		//virtual f32 read_f32() const;

		//virtual f64 read_f64() const;

		//virtual void read_str(wstring& str, i32 size) const;
		//virtual void read(vector<u8>& data, i32 size) const;
		virtual void read(u8* start, i32 size) const;

		/** Return the total size of the file.
		
		*/
		virtual i64 size() const;
		virtual i64 position() const;
		virtual sys_buffer_reader* seek(i64 offset, sys_File_Seek seek_type);

	protected:

		FILE* m_file;
		b8 m_need_close_file;
		mutable sys_IO_Status m_file_io_status;
	};



	class sys_string_buffer_writer : public sys_buffer_writer {
	public:

		sys_string_buffer_writer(wstring& str) {
			m_position = 0;
			m_str = &str;
		}

		virtual ~sys_string_buffer_writer();

		b8 is_string_buffer() const {
			return true;
		}

		virtual void close() {

		}

		virtual b8 is_closed() const {
			return sys_false;
		}

		virtual sys_buffer_writer* write(b8 val);
		virtual sys_buffer_writer* write(i8 val);
		virtual sys_buffer_writer* write(c16 val);
		virtual sys_buffer_writer* write(u8 val);
		virtual sys_buffer_writer* write(u16 val);
		virtual sys_buffer_writer* write(i16 val);
		virtual sys_buffer_writer* write(u32 val);
		virtual sys_buffer_writer* write(i32 val);
		virtual sys_buffer_writer* write(u64 val);
		virtual sys_buffer_writer* write(i64 val);
		virtual sys_buffer_writer* write(f32 val);
		virtual sys_buffer_writer* write(f64 val);

		virtual sys_buffer_writer* write(const wstring& val);

		virtual i64 position() const {
			return m_position;
		}

		virtual sys_buffer_writer* seek(i64 offset, sys_File_Seek seek_type);

	protected:

		wstring* m_str;
		i64 m_position;
	};

	class sys_string_buffer_reader : public sys_buffer_reader {
	public:

		sys_string_buffer_reader(const wstring& str) {
			m_str = &str;
			m_position = 0;
		}

		virtual ~sys_string_buffer_reader() {
			close();
		}

		virtual void close() {

		}

		virtual b8 is_closed() const {
			return sys_false;
		}

		b8 is_string_buffer() const {
			return sys_true;
		}

		virtual double read(i32 size) const;
		virtual c16 read_char() const;

		virtual wstring read_str(i32 size) const {
			wstring text;
			read_str(text, size);
			return text;
		}

		virtual void read_str(wstring& str, i32 size) const;

		virtual void read_line(wstring& line) const;
		wstring read_line() const {
			wstring str;
			read_line(str);
			return str;
		}

		virtual i64 position() const {
			return m_position;
		}

		virtual i64 size() const {
			return (i64)m_str->size() * sizeof(c16);
		}

		virtual sys_buffer_reader* seek(i64 offset, sys_File_Seek seek_type);

		sys_string_buffer_reader* to_string_buffer_reader() {return this;}

		virtual b8 is_eof() const {
			return remain_size() == 0;
		}

	protected:

		const wstring* m_str;
		mutable i64 m_position;
	};



	class sys_string_file_buffer_writer : public sys_string_buffer_writer {
	public:

		sys_string_file_buffer_writer(const wstring& file_path, b8 is_append = false);
		sys_string_file_buffer_writer(FILE* file, b8 need_close_file = false);

		virtual ~sys_string_file_buffer_writer();

		virtual void close();

		virtual b8 is_closed() const {
			return m_file == NULL;
		}

		virtual sys_buffer_writer* write(const wstring& val);

		virtual i64 position() const;
		
		virtual sys_buffer_writer* seek(i64 offset, sys_File_Seek seek_type);

		virtual sys_IO_Status get_status() {
			return m_file_io_status;
		}

	protected:

		FILE* m_file;
		b8 m_need_close_file;
		mutable sys_IO_Status m_file_io_status;
	};

	class sys_string_file_buffer_reader : public sys_string_buffer_reader {
	public:

		sys_string_file_buffer_reader(const wstring& file_path);
		sys_string_file_buffer_reader(FILE* file, b8 need_close_file = false);

		virtual ~sys_string_file_buffer_reader();

		virtual void close();

		virtual b8 is_closed() const {
			return m_file == NULL;
		}

		

		virtual f64 read(i32 size) const;
		virtual c16 read_char() const;
		virtual void read_str(wstring& str, i32 size) const;
		virtual void read_line(wstring& str) const;

		virtual i64 size() const;
		virtual i64 position() const;
		virtual sys_buffer_reader* seek(i64 offset, sys_File_Seek seek_type);

	protected:

		FILE* m_file;
		b8 m_need_close_file;
		mutable sys_IO_Status m_file_io_status;
	};


}