#include "test.h"
#include <mkl.h>


class mt_create_test {
public:

	static void test(b8 performance) {
		test_create_zero_2d_mat();
		test_create_incremental_mat_2d_mat();
		test_read_vector();
		test_create_multi_channel_2d_mat();

		if (performance) {
			performace_test_create_big_mat();
		}
	}

protected:

	static void test_create_zero_2d_mat() {
		mt_mat a(3, 3, mt_U8C1, mt_scalar(0));
		basiclog_info2(a);

		sys_test_equal(a, mt_mat(3, 3, mt_U8C1).set(0));
	}

	static void test_create_incremental_mat_2d_mat() {
		mt_mat a = (mt_mat(3, 3, mt_U8C1).set_incremental(0));

		basiclog_info2(a);
		sys_test_equal(a, mt_mat_t<u8>(3, 3, 1).read(0, 1, 2, 3, 4, 5, 6, 7, 8));
	}

	static void test_read_vector() {
		vector<int> vec;
		vec.push_back(1);

		basiclog_info2(mt_mat_t<i32>::read(vec));
	}

	static void test_create_multi_channel_2d_mat() {
		mt_mat a = mt_mat(1, 3, mt_U8C3).set_incremental(0, sys_false);
		basiclog_info2(a);

		sys_test_equal(a.at<u8>(0, 1, 2), 5);

		sys_test_equal(a, mt_mat_t<u8>(1, 3, 3).read(0, 1, 2, 3, 4, 5, 6, 7, 8));

		a = mt_mat(1, 3, mt_U8C3).set_incremental(0, sys_true);
		basiclog_info2(a);

		sys_test_equal(a.at<u8>(0, 1, 2), 1);

		sys_test_equal(a, mt_mat_t<u8>(1, 3, 3).read(0, 0, 0, 1, 1, 1, 2, 2, 2));
	}

	static void performace_test_create_big_mat() {
		sys_timer big_create_single_channel(L"big_create_single_channel");
		mt_mat big1(1000, 100, 100, mt_F64C3, mt_scalar(0, 0, 0, 0));
		mt_mat big2(1000, 100, 100, mt_F64C3, mt_scalar(0, 1, 2, 3));

		big_create_single_channel.begin();

		for (i32 i = 0; i < 10; ++i) {
			big1.set(mt_scalar(0, 0, 0, 0));
		}

		big_create_single_channel.end();

		sys_timer big_create_multi_channel(L"big_create_multi_channel");
		big_create_multi_channel.begin();

		for (i32 i = 0; i < 10; ++i) {
			big2.set(mt_scalar(0, 1, 2, 3));
		}

		big_create_multi_channel.end();
	}
};

