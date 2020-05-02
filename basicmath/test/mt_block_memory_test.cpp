#include "test.h"

void mt_block_memory_test::run(vector<string>& argvs) {
	mt_block_memory<i32> bm(1000, false);

	for (i32 i = 0; i < 1000000; ++i) {
		i32* data;

		basicmath_block_memory_get(bm, data, i32);
		*data = i;
		basicmath_block_memory_add(bm);
	}

	//basiclog_info2(bm);

	sys_timer t("a");
	t.begin();

	i64 sum = 0;

	for (i32 i = 0; i < bm.m_element_number; ++i) {
		sum += bm.at(i);
	}

	t.end();

	basiclog_info2(sum);
	sum = 0;

	sys_timer tb("b");
	tb.begin();
	mt_block_memory<i32>::block_pos bp;

	for (;;) {
		basicmath_block_memory_visit(bm, bp);

		sum += *bp.m_data;
	}

	tb.end();
	basiclog_info2(sum);

	bm.init(2000, sys_false);

	for (i32 i = 0; i < 100; ++i) {
		i32* data;

		basicmath_block_memory_get(bm, data, i32);
		*data = i;
		basicmath_block_memory_add(bm);
	}

	basiclog_info2(bm);

	bm.clear();
	bm.init(100, sys_false);

	for (i32 i = 0; i < 100; ++i) {
		i32* data;

		basicmath_block_memory_get(bm, data, i32);
		*data = i;
		basicmath_block_memory_add(bm);
	}

	basiclog_info2(bm);
}