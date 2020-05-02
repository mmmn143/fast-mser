#pragma once


namespace basicsys {
	class sys_strhelper {
	public:

		static string combine(const char* pszFormat, ...);

		static vector<string> split(const string& text, const string& split_tag);

		/** split text by split_tag.

		@note such special cases
		text = "", return empty vector result
		text = ",", split_tag = ",", return ["", ""]
		split_tag = "", return [text]
		*/
		static void split(vector<string>& elements, const string& text, const string& split_tag);

		/**
		*/
		static void replace(string& text, const string& newValue, const string& oldValue);

		static void trim_left(string& text, i8 c);

		static int levenshtein_distance(const string& word1, const string& word2, int insertCost = 1, int replaceCost = 1, int deleteCost = 1, bool ingoreCase = false);

		static bool equal(const string& str1, const string& str2, bool ingoreCase = false);

		static void upper(string& text);

		static void lower(string& text);

		static vector<string> wstring_from_char_array(int argc, char** argv);
		static void wstring_from_char_array(vector<string>& argvs, int argc, char** argv);
	};
}

