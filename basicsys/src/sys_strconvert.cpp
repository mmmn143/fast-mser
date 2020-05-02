#include "stdafx.h"
#include "sys_strconvert.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <locale.h>
#endif

string sys_strconvert::utf8_from_utf16(const wstring& utf16) {
	int nLengthW = (int)utf16.size() + 1;		 
	int nLengthA = nLengthW*4;

	char* mBuffer = new char[nLengthA];
	memset(mBuffer, 0, nLengthA);

#ifdef _WIN32
	BOOL bFailed=(0 == ::WideCharToMultiByte( CP_UTF8, 0, utf16.c_str(), nLengthW, mBuffer, nLengthA, NULL, NULL ));
	if (bFailed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthA = ::WideCharToMultiByte( CP_UTF8, 0, utf16.c_str(), nLengthW, NULL, 0, NULL, NULL );
			delete[] mBuffer;
			mBuffer = new char[nLengthA];

			bFailed=(0 == ::WideCharToMultiByte( CP_UTF8, 0, utf16.c_str(), nLengthW, mBuffer, nLengthA, NULL, NULL ));
		}			
	}

#endif

	string res(mBuffer);
	delete[] mBuffer;
	return res;
}

wstring sys_strconvert::utf16_from_utf8(const string& utf8) {
	int nLengthA = (int)utf8.size() + 1;		 
	int nLengthW = nLengthA;

	wchar_t* mBuffer = new wchar_t[nLengthW];
	memset(mBuffer, 0, nLengthW * sizeof(wchar_t));

#ifdef _WIN32
	BOOL bFailed=(0 == ::MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), nLengthA, mBuffer, nLengthW));
	if (bFailed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthA = ::MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), nLengthA, NULL, 0);
			delete[] mBuffer;
			mBuffer = new wchar_t[nLengthA];

			bFailed=(0 == ::MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), nLengthA, mBuffer, nLengthW));
		}			
	}
#endif

	wstring res(mBuffer);
	delete[] mBuffer;
	return res;
}

string sys_strconvert::local_from_utf16(const wstring& utf16) {
	int nLengthW = (int)utf16.size() + 1;		 
	int nLengthA = nLengthW*4;

	char* mBuffer = new char[nLengthA];
	memset(mBuffer, 0, nLengthA);

#ifdef _WIN32
	BOOL bFailed=(0 == ::WideCharToMultiByte(CP_THREAD_ACP, 0, utf16.c_str(), nLengthW, mBuffer, nLengthA, NULL, NULL ));
	if (bFailed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthA = ::WideCharToMultiByte(CP_THREAD_ACP, 0, utf16.c_str(), nLengthW, NULL, 0, NULL, NULL );
			delete[] mBuffer;
			mBuffer = new char[nLengthA];

			bFailed=(0 == ::WideCharToMultiByte(CP_THREAD_ACP, 0, utf16.c_str(), nLengthW, mBuffer, nLengthA, NULL, NULL ));
		}			
	}


#else
	setlocale(LC_ALL, "");
	wcstombs(mBuffer, utf16.c_str(), nLengthA);
#endif
	string res(mBuffer);
	delete[] mBuffer;
	return res;
}

wstring sys_strconvert::utf16_from_local(const string& ansi) {
	int nLengthA = (int)ansi.size() + 1;		 
	int nLengthW = nLengthA;

	wchar_t* mBuffer = new wchar_t[nLengthW];
	memset(mBuffer, 0, nLengthW * sizeof(wchar_t));

#ifdef _WIN32

	BOOL bFailed=(0 == ::MultiByteToWideChar(CP_THREAD_ACP, 0, ansi.c_str(), nLengthA, mBuffer, nLengthW));
	if (bFailed)
	{
		if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
		{
			nLengthA = ::MultiByteToWideChar(CP_THREAD_ACP, 0, ansi.c_str(), nLengthA, NULL, 0);
			delete[] mBuffer;
			mBuffer = new wchar_t[nLengthA];

			bFailed=(0 == ::MultiByteToWideChar(CP_THREAD_ACP, 0, ansi.c_str(), nLengthA, mBuffer, nLengthW));
		}			
	}
#else
	setlocale(LC_ALL, "");
	mbstowcs(mBuffer, ansi.c_str(), nLengthW);
#endif

	wstring res(mBuffer);
	delete[] mBuffer;

	return res;
}

string sys_strconvert::local_from_utf8(const string& utf8) {
#ifdef _WIN32
	wstring utf16 = utf16_from_utf8(utf8);
	return local_from_utf16(utf16);
#else
	return utf8;
#endif
}

string sys_strconvert::utf8_from_local(const string& ansi) {
#ifdef _WIN32
	wstring utf16 = utf16_from_local(ansi);
	return utf8_from_utf16(utf16);
#else
	return ansi;
#endif
}