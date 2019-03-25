#pragma once

namespace basicimg {

	class img_canny {
	public:
		img_canny() {
			m_low_threshold = 40;
			m_high_threshold = 100;
			m_apeture_size = 3;
			m_l2_gradient = sys_true;
			m_enable_memory_cache = sys_true;

			m_buffer = NULL;
			m_buffer_size = 0;
		}

		img_canny(i32 low_threshold, i32 high_threshold, b8 enable_memory_cache = sys_true) {
			m_low_threshold = low_threshold;
			m_high_threshold = high_threshold;

			m_apeture_size = 3;
			m_l2_gradient = sys_true;
			m_enable_memory_cache = enable_memory_cache;

			m_buffer = NULL;
			m_buffer_size = 0;
		}

		img_canny(i32 low_threshold, i32 high_threshold, i32 apeture_size, b8 l2_gradient, b8 enable_memory_cache = sys_true) {
			m_low_threshold = low_threshold;
			m_high_threshold = high_threshold;

			m_apeture_size = apeture_size;
			m_l2_gradient = l2_gradient;
			m_enable_memory_cache = enable_memory_cache;
			m_buffer = NULL;
			m_buffer_size = 0;
		}
		
		~img_canny();


		void set_low_threshold(i32 threshold) {
			m_low_threshold = threshold;
		}

		i32 low_threshold() {
			return m_low_threshold;
		}

		void set_high_threshold(i32 threshold) {
			m_high_threshold = threshold;
		}

		i32 high_threshold() {
			return m_high_threshold;
		}

		void set_apeture_size(i32 size) {
			m_apeture_size = size;
		}

		i32 apeture_size() {
			return m_apeture_size;
		}

		void set_l2_gradient(b8 l2_gradient) {
			m_l2_gradient = l2_gradient;
		}

		b8 is_l2_gradient() {
			return m_l2_gradient;
		}

		void enable_memory_cache(b8 enable) {
			m_enable_memory_cache = enable;
		}

		b8 is_enable_memort_cache() {
			return m_enable_memory_cache;
		}

		mt_mat edge_detection(const mt_mat& src, const mt_mat& gradient_x = mt_mat(), const mt_mat& gradient_y = mt_mat(), const mt_mat& gradient_mag = mt_mat());
		void edge_detection(mt_mat& edge, const mt_mat& src, const mt_mat& gradient_x = mt_mat(), const mt_mat& gradient_y = mt_mat(), const mt_mat& gradient_mag = mt_mat());

		mt_mat& gradient_x() {
			return m_gradient_x;
		}

		mt_mat& gradient_y() {
			return m_gradient_y;
		}

		mt_mat& gradient_mag() {
			return m_gradient_mag;
		}

		void clear_memory_cache();

	protected:

		friend class private_auto_threshold_canny_helper;

		void canny_impl(mt_mat& edge);

		i32 m_low_threshold;
		i32 m_high_threshold;

		i32 m_apeture_size;
		i32 m_l2_gradient;

		b8 m_enable_memory_cache;

		mt_mat m_gradient_x;
		b8 m_inner_gradient_x;
		mt_mat m_gradient_y;
		b8 m_inner_gradient_y;

		mt_mat m_gradient_mag;
		b8 m_inner_gradient_mag;

		u8* m_buffer;
		i32 m_buffer_size;

		vector<u8*> m_stack;
	};

}