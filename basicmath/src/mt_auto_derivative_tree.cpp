#include "stdafx.h"
#include "mt_auto_derivative_tree.h"


class private_ad_helper {
public:

	template<class T>
	static mt_mat softmax_derivate(const mt_mat& softmax_res) {
		mt_mat res(softmax_res, 0);
		const u8* ptr_softmax_res_dim0 = softmax_res.data();
		u8* ptr_derivate_dim0 = res.data();

		for (i32 row = 0; row < softmax_res.size()[0]; ++row) {
			const T* ptr_softmax_res_dim1 = (const T*)ptr_softmax_res_dim0;
			T* ptr_derivate_dim1 = (T*)ptr_derivate_dim0;

			for (i32 col = 0; col < softmax_res.size()[1]; ++col) {
				for (i32 i = 0; i < softmax_res.size()[1]; ++i) {
					if (col == i) {
						ptr_derivate_dim1[col] += ptr_softmax_res_dim1[i] * (1 - ptr_softmax_res_dim1[i]);
					} else {
						ptr_derivate_dim1[col] += -ptr_softmax_res_dim1[i] * ptr_softmax_res_dim1[col];
					}
				}
			}

			ptr_softmax_res_dim0 += softmax_res.step()[0];
			ptr_derivate_dim0 += res.step()[0];
		}

		return res;
	}

	template<class T>
	static mt_mat relu_derivate(const mt_mat& relu_res, f64 negative_slope) {
		mt_mat res(relu_res, mt_mat::Construct_Type_Create_As_Size);

		mt_array_element_const_iterator relu_res_iter(relu_res);
		mt_array_element_iterator res_iter(res);

		for (;;) {
			const T* ptr_relu_res = (const T*)relu_res_iter.data();

			if (ptr_relu_res == NULL) {
				break;
			}

			T* ptr_res = (T*)res_iter.data();

			for (i32 c = 0; c < relu_res.channel(); ++c) {
				if (ptr_relu_res[c] > 0) {
					ptr_res[c] = 1;
				} else {
					ptr_res[c] = (T)negative_slope;
				}
			}
		}

		return res;
	}

	template<class T>
	static mt_mat logarithmic_loss_derivative(const mt_mat& predicted_mat, const mt_mat& matching_mat) {
		if (predicted_mat.size()[1] == 1) {
			//logistic
			return ((predicted_mat - matching_mat) / (predicted_mat * (1 - predicted_mat)));
		} else {
			//softmax

			mt_mat res(predicted_mat, mt_mat::Construct_Type_Create_As_Size);

			basiclog_info2(predicted_mat);

			const u8* ptr_predicted_dim0 = predicted_mat.data();
			const u8* ptr_matching_dim0 = matching_mat.data();
			u8* ptr_res_dim0 = res.data();

			for (i32 row = 0; row < matching_mat.size()[0]; ++row) {
				
				const T* ptr_predicted_dim1 = (const T*)ptr_predicted_dim0;
				const T* ptr_matching_dim1 = (const T*)ptr_matching_dim0;
				T* ptr_res_dim1 = (T*)ptr_res_dim0;

				for (i32 col = 0; col < matching_mat.size()[1]; ++col) {
					if ((i32)ptr_matching_dim1[col] == 1) {
						ptr_res_dim1[col] = -1 / ptr_predicted_dim1[col];
					}
				}

				ptr_predicted_dim0 += predicted_mat.step()[0];
				ptr_matching_dim0 += matching_mat.step()[0];
				ptr_res_dim0 += res.step()[0];
			}

			basiclog_info2(res);

			return res;
		}
	}

	template<class T>
	static mt_mat repeat_derivative(const mt_mat& src, const mt_mat& derivative_res) {
		mt_mat res = mt_mat(src, 0);

		basicmath_mat_request_memory(i32, repeated_times, res.dim());

		for (i32 i = 0; i < res.dim(); ++i) {
			repeated_times[i] = derivative_res.size()[i] / res.size()[i];
		}

		mt_array_index_iterator iter(res.dim(), repeated_times);
		i32 src_channel = src.channel();
		i32 nchannels = derivative_res.channel() / src_channel;

		vector<mt_range> start_ranges;
		start_ranges.resize(res.dim());

		while (iter.next()) {

			for (i32 i = 0; i < res.dim(); ++i) {
				start_ranges[i].m_start = iter.position()[i] * res.size()[i];
				start_ranges[i].m_end = start_ranges[i].m_start + res.size()[i];
			}

			for (i32 c = 0; c < nchannels; ++c) {
				i32 start_channel = c * src_channel;
				res += derivative_res.sub(start_ranges).sub_channel(start_channel, start_channel + src_channel);
			}
		}

		basicmath_mat_release(repeated_times);
		return res;
	}

};

