#include "stdafx.h"

#include "img_helper.h"

wstring img_helper::get_resource_dir_path() {
	wstring module_path = sys_path::get_module_path();
	wstring::size_type index = module_path.find(L"MATEL\\");

	basiclog_assert2(index != wstring::npos);

	wstring basicimg_root_dir_path = module_path.substr(0, index);

	return basicimg_root_dir_path + L"matel_resource/image_resource//";
}