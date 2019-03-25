#include "stdafx.h"
#include "sys_strhelper.h"
#include "sys_strcombine.h"


sys_strcombine::operator wstring() const {
	return m_text;
}

sys_strcombine& sys_strcombine::operator <<(int value)  {
	m_text += sys_strhelper::combine(L"%d", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(unsigned int value)  {
	m_text += sys_strhelper::combine(L"%u", value);

	return *this;
}


sys_strcombine& sys_strcombine::operator <<(char value)  {
	m_text += sys_strhelper::combine(L"%d", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(uint8_t value)  {
	m_text += sys_strhelper::combine(L"%u", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(short value)  {
	m_text += sys_strhelper::combine(L"%d", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(unsigned short value)  {
	m_text += sys_strhelper::combine(L"%u", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(float value)  {
	m_text += sys_strhelper::combine(L"%.7f", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(double value)  {
	m_text += sys_strhelper::combine(L"%.16f", value);
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(wchar_t character)  {
	m_text += sys_strhelper::combine(L"%c", character);
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(b8 value)  {
	m_text += value ? L"true" : L"false";
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(const wchar_t* text)  {
	m_text += text;
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(const wstring& text)  {
	m_text += text;
	return *this;
}

sys_strcombine& sys_strcombine::operator<<(int64_t value) {
	m_text += sys_strhelper::combine(L"%lld", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator<<(uint64_t value) {
	m_text += sys_strhelper::combine(L"%llu", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator<<(const sys_strcombine& other) {
	m_text += other.m_text;

	return *this;
}