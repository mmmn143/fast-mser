#pragma once

namespace basicimg {

}

#pragma warning(disable:4819)

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace cv::ml;

#include <basiclog.h>
using namespace basiclog;

#include <basicsys.h>
using namespace basicsys;

#include <basicmath.h>
using namespace basicmath;



#include "img_types.h"
#include "img_img.h"
#include "img_draw.h"
#include "img_canny.h"
#include "img_auto_threshold_canny.h"
#include "img_helper.h"

using namespace basicimg;