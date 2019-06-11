#pragma once

namespace basiclog {

	void error_message_box(const wstring& msg, const wstring& path, int line);

	void ide_console_print(const wstring& log_info);
}