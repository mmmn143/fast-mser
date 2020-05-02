#pragma once

#include "mt_mat.h"
#include "mt_mat_cache.h"

namespace basicmath {

	

	enum mt_Operation_Type {
		mt_Operation_Type_Const_Leaf,
		mt_Operation_Type_Mat_Leaf,

		mt_Operation_Type_Clone,

		mt_Operation_Type_Add,
		mt_Operation_Type_Subtract,
		mt_Operation_Type_Dot,
		mt_Operation_Type_Div,
		mt_Operation_Type_Add_Bias,
		mt_Operation_Type_Substract_Bias,
		mt_Operation_Type_Mul_Bias,
		mt_Operation_Type_Div_Bias,
		mt_Operation_Type_Pow,
		mt_Operation_Type_Exp,
		mt_Operation_Type_Log,
		mt_Operation_Type_Abs,

		mt_Operation_Type_Mul,
		mt_Operation_Type_Sub,
		mt_Operation_Type_Flip,
		mt_Operation_Type_Expand,
		mt_Operation_Type_Repeat,
		mt_Operation_Type_Reshape,
		mt_Operation_Type_Transpose,
		mt_Operation_Type_Pooling,
		mt_Operation_Type_Unpooling,
		mt_Operation_Type_Sub_Stride,
		mt_Operation_Type_Sub_Channel,
		mt_Operation_Type_Mat_Conv,
		mt_Operation_Type_Activate,
		mt_Operation_Type_Loss,
		mt_Operation_Type_Fake_Loss,

		mt_Operation_Type_Merge_Align_Channel,
		mt_Operation_Type_Merge_Align_Dim,
	};

	static const i32 mt_Auto_Derivative_Default_Max_Cache_Size = 30;

	class mt_ad_mat_tree_node;
	class mt_ad_const_leaf_tree_node;

	class mt_ad_tree_node {
	public:

		virtual ~mt_ad_tree_node() {}

		virtual mt_Operation_Type op_type() const = 0;

		virtual b8 match(const mt_mat& other) const = 0;
		virtual b8 related(const mt_mat& other) const = 0;

		virtual mt_mat derivate_child(mt_ad_mat_tree_node* child_node, const mt_mat& src) = 0;
		virtual mt_ad_mat_tree_node* to_mat_tree_node() {return NULL;}
		virtual mt_ad_const_leaf_tree_node* to_const_leaf_tree_node() {return NULL;}

		vector<mt_ad_tree_node*> m_childs;
	};

	class mt_ad_const_leaf_tree_node : public mt_ad_tree_node {
	public:

		mt_ad_const_leaf_tree_node(const vector<f64>& const_value) {
			m_values = const_value;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Const_Leaf;
		}

		b8 match(const mt_mat& other) const {
			return sys_false;
		}

		 b8 related(const mt_mat& other) const {
			 return sys_false;
		 }

		mt_mat derivate_child(mt_ad_mat_tree_node* child_node, const mt_mat& src) {return mt_mat();}
		mt_ad_const_leaf_tree_node* to_const_leaf_tree_node() {return this;}

		vector<f64> m_values;
	};

	class mt_ad_mat_tree_node : public mt_ad_tree_node {
	public:

		mt_ad_mat_tree_node(i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_max_cache_size = max_cache_size;
		}

		virtual ~mt_ad_mat_tree_node() {}

		b8 match(const mt_mat& other) const {
			return m_mat.is_same(other);
		}

		b8 related(const mt_mat& other) const {
			return m_mat.is_memory_shared(other);
		}

		mt_ad_mat_tree_node* to_mat_tree_node() {
			return this;
		}

		mt_mat derivate_child(mt_ad_mat_tree_node* child_node, const mt_mat& src) {
			mt_mat child_derivative = derivate_child_on_operation(child_node, derivative_res(src));

			if (!child_derivative.is_continuous()) {
				// make the mat to be continuous
				child_derivative = child_derivative.clone();
			}

			return child_derivative;
		}

		void init_construct(const mt_mat& res, mt_ad_tree_node* node) {
			m_mat = res;

			m_childs.push_back(node);
		}

