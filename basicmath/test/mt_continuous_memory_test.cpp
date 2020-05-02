#include "test.h"


void mt_continuous_memory_test::run(vector<string>& argvs) {

	sys_timer t("mt_continuous_memory_test");
	t.begin();

	mt_point* ps = new mt_point[20 * 20 * 5000];

	delete[] ps;
	t.end();

	sys_timer t2("mt_discontinuous_memory_test");
	t2.begin();

	for (int i = 0; i < 5000; ++i) {
		mt_point* ps = new mt_point[20 * 20];

		delete[] ps;
	}

	


	t2.end();

}