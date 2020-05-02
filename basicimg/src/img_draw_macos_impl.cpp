#ifndef _WIN32

#include "stdafx.h"
#include "img_draw.h"

static mt_mat s_helper_image;
static ULONG_PTR s_gdiplusToken;

cv_font::cv_font() {
	m_font_name = ("ËÎÌו");
	m_size = 20;
	m_italic = false;
	m_bold = false;
	m_under_line = false;
}

cv_font::cv_font(const string& name, int size, b8 italic, b8 bold, b8 under_line) {
	m_font_name = name;
	m_size = size;
	m_italic = italic;
	m_bold = bold;
	m_under_line = under_line;
}

cv_pen::cv_pen() {
	m_dash_style = Dash_Type_Solid;
	m_thickness = 1;
	m_color = mt_scalar(0, 0, 0);
}

cv_pen::cv_pen(const mt_scalar& color, int dash_style, int thickness) {
	m_dash_style = dash_style;
	m_thickness = thickness;
	m_color = color;
}

void cv_pen::create(const mt_scalar& color, int dash_style, int thickness) {
	m_dash_style = dash_style;
	m_thickness = thickness;
	m_color = color;
}


img_draw::img_draw(mt_mat& image) {
	
}

img_draw::img_draw(void* draw_engine) {
}

img_draw::~img_draw() {
}

void img_draw::end_draw() {
}

void img_draw::reset_transform() {
}

void img_draw::translate_transform(const mt_point& orign) {
}

void img_draw::set_clip(const mt_rect& rect) {
}

void img_draw::draw_point(const mt_point& start, int size, const mt_scalar& color) {

}

void img_draw::draw_line(const mt_point& start, const mt_point& stop, const cv_pen& pen) {

}

void img_draw::draw_rectangle(const mt_rect& rect, const cv_pen& pen) {

}

void img_draw::fill_rectangle(const mt_rect& rect, const cv_brush* brush) {

}

void img_draw::fill_rectangle(const mt_rect& rect, const mt_scalar& color) {

}


void img_draw::draw_curve(vector<mt_point>& points, const cv_pen& pen, b8 closed /* = false */) {

}

void img_draw::draw_curve(vector<mt_point2f>& points, const cv_pen& pen, b8 closed /* = false */) {

}

void img_draw::drawDirection(const mt_point& start, int angle, int length, const mt_scalar& color, int thicknes) {	
	//mt_point stop;
	//stop.x = ml_math_define::neibourDouble(start.x + length * ml_math_define::getCosValue(angle));
	//stop.y = ml_math_define::neibourDouble(start.y - length * ml_math_define::getSinValue(angle));

	//line(colorImage, start, stop, color, thicknes);
	//circle(colorImage, stop, 4, color, thicknes);
}

void img_draw::drawDirection(const mt_point& start, const mt_point& stop, const mt_scalar& color, int thicknes) {	
	//line(colorImage, start, stop, color, thicknes);
	//circle(colorImage, stop, 4, color, thicknes);
}

mt_size img_draw::measure_text_size(const string& text, const cv_font& font, int format) {
	
}

void img_draw::draw_text(const string& text, const mt_rect& rect, const cv_font& font, const mt_scalar& color, int horizontal_align_type, int vertical_align_type, int format) {
	
}

void img_draw::draw_text(const string& text, const mt_point& offset, const cv_font& font, const mt_scalar& color, int horizontal_align_type, int vertical_align_type, int format) {
	
}


#endif