static void test_mat_add() {
	mt_mat a = mt_mat_t<u8>(3, 3, 1).read(0, 1, 2, 3, 4, 5, 6, 7, 8);
	mt_mat b = mt_mat_t<u8>(3, 3, 1).read(1, 1, 1, 1, 1, 1, 1, 1, 1);

	mt_mat c = a + b;
	sys_test_equal(c, mt_mat_t<u8>(3, 3, 1).read(1, 2, 3, 4, 5, 6, 7, 8, 9));

	a += 1;

	sys_test_equal(a, c);

	a = mt_mat_t<u8>(2, 2, 1).read(0, 1, 2, 3);
	b = a + a.flip(0);

	sys_test_equal(b, mt_mat_t<u8>(2, 2, 1).read(2, 4, 2, 4));

	a = mt_mat_t<f32>(3, 3, 1).read(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
	mt_mat negative_a = -a;

	basiclog_info2(negative_a);
	sys_test_equal(negative_a, mt_mat_t<f32>(3, 3, 1).read(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0) * -1);


	mt_mat big1(1000, 100, 100, mt_F64C3, mt_scalar(0, 0, 0, 0));
	mt_mat big2(1000, 100, 100, mt_F64C3, mt_scalar(0, 1, 2, 3));

	sys_timer big_create_multi_channel(L"big_add");
	big_create_multi_channel.begin();

	for (i32 i = 0; i < 10; ++i) {

		big1 += big2;
	}	
	
	big_create_multi_channel.end();

}

static void test_mat_sub() {
	basiclog_info2(L"test_mat_sub");

	mt_mat a = mt_mat(3, 3, mt_U8C1).set_incremental(0);
	basiclog_info2(a);

	sys_test_equal(a.row(0), mt_mat_t<u8>(1, 3, 1).read(0, 1, 2));

	basiclog_info2(a.row(1));
	sys_test_equal(a.row(1), mt_mat_t<u8>(1, 3, 1).read(3, 4, 5));
	sys_test_equal(a.row(2), mt_mat_t<u8>(1, 3, 1).read(6, 7, 8));

	basiclog_info2(a.col(0));
	basiclog_info2(mt_mat_t<u8>(3, 1, 1).read(0, 3, 6));

	sys_test_equal(a.col(0), mt_mat_t<u8>(3, 1, 1).read(0, 3, 6));
	sys_test_equal(a.col(1), mt_mat_t<u8>(3, 1, 1).read(1, 4, 7));
	sys_test_equal(a.col(2), mt_mat_t<u8>(3, 1, 1).read(2, 5, 8));

	basiclog_info2(a.t().row(0));
	basiclog_info2(mt_mat_t<u8>(1, 3, 1).read(0, 3, 6));
	sys_test_equal(a.t().row(0), mt_mat_t<u8>(1, 3, 1).read(0, 3, 6));
	sys_test_equal(a.t().row(1), mt_mat_t<u8>(1, 3, 1).read(1, 4, 7));
	sys_test_equal(a.t().row(2), mt_mat_t<u8>(1, 3, 1).read(2, 5, 8));


	sys_test_equal(a.t().col(0), mt_mat_t<u8>(3, 1, 1).read(0, 1, 2));
	sys_test_equal(a.t().col(1), mt_mat_t<u8>(3, 1, 1).read(3, 4, 5));
	sys_test_equal(a.t().col(2), mt_mat_t<u8>(3, 1, 1).read(6, 7, 8));

	sys_test_equal(a.front(1), mt_mat_t<u8>(1, 3, 1).read(0, 1, 2));
	sys_test_equal(a.front(2), mt_mat_t<u8>(2, 3, 1).read(0, 1, 2, 3, 4, 5));
	sys_test_equal(a.front(3), mt_mat_t<u8>(3, 3, 1).read(0, 1, 2, 3, 4, 5, 6, 7, 8));

	sys_test_equal(a.back(1), mt_mat_t<u8>(1, 3, 1).read(6, 7, 8));
	sys_test_equal(a.back(2), mt_mat_t<u8>(2, 3, 1).read(3, 4, 5, 6, 7, 8));
	sys_test_equal(a.back(3), mt_mat_t<u8>(3, 3, 1).read(0, 1, 2, 3, 4, 5, 6, 7, 8));

	sys_test_equal(a.index(1), mt_mat_t<u8>(1, 3, 1).read(3, 4, 5));
	sys_test_equal(a.index(1, 1), mt_mat_t<u8>(3, 1, 1).read(1, 4, 7));


	
	mt_mat b = mt_mat(4, 4, mt_U8C1).set_incremental(0);
	basiclog_info2(b);

	mt_range ranges[2];
	ranges[0].m_start = 1;
	ranges[0].m_end = 3;

	ranges[1] = ranges[0];

	basiclog_info2(b.sub(2, ranges));
	basiclog_info2(b.sub(2, ranges).t().front(1));



	mt_mat c = mt_mat(3, 4, 4, mt_U8C1).set_incremental(0);
	basiclog_info2(c);

	basiclog_info2(c.flip(1));
	basiclog_info2(c.flip(1).swap_dim(1, 2).index(0).decrease_dim(0).row(1).t());

	//BASICLOG_INFO2(c.sub(1, 0));

	//BASICLOG_INFO2(c.sub(1, 0).decrease_dim(0).t());

	basiclog_info2(c.swap_dim(0, 2));
	basiclog_info2(c.swap_dim(0, 2).index(1).decrease_dim(0).flip(0).flip(1));
}

static void test_mat_flip() {
	basiclog_info2(L"test_mat_flip");

	mt_mat a = mt_mat(3, 3, mt_U8C1).set_incremental(0);

	basiclog_info2(a.flip(0).flip(1));
	basiclog_info2(a.flip(0).flip(1).t());

	basiclog_info2(a.flip(0).flip(1).index(1).t());


}

static void test_dim_channel() {
	mt_mat a = mt_mat(3, 3, mt_U8C1).set_incremental(0);

	basiclog_info2(a.last_dim_as_channel());

	basiclog_info2(a.last_dim_as_channel().channel_as_last_dim());
	sys_test_equal(a.last_dim_as_channel().channel_as_last_dim(), a);

	a = a.t().clone();

	basiclog_info2(a);
	basiclog_info2(a.last_dim_as_channel());

	basiclog_info2(a.last_dim_as_channel().channel_as_last_dim());
	sys_test_equal(a.last_dim_as_channel().channel_as_last_dim(), a);

	a = a.sub(mt_range(1, 3), 0).sub(mt_range(1, 3), 1);
	basiclog_info2(a.last_dim_as_channel());

	basiclog_info2(a.last_dim_as_channel().channel_as_last_dim());
	sys_test_equal(a.last_dim_as_channel().channel_as_last_dim(), a);

	a = a.flip(0);
	basiclog_info2(a);
	basiclog_info2(a.last_dim_as_channel().channel_as_last_dim());
	sys_test_equal(a.last_dim_as_channel().channel_as_last_dim(), a);
}

static void test_mat_t() {
	mt_mat a = mt_mat(4, 4, mt_U8C1).set_incremental(0);
	mt_mat sub_a = a.sub(1, 3, 0).sub(1, 3, 1);

	sys_test_equal(sub_a.t(), mt_mat_t<u8>(2, 2, 1).read(5, 9, 6, 10));

	mt_mat b = mt_mat(4, 4, mt_U8C1).set_incremental(0);
	
	
	mt_mat c = b.t();

	basiclog_info2(c);

	mt_mat sub_c = c.sub(1, 3, 0).sub(1, 3, 1);
	basiclog_info2(sub_c);

	sys_test_equal(sub_c, mt_mat_t<u8>(2, 2, 1).read(5, 9, 6, 10));
}

static void test_get_index() {
	mt_mat a = mt_mat(4, 4, mt_U8C1).set_incremental(0);
	basiclog_info2(a);

	vector<int> index;
	index.push_back(3);
	index.push_back(3);

	basiclog_info2(a.get_index(a.ptr<u8>(index, 0)));

	sys_test_equal(index, a.get_index(a.ptr<u8>(index, 0)));

	//a = a.flip(0);
	a = a.sub(mt_range(1, 3), 0).sub(mt_range(1, 3), 1);

	basiclog_info2(a);

	index[0] = 1;
	index[1] = 0;

	a = a.flip(0);

	basiclog_info2(a.get_index(a.ptr<u8>(index, 0)));

	sys_test_equal(index, a.get_index(a.ptr<u8>(index, 0)));
}

class mt_auto_derivative_test {
public:

	static void test(b8 performance) {
		test_add();
		test_normal();
		test_merge_dim();
		test_add_bias();
	}

protected:

	static void test_add() {
		mt_mat a = mt_mat(2, 2, mt_F32C1).set_incremental(0);
		mt_mat b = mt_mat(2, 2, mt_F32C1).set_incremental(0);

		mt_auto_derivative auto_derivative;
		a.attach(&auto_derivative);


		//we first test the derivative on mat add
		mt_mat c = a + b;

		mt_mat derivate_c_to_a = c.derivative(a);
		basiclog_info2(derivate_c_to_a);
	}

	static void test_normal() {
		mt_mat a = mt_mat(4, 4, mt_F32C1).set_incremental(0);
		mt_mat b = mt_mat(2, 2, mt_F32C1).set_incremental(0);

		mt_auto_derivative auto_derivative;
		a.attach(&auto_derivative);


		//we first test the derivative on mat add
		mt_mat sub_a = a.sub(mt_range(1, 3), 0).sub(mt_range(1, 3), 1);

		mt_mat c = sub_a + b;
		mt_mat d = c + sub_a;
		mt_mat e = c + d;


		//e = c + d = c + (c + sub_a) = sub_a + b + (sub_a + b + sub_a) = 3sub_a + 2b
		// 0 0 0 0
		// 0 3 3 0
		// 0 3 3 0
		// 0 0 0 0
		mt_mat derivate_c_to_a = auto_derivative.derivative(a, e);
		basiclog_info2(derivate_c_to_a);

		sys_test_equal(derivate_c_to_a, mt_mat_t<f32>(4, 4, 1).read(0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 3.0, 0.0, 0.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 0.0));

		//auto_derivative.reset();

		//then we test derivative on mul
		//e*b = k

		mt_mat k = e.mul(b);

		basiclog_info2(k);

		mt_mat derivative_k_to_e = auto_derivative.derivative(e, k);

		basiclog_info2(derivative_k_to_e);

		sys_test_equal(derivative_k_to_e, mt_mat_t<f32>(2, 2, 1).read(1.0, 5.0, 1.0, 5.0));

		mt_mat derivative_k_to_b = auto_derivative.derivative(b, k);

		basiclog_info2(derivative_k_to_b);

		sys_test_equal(derivative_k_to_b, mt_mat_t<f32>(2, 2, 1).read(48.0, 56.0, 58.0, 66.0));

		mt_mat derivate_k_to_a = auto_derivative.derivative(a, k);

		basiclog_info2(derivate_k_to_a);

		sys_test_equal(derivate_k_to_a, mt_mat_t<f32>(4, 4, 1).read(0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 15.0, 0.0, 0.0, 3.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0));

		k = k + k;

		mt_mat derivative_2k_to_e = k.derivative(e);

		basiclog_info2(derivative_2k_to_e);
		sys_test_equal(derivative_2k_to_e, derivative_k_to_e * 2);

		mt_mat derivative_2k_to_b = k.derivative(b);

		basiclog_info2(derivative_2k_to_b);
		sys_test_equal(derivative_2k_to_b, derivative_k_to_b * 2);

		//test conv
		mt_mat p = a.conv(b, mt_Conv_Boundary_Type_Valid);

		mt_mat derivative_p_a = p.derivative(a);

		basiclog_info2(derivative_p_a);

		sys_test_equal(derivative_p_a, mt_mat_t<f32>(4, 4, 1).read(3.0, 5.0, 5.0, 2.0, 4.0, 6.0, 6.0, 2.0, 4.0, 6.0, 6.0, 2.0, 1.0, 1.0, 1.0, 0.0));

		mt_mat derivative_p_b = p.derivative(b);

		basiclog_info2(derivative_p_b);

		sys_test_equal(derivative_p_b, mt_mat_t<f32>(2, 2, 1).read(90.0, 81.0, 54.0, 45.0));

		//test repeat
		mt_mat g = a + b.reshape_2d(1, 4).repeat_2d(4, 1);

		mt_mat derivative_g_to_b = g.derivative(b);

		basiclog_info2(derivative_g_to_b);

		//test merge align channel
		vector<mt_mat> elements;
		elements.push_back(a.clone());
		elements.push_back(a.clone());

		mt_mat channel_2 = mt_mat_helper::merge_align_channel(elements);

		basiclog_info2(channel_2);

		mt_mat derivative_channel_2_to_element_0 = channel_2.derivative(elements[0]);
		basiclog_info2(derivative_channel_2_to_element_0);

		sys_test_equal(derivative_channel_2_to_element_0, mt_mat(4, 4, mt_F32C1, mt_scalar(1.0)));

		mt_mat merged_dim_0 = mt_mat_helper::merge_align_dim(elements, 0);
		basiclog_info2(merged_dim_0);

		mt_mat derivative_merged_dim_0_to_element_0 = merged_dim_0.derivative(elements[0]);
		basiclog_info2(derivative_merged_dim_0_to_element_0);

		sys_test_equal(derivative_merged_dim_0_to_element_0, mt_mat(4, 4, mt_F32C1, mt_scalar(1.0)));

		// test pooling
		i32 pooling_kernel_sizes[] = {2, 2};
		mt_mat pooling_a = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 2, pooling_kernel_sizes, pooling_kernel_sizes);

		mt_mat h = pooling_a.mul(b);

		mt_mat derivative_h_to_a = h.derivative(a);
		basiclog_info2(derivative_h_to_a);

		sys_test_equal(derivative_h_to_a, mt_mat_t<f32>(4, 4, 1).read(0.2500000, 0.2500000, 1.2500000, 1.2500000, 0.2500000, 0.2500000, 1.2500000, 1.2500000, 0.2500000, 0.2500000, 1.2500000, 1.2500000, 0.2500000, 0.2500000, 1.2500000, 1.2500000))

	}

	static void test_merge_dim() {
		mt_auto_derivative at;
		mt_mat src = mt_mat(10, 2, 2, mt_F32C4).set_incremental(0);
		src.attach(&at);

		mt_mat a = src.reshape_2d(10, 4);

		vector<mt_mat> all_channels;
		a.all_channel(all_channels);

		mt_mat res = mt_mat_helper::merge_align_dim(all_channels, 1);

		mt_mat derivative_res = res.derivative(src);
		basiclog_info2(derivative_res);
	}

	static void test_add_bias() {
		mt_auto_derivative at;
		mt_mat a = mt_mat(10, 3, 3, mt_F32C4).set_incremental(0);
		a.attach(&at);

		mt_mat bias = mt_mat(1, 1, 1, mt_F32C4).set_incremental(0);

		mt_mat res = a.add_bias(bias);

		mt_mat derivative_res = res.derivative(bias);
		basiclog_info2(derivative_res);
	}
};


