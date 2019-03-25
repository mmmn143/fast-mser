#include "stdafx.h"
#include "img_img.h"

namespace basicimg {
	class private_img_img {
	public:
	};


}


mt_mat img_img::load(const wstring& path, Load_Type type) {
	mt_mat img;
	load(img, path, type);

	return img;
}

void img_img::load(basicmath::mt_mat& img, const wstring& path, Load_Type type /* = Load_Unchanged */) {
	Mat cv_img;
	imread(cv_img, basicsys_path_string(path), (i32)type);

	if (cv_img.empty()) {
		basiclog_warning2(sys_strcombine()<<L"load "<<path<<L" failed!");
	}

	img = use_opencv_memory(cv_img);
}

b8 img_img::save(const wstring& path, const mt_mat& mat) {
	return imwrite(basicsys_path_string(path), from_basiccv(mat)) ? sys_true : sys_false;
}

//mt_mat img_img::from_opencv(const Mat& mat) {
//	if (mat.empty()) {
//		return mt_mat();
//	}
//
//	i32 channel = mat.channels();
//
//	mt_Depth depth = depth_from_opencv(mat.depth());
//
//	mt_Depth_Channel depth_channel = mt_Depth_Channel(depth, channel);
//
//	if (mat.step.p != NULL) {
//		return mt_mat(mat.dims, mat.size.p, depth_channel, mat.data, NULL).clone();
//	} else {
//		basicmath_mat_request_memory(i32, steps, mat.dims);
//
//		for (i32 i = 0; i < mat.dims; ++i) {
//			steps[i] = (i32)mat.step.p[i];
//		}
//
//		mt_mat res = mt_mat(mat.dims, mat.size.p, depth_channel, mat.data, steps).clone();
//
//		basicmath_mat_release(steps);
//
//		return res;
//	}
//}

Mat img_img::from_basiccv(const mt_mat& mat) {
	if (mat.is_empty()) {
		return Mat();
	}

	i32 channel = mat.channel();
	i32 depth = depth_from_basiccv(mat.depth());

	i32 depth_channel = CV_MAKETYPE(depth, channel);

	if (mat.is_step_positive() && mat.is_min_abs_step_equal_element_size()) {
		basicmath_mat_request_memory(u64, steps, mat.dim());

		for (i32 i = 0; i < mat.dim(); ++i) {
			steps[i] = (u64)mat.step()[i];
		}

		Mat res(mat.dim(), mat.size(), depth_channel, (void*)mat.data(), steps);

		basicmath_mat_release(steps);

		return res;
	} else {
		basiclog_warning(basiclog_performance_warning, L"this will reduce the performance, please input the mat with positive steps and min abs step to be the channel size!");
		return img_img::from_basiccv(mat.clone());
	}
}

basicmath::mt_mat img_img::use_opencv_memory(cv::Mat& mat) {
	if (mat.empty()) {
		return mt_mat();
	}

	mat.u->flags |= UMatData::USER_ALLOCATED;

	//Note that the start memory pointer of opencv Mat is udata defined as followed, not the mat.u->data and mat.u->origdata and mat.data 
	uchar* udata = ((uchar**)mat.u->origdata)[-1];

	i32 channel = mat.channels();

	mt_Depth depth = depth_from_opencv(mat.depth());

	mt_Depth_Channel depth_channel = mt_Depth_Channel(depth, channel);

	if (mat.step.p != NULL) {
		return mt_mat(mat.dims, mat.size.p, depth_channel, mat.data, NULL, udata);
	} else {
		basicmath_mat_request_memory(i32, steps, mat.dims);

		for (i32 i = 0; i < mat.dims; ++i) {
			steps[i] = (i32)mat.step.p[i];
		}

		mt_mat res = mt_mat(mat.dims, mat.size.p, depth_channel, mat.data, steps, udata);

		basicmath_mat_release(steps);

		return res;
	}
}

basicmath::mt_mat img_img::resize(const basicmath::mt_mat& src, const mt_size& dst_size, Inter_Type type /* = Inter_Type_Cubic */) {
	mt_mat dst;
	resize(dst, src, dst_size, type);

	return dst;
}

void img_img::resize(basicmath::mt_mat& dst, const basicmath::mt_mat& src, const mt_size& dst_size, Inter_Type type /* = Inter_Type_Cubic */) {
	if (dst.is_memory_shared(src)) {
		mt_mat temp;
		resize(temp, src, dst_size, type);
		dst = temp;
	} else {
		Mat cv_dst_mat = from_basiccv(dst);
		Mat cv_src_mat = from_basiccv(src);

		cv::resize(cv_src_mat, cv_dst_mat, Size(dst_size.m_width, dst_size.m_height), 0, 0, (i32)type);

		if (dst.data() != cv_dst_mat.data) {
			dst = use_opencv_memory(cv_dst_mat);
		}
	}
}

mt_mat img_img::cvt_color(const mt_mat& src, Color_Covert_Type type) {
	mt_mat res;
	cvt_color(res, src, type);

	return res;
}

