#pragma once


namespace basicsys {
	class sys_path {
	public:


		static void get_file_path(vector<wstring>& filePaths, const wstring& dirPath, const wstring& expandName = L"");

		static void get_file_name(vector<wstring>& fileNames, const wstring& dirPath, const wstring& expandName = L"");

		static void get_dir_path(vector<wstring>& dirPaths, const wstring& dirPath);

		static wstring get_dir_path(const wstring& path);
		static wstring get_dir_name(const wstring& dirPath);

		//包含扩展名
		static wstring get_file_name(const wstring& filePath);

		//不包含扩展名
		static wstring get_file_title(const wstring& filePath);

		//获取扩展名
		static wstring get_file_extend_name(const wstring& filePath);

		static bool create_multi_dir(const wstring& dir);

		static bool is_dir(const wstring& path);

		static bool is_exist(const wstring& path);

		static void delete_file(const wstring& path);

		static void delete_dir(const wstring& dir, bool forceDeleteNoEmptyDir);

		static void delete_sub_dir(const wstring& dir);

		static wstring get_module_path();
		static wstring get_system_temp_dir();

		static bool is_valid_path(const wstring& path);

		static bool copy_file(const wstring& dst, const wstring& src, bool fail_if_exist);

		static bool move_file(const wstring& dst, const wstring& src, bool fail_if_exist);
	};
}