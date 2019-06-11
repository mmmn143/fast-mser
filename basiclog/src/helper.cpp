#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <string>

using namespace std;

#include "helper.h"

#include <basicsys.h>
using namespace basicsys;

namespace basiclog {

#ifdef _WIN32

	void error_message_box(const wstring& msg, const wstring& path, int line) {
		wstring out_message;

		if (msg.empty()) {
			out_message = sys_strhelper::combine(L"There is an error in file \"%s\", line %d.\n", path.c_str(), line);
		} else {
			out_message = sys_strhelper::combine(L"There is an error: \"%s\", in file \"%s\", line %d.\n", msg.c_str(), path.c_str(), line);
		}

		out_message += L"Would you like to debug(Yes/No)?";
		int res = MessageBox(NULL, out_message.c_str(), L"BasicML error", MB_YESNO | MB_ICONSTOP);

		if (res == IDNO) {
			exit(0);
		} else {
			terminate();
		}
	}

	void ide_console_print(const wstring& log_info) {
		OutputDebugString(log_info.c_str());
	}
#endif
}