static void test_mul() {
	//test normal mul

	mt_mat a = mt_mat(2, 2, mt_F32).set_incremental(0);
	mt_mat b = a.clone();

	mt_mat res = mt_mat_t<f32>(2, 2, 1).read(2.0, 3.0, 6.0, 11.0);

	basiclog_info2(a);
	basiclog_info2(b);

	mt_mat c = a.mul(b);

	basiclog_info2(c);
	basiclog_info2(res);

	sys_test_equal(c, res);

	//test mul on t
	a = mt_mat(2, 2, mt_F32).set_incremental(0);

	//test mul on t and sub and flip
	mt_mat e = mt_mat(3, 4, mt_F32).set_incremental(0);
	mt_mat f = e.sub(1, 3, 0).sub(1, 3, 1).t();

	basiclog_info2(f);

	basiclog_info2(a.flip_all_dim());
	c = a.flip_all_dim().mul(a.flip_all_dim());

	basiclog_info2(c);
	sys_test_equal(c, mt_mat_t<f32>(2, 2, 1).read(11.0, 6.0, 3.0, 2.0));

	mt_mat mat_331 = mt_mat(3, 3, mt_F32).set_incremental(0, true);
	mt_mat mat_333 = mt_mat(3, 3, mt_F32C3).set_incremental(0, true);
	basiclog_info2(mat_333.channel_at(1));

	basiclog_info2(mat_331.mul(mat_331));
	basiclog_info2(mat_333.channel_at(1).mul(mat_333.channel_at(2)));

	sys_test_equal(mat_333.channel_at(1).mul(mat_333.channel_at(2)), mat_331.mul(mat_331));

	
}

