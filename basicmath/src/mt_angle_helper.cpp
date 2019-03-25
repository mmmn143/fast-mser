#include "stdafx.h"

#include "mt_angle_helper.h"

static const f64 radian_to_angle = 180.0 / mt_PI;
static const f64 angle_to_radian = mt_PI / 180.0;

f64 mt_angle_helper::radian_from_angle(f64 angle) {
	return angle * angle_to_radian;
}

f64 mt_angle_helper::angle_from_radian(f64 radian) {
	return radian * radian_to_angle;
}