mt_mat mt_ad_add_bias_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (m_childs[0] == child_node) {
		return derivative_res;
	} else {
		b8 is_unit_bias = sys_true;

		const mt_mat& child_node_mat = child_node->to_mat_tree_node()->m_mat;

		for (i32 i = 0; i < child_node_mat.dim(); ++i) {
			if (child_node_mat.size()[i] != 1) {
				is_unit_bias = sys_false;
				break;
			}
		}

		if (is_unit_bias) {
			return derivative_res.reduce(mt_mat::Reduce_Type_Sum);
		}

		if (child_node_mat.dim() == 2 && child_node_mat.size()[0] == 1 && child_node_mat.size()[1] == derivative_res.size()[1]) {
			// row bias
			return derivative_res.reduce(mt_mat::Reduce_Type_Sum, 0);
		} else {
			if (derivative_res.depth() == mt_F32) {
				return private_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else if (derivative_res.depth() == mt_F64) {
				return private_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else {
				basiclog_unsupport2();
				return mt_mat();
			}
		}
	}
}

mt_mat mt_ad_substract_bias_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (m_childs[0] == child_node) {
		return derivative_res;
	} else {
		b8 is_unit_bias = sys_true;

		const mt_mat& child_node_mat = child_node->to_mat_tree_node()->m_mat;

		for (i32 i = 0; i < child_node_mat.dim(); ++i) {
			if (child_node_mat.size()[i] != 1) {
				is_unit_bias = sys_false;
				break;
			}
		}

		if (is_unit_bias) {
			return -derivative_res.reduce(mt_mat::Reduce_Type_Sum);
		}

		if (child_node_mat.dim() == 2 && child_node_mat.size()[0] == 1 && child_node_mat.size()[1] == derivative_res.size()[1]) {
			// row bias
			return -derivative_res.reduce(mt_mat::Reduce_Type_Sum, 0);
		} else {
			if (derivative_res.depth() == mt_F32) {
				return -private_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else if (derivative_res.depth() == mt_F64) {
				return -private_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else {
				basiclog_unsupport2();
				return mt_mat();
			}
		}
	}
}

mt_mat mt_ad_mul_bias_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (m_childs[0] == child_node) {
		return derivative_res.mul_bias(m_childs[1]->to_mat_tree_node()->m_mat);
	} else {
		b8 is_unit_bias = sys_true;

		const mt_mat& child_node_mat = child_node->to_mat_tree_node()->m_mat;

		for (i32 i = 0; i < child_node_mat.dim(); ++i) {
			if (child_node_mat.size()[i] != 1) {
				is_unit_bias = sys_false;
				break;
			}
		}

		mt_mat child_derivative = derivative_res * m_childs[0]->to_mat_tree_node()->m_mat;

		if (is_unit_bias) {
			return child_derivative.reduce(mt_mat::Reduce_Type_Sum);
		}

		if (child_node_mat.dim() == 2 && child_node_mat.size()[0] == 1 && child_node_mat.size()[1] == derivative_res.size()[1]) {
			// row bias
			return child_derivative.reduce(mt_mat::Reduce_Type_Sum, 0);
		} else {
			if (derivative_res.depth() == mt_F32) {
				return private_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else if (derivative_res.depth() == mt_F64) {
				return private_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else {
				basiclog_unsupport2();
				return mt_mat();
			}
		}
	}
}

mt_mat mt_ad_div_bias_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (m_childs[0] == child_node) {
		return derivative_res.div_bias(m_childs[1]->to_mat_tree_node()->m_mat);
	} else {
		b8 is_unit_bias = sys_true;

		const mt_mat& child_node_mat = child_node->to_mat_tree_node()->m_mat;

		for (i32 i = 0; i < child_node_mat.dim(); ++i) {
			if (child_node_mat.size()[i] != 1) {
				is_unit_bias = sys_false;
				break;
			}
		}

		mt_mat child_derivative = -derivative_res * m_childs[0]->to_mat_tree_node()->m_mat;

		if (is_unit_bias) {
			child_derivative = child_derivative.reduce(mt_mat::Reduce_Type_Sum);
		}

		if (child_node_mat.dim() == 2 && child_node_mat.size()[0] == 1 && child_node_mat.size()[1] == derivative_res.size()[1]) {
			// row bias
			child_derivative = child_derivative.reduce(mt_mat::Reduce_Type_Sum, 0);
		} else {
			if (derivative_res.depth() == mt_F32) {
				child_derivative = private_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else if (derivative_res.depth() == mt_F64) {
				child_derivative = private_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else {
				basiclog_unsupport2();
			}
		}

		return child_derivative / m_childs[1]->to_mat_tree_node()->m_mat.pow(2.0);
	}
}

mt_mat mt_ad_repeat_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (derivative_res.depth() == mt_F32) {
		return private_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
	} else if (derivative_res.depth() == mt_F64) {
		return private_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
	} else {
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_dot_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	vector<mt_mat> other_mats;
	other_mats.reserve(m_childs.size());

	vector<f64> constant_value;
	constant_value.push_back(1);

	for (i32 i = 0; i < (i32)m_childs.size(); ++i) {
		if (m_childs[i] != child_node) {
			if (m_childs[i]->to_mat_tree_node() != NULL) {
				other_mats.push_back(m_childs[i]->to_mat_tree_node()->m_mat);
			} else {
				constant_value = m_childs[i]->to_const_leaf_tree_node()->m_values;
				basiclog_assert2((int)m_childs.size() == 2);
				break;
			}
		}
	}

	if (other_mats.empty()) {
		return derivative_res * constant_value;
	} else {
		return mt_mat_helper::dot(other_mats, sys_false) * derivative_res;
	}
}

mt_mat mt_ad_div_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (child_node == m_childs[0]) {
		if (m_childs[1]->to_mat_tree_node() != NULL) {
			return derivative_res / m_childs[1]->to_mat_tree_node()->m_mat;
		} else {
			return derivative_res / m_childs[1]->to_const_leaf_tree_node()->m_values;
		}
	} else if (child_node == m_childs[1]) {
		if (m_childs[0]->to_mat_tree_node() != NULL) {
			return -derivative_res * m_childs[0]->to_mat_tree_node()->m_mat / m_childs[1]->to_mat_tree_node()->m_mat.pow(2.0);
		} else {
			return -derivative_res * m_childs[0]->to_const_leaf_tree_node()->m_values / m_childs[1]->to_mat_tree_node()->m_mat.pow(2.0);
		}
	} else {
		basiclog_assert2(sys_false);
		return mt_mat();
	}
}

mt_mat mt_ad_unpooling_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	mt_Pooling_Type type = m_type;
	if (type == mt_Pooling_Type_Max || type == mt_Pooling_Type_Min) {
		type = mt_Pooling_Type_Index;
	}
	
	return derivative_res.pooling(m_mask, type, (i32)m_kernel_sizes.size(), &m_kernel_sizes[0], &m_strides[0]);
}

mt_mat mt_ad_sub_stride_tree_node::derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	mt_mat child_derivative = mt_mat(child_node->m_mat, 0);

	mt_mat temp = child_derivative.sub_stride((i32)m_strides.size(), &m_strides[0]);
	temp.set(derivative_res);

	return child_derivative;
}

mt_mat mt_ad_conv_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	sys_timer timer(L"mt_ad_conv_tree_node::derivate_child_on_operation", sys_false);
	timer.begin();

	mt_mat derivate_mat = derivative_res;
	
	if (!m_strides.empty()) {
		basicmath_mat_request_memory(i32, valid_conv_sizes, derivate_mat.dim());
		mt_mat_helper::get_conv_result_size(derivate_mat.dim(), valid_conv_sizes, m_childs[0]->to_mat_tree_node()->m_mat.size(), m_childs[1]->to_mat_tree_node()->m_mat.size(), NULL, mt_Conv_Boundary_Type_Valid);
		derivate_mat = derivative_res.unpooling(valid_conv_sizes, mt_mat(), mt_Pooling_Type_First_Value, (i32)m_strides.size(), &m_strides[0], &m_strides[0]);
		basicmath_mat_release(valid_conv_sizes);
	}

	if (child_node == m_childs[0]) {
		return derivate_mat.conv(m_childs[1]->to_mat_tree_node()->m_mat.flip_all_dim(), mt_Conv_Boundary_Type_Full, mt_Conv_Kernel_Align_Type_Col);
	}

	if (child_node == m_childs[1]) {
		return m_childs[0]->to_mat_tree_node()->m_mat.flip_all_dim().conv(derivate_mat, mt_Conv_Boundary_Type_Valid, mt_Conv_Kernel_Align_Type_Cross_Row);
	}

	basiclog_assert2(sys_false);
	return mt_mat();
}