class mt_conv_test {
public:

	static void test(b8 performance) {
		normal_test();
		multi_channel_test();

		if (performance) {
			performance_test_channel_conv();
		}
	}

private:

	static void normal_test() {
		mt_mat mat_33 = mt_mat(3, 3, mt_F32).set_incremental(0);
		mt_mat mat_22 = mt_mat(2, 2, mt_F32).set_incremental(0);

		sys_test_equal(mat_33.conv(mat_22), mt_mat_t<f32>(2, 2, 1).read(5.0, 11.0, 23.0, 29.0));

		basiclog_info2(mat_33.t().conv(mat_22.t()));

		sys_test_equal(mat_33.t().conv(mat_22.t()), mt_mat_t<f32>(2, 2, 1).read(5.0, 23.0, 11.0, 29.0));

		mt_mat a = mt_mat(1, 3, mt_F32).set_incremental(0);
		mt_mat b = mt_mat(1, 2, mt_F32).set_incremental(0);

		basiclog_info2(a.decrease_dim(0).conv(b.decrease_dim(0), mt_Conv_Boundary_Type_Full));
		sys_test_equal(a.decrease_dim(0).conv(b.decrease_dim(0), mt_Conv_Boundary_Type_Full), mt_mat_t<f32>(1, 4, 1).read(0.000000, 0.000000, 1.000000, 2.000000).decrease_dim(0));

		mt_mat src_3d = mt_mat(10, 10, 10, mt_F64).set_incremental(0);
		mt_mat kernel_3d = mt_mat(1, 5, 5, mt_F64).set_incremental(0);

		mt_mat res_3d = src_3d.conv(kernel_3d, mt_Conv_Boundary_Type_Valid);

		basiclog_info2(res_3d);
	}

