#pragma once

#define sys_test(condition)	\
	if (basicsys::sys_tester::get_instance()->get_test_type() == basicsys::sys_tester::Test_Type_Assert) {\
		basiclog_assert2((condition));	\
	} else {\
		basicsys::sys_tester::get_instance()->add_case_condition((condition));	\
		if (!(condition)) { \
			basiclog_info(L"SYS_TEST", L"wrong condition");\
		} \
	}

#define sys_test_equal(a, b)	\
	if (basicsys::sys_tester::get_instance()->get_test_type() == basicsys::sys_tester::Test_Type_Assert) {\
	basiclog_assert2((a) == (b));	\
	} else {\
	if ((a) != (b)) { \
	if (log_logger::get_logger() != NULL && log_logger::get_logger()->is_enable_info()) log_logger::get_logger()->print(L"info", L"SYS_TEST_EQUAL", wstring(L#a) + wstring(L" is not equal to ") + wstring(L#b), __FILE__, __LINE__); \
	basicsys::sys_tester::get_instance()->add_case_condition(false);	\
	} else { \
		basicsys::sys_tester::get_instance()->add_case_condition(true);	\
	}	\
	}


#define sys_declare_test(name)	\
	class name##_test : public sys_test_interface {	\
	public:	\
	void run(vector<wstring>& argvs);	\
	};

namespace basicsys {
	class sys_tester {
	public:

		enum Test_Type {
			Test_Type_Assert,
			Test_Type_Slience,
		};

		sys_tester();
		~sys_tester();

		static void init(Test_Type type = Test_Type_Slience);
		static sys_tester* get_instance();
		static void release();

		void add_case_condition(bool condition);

		Test_Type get_test_type() {
			return m_test_type;
		}

	protected:

		Test_Type m_test_type;

		int m_total_case_number;
		int m_correct_case_number;
	};

	class sys_test_interface {
	public:

		virtual void run(vector<wstring>& argvs) = 0;
	};
}