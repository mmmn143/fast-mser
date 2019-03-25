#pragma once

namespace basicimg {
	/** Implementation of stroke width transform.

	For details, see the paper of Detecting Text in Natural Scenes with Stroke Width Transform, Boris Epshtein and Eyal Ofek and Yonatan Wexler, in CVPR2010.
	*/
	class img_stroke_width_transform {
	public:

		class params {
		public:
			params() 
				: m_pair_intersect_angle(45)
				, m_invalid_stroke_width(mt_I16_Max)
				, m_dark_on_light(sys_true) {

			}

			i32 m_pair_intersect_angle;
			i16 m_invalid_stroke_width;
			b8 m_dark_on_light;
		};

		static mt_mat swt(__in mt_mat& edge, __in mt_mat& gradient_x, __in mt_mat& gradient_y, const params& pars = params());

		static mt_mat normalize_swt_image(mt_mat& swt_image, i16 invalid_stroke_width);

	private:

		class point_memory {
		public:
			point_memory() 
				: m_current_block_size(mt_I32_Max) {

			}

			~point_memory() {
				for (i32 i = 0; i < (i32)m_blocks.size(); ++i) {
					free(m_blocks[i]);
				}
			}

			vector<mt_point*> m_blocks;
			i32 m_current_block_size;
		};

		class swt_ray {
		public:
			swt_ray()
				: m_width(0) {
			}

			i32 m_width;
			mt_point* m_start;
			mt_point* m_stop;
		};

		static void set_ray_width(__in mt_mat& swt_image, const swt_ray& ray);

		static void median_stroke_width(mt_mat& swt_image, vector<swt_ray>& rays, const params& pars);

		static i32 get_next_point(__out mt_point* next_pts, __in i32 cur_x, __in i32 cur_y, __in i32 last_x, __in i32 last_y, __in float gradient_x, __in float gradient_y);	
	};
}
