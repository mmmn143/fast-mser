#pragma once


#include <functional>
#include <vector>

//#define sys_lambda(index)	[&](int index)

#define sys_for(index, v)	for (basicsys::i32 index = 0; index != (basicsys::i32)(v).size(); ++index)
#define sys_rfor(index, v)	for (basicsys::i32 index = (basicsys::i32)(v).size() - 1; index >= 0; --index)

namespace basicsys {
	
	//template<class T>
	//class sys_for_index_t {
	//public:
	//	sys_for_index_t(std::vector<T>& vec, std::function<void(int)> func, int step = 1) {
	//		for (i32_t i = 0; i != (i32_t)vec.size(); i += step) {
	//			func(i);
	//		}
	//	}
	//};

	//class sys_for_index {
	//public:
	//	sys_for_index(int start_index, int stop_index, std::function<void(int)> func, int step = 1) {
	//		for (i32_t i = start_index; i != stop_index; i += step) {
	//			func(i);
	//		}
	//	}
	//};

}