mt_mat mt_ad_log_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	mt_mat child_derivative = 1 / child_node->m_mat;
	
	if (mt_helper::compare_double(m_base, mt_E) != 0) {
		child_derivative *= log(m_base);	
	}

	return child_derivative * derivative_res;
}

mt_mat mt_ad_abs_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	mt_mat element_compared_mask = child_node->to_mat_tree_node()->m_mat.element_compared_mask(mt_mat(), mt_mat::Compare_Task_Non_Smaller, 1.0, -1.0);

	return derivative_res * element_compared_mask;
}

mt_mat mt_ad_activate_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (m_mix_loss_derivative) {
		basiclog_assert2(m_loss_type == mt_Loss_Type_Logarithmic);
		return derivative_res;
	} else {
		switch (m_activate_type) {
		case mt_Activate_Type_Linear:
			return derivative_res;
		case mt_Activate_Type_Sigmoid:
			return derivative_res * (1 - m_mat) * m_mat;
		case mt_Activate_Type_Tanh:

			f64 alpha_square_inver;
			f64 alpha_mul_belta;

			if (m_activated_params.empty()) {
				alpha_square_inver = 1 / (mt_Tanh_Default_Alpha * mt_Tanh_Default_Alpha);
				alpha_mul_belta = mt_Tanh_Default_Alpha * mt_Tanh_Default_Beta;
			} else {
				alpha_square_inver = 1 / (m_activated_params[0] * m_activated_params[0]);
				alpha_mul_belta = m_activated_params[0] * m_activated_params[1];
			}

			return alpha_mul_belta * (1 - m_mat.pow(2.0) * alpha_square_inver) * derivative_res;
		case mt_Activate_Type_Softmax:
			return derivative_res * softmax_derivate(m_mat);
		case mt_Activate_Type_Relu:
			return derivative_res * relu_derivate(child_node->m_mat);
		default:
			basiclog_unsupport2();
			return mt_mat();
		}
	}
}