	static void multi_channel_test() {
		mt_mat mat_33 = mt_mat(3, 3, mt_F32C2).set_incremental(0);
		mt_mat mat_22 = mt_mat(2, 2, mt_F32C4).set_incremental(0, sys_false);

		basiclog_info2(mat_33.channel_at(0));
		basiclog_info2(mat_22.flip_all_dim().clone().channel_at(0));

		mt_mat res = mat_33.conv(mat_22.flip_all_dim().clone());

		basiclog_info2(res);
		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(168.0000000, 184.0000000, 224.0000000, 248.0000000, 336.0000000, 376.0000000, 392.0000000, 440.0000000));


		res = mat_33.conv(mat_22.flip_all_dim(), mt_Conv_Boundary_Type_Valid, mt_Conv_Kernel_Align_Type_Col);
		basiclog_info2(res);
		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(160.0000000, 192.0000000, 212.0000000, 260.0000000, 316.0000000, 396.0000000, 368.0000000, 464.0000000));

		res = mat_33.conv(mat_22.flip_all_dim(), mt_Conv_Boundary_Type_Valid, mt_Conv_Kernel_Align_Type_Cross_Row);

		basiclog_info2(res);

		mt_mat gt = mt_mat_t<f32>(2, 2, 8).read(76.0000000, 84.0000000, 92.0000000, 100.0000000, 76.0000000, 84.0000000, 92.0000000, 100.0000000, 100.0000000, 112.0000000, 124.0000000, 136.0000000, 100.0000000, 112.0000000, 124.0000000, 136.0000000,
			148.0000000, 168.0000000, 188.0000000, 208.0000000, 148.0000000, 168.0000000, 188.0000000, 208.0000000, 172.0000000, 196.0000000, 220.0000000, 244.0000000, 172.0000000, 196.0000000, 220.0000000, 244.0000000);
		
		basiclog_info2(gt);
		sys_test_equal(res, gt);

		res = mat_33.conv(mat_22.flip_all_dim(), mt_Conv_Boundary_Type_Valid, mt_Conv_Kernel_Align_Type_Cross_Col);
		basiclog_info2(res);

		gt = mt_mat_t<f32>(2, 2, 8).read(76.0000000, 76.0000000, 84.0000000, 84.0000000, 92.0000000, 92.0000000, 100.0000000, 100.0000000, 100.0000000, 100.0000000, 112.0000000, 112.0000000, 124.0000000, 124.0000000, 136.0000000, 136.0000000,
			148.0000000, 148.0000000, 168.0000000, 168.0000000, 188.0000000, 188.0000000, 208.0000000, 208.0000000, 172.0000000, 172.0000000, 196.0000000, 196.0000000, 220.0000000, 220.0000000, 244.0000000, 244.0000000);
		sys_test_equal(res, gt);
	}

	static void performance_test_channel_conv() {
		mt_mat src_3d(1000, 60, 60, mt_F64C1, mt_scalar(1));
		mt_mat kernel_3d = mt_mat(1, 10, 10, mt_F64).set_incremental(0);

		mt_mat multi_channel_src_3d(1000, 60, 60, mt_Depth_Channel(mt_F64, 10), 0);

		mt_mat multi_channel_src_3d2(10000, 60, 60, mt_Depth_Channel(mt_F64, 1), 0);


		sys_timer channel_conv(L"discontinuous_memory_conv");
		channel_conv.begin();

		#pragma omp parallel for
		for (i32 i = 0; i < 10; ++i) {
			multi_channel_src_3d.channel_at(i).conv(kernel_3d, mt_Conv_Boundary_Type_Valid);
		}

		channel_conv.end();

		sys_timer single_conv(L"continuous_memory_conv");
		single_conv.begin();

		#pragma omp parallel for
		for (i32 i = 0; i < 10; ++i) {
			multi_channel_src_3d2.sub(i * 1000, (i + 1) * 1000).conv(kernel_3d, mt_Conv_Boundary_Type_Valid);
		}

		single_conv.end();


	}
};

class mt_bias_test {
public:

	static void test(b8 performance) {
		add_test();
		div_test();
	}

protected:

	static void add_test() {
		mt_mat mat_22 = mt_mat(2, 2, mt_F32C2).set_incremental(0);
		mt_mat mat_11 = mt_mat(1, 1, mt_F32C2).set_incremental(0, sys_false);

		mt_mat res = mat_22.add_bias(mat_11);
		basiclog_info2(mat_22);
		basiclog_info2(mat_11);
		basiclog_info2(res);

		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(0.0000000, 1.0000000, 1.0000000, 2.0000000, 2.0000000, 3.0000000, 3.0000000, 4.0000000));

