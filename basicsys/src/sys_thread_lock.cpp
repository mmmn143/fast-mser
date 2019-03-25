#include "stdafx.h"

#include "sys_thread_lock.h"

#include <windows.h>

void* sys_thread_lock::create_mutex() {
	void* mutex = new CRITICAL_SECTION();
	InitializeCriticalSection((CRITICAL_SECTION*)mutex);

	return mutex;
}

void sys_thread_lock::release_mutex(void*& mutex) {
	DeleteCriticalSection((CRITICAL_SECTION*)mutex);
	delete (CRITICAL_SECTION*)mutex;
	mutex = NULL;
}

sys_thread_lock::sys_thread_lock(void* mutex) {
	m_mutex = mutex;
	EnterCriticalSection((CRITICAL_SECTION*)m_mutex);
}

sys_thread_lock::~sys_thread_lock() {
	LeaveCriticalSection((CRITICAL_SECTION*)m_mutex);
}