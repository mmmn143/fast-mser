#pragma once





namespace basicimg {

	class cv_font {
	public:

		cv_font();

		cv_font(const wstring& name, i32 size, b8 italic = sys_false, b8 bold = sys_false, b8 under_line = sys_false);

		wstring m_font_name;
		i32 m_size;
		b8 m_italic;
		b8 m_bold;
		b8 m_under_line;
	};

	class cv_pen {
	public:

		enum Dash_Style {
			Dash_Type_Solid = 0,
			Dash_Style_Dash = 1,
			Dash_Style_Dot = 2,
			Dash_Style_Dash_Dot = 3,
			Dash_Style_Dash_Dot_Dot = 4,
		};

		cv_pen();

		cv_pen(const mt_scalar& color, i32 dash_style = Dash_Type_Solid, i32 thickness = 1);

		void create(const mt_scalar& color, i32 dash_style = Dash_Type_Solid, i32 thickness = 1);

		mt_scalar m_color;
		i32 m_thickness;
		i32 m_dash_style;
	};

	class cv_brush {
	public:

		virtual ~cv_brush() {}

	protected:

		cv_brush() {}	
	};

	class cv_solid_brush : public cv_brush {
	public:

		cv_solid_brush(const mt_scalar& color) {
			m_color = color;
		}

		mt_scalar m_color;
	};

	class img_draw {
	public:

		img_draw(mt_mat& image);
		img_draw(void* draw_engine);

		~img_draw();

		void reset_transform();
		void translate_transform(const mt_point& orign);
		void set_clip(const mt_rect& rect);

		void draw_point(const mt_point& start, i32 size, const mt_scalar& color);
		void draw_line(const mt_point& start, const mt_point& stop, const cv_pen& pen);

		void draw_rectangle(const mt_rect& rect, const cv_pen& pen);
		void fill_rectangle(const mt_rect& rect, const cv_brush* brush);
		void fill_rectangle(const mt_rect& rect, const mt_scalar& color);

		//void draw_box(const mt_box& box, const cv_pen& pen);
		//void draw_polygon(const ml_polygon& polygon, const cv_pen& pen);

		void draw_curve(vector<mt_point>& points, const cv_pen& pen, b8 closed = sys_false);
		void draw_curve(vector<mt_point2f>& points, const cv_pen& pen, b8 closed = sys_false);

		void drawDirection(const mt_point& start, i32 angle, i32 length, const mt_scalar& color, i32 thicknes);

		void drawDirection(const mt_point& start, const mt_point& stop, const mt_scalar& color, i32 thicknes);

		/**
		* String alignment.
		*
		* Left edge for left-to-right text, right for right-to-left text, and top for vertical.
		*/
		enum String_Alignment {
			String_Alignment_Near   = 0,
			String_Alignment_Center = 1,
			String_Alignment_Far    = 2
		};

		/*
		* String format flags.
		*
		*/
		enum String_Format_Flag {
			/**
			* Direction from right to left.
			* 
			* For horizontal text, the reading order is right to left. This value is called the base embedding level by the Unicode
			* bidirectional engine. For vertical text, columns are read from right to left.
			* By default, horizontal or vertical text is read from left to right.
			*/
			String_Format_Flags_Direction_Right_To_Left        = 0x00000001,	

			/**
			* Vertical direction.
			* 
			* Individual lines of text are vertical. In each line, characters progress from top to bottom. 
			* By default, lines of text are horizontal, each new line below the previous line.
			*/
			String_Format_Flags_Direction_Vertical           = 0x00000002,	

			/**
			*
			* Allows parts of glyphs to overhang the bounding rectangle. By default glyphs are first aligned inside the margines, 
			* then any glyphs which still overhang the bounding box are repositioned to avoid any overhang. For example when an italic
			* lower case letter f in a font such as Garamond is aligned at the far left of a rectangle, the lower part of the f will
			* reach slightly further left than the left edge of the rectangle. Setting this flag will ensure the character aligns visually
			* with the lines above and below, but may cause some pixels outside the formatting rectangle to be clipped or painted.
			*/
			String_Format_Flags_No_Fit_Black_Box               = 0x00000004,

			/**
			* Causes control characters such as the left-to-right mark to be shown in the output with a representative glyph.
			*/
			String_Format_Flags_Display_Format_Control        = 0x00000020,

			/**
			* Disables fallback to alternate fonts for characters not supported in the requested font. Any missing characters will be
			* be displayed with the fonts missing glyph, usually an open square.
			*/
			String_Format_Flags_No_Font_Fallback              = 0x00000400,

			/**
			*
			*/		
			String_Format_Flags_Measure_Trailing_Spaces       = 0x00000800,

			/**
			* Disables wrapping of text between lines when formatting within a rectangle. NoWrap is implied when a point is passed
			* instead of a rectangle, or when the specified rectangle has a zero line length.
			*/		
			String_Format_Flags_No_Wrap                      = 0x00001000,

			/**
			*
			*/	
			String_Format_Flags_Line_Limit                   = 0x00002000,

			/**
			* By default text is clipped to the formatting rectangle. Setting NoClip allows overhanging pixels to affect the
			* device outside the formatting rectangle. Pixels at the end of the line may be affected if the glyphs overhang their
			* cells, and either the NoFitBlackBox flag has been set, or the glyph extends to far to be fitted. Pixels above/before 
			* the first line or below/after the last line may be affected if the glyphs extend beyond their cell ascent / descent. 
			* This can occur rarely with unusual diacritic mark combinations.
			*/
			String_Format_Flags_No_Clip                      = 0x00004000,
		};

		static mt_size measure_text_size(const wstring& text, const cv_font& font, i32 format = 0);

		void draw_text(const wstring& text, const mt_rect& rect, const cv_font& font, const mt_scalar& color, i32 horizontal_align_type = String_Alignment_Near, i32 vertical_align_type = String_Alignment_Near, i32 format = 0);

		void draw_text(const wstring& text, const mt_point& offset, const cv_font& font, const mt_scalar& color, i32 horizontal_align_type = String_Alignment_Near, i32 vertical_align_type = String_Alignment_Near, i32 format = 0);

	private:

		void end_draw();

		friend class private_draw;

		void* m_draw_map;
		void* m_draw_engineer;
		mt_mat m_temp;
		mt_mat m_image;
		img_img::Color_Covert_Type m_cvt_code;
	};
}