#include "stdafx.h"

#include "sys_thread_lock.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

void* sys_thread_lock::create_mutex() {
#ifdef _WIN32
	void* mutex = new CRITICAL_SECTION();
	InitializeCriticalSection((CRITICAL_SECTION*)mutex);
#else
	void* mutex = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
#endif
	return mutex;
}

void sys_thread_lock::release_mutex(void*& mutex) {
#ifdef _WIN32
	DeleteCriticalSection((CRITICAL_SECTION*)mutex);
	delete (CRITICAL_SECTION*)mutex;
#else
	pthread_mutex_destroy((pthread_mutex_t*)mutex);
	delete (pthread_mutex_t*)mutex;
#endif
	mutex = NULL;
}

sys_thread_lock::sys_thread_lock(void* mutex) {
	m_mutex = mutex;
#ifdef _WIN32
	EnterCriticalSection((CRITICAL_SECTION*)m_mutex);
#else
	pthread_mutex_lock((pthread_mutex_t*)mutex);
#endif
}

sys_thread_lock::~sys_thread_lock() {
#ifdef _WIN32
	LeaveCriticalSection((CRITICAL_SECTION*)m_mutex);
#else
	pthread_mutex_unlock((pthread_mutex_t*)m_mutex);
#endif

	
}