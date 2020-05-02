/** @file basicmath.h

Export head file.
Other project should include this head file to use the features of BasicMath. 
*/

#pragma once

#include "mt_data_type.h"
#include "mt_helper.h"
#include "mt_point_t.h"
#include "mt_rect_t.h"
#include "mt_scalar_t.h"
#include "mt_range_t.h"

#include "mt_mat.h"
#include "mt_mat_t.h"
#include "mt_mat_helper.h"
#include "mt_auto_derivative.h"
#include "mt_random.h"
#include "mt_hist.h"
#include "mt_sort.h"
#include "mt_trigonometric_function.h"
#include "mt_top_k.h"
#include "dynamic_programming/mt_01_bag.h"
#include "dynamic_programming/mt_max_sum_for_continuous_element.h"
#include "dynamic_programming/mt_longest_common_substring.h"

#include "mt_block_memory.h"


#if defined basicsys_enable_mkl
#pragma comment(lib, "mkl_core.lib")
#pragma comment(lib, "mkl_intel_lp64.lib")
#pragma comment(lib, "mkl_intel_thread.lib")
#pragma comment(lib, "libiomp5md.lib")
#else 
#endif