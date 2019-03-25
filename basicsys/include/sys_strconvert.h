#pragma once

#define basicsys_path_string(x)	(sys_strconvert::ansi_from_utf16(x).c_str())
#define basicsys_utf8_string(x)	(sys_strconvert::utf8_from_utf16(x).c_str())

namespace basicsys {
	class sys_strconvert {

	public:

		static string utf8_from_utf16(const wstring& utf16);
		static wstring utf16_from_utf8(const string& utf8);

		static string ansi_from_utf16(const wstring& utf16);
		static wstring utf16_from_ansi(const string& ansi);

		static string ansi_from_utf8(const string& utf8);
		static string utf8_from_ansi(const string& ansi);
	};
}