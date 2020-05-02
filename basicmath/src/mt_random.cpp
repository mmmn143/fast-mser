#include "stdafx.h"

#include "mt_random.h"


void mt_random::set_seed(u32 seed) {
	srand(seed);
}

i32 mt_random::random_next_i32(i32 min, i32 max) {
	f64 value = rand() / (f64)(RAND_MAX + 1);	//[0,1)
	return (i32)(value * (max - min + 1)) + min;
}

f64 mt_random::random_uniform(f64 min, b8 min_opened, f64 max, b8 max_opened) {
	if (min_opened) {
		min += DBL_EPSILON;
	}

	if (max_opened) {
		max -= DBL_EPSILON;
	}

	return rand() / (f64)RAND_MAX * (max - min) + min;
}

mt_mat mt_random::random_uniform_iid(i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 min /* = 0.0 */, b8 min_opened /* = sys_true */, f64 max /* = 1.0 */, b8 max_opened /* = sys_true */) {
	i32 sizes[] = {rows, cols};
	return random_uniform_iid(2, sizes, depth_channel, min, min_opened, max, max_opened);
}

mt_mat mt_random::random_uniform_iid(i32 planes, i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 min /* = 0.0 */, b8 min_opened /* = sys_true */, f64 max /* = 1.0 */, b8 max_opened /* = sys_true */) {
	i32 sizes[] = {planes, rows, cols};

	return random_uniform_iid(3, sizes, depth_channel, min, min_opened, max, max_opened);
}

mt_mat mt_random::random_uniform_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 min, b8 min_opened, f64 max, b8 max_opened) {
	mt_mat mat(dims, sizes, depth_channel);

	mt_array_element_iterator iter(mat);

	basicmath_mat_request_memory(f64, random_values, mat.channel());

	for (;;) {
		u8* ptr_data = iter.data();

		if (ptr_data == NULL) {
			break;
		}

		for (i32 i = 0; i < mat.channel(); ++i) {
			random_values[i] = mt_random::random_uniform(min, min_opened, max, max_opened);
		}

		mat.set(random_values, dims, &iter.position()[0]);
	}

	basicmath_mat_release(random_values);

	return mat;
}

mt_mat mt_random::random_zero_center_uniform_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 std_variance) {
	f64 max_value = 2 * sqrt(3) * std_variance / 2;
	f64 min_value = -max_value;

	return mt_random::random_uniform_iid(dims, sizes, depth_channel, min_value, sys_false, max_value, sys_false);
}

void mt_random::randperm(vector<i32>& results, i32 size) {
	results.resize(size, 0);

	for (i32 i = 0; i < (i32)results.size(); ++i) {
		results[i] = i;
	}

	random_shuffle(results.begin(), results.end());
}

void mt_random::randSample(i32 size, vector<i32>& results) {
	results.resize(size, 0);

	for (i32 i = 0; i < (i32)results.size(); ++i) {
		results[i] = random_next_i32(0, size - 1);
	}
}


i32 mt_random::bernoulli(i32 n, f64 p) {
	vector<f64> probabilitys;
	probabilitys.resize(n + 1);

	for (i32 k = 0; k <= n; ++k) {
		probabilitys[k] = mt_helper::combination(n, k) * pow(p, k) * pow(1 - p, n - k);
	}

	for (i32 k = 1; k < n; ++k) {
		probabilitys[k] += probabilitys[k - 1];
	}

	probabilitys[n] = DBL_MAX;

	f64 random_p = mt_random::random_uniform(0, sys_true, 1, sys_true);

	for (i32 k = 0; k <= n; ++k) {
		if (probabilitys[k] >= random_p) {
			return k;
		}
	}

	basiclog_assert2(false);
	return 0;
}

mt_mat mt_random::bernoulli_iid(i32 rows, i32 cols, mt_Depth_Channel depth_channel, i32 n, f64 p) {
	i32 sizes[] = {rows, cols};

	return bernoulli_iid(2, sizes, depth_channel, n, p);
}

mt_mat mt_random::bernoulli_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, i32 n, f64 p) {	
	mt_mat mat(dims, sizes, depth_channel);

	vector<f64> probabilitys;
	probabilitys.resize(n + 1);

	for (i32 k = 0; k <= n; ++k) {
		probabilitys[k] = mt_helper::combination(n, k) * pow(p, k) * pow(1 - p, n - k);
	}

	for (i32 k = 1; k < n; ++k) {
		probabilitys[k] += probabilitys[k - 1];
	}

	probabilitys[n] = DBL_MAX;

	mt_array_element_iterator iter(mat);

	basicmath_mat_request_memory(f64, random_values, mat.channel());

	for (;;) {
		u8* ptr_data = iter.data();

		if (ptr_data == NULL) {
			break;
		}

		for (i32 i = 0; i < mat.channel(); ++i) {
			f64 random_p = mt_random::random_uniform(0, sys_true, 1, sys_true);

			for (i32 k = 0; k <= n; ++k) {
				if (probabilitys[k] >= random_p) {
					random_values[i] = k;
					break;
				}
			}
		}

		mat.set(random_values, dims, &iter.position()[0]);
	}

	return mat;
}

