#include "stdafx.h"


#include "mt_hist.h"



void mt_hist::hist(vector<mt_range_t<f64>>& ranges, vector<i32>& numbers, const vector<f64>& data, i32 hist_bin_number) {
	f64 max_value = mt_helper::compute_max<f64>((i32)data.size(), &data[0]);
	max_value += max_value / 100;

	f64 min_value = mt_helper::compute_min<f64>((i32)data.size(), &data[0]);

	f64 gap = (max_value - min_value) / hist_bin_number;

	ranges.resize(hist_bin_number);

	f64 base_value = min_value;

	for (i32 i = 0; i < hist_bin_number; ++i) {
		ranges[i].m_start = base_value;
		ranges[i].m_end = ranges[i].m_start + gap;
		base_value = ranges[i].m_end;
	}

	for (i32 i = 0; i < (i32)data.size(); ++i) {
		for (i32 j = 0; j < (i32)ranges.size(); ++j) {
			if (data[i] >= ranges[j].m_start && data[i] < ranges[j].m_end) {
				++numbers[j];
			}
		}
	}
}