		void init_construct(const mt_mat& res, mt_ad_tree_node* left, mt_ad_tree_node* right) {
			m_mat = res;

			m_childs.push_back(left);
			m_childs.push_back(right);
		}

		void add_derivative_res(const mt_mat& derivative_res, const mt_mat& src) {
			for (i32 i = 0; i < (i32)m_src_mats.size(); ++i) {
				if (m_src_mats[i].is_same(src)) {
					if (m_mat.is_same(src)) {
						return;
					} else {
						basiclog_assert2(sys_false);
					}
				}
			}

			m_derivated_mats.push_back(derivative_res);
			m_src_mats.push_back(src);

			if ((i32)m_derivated_mats.size() > m_max_cache_size) {
				m_derivated_mats.erase(m_derivated_mats.begin());
				m_src_mats.erase(m_src_mats.begin());
			}
		}

		void set_max_cache_size(i32 size) {
			m_max_cache_size = size;
		}

		mt_mat derivative_res(const mt_mat& src) {
			for (i32 i = 0; i < (i32)m_src_mats.size(); ++i) {
				if (m_src_mats[i].is_same(src)) {
					return m_derivated_mats[i];
				}
			}

			return mt_mat();
		}

		mt_mat m_mat;

		vector<mt_mat> m_derivated_mats;
		vector<mt_mat> m_src_mats;
		

	protected:

		i32 m_max_cache_size;

