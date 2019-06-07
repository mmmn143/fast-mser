#include "stdafx.h"

#include "sys_os.h"

#if defined _WIN32
#include <windows.h>
#elif defined
#endif // DEBUG






u64 sys_os::current_thread_id() {
#if defined _WIN32
	return GetCurrentThreadId();
#elif defined
	return 0;
#endif // DEBUG
}