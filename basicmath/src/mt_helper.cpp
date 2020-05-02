#include "stdafx.h"

static float mt_float_eps = 0.000001f;
static double mt_double_eps = 0.00000000001;

b8 mt_helper::is_nan(f64 val) {
	return _isnan(val) != 0;
}

b8 mt_helper::is_infinity(f64 val) {
	if (is_nan(val)) {
		return sys_false;
	}

	return _finite(val) == 0;
}

b8 mt_helper::is_number(f64 val) {
	return !is_nan(val) && !is_infinity(val);
}

void mt_helper::set_float_eps(float eps) {
	mt_float_eps = eps;
}

float mt_helper::get_float_eps() {
	return mt_float_eps;
}

void mt_helper::set_double_eps(double eps) {
	mt_double_eps = eps;
}

double mt_helper::get_double_eps() {
	return mt_double_eps;
}

int mt_helper::compare_float(float a, float b) {
	float value = a - b;

	if (abs(value) < mt_float_eps) {
		return 0;
	}

	float max_abs = max(abs(a), abs(b));

	if (abs(value) / max_abs < mt_float_eps) {
		return 0;
	}

	if (value > 0) {
		return 1;
	} else {
		return -1;
	}
}

int mt_helper::compare_double(double a, double b) {
	double value = a - b;

	if (abs(value) < mt_double_eps) {
		return 0;
	}

	double max_abs = max(abs(a), abs(b));

	if (abs(value) / max_abs < mt_double_eps) {
		return 0;
	}

	if (value > 0) {
		return 1;
	} else {
		return -1;
	}
}

i32 mt_helper::reverse_i32(i32 val) {
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = val & 255;
	ch2 = (val >> 8) & 255;
	ch3 = (val >> 16) & 255;
	ch4 = (val >> 24) & 255;
	return((i32) ch1 << 24) + ((i32)ch2 << 16) + ((i32)ch3 << 8) + ch4;
}

i32 mt_helper::number_of_1(i32 n) {
	i32 number = 0;

	for (i32 index = 0; index < 32; ++index) {
		if ((n & 1) != 0) {
			number += 1;
		}

		n = n >> 1;
	}

	return number;
}

i32 mt_helper::number_of_1(i64 n) {
	i32 number = 0;

	for (i32 index = 0; index < 64; ++index) {
		if ((n & 1) != 0) {
			number += 1;
		}

		n = n >> 1;
	}

	return number;
}

u32 mt_helper::one_number(u32 value) {
	u32 res = 0;
	for (u32 i = 0; i <= value; ++i) {
		int zhengshu = i;
		int yushu = 0;

		while (zhengshu != 0) {
			yushu = zhengshu % 10;
			zhengshu = zhengshu / 10;

			if (yushu == 1) {
				res += 1;
			}
		}
	}

	return res;
} 

i32 mt_helper::add_five(i32 val) {
	char s[32];
#ifdef _WIN32
	if (val < 0) {
		sprintf_s(s, "%d", -val);
	} else {
		sprintf_s(s, "%d", val);
	}
#else
	if (val < 0) {
		sprintf(s, "%d", -val);
	} else {
		sprintf(s, "%d", val);
	}
#endif

	int len = (i32)strlen(s);
	i32 i = 0;
	for (; i < len; ++i) {
		if (val >= 0) {
			if (s[i] < '5') {
				// stop
				break;
			}
		} else {
			if (s[i] > '5') {
				// stop
				break;
			}
		}
	}

	for (i32 j = len; j >= i; --j) {
		s[j + 1] = s[j];
	}

	s[i] = '5';

	return val < 0 ? -atoi(s) : atoi(s);
}

string mt_helper::max_lexi_order(const string& val) {
	if ((i32)val.size() <= 1) {
		return "";
	}

	string res = val;
	i32 i = 0;

	for (; i < (i32)val.size() - 1; ++i) {
		if (val[i] > val[i + 1]) {
			break;
		}
	}

	res.erase(res.begin() + i);
	return res;
}

