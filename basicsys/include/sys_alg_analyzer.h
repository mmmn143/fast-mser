#pragma once

namespace basicsys {

	class sys_alg_analyzer {
	public:

		static void init();

		static void set(const string& tag, i64 v);

		static void add(const string& tag, i64 v);

		static void max_(const string& tag, i64 v);

		static void release();

	protected:

		static map<string, i64> m_info;
		static void* m_mutex;
	};

}