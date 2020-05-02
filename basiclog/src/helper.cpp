#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>



#include <basicsys.h>
using namespace basicsys;

#include "helper.h"

namespace basiclog {

	void error_message_box(const string& msg, const string& path, int line) {
		string out_message;

		if (msg.empty()) {
			out_message = sys_strhelper::combine("There is an error in file \"%s\", line %d.\n", path.c_str(), line);
		} else {
			out_message = sys_strhelper::combine("There is an error: \"%s\", in file \"%s\", line %d.\n", msg.c_str(), path.c_str(), line);
		}

#ifdef _WIN32
		out_message += "Would you like to debug(Yes/No)?";
		int res = MessageBoxA(NULL, out_message.c_str(), "BasicML error", MB_YESNO | MB_ICONSTOP);

		if (res == IDNO) {
			exit(0);
		} else {
			terminate();
		}
#else
#endif
	}

	void ide_console_print(const string& log_info) {
#ifdef _WIN32
		OutputDebugStringA(log_info.c_str());
#else
		printf("%s", log_info.c_str());
#endif
	}
}