		virtual mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) = 0;
	};

	class mt_ad_leaf_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_leaf_tree_node(const mt_mat& mat) {
			m_mat = mat;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Mat_Leaf;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {return mt_mat();}
	};

	class mt_ad_clone_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_clone_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Clone;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res;
		}
	};

	class mt_ad_add_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_add_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);

			m_max_cache_size = max_cache_size;
		}

		mt_ad_add_tree_node(const mt_mat& res, vector<mt_ad_tree_node*>& elements, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs = elements;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Add;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res;
		}
	};

	class mt_ad_subtract_tree_node : public mt_ad_mat_tree_node {
	public:
		mt_ad_subtract_tree_node(const mt_mat& res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, left, right);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Subtract;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			mt_mat child_derivative = derivative_res;

			if (child_node == m_childs[0]) {
				return derivative_res;
			} else if (child_node == m_childs[1]) {
				return -derivative_res;
			} else {
				basiclog_assert_message2(sys_false, "input child_node is not in the m_childs!");
				return mt_mat();
			}
		}

	};

	class mt_ad_dot_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_dot_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);

			m_max_cache_size = max_cache_size;
		}

		mt_ad_dot_tree_node(const mt_mat& res, vector<mt_ad_tree_node*>& elements, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs = elements;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Dot;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_div_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_div_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);

			m_max_cache_size = max_cache_size;
		}

		mt_ad_div_tree_node(const mt_mat& res, vector<mt_ad_tree_node*>& elements, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs = elements;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Div;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_mul_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_mul_tree_node(const mt_mat& res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, left, right);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Mul;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			if (child_node == m_childs[0]) {
				return derivative_res.mul(m_childs[1]->to_mat_tree_node()->m_mat.t());
			} else if (child_node == m_childs[1]) {
				//basiclog_info2(m_childs[0]->to_mat_tree_node()->m_mat.t().mul(derivative_res));
				return m_childs[0]->to_mat_tree_node()->m_mat.t().mul(derivative_res);
			} else {
				basiclog_assert2(sys_false);
				return mt_mat();
			}
		}
	};

	class mt_ad_add_bias_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_add_bias_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Add_Bias;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_substract_bias_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_substract_bias_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Substract_Bias;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_mul_bias_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_mul_bias_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Substract_Bias;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_div_bias_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_div_bias_tree_node(const mt_mat& add_res, mt_ad_tree_node* left, mt_ad_tree_node* right, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(add_res, left, right);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Substract_Bias;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};


	class mt_ad_sub_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_sub_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 size, const mt_range* ranges, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			mt_helper::vec_from_array(m_ranges, size, ranges);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Sub;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			mt_mat child_derivative = mt_mat(child_node->m_mat, 0);

			child_derivative.sub(m_ranges).set(derivative_res);

			return child_derivative;
		}

		vector<mt_range> m_ranges;
	};

	class mt_ad_sub_stride_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_sub_stride_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 size, const i32* strides, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			mt_helper::vec_from_array(m_strides, size, strides);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Sub_Stride;
		}

		

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		vector<i32> m_strides;
	};

	class mt_ad_sub_channel_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_sub_channel_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 start_channel, i32 opened_stop_channel, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_start_channel = start_channel;
			m_opened_stop_channel = opened_stop_channel;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Sub_Channel;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			mt_mat child_derivative = mt_mat(child_node->m_mat, 0);

			child_derivative.sub_channel(m_start_channel, m_opened_stop_channel).set(derivative_res);

			return child_derivative;
		}

		i32 m_start_channel;
		i32 m_opened_stop_channel;
	};

	class mt_ad_expand_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_expand_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 size, const mt_range* ranges, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			mt_helper::vec_from_array(m_ranges, size, ranges);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Sub;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			mt_mat child_derivative = mt_mat(child_node->m_mat, mt_mat::Construct_Type_Create_As_Size);

			child_derivative.set(derivative_res.sub(m_ranges));

			return child_derivative;
		}

		vector<mt_range> m_ranges;
	};

	class mt_ad_pooling_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_pooling_tree_node(const mt_mat& res, const mt_mat& mask_mat, mt_ad_tree_node* src, mt_Pooling_Type type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_type = type;
			m_mask = mask_mat;

			mt_helper::vec_from_array(m_kernel_sizes, size, kernel_sizes);
			mt_helper::vec_from_array(m_strides, size, strides);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Pooling;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res.unpooling(child_node->m_mat.size(), m_mask, m_type, (i32)m_kernel_sizes.size(), &m_kernel_sizes[0], &m_strides[0]);
		}

		mt_mat m_mask;
		mt_Pooling_Type m_type;
		vector<i32> m_kernel_sizes;
		vector<i32> m_strides;
	};

	class mt_ad_unpooling_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_unpooling_tree_node(const mt_mat& res, const mt_mat& mask_mat, mt_ad_tree_node* src, mt_Pooling_Type type, i32 size, const basicsys::i32* kernel_sizes, const basicsys::i32* strides, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_type = type;
			m_mask = mask_mat;

			mt_helper::vec_from_array(m_kernel_sizes, size, kernel_sizes);
			mt_helper::vec_from_array(m_strides, size, strides);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Unpooling;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		mt_mat m_mask;
		mt_Pooling_Type m_type;
		vector<i32> m_kernel_sizes;
		vector<i32> m_strides;
	};

	class mt_ad_repeat_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_repeat_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Repeat;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_flip_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_flip_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 size, const b8* flip_flags, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			mt_helper::vec_from_array(m_flip_flags, size, flip_flags);
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Flip;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res.flip(m_flip_flags).clone();
		}

		vector<b8> m_flip_flags;
	};

	class mt_ad_reshape_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_reshape_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);
			
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Reshape;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res.reshape(child_node->m_mat.dim(), child_node->m_mat.size(), child_node->m_mat.channel());
		}
	};

	/** Only support valid type conv
	*/
	class mt_ad_conv_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_conv_tree_node(const mt_mat& res, mt_ad_tree_node* src, mt_ad_tree_node* kernel, const i32* strides, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs.push_back(src);
			m_childs.push_back(kernel);

			if (strides != NULL) {
				mt_helper::vec_from_array(m_strides, res.dim(), strides);
			}

			if (mt_helper::is_vec_value<i32>(m_strides, 1)) {
				m_strides.clear();
			}

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Sub;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		vector<i32> m_strides;
	};

	class mt_ad_exp_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_exp_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Exp;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return m_mat * derivative_res;
		}

		vector<mt_range> m_ranges;
	};

	class mt_ad_pow_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_pow_tree_node(const mt_mat& res, mt_ad_tree_node* src, f64 number, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_number = number;
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Pow;
		}

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			return derivative_res * child_node->m_mat.pow(m_number - 1);
		}

	protected:

		f64 m_number;
	};

	class mt_ad_log_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_log_tree_node(const mt_mat& res, mt_ad_tree_node* src, f64 base, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_base = base;
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Log;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		f64 m_base;
	};

	class mt_ad_abs_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_abs_tree_node(const mt_mat& res, mt_ad_tree_node* src, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Abs;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_activate_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_activate_tree_node(const mt_mat& res, mt_ad_tree_node* src, mt_Activate_Type type, const vector<f64>& activated_params, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, src);

			m_activate_type = type;
			m_activated_params = activated_params;
			m_mix_loss_derivative = sys_false;
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Activate;
		}

		void set_mix_loss_derivative(mt_Loss_Type type) {
			m_mix_loss_derivative = sys_true;
			m_loss_type = type;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
		mt_mat softmax_derivate(const mt_mat& softmax_res);
		mt_mat relu_derivate(const mt_mat& relu_res);

		mt_Activate_Type m_activate_type;
		vector<f64> m_activated_params;
		b8 m_mix_loss_derivative;
		mt_Loss_Type m_loss_type;
	};

	class mt_ad_loss_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_loss_tree_node(const mt_mat& res, mt_ad_tree_node* data_1, mt_ad_tree_node* data_2, mt_Loss_Type type, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, data_1, data_2);

			m_loss_type = type;
			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Loss;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		mt_Loss_Type m_loss_type;
	};

	/** Calculate the fake loss derivative for mt_Loss_Type_Lograthimc
	*/
	class mt_ad_fake_loss_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_fake_loss_tree_node(const mt_mat& res, mt_ad_tree_node* predicted_node, mt_ad_tree_node* matching_node, mt_Loss_Type type, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			init_construct(res, predicted_node, matching_node);

			basiclog_assert2(type == mt_Loss_Type_Logarithmic);

			m_loss_type = type;
			m_max_cache_size = max_cache_size;

			basiclog_assert2(typeid(*predicted_node) == typeid(mt_ad_activate_tree_node));

			((mt_ad_activate_tree_node*)predicted_node)->set_mix_loss_derivative(type);
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Fake_Loss;
		}

	protected:

		mt_mat derivate_child_on_operation( mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);

		mt_Loss_Type m_loss_type;
	};

	class mt_ad_merge_align_channel_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_merge_align_channel_tree_node(const mt_mat& res, const vector<mt_ad_tree_node*>& nodes, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs = nodes;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Merge_Align_Channel;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res);
	};

	class mt_ad_merge_align_dim_tree_node : public mt_ad_mat_tree_node {
	public:

		mt_ad_merge_align_dim_tree_node(const mt_mat& res, const vector<mt_ad_tree_node*>& nodes, i32 dim, i32 max_cache_size = mt_Auto_Derivative_Default_Max_Cache_Size) {
			m_mat = res;
			m_childs = nodes;

			m_dim = dim;

			m_max_cache_size = max_cache_size;
		}

		mt_Operation_Type op_type() const {
			return mt_Operation_Type_Merge_Align_Dim;
		}

	protected:

		mt_mat derivate_child_on_operation(mt_ad_mat_tree_node* child_node, const mt_mat& derivative_res) {
			i32 dim_start_index = 0;
			
			for (i32 i = 0; i < (i32)m_childs.size(); ++i) {
				if (m_childs[i] == child_node) {
					return derivative_res.sub(dim_start_index, dim_start_index + m_childs[i]->to_mat_tree_node()->m_mat.size()[m_dim], m_dim);
				} else {
					dim_start_index += m_childs[i]->to_mat_tree_node()->m_mat.size()[m_dim];
				}
			}

			basiclog_assert2(sys_false);
			return mt_mat();
		}

		i32 m_dim;
	};

	class mt_ad_helper {
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

		static mt_mat softmax_derivate(const mt_mat& src);

		static mt_mat relu_derivate(const mt_mat& src, const vector<f64>& activated_params);

		static mt_mat activate_derivative(const mt_mat& src, mt_Activate_Type activate_type, const vector<f64>& activated_params);

	};
}