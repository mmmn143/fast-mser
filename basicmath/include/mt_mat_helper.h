#pragma once

#include "mt_mat.h"

namespace basicmath {

	class mt_mat_helper {
	public:

		enum Math_Op_Code {
			Math_Op_Code_Add,
			Math_Op_Code_Subtract,
			Math_Op_Code_Dot_Mul,
			Math_Op_Code_Div,
			Math_Op_Code_Pow,
			Math_Op_Code_Exp,
			Math_Op_Code_Log,
			Math_Op_Code_Ln,
			Math_Op_Code_Abs,
		};

		static void max_out(vector<mt_mat>& res, vector<mt_mat>& max_indexes, const vector<mt_mat>& src, int k);
		static void restore_max_out(vector<mt_mat>& src, int src_number, const vector<mt_mat>& max_res, vector<mt_mat>& max_indexes);

		static int get_conv_result_size(int src_size, int kernel_size, int stride, mt_Conv_Boundary_Type boundary_type);
		static void get_conv_result_size(i32 dims, i32* sizes, const i32* src_sizes, const i32* kernel_sizes, const i32* stride_sizes, mt_Conv_Boundary_Type boundary_type);

		static int get_pooling_result_size(int src_size, int kernel_size, int stride);
		static void get_pooling_result_size(i32 dims, i32* res_sizes, const i32* src_sizes, const i32* kernel_sizes, const i32* stride_sizes);

		static mt_mat add(const vector<mt_mat>& elements, b8 can_share_memory = sys_false);

		static mt_mat dot(const vector<mt_mat>& elements, b8 can_share_memory = sys_false);

		static void parallel_pooling(vector<mt_mat>& pooling_res, const vector<mt_mat>& srcs);

		static mt_mat merge_align_dim(const vector<mt_mat>& elements, i32 dim, b8 can_share_memory = sys_false);
		static mt_mat merge_align_channel(const vector<mt_mat>& elements, b8 can_share_memory = sys_false);

		static void clone(vector<mt_mat>& dsts, const vector<mt_mat>& srcs);

		static void save(const wstring& file_path, const mt_mat& mat, b8 text_file = sys_true);
		static void save(sys_buffer_writer* writer, const mt_mat& mat);
		static mt_mat load(const wstring& file_path, b8 text_file = sys_true);
		static mt_mat load(sys_buffer_reader* reader);

		static b8 need_omp_for_multi_channel(const mt_mat& mat);

		protected:

			static mt_auto_derivative* check_mat_array_auto_derivative(const vector<mt_mat>& elements);

	//static 
	};
}