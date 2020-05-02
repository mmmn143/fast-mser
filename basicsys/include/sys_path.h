#pragma once


namespace basicsys {
	class sys_path {
	public:

		static string get_match_resource_dir();

		static void get_file_path(vector<string>& filePaths, const string& dirPath, const string& expandName = "");

		static void get_file_name(vector<string>& fileNames, const string& dirPath, const string& expandName = "");

		static void get_dir_path(vector<string>& dirPaths, const string& dirPath);

		static string get_dir_path(const string& path);
		static string get_dir_name(const string& dirPath);

		//包含扩展名
		static string get_file_name(const string& filePath);

		//不包含扩展名
		static string get_file_title(const string& filePath);

		//获取扩展名
		static string get_file_extend_name(const string& filePath);

		static bool create_multi_dir(const string& dir);

		static bool is_dir(const string& path);

		static bool is_exist(const string& path);

		static void delete_file(const string& path);

		static void delete_dir(const string& dir, bool forceDeleteNoEmptyDir);

		static void delete_sub_dir(const string& dir);

		static string get_module_path();
		static string get_system_temp_dir();

		static bool is_valid_path(const string& path);

		static bool copy_file(const string& dst, const string& src, bool fail_if_exist);

		static bool move_file(const string& dst, const string& src, bool fail_if_exist);
	};
}