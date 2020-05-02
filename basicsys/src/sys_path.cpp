#include "stdafx.h"
#include "sys_path.h"
#include "sys_strhelper.h"

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <dirent.h>
#include <unisted.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <mach-o/dyld.h>
#endif


string sys_path::get_match_resource_dir() {
	string module_path = sys_path::get_module_path();

#ifdef _WIN32
	string::size_type index = module_path.find("MATEL\\");
#else
	string::size_type index = module_path.find("MATEL/");
#endif
	basiclog_assert2(index != string::npos);

	string basicimg_root_dir_path = module_path.substr(0, index);

	return basicimg_root_dir_path + "matel_resource/";
}

void sys_path::get_file_path(vector<string>& filePaths, const string& dirPath, const string& expandName) {
	filePaths.clear();

#ifdef _WIN32
	string temp = dirPath + "*" + expandName;

	WIN32_FIND_DATAA  fileAttr;
	HANDLE  handle = FindFirstFileA(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string((".")) == fileName || string(("..")) == fileName) {
				continue;
			}

			filePaths.push_back(dirPath + fileName);
		} while (false);

		while (FindNextFileA(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string(".") == fileName || string(("..")) == fileName) {
				continue;
			}

			filePaths.push_back(dirPath + fileName);
		}

		FindClose(handle);
	}
#else
	DIR* dir = opendir(dirPath.c_str());

	if (dir == NULL) {
		return;
	}

	dirent* entry;

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			continue;
		}

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		string s_name(entry->d_name);

		if (!expandName.empty() && (i32)s_name.find(expandName) != (i32)s_name.size() - (i32)expandName.size()) {
			continue;
		}

		fileNames.push_back(dirPath + entry->d_name);
	}

	closedir(dir);
#endif
}

void sys_path::get_file_name(vector<string>& fileNames, const string& dirPath, const string& expandName) {	
	fileNames.clear();

#ifdef _WIN32
	string temp = dirPath + "*" + expandName;

	WIN32_FIND_DATAA  fileAttr;
	HANDLE  handle = FindFirstFileA(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string((".")) == fileName || string(("..")) == fileName) {
				continue;
			}

			fileNames.push_back(fileName);
		} while (false);

		while (FindNextFileA(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string((".")) == fileName || string(("..")) == fileName) {
				continue;
			}

			fileNames.push_back(fileName);
		}

		FindClose(handle);
	}
#else
	DIR* dir = opendir(dirPath.c_str());

	if (dir == NULL) {
		return;
	}

	dirent* entry;

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			continue;
		}

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		string s_name(entry->d_name);

		if (!expandName.empty() && (i32)s_name.find(expandName) != (i32)s_name.size() - (i32)expandName.size()) {
			continue;
		}

		fileNames.push_back(entry->d_name);
	}

	closedir(dir);
#endif
}

void sys_path::get_dir_path(vector<string>& dirPaths, const string& dirPath) {
	dirPaths.clear();
#ifdef _WIN32
	string temp = dirPath + ("*");

	WIN32_FIND_DATAA  fileAttr;
	HANDLE  handle = FindFirstFileA(temp.c_str(), &fileAttr);

	if(INVALID_HANDLE_VALUE != handle) {
		do 
		{
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string((".")) == fileName || string(("..")) == fileName) {
				continue;
			}

			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dirPaths.push_back(dirPath + fileName + ("/"));
			}

		} while (false);

		while (FindNextFileA(handle, &fileAttr)) {
			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
				continue;
			}

			string fileName = fileAttr.cFileName;

			if (string((".")) == fileName || string(("..")) == fileName) {
				continue;
			}

			if (fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dirPaths.push_back(dirPath + fileName + ("/"));
			}
		}

		FindClose(handle);
	}
#else
	DIR* dir = opendir(dirPath.c_str());

	if (dir == NULL) {
		return;
	}

	dirent* entry;

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type != DT_DIR) {
			continue;
		}

		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		dirPaths.push_back(dirPath + entry->d_name + "/");
	}

	closedir(dir);
#endif
}

string sys_path::get_dir_name(const string& dirPath) {
	if (dirPath.empty()) {
		return string((""));
	}

	string temp = dirPath;
	temp.erase(temp.end() - 1);

	return get_file_name(temp);
}

string sys_path::get_file_name(const string& dirPath) {
	if (dirPath.empty()) {
		return string("");
	}

	string temp = dirPath;
	sys_strhelper::replace(temp, "/", "\\");

	return temp.substr(temp.rfind('/') + 1);
}

