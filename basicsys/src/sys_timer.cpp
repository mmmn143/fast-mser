#include "stdafx.h"

#include "sys_timer.h"

#include "stdafx.h"

#ifdef _WIN32
#include <Windows.h>

static i64 s_detector_freq;

class auto_init_detector {
public:

	auto_init_detector() {
		QueryPerformanceFrequency((LARGE_INTEGER*)&s_detector_freq);
	}
};

static auto_init_detector s_init_helper;

#else
#include <sys/time.h>
#endif



sys_timer::sys_timer(const string& tag, b8 auto_out_cost, b8 debug_out) {
	m_cost = 0;
	m_count = 0;
	m_name = tag;
	m_auto_out_cost = auto_out_cost;
	m_debug_auto_out = debug_out;

#ifdef _WIN32
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
#endif
}

sys_timer::~sys_timer() {
	if (m_auto_out_cost) {
		out(m_debug_auto_out);
	}
}


void sys_timer::begin() {
#ifdef _WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&m_begin);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	m_begin = tv.tv_sec * 1000000 + tv.tv_usec;
#endif
	++m_count;
}

void sys_timer::end() {
	i64 endTimer;
#ifdef _WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&endTimer);
	m_cost += int((endTimer - m_begin) * 1000 / m_freq);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	endTimer = tv.tv_sec * 1000000 + tv.tv_usec;
	m_cost += (endTimer - m_begin) / 1000;
#endif // _WIN32

}

void sys_timer::out(b8 debug_output) {
	string info = sys_strhelper::combine(("%s cost %d ms, count = %d"), m_name.c_str(), m_cost, m_count);

	if (debug_output) {
		basiclog_debug2(info);
	} else {
		basiclog_info2(info);
	}
	
}

int sys_timer::get_cost() const {
	return (int)m_cost;
}


i64 sys_timer::get_tick_cout() {
#ifdef _WIN32
	i64 tick;
	QueryPerformanceCounter((LARGE_INTEGER*)&tick);
	return tick * 1000 / s_detector_freq;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000 + tv.tv_usec) / 1000;
#endif
}