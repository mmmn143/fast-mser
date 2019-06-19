#include "stdafx.h"


namespace basicmath {
	class private_multiplication {
	public:

		static void mul(mt_mat& res, const mt_mat& a, const mt_mat& b) {
			basiclog_assert2(a.channel() == 1);

			if (a.is_step_positive() 
				&& b.is_step_positive()
				&& a.is_min_abs_step_equal_element_size()
				&& b.is_min_abs_step_equal_element_size()) {
					i32 row_a = a.size()[0];
					i32 col_a = a.size()[1];

					i32 row_b = b.size()[0];
					i32 col_b = b.size()[1];

					basiclog_assert2(col_a == row_b);

					const u8* ptr_cur_data = a.data();
					const u8* ptr_other_data = b.data();
					const u8* ptr_res_data = res.data();

					b8 cur_transpose = sys_false;
					b8 other_transpose = sys_false;

					i32 cur_ld = (a.step()[0] / a.element_size());
					i32 other_ld = (b.step()[0] / b.element_size());
					i32 res_ld = (res.step()[0] / res.element_size());

					if (a.step()[0] < a.step()[1]) {
						cur_transpose = sys_true;
						cur_ld = (a.step()[1] / a.element_size());
					}

					if (b.step()[0] < b.step()[1]) {
						other_transpose = sys_true;
						other_ld = (b.step()[1] / b.element_size());
					}

					if (mt_F32 == res.depth()) {
						basicmath_sgemm(cur_transpose, other_transpose, row_a, col_b, col_a, (const f32*)ptr_cur_data, cur_ld, (const f32*)ptr_other_data, other_ld, (f32*)ptr_res_data, res_ld);
					} else if (mt_F64 == res.depth()) {
						basicmath_dgemm(cur_transpose, other_transpose, row_a, col_b, col_a, (const f64*)ptr_cur_data, cur_ld, (const f64*)ptr_other_data, other_ld, (f64*)ptr_res_data, res_ld);
					}
			} else {
				mt_mat temp_cur = a;
				if (a.is_step_negative()) {
					basiclog_warning(basiclog_performance_warning, L"the step of current mat has negative values, this will reduce the performance, you should better input a mat with all positive steps!");
					temp_cur = a.clone();
				} else if (!a.is_min_abs_step_equal_element_size()) {
					basiclog_warning(basiclog_performance_warning, L"this mat is result of the channel_at() on a mat with more than 1 channel, this will reduce the performance!");
					temp_cur = a.clone();
				}

				mt_mat temp_value = b;
				if (b.is_step_negative()) {
					basiclog_warning(basiclog_performance_warning, L"the step of other mat has negative values, this will reduce the performance, you should better input a mat with all positive steps!");
					temp_value = b.clone();
				} else if (!b.is_min_abs_step_equal_element_size()) {
					basiclog_warning(basiclog_performance_warning, L"other mat is result of the channel_at() on a mat with more than 1 channel, this will reduce the performance!");
					temp_value = b.clone();
				}

				mul(res, temp_cur, temp_value);
			}
		}



	private:

		static void basicmath_sgemm(b8 trans_a, b8 trans_b, i32 m, i32 n, i32 k, const f32* a, i32 lda, const f32* b, i32 ldb, f32* c, i32 ldc) {
#if defined basicsys_enable_mkl
			cblas_sgemm(CblasRowMajor, trans_a ? CblasTrans : CblasNoTrans, trans_b ? CblasTrans : CblasNoTrans, m, n, k, 1.0f, a, lda, b, ldb, 0.0f, c, ldc);
#else
			sgemm_impl(trans_a, trans_b, m, n, k, a, lda, b, ldb, c, ldc);
#endif
		}

		static void basicmath_dgemm(b8 trans_a, b8 trans_b, i32 m, i32 n, i32 k, const f64* a, i32 lda, const f64* b, i32 ldb, f64* c, i32 ldc) {
#if defined basicsys_enable_mkl
			cblas_dgemm(CblasRowMajor, trans_a ? CblasTrans : CblasNoTrans, trans_b ? CblasTrans : CblasNoTrans, m, n, k, 1.0f, a, lda, b, ldb, 0.0f, c, ldc);
#else
			dgemm_impl(trans_a, trans_b, m, n, k, a, lda, b, ldb, c, ldc);
#endif
		}

		static void sgemm_impl(b8 trans_a, b8 trans_b, i32 m, i32 n, i32 k, const f32* a, i32 lda, const f32* b, i32 ldb, f32* c, i32 ldc) {
			basiclog_unsupport2();
		}

		static void dgemm_impl(b8 trans_a, b8 trans_b, i32 m, i32 n, i32 k, const f64* a, i32 lda, const f64* b, i32 ldb, f64* c, i32 ldc) {
			basiclog_unsupport2();
		}
	};


}

mt_mat mt_mat::mul(const mt_mat& value) const {
	basiclog_assert2(depth_channel() == value.depth_channel());
	basiclog_assert2(depth() == mt_F32 || depth() == mt_F64);
	basiclog_assert_message2(size()[1] == value.size()[0], L"matrix a * b mul must be satisfied the rule of a.col = b.row!");

	mt_mat res(size()[0], value.size()[1], depth_channel());

	if (channel() == 1) {
		private_multiplication::mul(res, *this, value);
	} else {
		mt_mat res_channel(size()[0], value.size()[1], mt_Depth_Channel(depth(), 1));
		mt_mat src_channel(size()[0], size()[1], mt_Depth_Channel(depth(), 1));
		mt_mat value_channel(value.size()[0], value.size()[1], mt_Depth_Channel(depth(), 1));

		for (i32 c = 0; c < channel(); ++c) {
			src_channel.set(channel_at(c));
			value_channel.set(value.channel_at(c));
			private_multiplication::mul(res_channel, src_channel, value_channel);

			res.channel_at(c).set(res_channel);
		}
	}
	
	if (m_auto_derivative != NULL && m_auto_derivative->math_operation_recorded()) {
		res.m_auto_derivative = m_auto_derivative;
		value.m_auto_derivative = m_auto_derivative;

		m_auto_derivative->mul(res, *this, value);
	}

	return res;
}