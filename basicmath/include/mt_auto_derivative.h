#pragma once

#include "mt_auto_derivative_tree.h"
#include "mt_mat_cache.h"

namespace basicmath {

	class mt_mat;



	class mt_auto_derivative {
	public:
		mt_auto_derivative(i32 max_cache_size = 30) {
			m_enable_math_operation = sys_true;
			m_max_cache_size = max_cache_size;
		}

		~mt_auto_derivative();
		
		void attach(mt_mat& mat);
		void attach(vector<mt_mat>& mats);

		static void detach(mt_mat& mat);
		static void detach(vector<mt_mat>& mats);
		
		/** res = derivative src / derivative target
		*/
		mt_mat derivative(const mt_mat& target, const mt_mat& src);
		mt_mat derivative(const mt_mat& target, const vector<mt_mat>& srcs);

		void derivative(vector<mt_mat>& reses, const vector<mt_mat>& targets, const mt_mat& src);

		void derivative(vector<mt_mat>& reses, const vector<mt_mat>& targets, const vector<mt_mat>& srcs);

		vector<mt_mat> derivative(vector<mt_mat>& targets, const mt_mat& src);

		vector<mt_mat> derivative(vector<mt_mat>& targets, const vector<mt_mat>& srcs);


		void clone(const mt_mat& res, const mt_mat& src);


		/**
		@note all variable must be represented as mt_mat, here b is a const value!
		*/
		void add(const mt_mat& res, const mt_mat& a, const vector<double>& b);
		void add(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		

		void add(const mt_mat& res, const vector<mt_mat>& elements);
		
		void add_bias(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		void substract_bias(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		void mul_bias(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		void div_bias(const mt_mat& res, const mt_mat& a, const mt_mat& b);

		void dot(const mt_mat& res, const mt_mat& a, const vector<double>& b);
		void dot(const mt_mat& res, const mt_mat& a, const mt_mat& b);

		void dot(const mt_mat& res, const vector<mt_mat>& elements);

		void div(const mt_mat& res, const vector<double>& b, const mt_mat& a);
		void div(const mt_mat& res, const mt_mat& a, const vector<double>& b);
		void div(const mt_mat& res, const mt_mat& a, const mt_mat& b);


		void subtract(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		void subtract(const mt_mat& res, const mt_mat& a,  const vector<double>& b);
		void subtract(const mt_mat& res, const vector<double>& a, const mt_mat& b);
		
		void mul(const mt_mat& res, const mt_mat& a, const mt_mat& b);
		void cnov(const mt_mat& res, const mt_mat& src, const mt_mat& kernel, mt_Conv_Boundary_Type boundary_type, const i32* strides);

		void flip(const mt_mat& res, const mt_mat& src, i32 size, const b8* flip_flags);
		void reshape(const mt_mat& res, const mt_mat& src);
		void sub(const mt_mat& res, const mt_mat& a, i32 size, const mt_range* ranges);
		void sub_stride(const mt_mat& res, const mt_mat& a, i32 size, const i32* strides);
		void sub_channel(const mt_mat& res, const mt_mat& a, i32 start_channel, i32 opened_stop_channel);

		void expand(const mt_mat& res, const mt_mat& a, i32 size, const mt_range* ranges);
		void repeat(const mt_mat& res, const mt_mat& a);

		void pooling(const mt_mat& res, const mt_mat& mask_mat, const mt_mat& src, mt_Pooling_Type type, i32 size, const i32* kernel_sizes, const i32* stride_sizes);

		void merge_align_channel(const mt_mat& res, const vector<mt_mat>& elements);
		void merge_align_dim(const mt_mat& res, const vector<mt_mat>& elements, i32 dim);

		void activate(const mt_mat& res, const mt_mat& src, mt_Activate_Type type, const vector<f64>& activate_params);
		void loss(const mt_mat& res, const mt_mat& predicted_mat, const mt_mat& matching_mat, mt_Loss_Type type);

		void exp(const mt_mat& res, const mt_mat& src);
		void pow(const mt_mat& res, const mt_mat& src, f64 number);
		void log(const mt_mat& res, const mt_mat& src, f64 base);
		void abs(const mt_mat& res, const mt_mat& src);

		/**
		@param enable = sys_true indicates the auto_derivative instance will record the math operation in the derivative tree.
		*/
		void record_math_operation(b8 enable);

		b8 math_operation_recorded() const; 

		void reset();

	protected:

		void find_node_in_parent(vector<mt_ad_tree_node*>& parents, const mt_ad_tree_node* node) const;

		void derivative(mt_ad_tree_node* target, const mt_ad_tree_node* src, const mt_mat& src_mat);

		mt_ad_tree_node* find_same(const mt_mat& mat) const;

		mt_ad_tree_node* get_node(const mt_mat& mat);
		mt_ad_tree_node* get_node(const vector<double>& const_value);

		void derivative(mt_mat& derivated_mat, const mt_mat& target_mat, const mt_mat& src_mat, mt_ad_tree_node* src_node, const mt_ad_tree_node* src_parent_node);
		//void derivate(mt_mat& derivated_mat, const mt_mat& target_mat, mt_ad_mat_tree_node* target_nodes, mt_ad_tree_node* src_node);

		int find_unused_derivated_mat(mt_ad_mat_tree_node* target_node);

		b8 is_need_derivate(const mt_mat& target_mat, mt_ad_tree_node* child_node) const;

		vector<mt_ad_tree_node*> m_nodes;
		b8 m_computed_flag;
		b8 m_enable_math_operation;
		i32 m_max_cache_size;

		vector<mt_auto_derivative**> m_self_references;  
	};
}