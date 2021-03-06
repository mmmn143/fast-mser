#pragma once

#include <basiclog.h>
using namespace basiclog;

#include <basicsys.h>
using namespace basicsys;



#if defined basicsys_enable_mkl
#include <mkl.h>
#else 
#endif


#include <stdio.h>
#include <stdarg.h>

#include <float.h>
#include <math.h>
#include <vector>
#include <map>
using namespace std;



#include "mt_data_type.h"

#include "mt_range_t.h"
#include "mt_rect_t.h"
#include "mt_scalar_t.h"

#include "mt_point_t.h"
#include "mt_size_t.h"

#include "mt_mat.h"
#include "mt_rect_t.h"
#include "mt_mat_t.h"

#include "mt_mat_helper.h"
#include "mt_array_iteration.h"

#include "mt_auto_derivative.h"
#include "mt_trigonometric_function.h"
#include "mt_angle_helper.h"
#include "mt_top_k.h"

using namespace basicmath;