		mt_mat mat_12 = mt_mat(1, 2, mt_F32C2).set_incremental(0, sys_false);

		res = mat_22.add_bias(mat_12);
		basiclog_info2(mat_22);
		basiclog_info2(mat_12);
		basiclog_info2(res);

		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(0.0000000, 1.0000000, 3.0000000, 4.0000000, 2.0000000, 3.0000000, 5.0000000, 6.0000000));
	}

	static void div_test() {
		mt_mat mat_22 = mt_mat(2, 2, mt_F32C2).set_incremental(1);
		mt_mat mat_11 = mt_mat(1, 1, mt_F32C2).set_incremental(1, sys_false);

		mt_mat res = mat_22.div_bias(mat_11);
		basiclog_info2(mat_22);
		basiclog_info2(mat_11);
		basiclog_info2(res);

		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(1.0000000, 0.5000000, 2.0000000, 1.0000000, 3.0000000, 1.5000000, 4.0000000, 2.0000000));

		mt_mat mat_12 = mt_mat(1, 2, mt_F32C2).set_incremental(1, sys_false);

		res = mat_22.div_bias(mat_12);
		basiclog_info2(mat_22);
		basiclog_info2(mat_12);
		basiclog_info2(res);

		sys_test_equal(res, mt_mat_t<f32>(2, 2, 2).read(1.0000000, 0.5000000, 0.6666667, 0.5000000, 3.0000000, 1.5000000, 1.3333334, 1.0000000));
	}
};

static void test_convert() {
	mt_mat a = mt_mat(3, 3, mt_U8).set_incremental(0);
	
	basiclog_info2(a.convert(mt_F32));

	sys_test_equal(a.convert(mt_F32), mt_mat(3, 3, mt_F32).set_incremental(0));

	mt_mat int_mat = mt_mat(3, 3, mt_I32).set_incremental(0);
}

static void test_mat_channel() {
	mt_mat mat_33 = mt_mat(3, 3, mt_F32C3).set_incremental(0, false);
	basiclog_info2(mat_33);

	mt_mat mat_channel = mat_33.channel_at(1);
	basiclog_info2(mat_channel);

	vector<mt_mat> channels;
	mat_33.split(channels);

	basiclog_info2(channels[0]);
	basiclog_info2(channels[1]);

	basiclog_info2(mt_mat_helper::merge_align_channel(channels));

	sys_test_equal(mt_mat_helper::merge_align_channel(channels), mat_33);
}

static void test_reshape() {
	mt_mat a = mt_mat(4, 4, mt_U8).set_incremental(0);
	mt_mat b = mt_mat(2, 8, mt_U8).set_incremental(0);

	sys_test_equal(a.reshape_2d(2, 8), b);
	sys_test_equal(a.reshape_2d(1, 16), b.reshape_2d(1, 16));
}

static void test_repeat() {
	mt_mat a = mt_mat(2, 2, mt_U8).set_incremental(0);
	mt_mat b = mt_mat_t<u8>(2, 4, 1).read(0, 1, 0, 1, 2, 3, 2, 3);

	sys_test_equal(a.repeat(2, 1), b);

	basiclog_info2(a.repeat(2, 1));
	basiclog_info2(b);

	mt_mat c = a.increase_dim(0);
	basiclog_info2(c.repeat(2, 0));
}

static void test_save_mat() {
	mt_mat a = mt_mat(20, 20, mt_U16C3).set_incremental(0, sys_false);

	mt_mat_helper::save(L"test.txt", a);

	mt_mat b = mt_mat_helper::load(L"test.txt");

	sys_test_equal(a, b);

	mt_mat_helper::save(L"test.b", a, sys_false);

	b = mt_mat_helper::load(L"test.b", sys_false);

	sys_test_equal(a, b);


	//basiclog_info2(b);
}

class mt_pooling_test {
public:

	static void test(b8 performance = sys_false) {
		// mean pooling
		test_mean_2d_channel_1();
		test_mean_2d_channel_n();
		test_meam_3d_channel_1();

		// max pooling
		test_max_2d_channel_1();
		test_max_3d_channel_n();
		test_max_sub_3d_channel_n();

		if (performance) {
			test_performance_mean_2d_channel_1();
			test_performance_mean_3d_channel_1();
		}
	}



private:

	static void test_mean_2d_channel_1() {
		i32 kernel_sizes[] = {2, 2};
		i32 strides[] = {2, 2};

		mt_mat a = mt_mat(4, 4, mt_F32C1).set_incremental(0, sys_false);
		basiclog_info2(a);

		mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 2, kernel_sizes, strides);
		basiclog_info2(b);

