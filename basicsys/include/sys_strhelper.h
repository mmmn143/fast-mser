#pragma once


namespace basicsys {
	class sys_strhelper {
	public:

		static wstring combine(const wchar_t* pszFormat, ...);

		static vector<wstring> split(const wstring& text, const wstring& split_tag);

		/** split text by split_tag.

		@note such special cases
		text = L"", return empty vector result
		text = L",", split_tag = L",", return [L"", L""]
		split_tag = L"", return [text]
		*/
		static void split(vector<wstring>& elements, const wstring& text, const wstring& split_tag);

		/**
		*/
		static void replace(wstring& text, const wstring& newValue, const wstring& oldValue);

		static void trim_left(wstring& text, c16 c);

		static int levenshtein_distance(const wstring& word1, const wstring& word2, int insertCost = 1, int replaceCost = 1, int deleteCost = 1, bool ingoreCase = false);

		static bool equal(const wstring& str1, const wstring& str2, bool ingoreCase = false);

		static void upper(wstring& text);

		static void lower(wstring& text);

		static vector<wstring> wstring_from_char_array(int argc, char** argv);
		static void wstring_from_char_array(vector<wstring>& argvs, int argc, char** argv);
	};
}

