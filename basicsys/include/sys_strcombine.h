#pragma once

namespace basicsys {
class sys_strcombine {
public:

	operator wstring() const;

	sys_strcombine() {

	}

	sys_strcombine(const wstring& text)
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



	/**
	* Out character.
	* Notice that the reference type for parameter transformation is not valid due to the overload mechanism will adopt the method of <<(int value)
	* when you use ml_vs_output<<wchar_t('A'). However, when you use wchar_t a = wchar_t('A'), ml_vs_output<<a will pass the following method in reference 
	* type for parameter transformation; 
	* 
	*/
	sys_strcombine& operator<<(basicsys::c16 character);
	sys_strcombine& operator<<(const wchar_t* text);

	sys_strcombine& operator<<(const wstring& text);

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
		*this<<L"[";

		for (int i = 0; i < (int)data.size(); ++i) {
			*this<<data[i];

			if (i != (int)data.size() - 1) {
				*this<<L", ";
			}
		}

		*this<<L"]";
		return *this;
	}

	template<class T1, class T2>
	sys_strcombine& operator<<(const map<T1, T2>& data) {
		*this<<L"{\n";

		for (map<T1, T2>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
			*this<<iter->first<<L":"<<iter->second<<L"\n";
		}

		*this<<L"}";
		return *this;
	}

private:

	wstring m_text;
};

template<class T>
void write(sys_strcombine& str, const T& data);
}