#include "stdafx.h"

#include "sys_timer.h"

#include "stdafx.h"

#include <Windows.h>

static i64 s_detector_freq;

class auto_init_detector {
public:

	auto_init_detector() {
		QueryPerformanceFrequency((LARGE_INTEGER*)&s_detector_freq);
	}
};

static auto_init_detector s_init_helper;

sys_timer::sys_timer(const wstring& tag, b8 auto_out_cost, b8 debug_out) {
	m_cost = 0;
	m_count = 0;
	m_name = tag;
	m_auto_out_cost = auto_out_cost;
	m_debug_auto_out = debug_out;

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
}

sys_timer::~sys_timer() {
	if (m_auto_out_cost) {
		out(m_debug_auto_out);
	}
}


void sys_timer::begin() {
	QueryPerformanceCounter((LARGE_INTEGER*)&m_begin);
	++m_count;
}

void sys_timer::end() {
	__int64 endTimer;
	QueryPerformanceCounter((LARGE_INTEGER*)&endTimer);
	m_cost += int((endTimer - m_begin) * 1000 / m_freq);
}

void sys_timer::out(b8 debug_output) {
	wstring info = sys_strhelper::combine(TEXT("%s cost %d ms, count = %d"), m_name.c_str(), m_cost, m_count);

	if (debug_output) {
		basiclog_debug2(info);
	} else {
		basiclog_info2(info);
	}
	
}

int sys_timer::get_cost() const {
	return (int)m_cost;
}


__int64 sys_timer::get_tick_cout() {
	__int64 tick;
	QueryPerformanceCounter((LARGE_INTEGER*)&tick);
	return tick * 1000 / s_detector_freq;
}