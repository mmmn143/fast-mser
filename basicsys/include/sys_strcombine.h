#pragma once

namespace basicsys {
class sys_strcombine {
public:

	operator string() const;

	sys_strcombine() {

	}

	sys_strcombine(const string& text)
	: m_text(text) {
	}

	sys_strcombine& operator<<(basicsys::i32 value);
	sys_strcombine& operator<<(basicsys::u32 value);
	
	/** %c

	*/
	sys_strcombine& operator<<(basicsys::i8 value);

	/** %d
	*/
	sys_strcombine& operator<<(basicsys::u8 value);
	sys_strcombine& operator<<(basicsys::i16 value);
	sys_strcombine& operator<<(basicsys::u16 value);
	sys_strcombine& operator<<(basicsys::f32 value);
	sys_strcombine& operator<<(basicsys::f64 value);
	sys_strcombine& operator<<(basicsys::b8 value);
	sys_strcombine& operator<<(basicsys::i64 value);
	sys_strcombine& operator<<(basicsys::u64 value);

	sys_strcombine& operator<<(const char* text);

	sys_strcombine& operator<<(const string& text);

	sys_strcombine& operator<<(const sys_strcombine& other);

	template<class T>
	sys_strcombine& operator<<(const T& data) {
		write(*this, data);
		return *this;
	}

	template<class T>
	sys_strcombine operator<<(const T& data) const {
		return sys_strcombine(m_text) << data;
	}

	template<class T>
	sys_strcombine& operator<<(const vector<T>& data) {
		*this<<"[";

		for (int i = 0; i < (int)data.size(); ++i) {
			*this<<data[i];

			if (i != (int)data.size() - 1) {
				*this<<", ";
			}
		}

		*this<<"]";
		return *this;
	}

	template<class T1, class T2>
	sys_strcombine& operator<<(const map<T1, T2>& data) {
		*this<<"{\n";
		typename map<T1, T2>::const_iterator iter;
		for (iter = data.begin(); iter != data.end(); ++iter) {
			*this<<iter->first<<":"<<iter->second<<"\n";
		}

		*this<<"}";
		return *this;
	}

private:

	string m_text;
};

template<class T>
void write(sys_strcombine& str, const T& data);
}