mt_mat mt_ad_activate_tree_node::softmax_derivate(const mt_mat& softmax_res) {
	basiclog_assert2(softmax_res.dim() == 2);
	basiclog_assert2(softmax_res.channel() == 1);

	if (softmax_res.depth() == mt_F32) {
		return private_ad_helper::softmax_derivate<f32>(softmax_res);
	} else if (softmax_res.depth() == mt_F64) {
		return private_ad_helper::softmax_derivate<f64>(softmax_res);
	} else {
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_activate_tree_node::relu_derivate(const mt_mat& softmax_res) {
	if (softmax_res.depth() == mt_F32) {
		return private_ad_helper::relu_derivate<f32>(softmax_res, m_activated_params.empty() ? 0 : m_activated_params[0]);
	} else if (softmax_res.depth() == mt_F64) {
		return private_ad_helper::relu_derivate<f64>(softmax_res, m_activated_params.empty() ? 0 : m_activated_params[0]);
	} else {
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_loss_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	f64 loss_derivative_value = derivative_res.get(0, 0)[0];
	
	//basiclog_info2(loss_derivative_value);

	switch (m_loss_type) {
	case mt_Loss_Type_Logarithmic:
		
		if (child_node == m_childs[0]) {
			if (derivative_res.depth() == mt_F32) {
				return private_ad_helper::logarithmic_loss_derivative<f32>(m_childs[0]->to_mat_tree_node()->m_mat, m_childs[1]->to_mat_tree_node()->m_mat) * loss_derivative_value;
			} else if (derivative_res.depth() == mt_F64) {
				return private_ad_helper::logarithmic_loss_derivative<f64>(m_childs[0]->to_mat_tree_node()->m_mat, m_childs[1]->to_mat_tree_node()->m_mat) * loss_derivative_value;
			} else {
				basiclog_unsupport2();
			}

		} else if (child_node == m_childs[1]) {
			basiclog_assert2(sys_false);
			return mt_mat();
		}

	case mt_Loss_Type_Quadratic:
		
		if (child_node == m_childs[0]) {
			return (m_childs[0]->to_mat_tree_node()->m_mat - m_childs[1]->to_mat_tree_node()->m_mat) * loss_derivative_value;
		} else if (child_node == m_childs[1]) {
			basiclog_assert2(sys_false);
			return mt_mat();
		}
	default:
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_fake_loss_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	return (m_childs[0]->to_mat_tree_node()->m_mat - m_childs[1]->to_mat_tree_node()->m_mat) * derivative_res.get(0, 0)[0];
}

mt_mat mt_ad_merge_align_channel_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	i32 start_channel = 0;

	for (i32 i = 0; i < (i32)m_childs.size(); ++i) {
		i32 child_channel = m_childs[i]->to_mat_tree_node()->m_mat.channel();

		if (m_childs[i] == child_node) {
			return derivative_res.sub_channel(start_channel, start_channel + child_channel).clone();
		}

		start_channel += child_channel;
	}

	basiclog_assert2(sys_false);
	return mt_mat();
}