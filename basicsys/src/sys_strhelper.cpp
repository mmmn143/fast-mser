#include "stdafx.h"
#include <stdarg.h>


string sys_strhelper::combine(const char* pszFormat, ...) {
	char szBuffer[102400];

	va_list ptr;
	va_start(ptr, pszFormat);
#ifdef _WIN32
	vsprintf_s(szBuffer, sizeof(szBuffer) / sizeof(char), pszFormat, ptr);
#else
	vsprintf_s(szBuffer, pszFormat, ptr);
#endif
	va_end(ptr);

	return string(szBuffer);
}

vector<string> sys_strhelper::split(const string& text, const string& split_tag) {
	vector<string> res;
	split(res, text, split_tag);

	return res;
}

void sys_strhelper::split(vector<string>& elements, const string& text, const string& split_tag) {
	elements.clear();

	if (text.empty()) {
		return;
	}

	if (split_tag.empty()) {
		elements.push_back(split_tag);
		return;
	}

	string::size_type index = 0;
	while (true) {
		string::size_type tempIndex = text.find(split_tag, index);

		if (string::npos == tempIndex) {
			elements.push_back(text.substr(index));
			break;
		} else {
			elements.push_back(text.substr(index, tempIndex - index));

			index = tempIndex + 1;
		}
	}
}

void sys_strhelper::replace(string& text, const string& newValue, const string& oldValue) {
	for(string::size_type pos = 0; pos != string::npos; pos += newValue.length())   {     
		if ((pos = text.find(oldValue, pos)) != string::npos) {
			text.replace(pos, oldValue.length(), newValue);     
		} else {
			break;
		}
	}    
}

void sys_strhelper::trim_left(string& text, i8 c) {
	string::size_type pos = text.find_first_not_of(c);

	if (pos == string::npos) {
		text = "";
	} else if (pos > 0) {
		text.erase(text.begin(), text.begin() + pos);
	}
}

int sys_strhelper::levenshtein_distance(const string& word1, const string& word2, int insertCost, int replaceCost, int deleteCost, bool ingoreCase) {	
	//See details in http://www.cnblogs.com/ymind/archive/2012/03/27/fast-memory-efficient-Levenshtein-algorithm.html

	//Process the exception
	if (word1.empty()) {
		return (int)word2.size();
	} else if (word2.empty()) {
		return (int)word1.size();
	}

	//Select the small string as base to reduce the memory requirement
	const string* smallString = &word1;
	const string* largeString = &word2;

	string tempWord1;
	string tempWord2;
	if (ingoreCase) {
		tempWord1 = word1;
		tempWord2 = word2;
		upper(tempWord1);
		upper(tempWord2);

		smallString = &tempWord1;
		largeString = &tempWord2;
	}

	if (word1.size() > word2.size()) {
		swap(smallString, largeString);
	}

	wstring::size_type size1 = largeString->size();
	wstring::size_type size2 = smallString->size();
	int dist = 0;

#define STRING_DEFINE_BUFFER_ALLOC_STATIC(dataType, bufferSize)	\
	dataType buffer1[bufferSize];	\
	dataType buffer2[bufferSize];	\
	dataType* p = buffer1;	\
	dataType* q = buffer2;	\

#define STRING_DEFINE_BUFFER_ALLOC_DYMANAIC(dataType, bufferSize)	\
	dataType* p = new dataType[bufferSize];	\
	dataType* q = new dataType[bufferSize];	\

#define STRING_DEFINE_CALCULATE_DIST(dataType)	\
	p[0] = 0;	\
	for (string::size_type i = 1; i <= size2; ++i) {	\
	p[i] = p[i - 1] + (dataType)insertCost;	\
	}	\
	for (string::size_type i = 1; i <= size1; ++i) {	\
	q[0] = p[0] + deleteCost;	\
	for (string::size_type j = 1; j <= size2; ++j) {	\
	dataType d_del = p[j] + (dataType)deleteCost;	\
	dataType d_ins = q[j - 1] + (dataType)insertCost;	\
	dataType d_replace = p[j - 1] + ((*largeString)[i - 1] == (*smallString)[j - 1] ? 0 : (dataType)replaceCost);	\
	q[j] = d_del;	\
	if (d_ins < q[j]) {	\
	q[j] = d_ins;	\
	}	\
	if (d_replace < q[j]) {	\
	q[j] = d_replace;	\
	}	\
	}	\
	swap(p, q);	\
	}	\
	dist = (int)p[size2];	\

	//optimization
	if (size2 < 16) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint8_t, 16);
		STRING_DEFINE_CALCULATE_DIST(uint8_t);
	} else if (size2 < 32) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint8_t, 32);
		STRING_DEFINE_CALCULATE_DIST(uint8_t);
	} else if (size2 < 64) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint8_t, 64);
		STRING_DEFINE_CALCULATE_DIST(uint8_t);
	} else if (size2 < 256) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint8_t, 256);
		STRING_DEFINE_CALCULATE_DIST(uint8_t);
	} else if (size2 < 1024) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint16_t, 1024);
		STRING_DEFINE_CALCULATE_DIST(uint16_t);
	} else if (size2 < 4048) {
		STRING_DEFINE_BUFFER_ALLOC_STATIC(uint16_t, 4048);
		STRING_DEFINE_CALCULATE_DIST(uint16_t);
	} else if (size2 < SHRT_MAX) {
		STRING_DEFINE_BUFFER_ALLOC_DYMANAIC(uint16_t, size2 + 1);
		STRING_DEFINE_CALCULATE_DIST(uint16_t);
	} else {
		STRING_DEFINE_BUFFER_ALLOC_DYMANAIC(uint32_t, size2 + 1);
		STRING_DEFINE_CALCULATE_DIST(uint32_t);
	}

	return dist;
}

void sys_strhelper::upper(string& text) {
	std::transform(text.begin(), text.end(), text.begin(), towupper);
}

void sys_strhelper::lower(string& text) {
	std::transform(text.begin(), text.end(), text.begin(), towlower);
}

bool sys_strhelper::equal(const string& str1, const string& str2, bool ingoreCase /* = true */) {
	if (ingoreCase) {
		string temp1 = str1;
		string temp2 = str2;
		upper(temp1);
		upper(temp2);
		return temp1 == temp2;
	} else {
		return str1 == str2;
	}
}

vector<string> sys_strhelper::wstring_from_char_array(int argc, char** argv) {
	vector<string> argvs;
	wstring_from_char_array(argvs, argc, argv);

	return argvs;
}

void sys_strhelper::wstring_from_char_array(vector<string>& argvs, int argc, char** argv) {
	argvs.resize(argc);

	for (int i = 0; i < argc; ++i) {
		argvs[i] = argv[i];
	}
}