#pragma once

namespace basicsys {

	class sys_alg_analyzer {
	public:

		static void init();

		static void set(const wstring& tag, i64 v);

		static void add(const wstring& tag, i64 v);

		static void release();

	protected:

		static map<wstring, i64> m_info;
		static void* m_mutex;
	};

}