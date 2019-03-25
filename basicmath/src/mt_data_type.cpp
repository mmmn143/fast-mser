#include "stdafx.h"


wstring mt_Depth_Channel::depth_str(mt_Depth depth) {
	switch (depth) {
	case mt_U8:
		return wstring(L"mt_U8");
	case mt_I8:
		return wstring(L"mt_I8");
	case mt_U16:
		return wstring(L"mt_U16");
	case mt_I16:
		return wstring(L"mt_I16");
	case mt_U32:
		return wstring(L"mt_U32");
	case mt_I32:
		return wstring(L"mt_I32");
	case mt_U64:
		return wstring(L"mt_U64");
	case mt_I64:
		return wstring(L"mt_I64");
	case mt_F32:
		return wstring(L"mt_F32");
	case mt_F64:
		return wstring(L"mt_F64");
	case mt_User:
		return wstring(L"mt_User");
	}

	basiclog_assert2(false);
	return wstring(L"mt_Unknow");
}

mt_Depth mt_Depth_Channel::depth_i32(const wstring& depth) {
	if (depth == L"mt_U8") {
		return mt_U8;
	} else if (depth == L"mt_I8") {
		return mt_I8;
	} else if (depth == L"mt_U16") {
		return mt_U16;
	} else if (depth == L"mt_I16") {
		return mt_I16;
	} else if (depth == L"mt_I32") {
		return mt_I32;
	} else if (depth == L"mt_U32") {
		return mt_U32;
	} else if (depth == L"mt_I64") {
		return mt_I64;
	} else if (depth == L"mt_U64") {
		return mt_U64;
	} else if (depth == L"mt_F32") {
		return mt_F32;
	} else if (depth == L"mt_F64") {
		return mt_F64;
	}

	return mt_User;
}

wstring mt_Depth_Channel::depth_channel_str(mt_Depth_Channel depth_channel) {
	return sys_strcombine()<<depth_str(depth_channel.depth())<<L"C"<<depth_channel.channel();
}

mt_Depth_Channel mt_Depth_Channel::depth_channel_i32(const wstring& depth_channel) {
	vector<wstring> elements;
	sys_strhelper::split(elements, depth_channel, L"C");

	i32 channel = 1;
	mt_Depth depth = depth_i32(elements[0]);

	if ((i32)elements.size() > 1) {
		channel = _wtoi(elements[1].c_str());
	}

	return mt_Depth_Channel(depth, channel);
}
