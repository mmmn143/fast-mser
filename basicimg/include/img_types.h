#pragma once



namespace basicimg {

	static const i32 img_DX_3_9[9] = {0, 1, 1, 0, -1, -1, -1, 0, 1};
	static const i32 img_DY_3_9[9] = {0, 0, -1, -1, -1, 0, 1, 1, 1};
	static const i32 img_DX_3_5[5] = {0, 1, 0, -1, 0};
	static const i32 img_DY_3_5[5] = {0, 0, -1, 0, 1};
	static const i32 img_DX_5_25[25] = {0, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2, -2, -1, 1, 2, -2, -1, 0, 1, 2, -2, -1, 0, 1, 2};
	static const i32 img_DY_5_25[25] = {0, -2, -2, -2, -2, -2, -1, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2};
	static const i32 img_DX_5_9[9] = {0, -2, -1, 0, 0, 0, 0, 1, 2};
	static const i32 img_DY_5_9[9] = {0, 0, 0, -2, -1, 1, 2, 0, 0};


	const static mt_scalar img_Color_White(255, 255, 255, 255);
	const static mt_scalar img_Color_Black(0, 0, 0, 255);
	const static mt_scalar img_Color_Green(0, 255, 0, 255);
	const static mt_scalar img_Color_Red(0, 0, 255, 255);
	const static mt_scalar img_Color_Gray(128, 128, 128, 255);
	const static mt_scalar img_Color_Yellow(0, 255, 255, 255);
}