		sys_test_equal(b, mt_mat_t<f32>(2, 2, 1).read(2.5, 4.5, 10.5, 12.5));
	}

	static void test_mean_2d_channel_n() {
		i32 kernel_sizes[] = {2, 2};
		i32 strides[] = {2, 2};

		mt_mat a = mt_mat(4, 4, mt_F32C3).set_incremental(0, sys_false);
		basiclog_info2(a);

		mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 2, kernel_sizes, strides);
		basiclog_info2(b);

		sys_test_equal(b, mt_mat_t<float>(2, 2, 3).read(7.500000, 8.5000000, 9.5000000, 13.5000000, 14.5000000, 15.5000000, 31.5000000, 32.5000000, 33.5000000, 37.5000000, 38.5000000, 39.5000000));
	}

	static void test_meam_3d_channel_1() {
		i32 kernel_sizes[] = {2, 2, 2};
		i32 strides[] = {2, 2, 2};

		mt_mat a = mt_mat(4, 4, 4, mt_F64C1).set_incremental(0, sys_false);
		basiclog_info2(a);

		mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 3, kernel_sizes, strides);
		basiclog_info2(b);

		sys_test_equal(b, mt_mat_t<f64>(2, 2, 2, 1).read(10.5, 12.5, 18.5, 20.5, 42.5, 44.5, 50.5, 52.5));
	}


	static void test_max_2d_channel_1() {
		i32 kernel_sizes[] = {2, 2};
		i32 strides[] = {2, 2};

		mt_mat a = mt_mat(4, 4, mt_F32C1).set_incremental(0, sys_false);
		basiclog_info2(a);

		mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Max, 2, kernel_sizes, strides);
		basiclog_info2(b);

		sys_test_equal(b, mt_mat_t<f32>(2, 2, 1).read(5.0, 7.0, 13.0, 15.0));
	}



	static void test_max_3d_channel_n() {
		i32 kernel_sizes[] = {2, 2, 2};
		i32 strides[] = {2, 2, 2};

		mt_mat a = mt_mat(4, 4, 4, mt_U16C3).set_incremental(0, sys_false);
		basiclog_info2(a);

		mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Max, 3, kernel_sizes, strides);
		basiclog_info2(b);

		sys_test_equal(b, mt_mat_t<u16>(2, 2, 2, 3).read(63, 64, 65, 69, 70, 71, 87, 88, 89, 93, 94, 95, 159, 160, 161, 165, 166, 167, 183, 184, 185, 189, 190, 191));
	}

	static void test_max_sub_3d_channel_n() {
		i32 kernel_sizes[] = {2, 2, 2};
		i32 strides[] = {2, 2, 2};

		mt_mat a = mt_mat(4, 4, 4, mt_U16C3).set_incremental(0, sys_false);
		mt_mat sub_a = a.sub(1, 3, 0).sub(1, 3, 1).sub(1, 3, 2);
		basiclog_info2(sub_a);

		mt_mat b = sub_a.pooling(mt_mat(), mt_Pooling_Type_Max, 3, kernel_sizes, strides);

		sys_test_equal(b, mt_mat_t<u16>(1, 1, 1, 3).read(126, 127, 128));
	}

	static void test_performance_mean_2d_channel_1() {
		i32 kernel_sizes[] = {2, 2};
		i32 strides[] = {2, 2};

		mt_mat a = mt_mat(1000, 1000, mt_F32C1).set_incremental(0, sys_false);

		sys_timer timer_1(L"src = 1000 * 1000, kernel = 2 * 2, stride = 2 * 2");
		timer_1.begin();

		for (i32 i = 0; i < 10; ++i) {
			mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 2, kernel_sizes, strides);
		}

		timer_1.end();

		i32 kernel_sizes_5[] = {5, 5};
		i32 strides_5[] = {5, 5};

		sys_timer timer_2(L"src = 1000 * 1000, kernel = 5 * 5, stride = 5 * 5");
		timer_2.begin();

		for (i32 i = 0; i < 10; ++i) {
			mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 2, kernel_sizes, strides);
		}

		timer_2.end();
	}

	static void test_performance_mean_3d_channel_1() {
		i32 kernel_sizes[] = {1, 5, 5};
		i32 strides[] = {1, 5, 5};

		mt_mat a = mt_mat(100, 28, 28, mt_F32C1).set_incremental(0, sys_false);

		sys_timer timer_1(L"src = 100 * 280 * 280, kernel = 1 * 5 * 5, stride = 1 * 5 * 5");
		timer_1.begin();

		for (i32 i = 0; i < 10; ++i) {
			mt_mat b = a.pooling(mt_mat(), mt_Pooling_Type_Mean, 3, kernel_sizes, strides);
		}

		timer_1.end();
	}
};

static void test_sub_stride() {
	mt_mat a = mt_mat(8, 8, mt_I32).set_incremental(0, sys_false);
	basiclog_info2(a);

	i32 strides[] = {2, 2};

	basiclog_info2(a.sub_stride(2, strides));

	sys_test_equal(a.sub(1, 7, 0).sub(1, 7, 1).t().flip_all_dim().sub_stride(2, strides), mt_mat_t<i32>(3, 3, 1).read(54, 38, 22, 52, 36, 20, 50, 34, 18));

	//basiclog_info2(a.sub(1, 7, 0).sub(1, 7, 1));
	//basiclog_info2(a.sub(1, 7, 0).sub(1, 7, 1).t());
	//basiclog_info2(a.sub(1, 7, 0).sub(1, 7, 1).t().flip_all_dim());
	//basiclog_info2(a.sub(1, 7, 0).sub(1, 7, 1).t().flip_all_dim().sub_stride(2, strides));
}

