#pragma once

namespace basicsys {
	class sys_strconvert {

	public:

		static string local_from_utf16(const wstring& utf16);
		static wstring utf16_from_local(const string& ansi);

		static string utf8_from_utf16(const wstring& utf16);
		static wstring utf16_from_utf8(const string& utf8);

		static string local_from_utf8(const string& utf8);
		static string utf8_from_local(const string& ansi);
	};
}