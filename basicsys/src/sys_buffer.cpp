#include "stdafx.h"
#include "sys_buffer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_byte_buffer_writer

sys_buffer_writer* sys_byte_buffer_writer::write(b8 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(i8 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(c16 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(u8 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(u16 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(i16 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(u32 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(i32 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(u64 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(i64 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(f32 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(f64 val) {
	u8* start = (u8*)&val;
	u8* stop = start + sizeof(val);

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(const vector<u8>& buffer) {
	u8* start = (u8*)&buffer[0];
	u8* stop = start + (i32)buffer.size();

	return write(start, stop);
}

sys_buffer_writer* sys_byte_buffer_writer::write(const u8* buffer_start, const u8* buffer_end) {
	i32 new_data_size = (i32)(buffer_end - buffer_start);
	expand_buffer(new_data_size);

	for (i32 i  = 0; i < new_data_size; ++i) {
		(*m_buffer)[m_position++] = buffer_start[i];
	}

	return this;
}

sys_buffer_writer* sys_byte_buffer_writer::write(const wstring& val) {
	u8* start = (u8*)val.c_str();
	u8* stop = start + sizeof(c16) * (i32)val.size();

	return write(start, stop);
}

void sys_byte_buffer_writer::expand_buffer(i32 size) {
	i64 end_position = m_position + size;
	i64 append_size = end_position - (i64)m_buffer->size();

	if (append_size > 0) {
		m_buffer->insert(m_buffer->end(), append_size, 0);
	}
}

sys_buffer_writer* sys_byte_buffer_writer::seek(i64 offset, sys_File_Seek seek_type) {
	if (seek_type == sys_File_Seek_Set) {
		m_position = offset;
	} else if (seek_type == sys_File_Seek_Cur) {
		m_position += offset;
	} else {
		m_position = (i64)m_buffer->size() - offset;
	}

	return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_byte_buffer_reader
b8 sys_byte_buffer_reader::read_b8() const {
	b8 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

i8 sys_byte_buffer_reader::read_c8() const {
	i8 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

u8 sys_byte_buffer_reader::read_u8() const {
	u8 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

c16 sys_byte_buffer_reader::read_c16() const {
	c16 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

u16 sys_byte_buffer_reader::read_u16() const {
	u16 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

i16 sys_byte_buffer_reader::read_i16() const {
	i16 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

u32 sys_byte_buffer_reader::read_u32() const {
	u32 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

i32 sys_byte_buffer_reader::read_i32() const {
	i32 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

u64 sys_byte_buffer_reader::read_u64() const {
	u64 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

i64 sys_byte_buffer_reader::read_i64() const {
	i64 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

f32 sys_byte_buffer_reader::read_f32() const {
	f32 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

f64 sys_byte_buffer_reader::read_f64() const {
	f64 value;
	u8* ptr_value = (u8*)&value;

	read(ptr_value, sizeof(value));

	return value;
}

void sys_byte_buffer_reader::read_str(wstring& str, i32 size) const {
	basiclog_assert2(size % 2 == 0);

	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		basiclog_assert2(sys_false);
		return;
	}

	i32 character_number = size / 2;

	c16* buffer = new c16[character_number + 1];
	buffer[character_number] = 0;

	read((u8*)buffer, size);

	if (m_io_status == sys_IO_Status_OK) {
		str = wstring(buffer);
	}
	
	delete[] buffer;
}

void sys_byte_buffer_reader::read(vector<u8>& data, i32 size) const {
	basiclog_assert2(size > 0);

	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		basiclog_assert2(sys_false);
		return;
	}

	data.resize(size);

	read(&data[0], size);
}

void sys_byte_buffer_reader::read(u8* data, i32 size) const {
	basiclog_assert2(size >= 0);

	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		basiclog_assert2(sys_false);
		return;
	}

	for (i32 i = 0; i < size; ++i) {
		data[i] = m_begin[m_position++];
	}
}

sys_buffer_reader* sys_byte_buffer_reader::seek(i64 offset, sys_File_Seek seek_type) {
	if (seek_type == sys_File_Seek_Set) {
		m_position = offset;
	} else if (seek_type == sys_File_Seek_Cur) {
		m_position += offset;
	} else {
		m_position = (m_end - m_begin) - offset;
	}

	return this;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_byte_file_buffer_writer

sys_byte_file_buffer_writer::sys_byte_file_buffer_writer(const wstring& file_path, b8 is_append /* = false */)
	: sys_byte_buffer_writer(vector<u8>()) {
		if (_wfopen_s(&m_file, file_path.c_str(), is_append ? L"wb+" : L"wb") != 0) {
			m_io_status = sys_IO_Status_Error_File_Operation;
			basiclog_warning2(L"failed to _wfopen_s");
		}

		m_need_close_file = true;
}

sys_byte_file_buffer_writer::sys_byte_file_buffer_writer(FILE* file, b8 need_close_file)
	: sys_byte_buffer_writer(vector<u8>()) {
		m_file = file;
		m_need_close_file = need_close_file;
}

sys_byte_file_buffer_writer::~sys_byte_file_buffer_writer() {
	close();
}

void sys_byte_file_buffer_writer::close() {
	if (m_file != NULL && m_need_close_file) {
		fclose(m_file);
	}

	m_file = NULL;
}

sys_buffer_writer* sys_byte_file_buffer_writer::write(const u8* start, const u8* stop) {

	if (fwrite(start, stop - start, 1, m_file) != 1) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fwrite");
	}

	return this;
}

i64 sys_byte_file_buffer_writer::position() const {
	i64 res = 0;

	if (fgetpos(m_file, &res) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
	}

	return res;
}

sys_buffer_writer* sys_byte_file_buffer_writer::seek(i64 offset, sys_File_Seek seek_type) {
	if (fseek(m_file, (long)offset, seek_type) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
	}

	return this;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_byte_file_buffer_reader
sys_byte_file_buffer_reader::sys_byte_file_buffer_reader(const wstring& file_path) 
: sys_byte_buffer_reader(NULL, NULL) {
	if (_wfopen_s(&m_file, file_path.c_str(), L"rb") != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to _wfopen_s");
	}

	m_need_close_file = true;
}

sys_byte_file_buffer_reader::sys_byte_file_buffer_reader(FILE* file, b8 need_close_file /* = false */) 
: sys_byte_buffer_reader(NULL, NULL) {
	m_file = file;
	m_need_close_file = need_close_file;
}

sys_byte_file_buffer_reader::~sys_byte_file_buffer_reader() {
	close();
}

//c16 sys_byte_file_buffer_reader::read_c16() const {
//	c16 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//u16 sys_byte_file_buffer_reader::read_u16() const {
//	u16 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//i16 sys_byte_file_buffer_reader::read_i16() const {
//	i16 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//u32 sys_byte_file_buffer_reader::read_u32() const {
//	u32 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//i32 sys_byte_file_buffer_reader::read_i32() const {
//	c16 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//u64 sys_byte_file_buffer_reader::read_u64() const {
//	u64 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//i64 sys_byte_file_buffer_reader::read_i64() const {
//	i64 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//f32 sys_byte_file_buffer_reader::read_f32() const {
//	f32 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//f64 sys_byte_file_buffer_reader::read_f64() const {
//	f64 value;
//	u8* ptr_value = (u8*)&value;
//
//	read(ptr_value, sizeof(value));
//
//	return value;
//}
//
//void sys_byte_file_buffer_reader::read_str(wstring& str, i32 size) const {
//	basiclog_assert2(size > 0 && size % 2 == 0);
//
//	if (remain_size() < size) {
//		m_io_status = sys_IO_Status_Error_Eof;
//		return;
//	}
//
//	i32 character_number = size / 2;
//
//	c16* buffer = new c16[character_number + 1];
//	buffer[character_number] = 0;
//	
//	(i32)fread(buffer, sizeof(c16), character_number, m_file) == character_number ? (m_io_status == sys_IO_Status_OK) : (m_io_status == sys_IO_Status_Error_File_Operation);
//	
//	for (i32 i = 0; i < character_number; ++i) {
//		basiclog_info2(buffer[i]);
//	}
//
//	if (m_io_status == sys_IO_Status_OK) {
//		str = wstring(buffer);
//	}
//
//	delete[] buffer;
//}
//
//void sys_byte_file_buffer_reader::read(vector<u8>& data, i32 size) const {
//	if (remain_size() < size) {
//		m_io_status = sys_IO_Status_Error_Eof;
//		return;
//	}
//
//	size = (i32)min(remain_size(), (i64)size);
//
//	if (size == 0) {
//		return;
//	}
//
//	data.resize(size);
//	(i32)fread(&data[0], sizeof(u8), size, m_file) == size ? (m_io_status == sys_IO_Status_OK) : (m_io_status == sys_IO_Status_Error_File_Operation);
//
//	if (m_io_status != sys_IO_Status_OK) {
//		data.clear();
//	}
//}

void sys_byte_file_buffer_reader::read(u8* start, i32 size) const {
	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		basiclog_assert2(false);
		return;
	}

	size = (i32)min(remain_size(), (i64)size);

	if (size == 0) {
		return;
	}

	if (fread(start, size, 1, m_file) != 1) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fread");
	}
}

i64 sys_byte_file_buffer_reader::position() const {
	i64 res = 0;
	if (fgetpos(m_file, &res) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");

		return -1;
	}

	return res;
}

i64 sys_byte_file_buffer_reader::size() const {
	i64 cur_pos = 0;
	if (fgetpos(m_file, &cur_pos) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
		return -1;
	}

	if (fseek(m_file, 0, SEEK_END) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
		return -1;
	}

	i64 length = -1;
	if (fgetpos(m_file, &length) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
		return -1;
	}

	if (fseek(m_file, (long)cur_pos, SEEK_SET) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
		return -1;
	}

	return length;
}

sys_buffer_reader* sys_byte_file_buffer_reader::seek(i64 offset, sys_File_Seek seek_type) {
	if (fseek(m_file, (long)offset, seek_type) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
	}

	return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_string_buffer_writer

sys_string_buffer_writer::~sys_string_buffer_writer() {
	close();
}

sys_buffer_writer* sys_string_buffer_writer::write(const wstring& val) {
	i32 writed_number = 0;
	
	for (i64 i = m_position; i < (i64)m_str->size() && writed_number < (i32)val.size(); ++i) {
		(*m_str)[m_position++] = val[writed_number++];
	}

	for (i32 i = writed_number; i < (i32)val.size(); ++i) {
		m_str->push_back(val[i]);
		++m_position;
	}

	return this;
}

sys_buffer_writer* sys_string_buffer_writer::write(b8 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(i8 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(c16 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(u8 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(u16 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(i16 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(u32 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(i32 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(u64 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(i64 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(f32 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::write(f64 val) {
	return write(sys_strcombine()<<val);
}

sys_buffer_writer* sys_string_buffer_writer::seek(i64 offset, sys_File_Seek seek_type) {
	basiclog_assert2(offset % sizeof(c16) == 0);

	if (seek_type == sys_File_Seek_Set) {
		m_position = offset;
	} else if (seek_type == sys_File_Seek_Cur) {
		m_position += offset;
	} else {
		m_position = (i64)m_str->size() * sizeof(c16) - offset;
	}

	return this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_string_buffer_reader

f64 sys_string_buffer_reader::read(i32 size) const {
	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		return 0;
	}

	wstring str;
	read_str(str, size);

	return _wtof(str.c_str());
}

c16 sys_string_buffer_reader::read_char() const {
	if (remain_size() < sizeof(c16)) {
		m_io_status = sys_IO_Status_Error_Eof;
		return 0;
	}

	return m_str->at(m_position / sizeof(c16));
	m_position += sizeof(c16);
}

void sys_string_buffer_reader::read_str(wstring& str, i32 size) const {
	if (remain_size() < size) {
		m_io_status = sys_IO_Status_Error_Eof;
		return;
	}

	basiclog_assert2(size % sizeof(c16) == 0);

	i32 start_index = (i32)m_position / (i32)sizeof(c16);
	i32 number = size / (i32)sizeof(c16);

	str = m_str->substr(start_index, number);

	m_position += size;
}

void sys_string_buffer_reader::read_line(wstring& str) const {
	if (is_eof()) {
		m_io_status = sys_IO_Status_Error_Eof;
		return;
	}

	while (!is_eof()) {
		c16 c = read_char();

		if (c != L'\n') {
			str.insert(str.end(), c);
		}
	}
}

sys_buffer_reader* sys_string_buffer_reader::seek(i64 offset, sys_File_Seek seek_type) {
	basiclog_assert2(offset % sizeof(c16) == 0);

	if (seek_type == sys_File_Seek_Set) {
		m_position = offset;
	} else if (seek_type == sys_File_Seek_Cur) {
		m_position += offset;
	} else {
		m_position = (i64)m_str->size() * sizeof(c16) - offset;
	}

	return this;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_string_file_buffer_writer

sys_string_file_buffer_writer::sys_string_file_buffer_writer(const wstring& file_path, b8 is_append /* = false */)
: sys_string_buffer_writer(wstring()) {
	if (_wfopen_s(&m_file, file_path.c_str(), is_append ? L"wb+" : L"wb") != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to _wfopen_s");
	}

	m_need_close_file = true;
}

sys_string_file_buffer_writer::sys_string_file_buffer_writer(FILE* file, b8 need_close_file)
: sys_string_buffer_writer(wstring()) {
	m_file = file;
	m_need_close_file = need_close_file;
}

sys_string_file_buffer_writer::~sys_string_file_buffer_writer() {
	close();
}

void sys_string_file_buffer_writer::close() {
	if (m_file != NULL && m_need_close_file) {
		fclose(m_file);
	}

	m_file = NULL;
}

sys_buffer_writer* sys_string_file_buffer_writer::write(const wstring& val) {
	i32 size = (i32)val.size() * sizeof(c16);
	i32 f_res = val.empty() ? 0 : 1;

	if (fwrite(val.c_str(), size, 1, m_file) != f_res) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fwrite");
	}

	return this;
}

i64 sys_string_file_buffer_writer::position() const {
	i64 res = 0;

	if (fgetpos(m_file, &res) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
	}

	return res;
}

sys_buffer_writer* sys_string_file_buffer_writer::seek(i64 offset, sys_File_Seek seek_type) {
	if (fseek(m_file, (long)offset, seek_type) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
	}

	return this;
}








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//sys_string_file_buffer_reader

sys_string_file_buffer_reader::sys_string_file_buffer_reader(const wstring& file_path)
	: sys_string_buffer_reader(wstring()) {
		if (_wfopen_s(&m_file, file_path.c_str(), L"rb") != 0) {
			m_file_io_status = sys_IO_Status_Error_File_Operation;
			basiclog_warning2(L"failed to _wfopen_s");
		}

		m_need_close_file = true;
}

sys_string_file_buffer_reader::sys_string_file_buffer_reader(FILE* file, b8 need_close_file)
	: sys_string_buffer_reader(wstring()) {
		m_file = file;
		m_need_close_file = need_close_file;
}

sys_string_file_buffer_reader::~sys_string_file_buffer_reader() {
	close();
}

void sys_string_file_buffer_reader::close() {
	if (m_file != NULL && m_need_close_file) {
		fclose(m_file);
	}

	m_file = NULL;
}

f64 sys_string_file_buffer_reader::read(i32 size) const {
	if (is_eof()) {
		m_io_status = sys_IO_Status_Error_Eof;
		return 0;
	}

	wstring text;
	read_str(text, size);

	return _wtof(text.c_str());
}

c16 sys_string_file_buffer_reader::read_char() const {
	if (is_eof()) {
		m_io_status = sys_IO_Status_Error_Eof;
		return 0;
	}

	c16 c;

	if (fread(&c, sizeof(c16), 1, m_file) != 1) {
		m_io_status = sys_IO_Status_Error_File_Operation;
	}

	return c;
}

void sys_string_file_buffer_reader::read_str(wstring& str, i32 size) const {
	if (is_eof()) {
		m_io_status = sys_IO_Status_Error_Eof;
		return;
	}

	i32 character_count = size / sizeof(wchar_t);
	wchar_t* temp = new wchar_t[character_count + 1];

	i32 f_res = size == 0 ? 0 : 1;

	if (fread(temp, size, 1, m_file) != f_res) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fread");
	}

	temp[character_count] = 0;
	str = temp;
}

i64 sys_string_file_buffer_reader::position() const {
	i64 res = 0;
	if (fgetpos(m_file, &res) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");

		return -1;
	}

	return res;
}

i64 sys_string_file_buffer_reader::size() const {
	i64 cur_pos = 0;
	if (fgetpos(m_file, &cur_pos) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
		return -1;
	}

	if (fseek(m_file, 0, SEEK_END) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
		return -1;
	}

	i64 length = -1;
	if (fgetpos(m_file, &length) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fgetpos");
		return -1;
	}

	if (fseek(m_file, (long)cur_pos, SEEK_SET) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");
		return -1;
	}

	return length;
}

sys_buffer_reader* sys_string_file_buffer_reader::seek(i64 offset, sys_File_Seek seek_type) {
	if (fseek(m_file, (long)offset, seek_type) != 0) {
		m_io_status = sys_IO_Status_Error_File_Operation;
		basiclog_warning2(L"failed to fseek");

	}

	return this;
}

void sys_string_file_buffer_reader::read_line(wstring& str) const {
	if (is_eof()) {
		m_io_status = sys_IO_Status_Error_Eof;
		return;
	}

	while (!is_eof()) {
		c16 c = read_char();

		if (c != L'\n') {
			str.insert(str.end(), c);
		}
	}
}