string mt_helper::battle_ship(const string& ships, const string& hits) {
	struct ship {

		vector<mt_point> points;
		vector<int> hit_flags;

		void hit(const mt_point& p) {
			for (i32 i = 0; i < (i32)points.size(); ++i) {
				if (points[i] == p) {
					hit_flags[i] = 1;
				}
			}
		}

		b8 is_destory() {
			for (i32 i = 0; i < (i32)hit_flags.size(); ++i) {
				if (hit_flags[i] == 0) {
					return sys_false;
				}
			}

			return sys_true;
		}

		b8 is_hit() {
			for (i32 i = 0; i < (i32)hit_flags.size(); ++i) {
				if (hit_flags[i] == 1) {
					return sys_true;
				}
			}

			return sys_false;
		}
	};

	vector<string> shipItems;

	sys_strhelper::split(shipItems, ships, ";");

	vector<ship> shipData;

	for (i32 i = 0; i < (i32)shipItems.size(); ++i) {
		vector<string> pointItems;
		sys_strhelper::split(pointItems, shipItems[i], ",");

		ship s;

		i32 top = pointItems[0][0] - '1';
		i32 left = pointItems[0][1] - 'A';

		i32 bottom = pointItems[1][0] - '1';
		i32 right = pointItems[1][1] - 'A';

		for (i32 r = top; r <= bottom; ++r) {
			for (i32 c = left; c <= right; ++c) {
				s.points.push_back(mt_point(c, r));
				s.hit_flags.push_back(0);
			}
		}

		shipData.push_back(s);
	}

	vector<string> hitItems;

	sys_strhelper::split(hitItems, hits, ",");

	for (i32 i = 0; i < (i32)hitItems.size(); ++i) {
		i32 y = hitItems[i][0] - '1';
		i32 x = hitItems[i][1] - 'A';

		for (i32 j = 0; j < (i32)shipData.size(); ++j) {
			shipData[j].hit(mt_point(x, y));
		}
	}

	i32 destoryCnt = 0;
	i32 hitCnt = 0;

	for (i32 j = 0; j < (i32)shipData.size(); ++j) {
		if (shipData[j].is_destory()) {
			++destoryCnt;
		} else if (shipData[j].is_hit()) {
			++hitCnt;
		}
	}

	return (sys_strcombine()<<destoryCnt<<","<<hitCnt);
}

void mt_helper::split_array_to_max_mean_sub_arrays(vector<i32>& a, vector<i32>&b , const vector<i32>& input_data) {
	if (input_data.empty()) {
		return;
	}

	vector<i32> data = input_data;
	sort(data.begin(), data.end());

	i32 left_sum = 0;
	f32 max_sub = -FLT_MAX;
	i32 split_index = -1;

	a.resize(data.size());

	i32 right_sum = 0;

	for (i32 i = (i32)data.size() - 1; i >= 0; --i) {
		right_sum += data[i];

		a[i] = right_sum / ((i32)data.size()  - i);
	}

	for (i32 i = 0; i < (i32)data.size() - 1; ++i) {
		left_sum += data[i];

		i32 right_mean = a[i + 1];
		f32 left_mean = left_sum / (f32)(i + 1);

		if (right_mean - left_mean > max_sub) {
			max_sub = right_mean - left_mean;

			split_index = i;
		}
	}

	b.reserve(split_index + 1);

	for (i32 i = 0; i <= split_index; ++i) {
		b.push_back(data[i]);
	}

	a.clear();
	for (i32 i = split_index + 1; i < (i32)data.size(); ++i) {
		a.push_back(data[i]);
	}
}

/**
#pragma omp parallel for num_threads(omp_get_max_threads()) is equivalent to #pragma omp parallel for.
*/
void mt_helper::enable_omp_mkl(b8 enable) {
	if (enable) {
		int sys_suggest_threads = omp_get_num_procs() - 1;
		if (sys_suggest_threads == 0) {
			sys_suggest_threads = 1;
		}

		omp_set_num_threads(sys_suggest_threads);
		omp_set_dynamic(1);

#if defined basicsys_enable_mkl
		mkl_set_num_threads(sys_suggest_threads);
		mkl_set_dynamic(1);
#endif
	} else {
		omp_set_num_threads(1);
#if defined basicsys_enable_mkl
		mkl_set_num_threads(1);
#endif
	}
}

i32 mt_helper::get_number_procs() {
	return omp_get_num_procs();
}