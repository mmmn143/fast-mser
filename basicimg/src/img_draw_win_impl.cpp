#ifdef _WIN32

#include <Windows.h>
#include <GdiPlus.h>

#include "stdafx.h"
#include "img_draw.h"




static mt_mat s_helper_image;
static ULONG_PTR s_gdiplusToken;

namespace basicimg {
	class private_draw {
	public:

		static Gdiplus::Point from_point(const mt_point& pt) {
			return Gdiplus::Point(pt.m_x, pt.m_y);
		}

		static Gdiplus::Rect from_rect(const mt_rect& rect) {
			return Gdiplus::Rect(rect.m_left, rect.m_top, rect.m_width, rect.m_height);
		}

		static Gdiplus::Graphics* from_engineer(void* engineer) {
			return (Gdiplus::Graphics*)engineer;
		}

		static Gdiplus::Color real_color(const mt_scalar& color, const img_draw* draw) {
			mt_scalar res;
			if (1 == draw->m_image.channel()) {
				res = img_img::cvt_color(color, img_img::Color_Covert_Type_GRAY2BGRA);
			} else {
				res = color;

				if (3 == draw->m_image.channel()) {
					res[3] = 255;
				}
			}

			return Gdiplus::Color((byte)res[3], (byte)res[2], (byte)res[1], (byte)res[0]);
		}

		static Gdiplus::Brush* from_brush(const cv_brush* brush, const img_draw* draw) {
			Gdiplus::Brush* gdibrush = NULL;
			if (typeid(*brush) == typeid(cv_solid_brush)) {
				cv_solid_brush* solid = (cv_solid_brush*)brush;

				gdibrush = new Gdiplus::SolidBrush(real_color(solid->m_color, draw));
			}

			return gdibrush;
		}

	};
}



class auto_gdiplus {
public:

	auto_gdiplus() {
		Gdiplus::GdiplusStartupInput StartupInput;  
		GdiplusStartup(&s_gdiplusToken,&StartupInput,NULL);  
	}

	~auto_gdiplus() {
		Gdiplus::GdiplusShutdown(s_gdiplusToken);  
	}
};

static auto_gdiplus s_gdiplus_helper;

cv_font::cv_font() {
	m_font_name = ("宋体");
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

int   GetEncoderClsid(const   WCHAR*   format,   CLSID*   pClsid)   
{   
	UINT     num   =   0;                     //   number   of   image   encoders   
	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes   

	Gdiplus::ImageCodecInfo*   pImageCodecInfo   =   NULL;   

	Gdiplus::GetImageEncodersSize(&num,   &size);   
	if(size   ==   0)   
		return   -1;     //   Failure   

	pImageCodecInfo   =   (Gdiplus::ImageCodecInfo*)(malloc(size));   
	if(pImageCodecInfo   ==   NULL)   
		return   -1;     //   Failure   

	Gdiplus::GetImageEncoders(num,   size,   pImageCodecInfo);   

	for(UINT   j   =   0;   j   <   num;   ++j)   
	{   
		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   )   
		{   
			*pClsid   =   pImageCodecInfo[j].Clsid;   
			free(pImageCodecInfo);   
			return   j;     //   Success   
		}           
	}   

	free(pImageCodecInfo);   
	return   -1;     //   Failure   
}   

img_draw::img_draw(mt_mat& image) {
	m_image = image;

	int pixel_format;
	if (1 == image.channel()) {
		m_temp = img_img::cvt_color(m_image, img_img::Color_Covert_Type_GRAY2BGRA);
		pixel_format = PixelFormat32bppARGB;
		m_cvt_code = img_img::Color_Covert_Type_BGRA2Gray;
	} else if (3 == image.channel()) {
		if (image.step()[0] % 4 != 0) {
			m_temp = img_img::cvt_color(m_image, img_img::Color_Covert_Type_BGR2BGRA);

			pixel_format = PixelFormat32bppARGB;
			m_cvt_code = img_img::Color_Covert_Type_BGRA2BGR;
		} else {
			pixel_format = PixelFormat24bppRGB;
			m_temp = m_image;
		}
	} else if (4 == image.channel()) {
		m_temp = image;
		pixel_format = PixelFormat32bppARGB;
	}

	Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(m_temp.size()[1], m_temp.size()[0], m_temp.step()[0], pixel_format, m_temp.data());

	m_draw_engineer = Gdiplus::Graphics::FromImage(bitmap);
	m_draw_map = bitmap;
}

