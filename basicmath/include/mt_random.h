#pragma once


namespace basicmath {

	class mt_random {
	public:



		static void set_seed(u32 seed);

		//[]
		static i32 random_next_i32(i32 min, i32 max);

		static f64 random_uniform(f64 min = 0.0, b8 min_opened = sys_true, f64 max = 1.0, b8 max_opened = sys_true);
		
		static mt_mat random_uniform_iid(i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 min = 0.0, b8 min_opened = sys_true, f64 max = 1.0, b8 max_opened = sys_true);
		static mt_mat random_uniform_iid(i32 planes, i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 min = 0.0, b8 min_opened = sys_true, f64 max = 1.0, b8 max_opened = sys_true);
		
		static mt_mat random_uniform_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 min = 0.0, b8 min_opened = sys_true, f64 max = 1.0, b8 max_opened = sys_true);

		static mt_mat random_zero_center_uniform_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 std_variance);

		static void randperm(vector<i32>& results, i32 size);

		static void randSample(i32 size, vector<i32>& results);

		static i32 bernoulli(i32 n, f64 p);
		static mt_mat bernoulli_iid(i32 rows, i32 cols, mt_Depth_Channel data_type, i32 n, f64 p);

		static mt_mat bernoulli_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, i32 n, f64 p);

		static f64 gaussian(f64 mean, f64 standard_deviation);

		static mt_mat gaussian_iid(i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation);

		static mt_mat gaussian_iid(i32 planes, i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation);

		static mt_mat gaussian_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation);
		static mt_mat gaussian_iid(const vector<i32>& sizes, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation);

		static void gaussian_iid(vector<mt_mat>& reses, i32 drawn_number, const mt_mat& mean, const mt_mat& variance);

		static void gaussian_joint(mt_mat& dst, mt_Depth_Channel depth_channel, i32 sample_number, const mt_mat& mean, const mt_mat& covariance);
	};

}