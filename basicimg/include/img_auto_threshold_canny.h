#pragma once

namespace basicimg {

	/** For details, see the paper of Scene Text Detection Based on Robust SWT and DBN, Hailiang Xu and Like Xue and Feng Su, in ACCV2014, Singapore.
	*/

	class img_auto_threshold_canny {
	public:
		img_auto_threshold_canny() {
			m_auto_threshold_window_size = 16;	// times of 4
			m_auto_threshold_max = 160;
			m_auto_threshold_min = 40;
			m_auto_threshold_density = 120;
			m_can_not_be_removed_threshold = 100;

			m_low_to_high_ratio = 0.5f;
			m_density_flag_number_to_window_ratio = 0.3f;
			m_density_window_flag_overlap_number_threshold = 3;

			m_auto_threshold_to_mean_ratio = 0.9f;

			m_noise_edge_number_threshold = 3;

			m_canny.set_apeture_size(3);
			m_canny.set_l2_gradient(sys_true);
		}

		void set_auto_threshold_window_size(i32 size) {
			m_auto_threshold_window_size = size;
		}

		void set_auto_threshold_max(i32 threshold) {
			m_auto_threshold_max = threshold;
		}

		void set_auto_threshold_min(i32 threshold) {
			m_auto_threshold_min = threshold;
		}

		void set_auto_threshold_density(i32 threshold) {
			m_auto_threshold_density = threshold;
		}

		void set_low_to_high_ratio(f32 ratio) {
			m_low_to_high_ratio = ratio;
		}

		void set_density_flag_number_to_window_ratio(f32 ratio) {
			m_density_flag_number_to_window_ratio = ratio;
		}

		void set_density_window_flag_overlap_number_threshold(i32 thrrshold) {
			m_density_window_flag_overlap_number_threshold = thrrshold;
		}

		void set_auto_threshold_to_mean_ratio(f32 ratio) {
			m_auto_threshold_to_mean_ratio = ratio;
		}

		void set_m_noise_edge_number_threshold(i32 threshold) {
			m_noise_edge_number_threshold = threshold;
		}
		
		mt_mat edge_detection(const mt_mat& src, const mt_mat& gradient_x = mt_mat(), const mt_mat& gradient_y = mt_mat(), const mt_mat& gradient_mag = mt_mat());
		void edge_detection(mt_mat& edge, const mt_mat& src, const mt_mat& gradient_x = mt_mat(), const mt_mat& gradient_y = mt_mat(), const mt_mat& gradient_mag = mt_mat());

		void clear_memory_cache();

	protected:

		void remove_small_edge(mt_mat& edgeImage, mt_mat& flagImage, const mt_point& pt);

		void trace_edge(mt_mat& edgeImage, f32 lowThreshold, const mt_point& pt);

		img_canny m_canny;

		i32 m_auto_threshold_window_size;
		i32 m_auto_threshold_max;
		i32 m_auto_threshold_min;
		i32 m_auto_threshold_density;
		f64 m_auto_threshold_to_mean_ratio;

		f32 m_can_not_be_removed_threshold;
		i32 m_noise_edge_number_threshold;

		f32 m_density_flag_number_to_window_ratio;
		i32 m_density_window_flag_overlap_number_threshold;

		f32 m_low_to_high_ratio;

		mt_mat m_temp_edge;
		mt_mat m_edge_gradient_integral_sum_image;
		mt_mat m_edge_gradient_integral_number_image;
		mt_mat m_gradient_mean;
		mt_mat m_gradient_count;
		mt_mat m_destiny_flags;

		b8 m_enable_memory_cache;
	};
}