string sys_path::get_file_title(const string& dirPath) {
	if (dirPath.empty()) {
		return string("");
	}

	string temp = get_file_name(dirPath);
	string::size_type extendNameStartIndex = temp.rfind('.');

	if (string::npos != extendNameStartIndex) {
		return temp.substr(0, extendNameStartIndex);
	} else {
		return temp;
	}
}

string sys_path::get_file_extend_name(const string& path) {
	if (path.empty()) {
		return string((""));
	}

	string::size_type index = path.rfind('.');

	if (string::npos != index) {
		string extend_name = path.substr(index);
		sys_strhelper::lower(extend_name);
		return extend_name;
	} else {
		return string((""));
	}
}

string sys_path::get_dir_path(const string& dirPath) {
	if (dirPath.empty()) {
		return string((""));
	}

	string temp = dirPath;
	sys_strhelper::replace(temp, "/", "\\");

	string::size_type lastDirIndex = temp.rfind('/');
	if (string::npos == lastDirIndex || temp.size() - 1 == lastDirIndex) {
		return temp;
	}

	return temp.substr(0, lastDirIndex + 1);
}

bool sys_path::create_multi_dir(const string& dir) {
	if (dir.empty()) {
		return true;
	}

	if (_access(dir.c_str(), 0) == 0)
		return true;

	char szTemp[1024] = {('\0')};
	string::size_type nPos = dir.find(('/'));
	while (string::npos != nPos)
	{
		strncpy_s(szTemp, 1024, dir.c_str(), nPos + 1);
		szTemp[nPos + 1] = '\0';
		if (_access(szTemp, 00) == -1) {
#ifdef _WIN32
			if (!CreateDirectoryA(szTemp, NULL))
				return false;
#else
			if (mkdir(szTemp, 0000777) != 0)
				return false;
#endif
		}

		nPos = dir.find('/', nPos + 1);
	}

	if (_access(dir.c_str(), 0) == -1) {
#ifdef _WIN32
		if (!CreateDirectoryA(dir.c_str(), NULL))
			return false;
#else
		if (mkdir(dir.c_str(), 0000777) != 0)
			return false;
#endif
	}

	return true;
}

bool sys_path::is_dir(const string& dir) {
	if (dir.empty()) {
		return false;
	}

	char lastChar = dir.at(dir.size() - 1);

	return '\\' == lastChar || '/' == lastChar;
}

bool sys_path::is_exist(const string& path) {
	return 0 == _access(path.c_str(), 0);
}

void sys_path::delete_file(const string& path) {
	remove(path.c_str());
}

void sys_path::delete_dir(const string& dir, bool forceDeleteNoEmptyDir) {
	if (!forceDeleteNoEmptyDir) {
		remove(dir.c_str());
	} else {
		delete_sub_dir(dir);

		//Remove itself
		remove(dir.c_str());
	}
}

void sys_path::delete_sub_dir(const string& dir) {
	vector<string> subFiles;

	get_file_path(subFiles, dir);

	for (int i = 0; i < (int)subFiles.size(); ++i) {
		delete_file(subFiles[i]);
	}	

	vector<string> subDirs;
	get_dir_path(subDirs, dir);

	for (int i = 0; i < (int)subFiles.size(); ++i) {
		delete_dir(subFiles[i], true);
	}
}

string sys_path::get_module_path() {
	char path[2048] = {0};

#ifdef _WIN32
	GetModuleFileNameA(NULL, path, 2048);
#else
	u32 size = 2048;
	_NSGetExecutablePath(path, &size);
#endif

	return string(path);
}

string sys_path::get_system_temp_dir() {
#ifdef _WIN32
	char path[2048] = {0};	
	GetTempPathA(MAX_PATH, path);

	return string(path);
#else
	return "";
#endif


}

bool sys_path::copy_file(const string& dst, const string& src, bool fail_if_exist) {
#ifdef _WIN32
	return ::CopyFileA(src.c_str(), dst.c_str(), fail_if_exist) == TRUE;
#else
	return system(((string)(sys_strcombine()<<"cp -r "<<src.c_str()<<" "<<dst.c_str())).c_str()) == 0;
#endif
}

bool sys_path::move_file(const string& dst, const string& src, bool fail_if_exist) {
	if (is_exist(dst)) {
		return false;
	}

	return rename(src.c_str(), dst.c_str()) == 0;
}