f64 mt_random::gaussian(f64 mean, f64 standard_deviation) {
	f64 random_value_1 = mt_random::random_uniform(0, sys_true, 1, sys_true);
	f64 random_value_2 = mt_random::random_uniform(0, sys_true, 1, sys_true);

	f64 norm_variable = sqrt(-2 * log(1 - random_value_1)) * cos(2 * mt_PI * random_value_2);

	return norm_variable * standard_deviation + mean;
}

mt_mat mt_random::gaussian_iid(i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation) {
	i32 sizes[] = {rows, cols};

	return gaussian_iid(2, sizes, depth_channel, mean, standard_deviation);
}

mt_mat mt_random::gaussian_iid(i32 planes, i32 rows, i32 cols, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation) {
	i32 sizes[] = {planes, rows, cols};

	return gaussian_iid(3, sizes, depth_channel, mean, standard_deviation);
}

mt_mat mt_random::gaussian_iid(i32 dims, const i32* sizes, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation) {
	mt_mat mat(dims, sizes, depth_channel);

	if (mt_helper::compare_double(standard_deviation, 0) == 0) {
		mat.set(mean);
	} else {
		mt_array_element_iterator iter(mat);

		basicmath_mat_request_memory(f64, random_values, mat.channel());

		for (;;) {
			u8* ptr_data = iter.data();

			if (ptr_data == NULL) {
				break;
			}

			for (i32 i = 0; i < mat.channel(); ++i) {
				random_values[i] = gaussian(mean, standard_deviation);
			}

			mat.set(random_values, dims, &iter.position()[0]);
		}

		basicmath_mat_release(random_values);
	}

	return mat;
}

mt_mat mt_random::gaussian_iid(const vector<i32>& sizes, mt_Depth_Channel depth_channel, f64 mean, f64 standard_deviation) {
	return gaussian_iid((i32)sizes.size(), &sizes[0], depth_channel, mean, standard_deviation);
}

void mt_random::gaussian_iid(vector<mt_mat>& reses, i32 drawn_number, const mt_mat& mean, const mt_mat& variance) {
	reses.resize(drawn_number);

	for (i32 i = 0; i < drawn_number; ++i) {
		reses[i] = mt_mat(mean, mt_mat::Construct_Type_Create_As_Size);
	}

	mt_array_element_const_iterator mean_iter(mean);

	basicmath_mat_request_memory(f64, mean_values, mean.channel());
	basicmath_mat_request_memory(f64, variance_values, variance.channel());
	basicmath_mat_request_memory(f64, drawn_values, variance.channel());

	for (;;) {
		const u8* ptr_mean = mean_iter.data();

		if (ptr_mean == NULL) {
			break;
		}

		mean.get(mean_values, mean.dim(), &mean_iter.position()[0]);
		variance.get(variance_values, variance.dim(), &mean_iter.position()[0]);

		for (i32 iter_drawn_number = 0; iter_drawn_number < drawn_number; ++iter_drawn_number) {
			for (i32 c = 0; c < mean.channel(); ++c) {
				drawn_values[c] = gaussian(mean_values[c], variance_values[c]);
			}
		
			reses[iter_drawn_number].set(drawn_values, reses[iter_drawn_number].dim(), &mean_iter.position()[0]);
		}
	}

	basicmath_mat_release(mean_values);
	basicmath_mat_release(variance_values);
	basicmath_mat_release(drawn_values);
}

void mt_random::gaussian_joint(mt_mat& dst, mt_Depth_Channel data_type, i32 sample_number, const mt_mat& mean, const mt_mat& covariance) {
	mt_mat eigen_value;
	mt_mat eigen_vector;

	//ml_mat_op::eigen_real_symmetric(eigen_value, eigen_vector, covariance);
	//dst.create(sample_number, mean.cols, data_type);

	//for (i32 row = 0; row < sample_number; ++row) {
	//	f64 random_value = mt_random::gaussian(0, 1);

	//	for (i32 col = 0; col < mean.cols; ++col) {
	//		ml_mat_op::set(dst.row(row), 0, col, Scalar(random_value * ml_mat_op::get(eigen_value, 0, col).val[0]));
	//	}

	//	ml_mat_op::cpu_mul(dst.row(row), dst.row(row), eigen_vector, false, sys_true);

	//	dst.row(row) += mean;
	//}
}