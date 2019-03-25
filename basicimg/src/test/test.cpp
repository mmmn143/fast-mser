#include "test.h"
#include <iostream>
using namespace std;

int main() {
	sys_exe_config exe_config(new log_ide_console_logger(), sys_tester::Test_Type_Slience);

	vector<wstring> argvs;
	//img_img_test().run(argvs);
	//img_canny_test().run(argvs);
	//img_auto_threshold_canny_test().run(argvs);
	//img_original_mser_test().run(argvs);
	img_mser_test().run(argvs);
	//img_stroke_width_transform_test().run(argvs);
	


	return 0;
}