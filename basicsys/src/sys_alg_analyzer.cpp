#include "stdafx.h"

#include "sys_alg_analyzer.h"
#include "sys_thread_lock.h"

map<wstring, i64> sys_alg_analyzer::m_info;
void* sys_alg_analyzer::m_mutex = NULL;

void sys_alg_analyzer::release() {
	if (basiclog::log_logger::get_logger() != NULL) {
		basiclog::log_logger::get_logger()->print(L"sys_alg_analyzer", L"sys_alg_analyzer", sys_strcombine()<<m_info, __FILE__, __LINE__);
	}

	sys_thread_lock::release_mutex(m_mutex);
	m_mutex = NULL;
}

void sys_alg_analyzer::init() {
	m_mutex = sys_thread_lock::create_mutex();
}

void sys_alg_analyzer::set(const wstring& tag, i64 v) {
	if (m_mutex == NULL) {
		return;
	}

	// A mutex operation is not time-consuming. It take about 0.0000781ms at a time. 
	sys_thread_lock lock(m_mutex);

	if (m_info.find(tag) == m_info.end()) {
		m_info.insert(make_pair(tag, v));
	} else {
		m_info.find(tag)->second = v;
	}
}

void sys_alg_analyzer::add(const wstring& tag, i64 v) {
	if (m_mutex == NULL) {
		return;
	}

	sys_thread_lock lock(m_mutex);

	if (m_info.find(tag) == m_info.end()) {
		m_info.insert(make_pair(tag, v));
	} else {
		m_info.find(tag)->second += v;
	}
}