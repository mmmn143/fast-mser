#include "stdafx.h"
#include "sys_path.h"
#include "sys_strhelper.h"

#include <windows.h>

void sys_path::get_file_path(vector<wstring>& filePaths, const wstring& dirPath, const wstring& expandName) {
	filePaths.clear();

	wstring temp = dirPath + TEXT("*") + expandName;

	WIN32_FIND_DATA  fileAttr;
	HANDLE  handle = FindFirstFile(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			filePaths.push_back(dirPath + fileName);
		} while (false);

		while (FindNextFile(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			filePaths.push_back(dirPath + fileName);
		}

		FindClose(handle);
	}
}

void sys_path::get_file_name(vector<wstring>& fileNames, const wstring& dirPath, const wstring& expandName) {		
	fileNames.clear();

	wstring temp = dirPath + TEXT("*") + expandName;

	WIN32_FIND_DATA  fileAttr;
	HANDLE  handle = FindFirstFile(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			fileNames.push_back(fileName);
		} while (false);

		while (FindNextFile(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			fileNames.push_back(fileName);
		}

		FindClose(handle);
	}
}

void sys_path::get_dir_path(vector<wstring>& dirPaths, const wstring& dirPath) {
	dirPaths.clear();

	wstring temp = dirPath + TEXT("*");

	WIN32_FIND_DATA  fileAttr;
	HANDLE  handle = FindFirstFile(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dirPaths.push_back(dirPath + fileName + TEXT("/"));
			}

		} while (false);

		while (FindNextFile(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
				continue;
			}

			wstring fileName = fileAttr.cFileName;

			if (wstring(TEXT(".")) == fileName || wstring(TEXT("..")) == fileName) {
				continue;
			}

			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dirPaths.push_back(dirPath + fileName + TEXT("/"));
			}
		}

		FindClose(handle);
	}
}

wstring sys_path::get_dir_name(const wstring& dirPath) {
	if (dirPath.empty()) {
		return wstring(TEXT(""));
	}

	wstring temp = dirPath;
	temp.erase(temp.end() - 1);

	return get_file_name(temp);
}

wstring sys_path::get_file_name(const wstring& dirPath) {
	if (dirPath.empty()) {
		return wstring(L"");
	}

	wstring temp = dirPath;
	sys_strhelper::replace(temp, L"/", L"\\");

	return temp.substr(temp.rfind(L'/') + 1);
}

wstring sys_path::get_file_title(const wstring& dirPath) {
	if (dirPath.empty()) {
		return wstring(L"");
	}

	wstring temp = get_file_name(dirPath);
	wstring::size_type extendNameStartIndex = temp.rfind(L'.');

	if (wstring::npos != extendNameStartIndex) {
		return temp.substr(0, extendNameStartIndex);
	} else {
		return temp;
	}
}

wstring sys_path::get_file_extend_name(const wstring& path) {
	if (path.empty()) {
		return wstring(TEXT(""));
	}

	wstring::size_type index = path.rfind(L'.');

	if (wstring::npos != index) {
		wstring extend_name = path.substr(index);
		sys_strhelper::lower(extend_name);
		return extend_name;
	} else {
		return wstring(TEXT(""));
	}
}

wstring sys_path::get_dir_path(const wstring& dirPath) {
	if (dirPath.empty()) {
		return wstring(TEXT(""));
	}

	wstring temp = dirPath;
	sys_strhelper::replace(temp, L"/", L"\\");

	wstring::size_type lastDirIndex = temp.rfind(L'/');
	if (wstring::npos == lastDirIndex || temp.size() - 1 == lastDirIndex) {
		return temp;
	}

	return temp.substr(0, lastDirIndex + 1);
}

bool sys_path::create_multi_dir(const wstring& dir) {
	if (dir.empty()) {
		return true;
	}

	if (_waccess(dir.c_str(), 0) == 0)
		return true;

	TCHAR szTemp[1024] = {TEXT('\0')};
	wstring::size_type nPos = dir.find(TEXT('/'));
	while (wstring::npos != nPos)
	{
		wcsncpy_s(szTemp, 1024, dir.c_str(), nPos + 1);
		szTemp[nPos + 1] = TEXT('\0');
		if (_waccess(szTemp, 00) == -1) {
			if (!CreateDirectory(szTemp, NULL))
				return false;
		}

		nPos = dir.find(TEXT('/'), nPos + 1);
	}

	if (_waccess(dir.c_str(), 0) == -1) {
		if (!CreateDirectory(dir.c_str(), NULL))
			return false;
	}

	return true;
}

bool sys_path::is_dir(const wstring& dir) {
	if (dir.empty()) {
		return false;
	}

	TCHAR lastChar = dir.at(dir.size() - 1);

	return L'\\' == lastChar || L'/' == lastChar;
}

bool sys_path::is_exist(const wstring& path) {
	return 0 == _waccess(path.c_str(), 0);
}

void sys_path::delete_file(const wstring& path) {
	DeleteFile(path.c_str());
}

void sys_path::delete_dir(const wstring& dir, bool forceDeleteNoEmptyDir) {
	if (!forceDeleteNoEmptyDir) {
		RemoveDirectory(dir.c_str());
	} else {
		delete_sub_dir(dir);

		//Remove itself
		RemoveDirectory(dir.c_str());
	}
}

void sys_path::delete_sub_dir(const wstring& dir) {
	vector<wstring> subFiles;

	get_file_path(subFiles, dir);

	for (int i = 0; i < (int)subFiles.size(); ++i) {
		delete_file(subFiles[i]);
	}	

	vector<wstring> subDirs;
	get_dir_path(subDirs, dir);

	for (int i = 0; i < (int)subFiles.size(); ++i) {
		delete_dir(subFiles[i], true);
	}
}

wstring sys_path::get_module_path() {
	TCHAR path[2048] = {0};
	GetModuleFileName(NULL, path, 2048);

	return wstring(path);
}

wstring sys_path::get_system_temp_dir() {
#ifdef _WIN32
	TCHAR path[2048] = {0};	
	GetTempPath(MAX_PATH, path);

	return wstring(path);
#else

#endif


}

bool sys_path::copy_file(const wstring& dst, const wstring& src, bool fail_if_exist) {
	return ::CopyFileW(src.c_str(), dst.c_str(), fail_if_exist) == TRUE;
}

bool sys_path::move_file(const wstring& dst, const wstring& src, bool fail_if_exist) {
	if (is_exist(dst)) {
		return false;
	}

	return ::MoveFileW(src.c_str(), dst.c_str()) == TRUE;
}