#include <basiclog.h>
#include <basicsys.h>


using namespace std;
using namespace basiclog;
using namespace basicsys;

static void print() {
	basiclog_debug2(L"debug");
	basiclog_info2(L"info");
}

static void test_file_logger() {
	log_logger::init_logger(new log_file_logger(L""));
	print();
}

static void test_console_logger() {
	log_logger::init_logger(new log_console_logger());
	print();
}

int main(int argc, char **argv) {
	test_file_logger();
	test_console_logger();

	getchar();
}