void img_img::cvt_color(mt_mat& dst, const mt_mat& src, Color_Covert_Type type) {
	i32 dst_channel;

	switch (type) {
	case img_img::Color_Covert_Type_BGR2BGRA:
		dst_channel = 4;
		break;
	case img_img::Color_Covert_Type_BGRA2BGR:
		dst_channel = 3;
		break;
	case img_img::Color_Covert_Type_GRAY2BGRA:
		dst_channel = 4;
		break;
	case img_img::Color_Covert_Type_BGR2Gray:
	case img_img::Color_Covert_Type_BGRA2Gray:
		dst_channel = 1;
		break;
	}

	Mat src_mat = from_basiccv(src);
	Mat dst_mat = from_basiccv(dst);

	cvtColor(src_mat, dst_mat, type);

	if (dst_mat.data != dst.data()) {
		dst = use_opencv_memory(dst_mat);
	}
}

mt_scalar img_img::cvt_color(const mt_scalar& src, Color_Covert_Type type) {
	i32 src_channel;

	switch (type) {
	case img_img::Color_Covert_Type_BGR2BGRA:
		src_channel = 3;
		break;
	case img_img::Color_Covert_Type_BGRA2BGR:
		src_channel = 4;
		break;
	case img_img::Color_Covert_Type_GRAY2BGRA:
		src_channel = 1;
		break;
	case img_img::Color_Covert_Type_BGR2Gray:
	case img_img::Color_Covert_Type_BGRA2Gray:
		src_channel = 4;
		break;
	}

	Mat temp_buffer(1, 1, CV_8UC(src_channel), Scalar(src.value[0], src.value[1], src.value[2], src.value[3]));
	cvtColor(temp_buffer, temp_buffer, type);

	u8* ptr_data = temp_buffer.data;

	return mt_scalar(ptr_data[0], ptr_data[1], ptr_data[2], ptr_data[3]);
}

mt_mat img_img::sobel(const mt_mat& src, mt_Depth dst_depth, i32 horizontal_order, i32 vertical_order, i32 kernel_size) {
	mt_mat dst;
	sobel(dst, src, dst_depth, horizontal_order, vertical_order, kernel_size);

	return dst;
}

void img_img::sobel(mt_mat& dst, const mt_mat& src, mt_Depth dst_depth, i32 horizontal_order, i32 vertical_order, i32 kernel_size) {
	Mat res_cv_mat = img_img::from_basiccv(dst);
	Mat src_cv_mat = img_img::from_basiccv(src);

	cv::Sobel(src_cv_mat, res_cv_mat, depth_from_basiccv(dst_depth), horizontal_order, vertical_order, kernel_size);

	if (dst.data() != res_cv_mat.data) {
		dst = use_opencv_memory(res_cv_mat);
	}
}

mt_mat img_img::generate_gradient_mag(const mt_mat& gradient_x, const mt_mat& gradient_y, Gradient_Mag_Type gradient_mag_type) {
	mt_mat dst;
	generate_gradient_mag(dst, gradient_x, gradient_y, gradient_mag_type);

	return dst;
}

void img_img::generate_gradient_mag(mt_mat& dst, const mt_mat& gradient_x, const mt_mat& gradient_y, Gradient_Mag_Type gradient_mag_type) {
	//TODO. 
	//optimize memory
	
	dst.copy_from(gradient_x);
	mt_mat y_helper;
	y_helper.copy_from(gradient_y);

	switch (gradient_mag_type) {
	case img_img::Gradient_Mag_Type_L1:
		
		dst.self_abs();
		
		y_helper.self_abs();
		dst += y_helper;
		break;
	case img_img::Gradient_Mag_Type_L2:

		dst.self_pow(2.0);
		y_helper.self_pow(2.0);

		dst += y_helper;
		dst.self_pow(0.5);
		break;
	case img_img::Gradient_Mag_Type_L2_Square:
		dst.self_pow(2.0);
		y_helper.self_pow(2.0);

		dst += y_helper;
		break;
	default:
		basiclog_assert2(sys_false);
		break;
	}
}

mt_Depth img_img::depth_from_opencv(i32 depth) {
	switch (depth) {
	case CV_8U:
		return mt_U8;
	case CV_8S:
		return mt_I8;
	case CV_16U:
		return mt_U16;
	case CV_16S:
		return mt_I16;
	case CV_32S:
		return mt_I32;
	case CV_32F:
		return mt_F32;
	case CV_64F:
		return mt_F64;
	default:
		basiclog_assert2(sys_false);
		return mt_User;
	}
}

i32 img_img::depth_from_basiccv(mt_Depth depth) {
	switch (depth) {
	case mt_U8:
		return CV_8U;
	case mt_I8:
		return CV_8S;
	case mt_U16:
		return CV_16U;
	case mt_I16:
		return CV_16S;
	case mt_I32:
		return CV_32S;
	case mt_F32:
		return CV_32F;
	case mt_F64:
		return CV_64F;
	default:
		basiclog_assert2(sys_false);
		return CV_USRTYPE1;
	}
}