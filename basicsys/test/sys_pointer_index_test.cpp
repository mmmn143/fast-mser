#include "test.h"

#include "basicsys.h"
#include "basicmath.h"

using namespace basicmath;


struct data_type_1 {
	u32 flag : 2;
	u32 size : 30;
};

struct data_type_2 {
	u32 v;

};

struct d {
	u16 a;
	u8 c;
	u16 b;
};

static void test_memory_access() {
	i32 s = sizeof(d);
	
	i32 size = 10000000;
	i32 epoch = 100;

	f64* data = new f64[size];

	vector<i32> indexes;

	mt_random::randperm(indexes, size);

	f64** pointer_ref = new f64*[size];

	cout<<sizeof(data)<<endl;

	for (i32 i = 0; i < size; ++i) {
		pointer_ref[i] = data + indexes[i];
	}

	i64* indexes_ref = new i64[size];

	for (i32 i = 0; i < size; ++i) {
		indexes_ref[i] = indexes[i];
	}

	//sys_timer t("p");
	//t.begin();

	//for (i32 i = 0; i < epoch; ++i) {

	//	for (i32 j = 0; j < size; ++j) {
	//		*pointer_ref[j] = j;
	//	}
	//}

	//t.end();

	sys_timer i("i");
	i.begin();

	for (i32 i = 0; i < epoch; ++i) {

		for (i32 j = 0; j < size; ++j) {
			*(data + indexes_ref[j]) = j;
		}
	}

	i.end();
}

static void test_mask() {
	i32 size = 10000000;
	i32 epoch = 100;

	data_type_1* d1 = new data_type_1[size];
	data_type_2* d2 = new data_type_2[size];

	memset(d1, 0, sizeof(data_type_1) * size);
	memset(d2, 0, sizeof(data_type_2) * size);


	sys_timer t(":");
	t.begin();

	for (i32 k = 0; k < epoch; ++k) {
		for (i32 i = 0; i < size; ++i) {
			d1[i].flag = 2;
			d1[i].size = i;
		}

		for (i32 i = 0; i < size; ++i) {
			if (d1[i].flag == 2) {
				d1[i].size += 1;
			}
		}
	}

	t.end();

	sys_timer t2("mask");
	t2.begin();

	for (i32 k = 0; k < epoch; ++k) {
		for (i32 i = 0; i < size; ++i) {
			d2[i].v |= 0x80000000;
			d2[i].v |= i;
		}


		for (i32 i = 0; i < size; ++i) {
			if ((d2[i].v & 0xc0000000) == 0x80000000) {
				d2[i].v += 1;
			}
		}
	}

	t2.end();
}

void sys_pointer_index_test::run(vector<string>& argvs) {

	//test_mask();
	test_memory_access();
}