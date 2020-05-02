#pragma once



namespace basicimg {

	class img_img {
	public:

		enum Load_Type {
			Load_Unchanged = -1,
			Load_Grayscale = 0,
			Load_Color = 1,
		};

		static basicmath::mt_mat load(const string& path, Load_Type type = Load_Unchanged);
		static void load(basicmath::mt_mat& img, const string& path, Load_Type type = Load_Unchanged);

		static b8 save(const string& path, const basicmath::mt_mat& mat);

		enum Inter_Type {
			Inter_Type_Nearest = 0,
			Inter_Type_Linear = 1,
			Inter_Type_Cubic = 2,
		};

		static basicmath::mt_mat resize(const basicmath::mt_mat& src, const basicmath::mt_size& dst_size, Inter_Type type = Inter_Type_Cubic);
		static void resize(basicmath::mt_mat& dst, const basicmath::mt_mat& src, const basicmath::mt_size& dst_size, Inter_Type type = Inter_Type_Cubic);

		static cv::Mat from_basiccv(const basicmath::mt_mat& mat);

		enum Color_Covert_Type {
			Color_Covert_Type_BGR2BGRA = 0,
			Color_Covert_Type_BGRA2BGR = 1,

			Color_Covert_Type_BGR2Gray = 6,

			Color_Covert_Type_GRAY2BGRA = 9,
			Color_Covert_Type_BGRA2Gray = 10,
		};

		static mt_mat cvt_color(const mt_mat& src, Color_Covert_Type type);
		static void cvt_color(mt_mat& dst, const mt_mat& src, Color_Covert_Type type);
		static mt_scalar cvt_color(const mt_scalar& src, Color_Covert_Type type);

		static void fusion(mt_mat& res, const mt_mat& forground, const mt_mat& background, const mt_mat& alpha_mask);

		static mt_mat sobel(const mt_mat& src, mt_Depth dst_depth, i32 horizontal_order, i32 vertical_order, i32 kernel_size);
		static void sobel(mt_mat& dst, const mt_mat& src, mt_Depth dst_depth, i32 horizontal_order, i32 vertical_order, i32 kernel_size);

		enum Gradient_Mag_Type {
			Gradient_Mag_Type_L1,
			Gradient_Mag_Type_L2,
			Gradient_Mag_Type_L2_Square,
		};

		static mt_mat generate_gradient_mag(const mt_mat& gradient_x, const mt_mat& gradient_y, Gradient_Mag_Type gradient_mag_type);
		static void generate_gradient_mag(mt_mat& dst, const mt_mat& gradient_x, const mt_mat& gradient_y, Gradient_Mag_Type gradient_mag_type);

		static b8 point_in_image_boundary(const mt_point& pt, const mt_mat& img) {
			return point_in_image(pt, img) && (pt.m_x == 0 || pt.m_y == 0 || pt.m_x == img.size()[1] - 1 || pt.m_y == img.size()[0] - 1);
		}

		static b8 point_in_image(const mt_point& pt, const mt_mat& img) {
			return pt.m_x >= 0 && pt.m_y >= 0 && pt.m_x < img.size()[1] && pt.m_y < img.size()[0];
		}

		static i32 image_pixel_offset(i32 row_step, i32 pixel_size, i32 dx, i32 dy) {
			return dx + dy * row_step / pixel_size;
		}

		static void gaussian_blur(mt_mat& res, const mt_mat& src, const mt_size& kernel_size, f64 sigma_x, f64 sigma_y);
		static mt_mat gaussian_blur(const mt_mat& src, const mt_size& kernel_size, f64 sigma_x, f64 sigma_y);

		static void genarate_chessboard(mt_mat& res, const mt_size& image_size, i32 chess_size, i32 channel = 1);

	protected:

		static i32 depth_from_basiccv(mt_Depth depth);
		static mt_Depth depth_from_opencv(i32 depth); 

		friend class img_draw;
		

		static basicmath::mt_mat use_opencv_memory(cv::Mat& mat);
		static void generate_gaussian_kernel(mt_mat& kernel, i32 size, f64 sigma);
	};

}