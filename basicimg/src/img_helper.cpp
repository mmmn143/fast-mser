#include "stdafx.h"

#include "img_helper.h"

string img_helper::get_resource_dir() {
	return sys_path::get_match_resource_dir() + "image_resource/";
}