img_draw::img_draw(void* draw_engine) {
	m_draw_engineer = draw_engine;
}

img_draw::~img_draw() {
	if (!m_image.is_empty()) {
		delete (Gdiplus::Graphics*)m_draw_engineer;
		delete (Gdiplus::Bitmap*)m_draw_map;
	}
}

void img_draw::end_draw() {
	if (!m_image.is_empty() && m_temp.data() != m_image.data()) {
		m_image = img_img::cvt_color(m_temp, m_cvt_code);
	}
}

void img_draw::reset_transform() {
	private_draw::from_engineer(m_draw_engineer)->ResetTransform();
}

void img_draw::translate_transform(const mt_point& orign) {
	private_draw::from_engineer(m_draw_engineer)->TranslateTransform((float)orign.m_x, (float)orign.m_y);
}

void img_draw::set_clip(const mt_rect& rect) {
	private_draw::from_engineer(m_draw_engineer)->SetClip(private_draw::from_rect(rect));
}

void img_draw::draw_point(const mt_point& start, int size, const mt_scalar& color) {
	cv_solid_brush color_brush(color);
	Gdiplus::Brush* gdibrush = private_draw::from_brush(&color_brush, this);

	Gdiplus::Rect rect(start.m_x - size / 2, start.m_y - size / 2, size, size);
	private_draw::from_engineer(m_draw_engineer)->FillEllipse(gdibrush, rect);
	delete gdibrush;

	end_draw();
}

void img_draw::draw_line(const mt_point& start, const mt_point& stop, const cv_pen& pen) {
	Gdiplus::Pen gdipen(private_draw::real_color(pen.m_color, this), (float)pen.m_thickness);
	gdipen.SetDashStyle(Gdiplus::DashStyle(pen.m_dash_style));

	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(start), private_draw::from_point(stop));

	//CLSID clsid;
	//GetEncoderClsid("image/png", &clsid);
	//((Gdiplus::Bitmap*)m_draw_map)->Save("haha.png", &clsid);

	end_draw();
}

void img_draw::draw_rectangle(const mt_rect& rect, const cv_pen& pen) {
	Gdiplus::Pen gdipen(private_draw::real_color(pen.m_color, this), (float)pen.m_thickness);
	gdipen.SetDashStyle(Gdiplus::DashStyle(pen.m_dash_style));

	private_draw::from_engineer(m_draw_engineer)->DrawRectangle(&gdipen, private_draw::from_rect(rect));
	end_draw();
}

void img_draw::fill_rectangle(const mt_rect& rect, const cv_brush* brush) {
	Gdiplus::Brush* gdibrush = private_draw::from_brush(brush, this);

	private_draw::from_engineer(m_draw_engineer)->FillRectangle(gdibrush, private_draw::from_rect(rect));
	delete gdibrush;

	end_draw();
}

void img_draw::fill_rectangle(const mt_rect& rect, const mt_scalar& color) {
	Gdiplus::SolidBrush gdibrush(private_draw::real_color(color, this));

	private_draw::from_engineer(m_draw_engineer)->FillRectangle(&gdibrush, private_draw::from_rect(rect));

	end_draw();
}

//void img_draw::draw_box(const ml_box& box, const cv_pen& pen) {
//	Gdiplus::Pen gdipen(private_draw::real_color(pen.m_color, this), (float)pen.m_thickness);
//	gdipen.SetDashStyle(Gdiplus::DashStyle(pen.m_dash_style));
//
//	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(box.get_pts()[0]), private_draw::from_point(box.get_pts()[1]));
//	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(box.get_pts()[1]), private_draw::from_point(box.get_pts()[2]));
//	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(box.get_pts()[2]), private_draw::from_point(box.get_pts()[3]));
//	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(box.get_pts()[3]), private_draw::from_point(box.get_pts()[0]));
//
//	end_draw();
//}

