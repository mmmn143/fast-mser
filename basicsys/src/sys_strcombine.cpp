#include "stdafx.h"
#include "sys_strhelper.h"
#include "sys_strcombine.h"


sys_strcombine::operator string() const {
	return m_text;
}

sys_strcombine& sys_strcombine::operator <<(int value)  {
	m_text += sys_strhelper::combine("%d", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(unsigned int value)  {
	m_text += sys_strhelper::combine("%u", value);

	return *this;
}


sys_strcombine& sys_strcombine::operator <<(char value)  {
	m_text += sys_strhelper::combine("%c", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(uint8_t value)  {
	m_text += sys_strhelper::combine("%u", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(short value)  {
	m_text += sys_strhelper::combine("%d", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(unsigned short value)  {
	m_text += sys_strhelper::combine("%u", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(float value)  {
	m_text += sys_strhelper::combine("%.7f", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator <<(double value)  {
	m_text += sys_strhelper::combine("%.16f", value);
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(b8 value)  {
	m_text += value ? "true" : "false";
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(const char* text)  {
	m_text += text;
	return *this;
}

sys_strcombine& sys_strcombine::operator <<(const string& text)  {
	m_text += text;
	return *this;
}

sys_strcombine& sys_strcombine::operator<<(int64_t value) {
	m_text += sys_strhelper::combine("%lld", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator<<(uint64_t value) {
	m_text += sys_strhelper::combine("%llu", value);

	return *this;
}

sys_strcombine& sys_strcombine::operator<<(const sys_strcombine& other) {
	m_text += other.m_text;

	return *this;
}