static void test_at_ptr() {
	mt_mat a = mt_mat(8, 8, mt_I32).set_incremental(0, sys_false);
	basiclog_info2(a);

	i32 value = a.at<i32>(1, 0);
	
	
	sys_test_equal(a.at<i32>(1, 0), 8);
	sys_test_equal(a.at<i32>(1, 0, 0), 8);

	sys_test_equal(*a.ptr<i32>(1, 0), 8);
	sys_test_equal(*a.ptr<i32>(1, 0, 0), 8);
}

static void test_activate() {
	mt_mat a = mt_mat(2, 2, 2, mt_F64).set_incremental(0, sys_false);

	mt_mat b = a.activate(mt_Activate_Type_Sigmoid);

	basiclog_info2(b);
}

static void test_eigen_case(const mt_mat& src, const mt_mat& eigen_values, const mt_mat& eigen_vectors) {
	mt_mat ev, ec;
	src.symmetry_eigen(ev, ec);

	sys_test_equal(ev, eigen_values);
	sys_test_equal(ec, eigen_vectors);

	basiclog_info2(ev);
	basiclog_info2(ec);
}

static void test_eigen() {
	basiclog_debug2(L"test eigen:");
	// example from https://en.wikipedia.org/wiki/Jacobi_eigenvalue_algorithm
	// result:
	// e1 = 2585.253662
	// v0 = (0.0291933, -0.3287122, 0.7914113, -0.5145529)
	// e2 = 37.101521
	// v1 = (-0.1791862, 0.7419176, -0.1002286, -0.6382831)
	// e3 = 1.478052
	// v2 = (-0.5820814, 0.3704996, 0.5095764, 0.5140461)
	// e4 = 0.166633
	// v3 = (0.7926043, 0.4519261, 0.3224201, 0.2521646)

	mt_mat a = mt_mat_t<f32>(4, 4, 1).read(4.0f, -30.0, 60.0, -35.0, -30.0, 300.0, -675.0, 420.0, 60.0, -675.0, 1620.0, -1050.0, -35.0, 420.0, -1050.0, 700.0);
	mt_mat result_ev = mt_mat_t<f32>(1, 4, 1).read(2585.253662f, 37.101521, 1.478052, 0.166633);
	mt_mat result_ec = mt_mat_t<f32>(4, 4, 1).read(0.0291933f, -0.3287122, 0.7914113, -0.5145529, -0.1791862, 0.7419176, -0.1002286, -0.6382831, -0.5820814, 0.3704996, 0.5095764, 0.5140461, 0.7926043, 0.4519261, 0.3224201, 0.2521646);

	test_eigen_case(a, result_ev, result_ec);

	a = mt_mat_t<f32>(3, 3, 1).read(1.0, -3.0, 3.0, 3.0, -5.0, 3.0, 6.0, -6.0, 4.0);
	basiclog_info2(a);
	result_ev = mt_mat_t<f32>(1, 3, 1).read(4.0, -2.0, -2.0);
	
	//test_eigen_case(a, result_ev, result_ec);


	//mt_mat ev, ec;
	//a.eigen(ev, ec);

	//sys_test_equal(ev.dim(), 2);
	//sys_test_equal(ev.depth_channel(), a.depth_channel());
	//sys_test_equal(ev.element_number(), a.size()[0]);
	//sys_test_equal(ec.dim(), 2);
	//sys_test_equal(ec.depth_channel(), a.depth_channel());
	//sys_test_equal(ec.element_number(), a.element_number());
	//sys_test_equal(ev, result_ev);
	//sys_test_equal(ec, result_ec);
	//
	//i32 n = ev.element_number();
	//basiclog_debug2(L"result:");
	//for(i32 i = 0; i < n; ++i) {
	//	basiclog_debug2(sys_strcombine() << L"e" << i << L" = " << ev.at<f32>(0, i, 1));
	//	sys_strcombine str;
	//	str << L"v" << i  << L" = (";
	//	for(i32 j = 0; j < n; ++j) {
	//		str << ec.at<f32>(i, j, 1);
	//		if(j != n-1) {
	//			str << L", ";
	//		}
	//	}
	//	str << L")";
	//	basiclog_debug2(str);
	//}
}

void mt_mat_test::run(vector<wstring>& argvs) {
	sys_multi_process::enable_omp_mkl(sys_true);

	b8 enable_performance_test = sys_false;

	mt_create_test::test(enable_performance_test);
	mt_bias_test::test(enable_performance_test);
	test_mat_sub();
	test_mat_t();
	test_mat_flip();
	test_dim_channel();
	test_get_index();
	mt_auto_derivative_test::test(enable_performance_test);
	test_mul();
	mt_conv_test::test(enable_performance_test);
	test_mat_channel();
	test_convert();
	test_mat_add();
	test_repeat();
	test_reshape();
	test_save_mat();
	mt_pooling_test::test(enable_performance_test);
	test_sub_stride();
	test_at_ptr();
	test_eigen();
}