//void img_draw::draw_polygon(const ml_polygon& polygon, const cv_pen& pen) {
//	Gdiplus::Pen gdipen(private_draw::real_color(pen.m_color, this), (float)pen.m_thickness);
//	gdipen.SetDashStyle(Gdiplus::DashStyle(pen.m_dash_style));
//
//	for (int i = 0; i < (int)polygon.get_pts().size() - 1; ++i) {
//		private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(polygon.get_pts()[i]), private_draw::from_point(polygon.get_pts()[i + 1]));
//	}
//
//	private_draw::from_engineer(m_draw_engineer)->DrawLine(&gdipen, private_draw::from_point(polygon.get_pts()[(int)polygon.get_pts().size() - 1]), private_draw::from_point(polygon.get_pts()[0]));
//
//	end_draw();
//}

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
	Gdiplus::Graphics gh(GetDesktopWindow());

	int font_style = Gdiplus::FontStyleRegular;

	if (font.m_italic) {
		font_style |= Gdiplus::FontStyleItalic;
	}

	if (font.m_bold) {
		font_style |= Gdiplus::FontStyleBold;
	}

	if (font.m_under_line) {
		font_style |= Gdiplus::FontStyleUnderline;
	}

	Gdiplus::Font gdifont(sys_strconvert::utf16_from_local(font.m_font_name).c_str(), (float)font.m_size, font_style);

	Gdiplus::RectF rect;
	Gdiplus::StringFormat str_format;

	str_format.SetFormatFlags(format);

	Gdiplus::RectF layout_rect(0, 0, FLT_MAX, FLT_MAX);

	gh.MeasureString(sys_strconvert::utf16_from_local(text).c_str(), (int)text.length(), &gdifont, layout_rect, &str_format, &rect);

	return mt_size((int)rect.Width + 1, (int)rect.Height + 1);
}

void img_draw::draw_text(const string& text, const mt_rect& rect, const cv_font& font, const mt_scalar& color, int horizontal_align_type, int vertical_align_type, int format) {
	int font_style = Gdiplus::FontStyleRegular;

	if (font.m_italic) {
		font_style |= Gdiplus::FontStyleItalic;
	}

	if (font.m_bold) {
		font_style |= Gdiplus::FontStyleBold;
	}

	if (font.m_under_line) {
		font_style |= Gdiplus::FontStyleUnderline;
	}

	Gdiplus::Font gdifont(sys_strconvert::utf16_from_local(font.m_font_name).c_str(), (float)font.m_size, font_style);
	Gdiplus::StringFormat str_format;

	str_format.SetAlignment((Gdiplus::StringAlignment)horizontal_align_type);
	str_format.SetLineAlignment((Gdiplus::StringAlignment)vertical_align_type);
	str_format.SetFormatFlags((Gdiplus::StringFormatFlags)format);

	Gdiplus::RectF gdirect((float)rect.m_left, (float)rect.m_top, (float)rect.m_width, (float)rect.m_height);

	Gdiplus::SolidBrush brush(private_draw::real_color(color, this));  
	private_draw::from_engineer(m_draw_engineer)->DrawString(sys_strconvert::utf16_from_local(text).c_str(), (int)text.length(), &gdifont, gdirect, &str_format, &brush);
	end_draw();
}

void img_draw::draw_text(const string& text, const mt_point& offset, const cv_font& font, const mt_scalar& color, int horizontal_align_type, int vertical_align_type, int format) {
	int font_style = Gdiplus::FontStyleRegular;

	if (font.m_italic) {
		font_style |= Gdiplus::FontStyleItalic;
	}

	if (font.m_bold) {
		font_style |= Gdiplus::FontStyleBold;
	}

	if (font.m_under_line) {
		font_style |= Gdiplus::FontStyleUnderline;
	}

	Gdiplus::Font gdifont(sys_strconvert::utf16_from_local(font.m_font_name).c_str(), (float)font.m_size, font_style);
	Gdiplus::StringFormat str_format;

	str_format.SetAlignment((Gdiplus::StringAlignment)horizontal_align_type);
	str_format.SetLineAlignment((Gdiplus::StringAlignment)vertical_align_type);
	str_format.SetFormatFlags((Gdiplus::StringFormatFlags)format);

	Gdiplus::PointF gdipoint((float)offset.m_x, (float)offset.m_y);

	Gdiplus::SolidBrush brush(private_draw::real_color(color, this));  
	private_draw::from_engineer(m_draw_engineer)->DrawString(sys_strconvert::utf16_from_local(text).c_str(), (int)text.length(), &gdifont, gdipoint, &str_format, &brush);
	end_draw();
}

#endif