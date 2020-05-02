#include "stdafx.h"
#include "mt_auto_derivative_tree.h"


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
				return mt_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else if (derivative_res.depth() == mt_F64) {
				return mt_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
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
				return -mt_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
			} else if (derivative_res.depth() == mt_F64) {
				return -mt_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
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
				return mt_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else if (derivative_res.depth() == mt_F64) {
				return mt_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, child_derivative);
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
				child_derivative = mt_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else if (derivative_res.depth() == mt_F64) {
				child_derivative = mt_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, child_derivative);
			} else {
				basiclog_unsupport2();
			}
		}

		return child_derivative / m_childs[1]->to_mat_tree_node()->m_mat.pow(2.0);
	}
}

mt_mat mt_ad_repeat_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	if (derivative_res.depth() == mt_F32) {
		return mt_ad_helper::repeat_derivative<f32>(child_node->to_mat_tree_node()->m_mat, derivative_res);
	} else if (derivative_res.depth() == mt_F64) {
		return mt_ad_helper::repeat_derivative<f64>(child_node->to_mat_tree_node()->m_mat, derivative_res);
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
	sys_timer timer("mt_ad_conv_tree_node::derivate_child_on_operation", sys_false);
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
		if (m_activate_type == mt_Activate_Type_Linear) {
			return derivative_res;
		} else {
			return derivative_res * mt_ad_helper::activate_derivative(m_mat, m_activate_type, m_activated_params);
		}
	}
}

mt_mat mt_ad_loss_tree_node::derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
	f64 loss_derivative_value = derivative_res.get(0, 0)[0];
	
	//basiclog_info2(loss_derivative_value);

	switch (m_loss_type) {
	case mt_Loss_Type_Logarithmic:
		
		if (child_node == m_childs[0]) {
			if (derivative_res.depth() == mt_F32) {
				return mt_ad_helper::logarithmic_loss_derivative<f32>(m_childs[0]->to_mat_tree_node()->m_mat, m_childs[1]->to_mat_tree_node()->m_mat) * loss_derivative_value;
			} else if (derivative_res.depth() == mt_F64) {
				return mt_ad_helper::logarithmic_loss_derivative<f64>(m_childs[0]->to_mat_tree_node()->m_mat, m_childs[1]->to_mat_tree_node()->m_mat) * loss_derivative_value;
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

















mt_mat mt_ad_helper::softmax_derivate(const mt_mat& src) {
	basiclog_assert2(src.dim() == 2);
	basiclog_assert2(src.channel() == 1);

	if (src.depth() == mt_F32) {
		return mt_ad_helper::softmax_derivate<f32>(src);
	} else if (src.depth() == mt_F64) {
		return mt_ad_helper::softmax_derivate<f64>(src);
	} else {
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_helper::relu_derivate(const mt_mat& src, const vector<f64>& activated_params) {
	if (src.depth() == mt_F32) {
		return mt_ad_helper::relu_derivate<f32>(src, activated_params.empty() ? 0 : activated_params[0]);
	} else if (src.depth() == mt_F64) {
		return mt_ad_helper::relu_derivate<f64>(src, activated_params.empty() ? 0 : activated_params[0]);
	} else {
		basiclog_unsupport2();
		return mt_mat();
	}
}

mt_mat mt_ad_helper::activate_derivative(const mt_mat& src, mt_Activate_Type activate_type, const vector<f64>& activated_params) {
	switch (activate_type)
	{
	case mt_Activate_Type_Linear:
		return mt_mat(src, mt_mat::Construct_Type_Operator_Equal).set(1);
	case mt_Activate_Type_Sigmoid:
		return (1 - src) * src;
	case mt_Activate_Type_Tanh:

		f64 alpha_square_inver;
		f64 alpha_mul_belta;

		if (activated_params.empty()) {
			alpha_square_inver = 1 / (mt_Tanh_Default_Alpha * mt_Tanh_Default_Alpha);
			alpha_mul_belta = mt_Tanh_Default_Alpha * mt_Tanh_Default_Beta;
		} else {
			alpha_square_inver = 1 / (activated_params[0] * activated_params[0]);
			alpha_mul_belta = activated_params[0] * activated_params[1];
		}

		return alpha_mul_belta * (1 - src.pow(2.0) * alpha_square_inver);
	case mt_Activate_Type_Relu:
		return relu_derivate(src, activated_params);
	case mt_Activate_Type_Softmax:
		return softmax_derivate(src);
	default:
		return mt_mat();
	}
}