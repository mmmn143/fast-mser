#pragma once





namespace basicmath {

	class mt_hist {
	public:

		static void hist(vector<mt_range_t<f64>>& ranges, vector<i32>& numbers, const vector<f64>& data, i32 hist_bin_number);
	};

}