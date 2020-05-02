#pragma once

#include "mt_point_t.h"
#include "mt_size_t.h"

namespace basicmath {

	class mt_mat;

//Opencv的rect不包含br，此处的rect包含br
template <class T>
class mt_rect_t {
public:
	mt_rect_t() 
		: m_left(0)
		, m_top(0)
		, m_width(0)
		, m_height(0) {

	}

	mt_rect_t(T left, T top, T width, T height) 
		: m_left(left)
		, m_top(top)
		, m_width(width)
		, m_height(height) {

	}

	mt_rect_t(const mt_rect_t<T>& rect) {
		m_left = rect.m_left;
		m_top = rect.m_top;
		m_width = rect.m_width;
		m_height = rect.m_height;
	}

	bool operator==(const mt_rect_t<T>& other) const {
		return m_left == other.m_left && m_top == other.m_top && m_width == other.m_width && m_height == other.m_height;
	}

	bool operator!=(const mt_rect_t<T>& other) const {
		return !(*this == other);
	}

	mt_rect_t<T> operator+(const mt_point_t<T>& other) const {
		return mt_rect_t<T>(m_left + other.m_x, m_top + other.m_y, m_width, m_height);
	}

	mt_rect_t<T> operator+(const mt_size_t<T>& other) const {
		return mt_rect_t<T>(m_left, m_top, m_width + other.width, m_height + other.height);
	}

	void operator+=(const mt_point_t<T>& other) {
		m_left += other.x;
		m_top += other.y;
	}

	void operator-=(const mt_point_t<T>& other) {
		m_left += other.x;
		m_top += other.y;
	}

	void operator+=(const mt_size_t<T>& other) {
		m_width += other.width;
		m_height += other.height;
	}

	void operator-=(const mt_size_t<T>& other) {
		m_width += other.width;
		m_height += other.height;
	}

	mt_rect_t<T> operator-(const mt_point_t<T>& other) const {
		return mt_rect_t<T>(m_left - other.x, m_top - other.y, m_width, m_height);
	}

	mt_rect_t<T> operator-(const mt_size_t<T>& other) const {
		return mt_rect_t<T>(m_left, m_top, m_width - other.width, m_height - other.height);
	}

	mt_rect_t(const mt_point_t<T>& left_top, const mt_size_t<T>& size) {
		set_rect(left_top, size);
	}

	mt_rect_t(const mt_point_t<T>& left_top, const mt_point_t<T>& rightBottom) {
		set_rect(left_top, rightBottom);
	}

	void set_rect(const mt_point_t<T>& left_top, const mt_size_t<T>& size) {
		m_left = left_top.m_x;
		m_top = left_top.m_y;
		m_width = size.m_width;
		m_height = size.m_height;
	}

	void set_rect(const mt_point_t<T>& left_top, const mt_point_t<T>& right_bottom) {
		m_left = left_top.m_x;
		m_top = left_top.m_y;
		m_width = right_bottom.m_x - left_top.m_x + 1;
		m_height = right_bottom.m_y - left_top.m_y + 1;
	}

	void set_rect(T left, T top, T width, T height) {
		m_left = left;
		m_top = top;
		m_width = width;
		m_height = height; 
	}

	void set_tl(const mt_point_t<T>& left_top) {
		m_top = left_top.m_y;
		m_left = left_top.m_x;
	}

	void set_size(const mt_size_t<T>& size) {
		m_width = size.m_width;
		m_height = size.m_height;
	}

	mt_rect_t<T> toRect() const {
		return mt_rect_t<T>(m_left, m_top, m_width, m_height);
	}

	T right() const {
		return m_left + m_width - 1;
	}

	T bottom() const {
		return m_top + m_height - 1;
	}

	mt_point_t<T> tl() const {
		return mt_point_t<T>(m_left, m_top);
	}

	mt_point_t<T> tr() const {
		return mt_point_t<T>(right(), m_top);
	}

	mt_point_t<T> bl() const {
		return mt_point_t<T>(m_left, bottom());
	}

	mt_point_t<T> br() const {
		return mt_point_t<T>(right(), bottom());
	}

	T getArea() const {
		return m_width * m_height;
	}

	T max_size() const {
		return max(m_width, m_height);
	}

	T min_size() const {
		return min(m_width, m_height);
	}

	void normlized() {
		if (m_width < 0 || m_height < 0) {
			*this = mt_rect_t<T>(mt_point_t<T>(min(m_left, right()), min(m_top, bottom())), mt_point_t<T>(max(m_left, right()), max(m_top, bottom())));
		}
	}

	bool is_intersect(mt_rect_t<T>& intersectRect, const mt_rect_t<T>& rect) const {
		mt_rect_t<T> currentNormlized = *this;
		mt_rect_t<T> otherNormlized = rect;

		currentNormlized.normlized();
		otherNormlized.normlized();

		int left = max(currentNormlized.mLeft, otherNormlized.mLeft);		
		int right = min(currentNormlized.right(), otherNormlized.right());
		int top = max(currentNormlized.mTop, otherNormlized.mTop);
		int bottom = min(currentNormlized.bottom(), otherNormlized.bottom());
		intersectRect.setRect(mt_point(left, top), mt_point(right, bottom));

		return (intersectRect.mWidth > 0) && (intersectRect.mHeight > 0);
	}

	bool is_intersect(const mt_rect_t<T>& rect) const {
		mt_rect_t<T> intersectRect;
		return is_intersect(intersectRect, rect);
	}

	bool contain(const mt_point_t<T>& point) const {
		if (point.x < m_left) {
			return false;
		}

		if (point.y < m_top) {
			return false;
		}

		if (point.x > right()) {
			return false;
		}

		if (point.y > bottom()) {
			return false;
		}

		return true;
	}

	bool boundary(const mt_point_t<T>& point) const {
		if (point.x == m_left && point.y >= m_top && point.y <= bottom()) {
			return true;
		}

		if (point.x == right() && point.y >= m_top && point.y <= bottom()) {
			return true;
		}

		if (point.y == m_top && point.x >= m_left && point.x <= right()) {
			return true;
		}

		if (point.y == bottom() && point.x >= m_left && point.x <= right()) {
			return true;
		}

		return false;
	}

	bool contain(const mt_rect_t<T>& rect) const {
		return contain(rect.tl()) && contain(rect.br());
	}
	
	bool in_boundary(const mt_point_t<T>& point) const {
		return point.x == m_left || point.y == m_top || point.x == m_left + m_width - 1 || point.y == m_top + m_height - 1;
	}

	bool is_intersect_area(const mt_rect_t<T>& r2, double threshold) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.getArea() / (double)r2.getArea() >= threshold; 
	}

	double get_iou(const mt_rect_t<T>& r2) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return 0;
		}	

		return intersectRect.getArea() / (double)(getArea() + r2.getArea() - intersectRect.getArea());
	}

	double get_intersect_aera_ratio(const mt_rect_t<T>& r2) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.getArea() / (double)r2.getArea(); 
	}

	bool is_intersect_width(const mt_rect_t<T>& r2, double threshold) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.mWidth / (double)r2.mWidth >= threshold; 
	}

	double get_intersect_width_ratio(const mt_rect_t<T>& r2) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.mWidth / (double)r2.mWidth; 
	}

	bool is_intersect_height(const mt_rect_t<T>& r2, double threshold) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.mHeight / (double)r2.mHeight >= threshold; 
	}

	double get_intersect_height_ratio(const mt_rect_t<T>& r2) const {
		mt_rect_t<T> intersectRect;
		if (!is_intersect(intersectRect, r2)) {
			return false;
		}	

		return intersectRect.mHeight / (double)r2.mHeight; 
	}

	/**

	@code
	new_rect.m_left = old_rect.m_left - leftOffset;
	new_rect.m_top = old_rect.m_top - topOffset;
	new_rect.m_right = old_rect.m_right + rightOffset;
	new_rect.m_bottom = old_rect.m_bottom + bottomOffset;
	@endcode
	*/
	void adjust(T leftOffset, T topOffset, T rightOffset, T bottomOffset) {
		m_width += leftOffset + rightOffset;
		m_height += topOffset + bottomOffset;

		m_left -= leftOffset;
		m_top -= topOffset;
	}

	void set_empty() {
		m_width = 0;
		m_height = 0;
	}

	bool empty() const {
		return 0 == m_width && 0 == m_height;
	}

	void adjust_by_append_point(const mt_point_t<T>& point) {
		if (empty()) {
			m_left = point.m_x;
			m_top = point.m_y;
			m_width = 1;
			m_height = 1;	
		} else {
			T r = right();
			if (point.m_x > r) {
				r = point.m_x;
			} else if (point.m_x < m_left) {
				m_left = point.m_x;
			}

			T b = bottom();

			if (point.m_y > b) {
				b = point.m_y;
			} else if (point.m_y < m_top) {
				m_top = point.m_y;
			}

			m_width = r - m_left + 1;
			m_height = b - m_top + 1;
		}
	}

	void adjust_by_append_rect(const mt_rect_t<T>& appendRect) {
		if (empty()) {
			*this = appendRect;
		} else {
			T minX = min(m_left, appendRect.m_left);
			T minY = min(m_top, appendRect.m_top);
			T maxX = max(right(), appendRect.right());
			T maxY = max(bottom(), appendRect.bottom());

			m_left = minX;
			m_top = minY;
			m_width = maxX - minX + 1;
			m_height = maxY - minY + 1;
		}
	}

	mt_point_t<T> center() const {
		return mt_point_t<T>(m_left + m_width / 2, m_top + m_height / 2);
	}

	mt_size_t<T> size() const {
		return mt_size_t<T>(m_width, m_height);
	}

	void validate_point(mt_point_t<T>& pt) const{
		if (pt.x < m_left) {
			pt.x = m_left;
		}

		if (pt.x > right()) {
			pt.x = right();
		}

		if (pt.y < m_top) {
			pt.y = m_top;
		}

		if (pt.y > bottom()) {
			pt.y = bottom();
		}
	}

	void validate_rect(mt_rect_t<T>& rect) {
		mt_point_t<T> tlPoint = rect.tl();
		mt_point_t<T> brPoint = rect.br();

		validate_point(tlPoint);
		validate_point(brPoint);

		rect.setRect(tlPoint, brPoint);
	}

	double aspect() const {
		return m_width / (double)m_height;
	}

	bool contain(const mt_point_t<T>& pt) {
		return pt.m_x >= m_left && pt.m_y >= m_top && pt.m_x <= right() && pt.m_y <= bottom();
	}

	void expand() {
		if (m_width == m_height) {
			return;
		}

		if (m_width < m_height) {
			T value = m_height - m_width;
			T halfValue = value / 2;			
			adjust(halfValue, 0, value - halfValue, 0);
		} else if (m_width > m_height){
			T value = m_width - m_height;
			T halfValue = value / 2;		
			adjust(0, halfValue, 0, value - halfValue);
		}
	}

	void shrink() {
		if (m_width == m_height) {
			return;
		}

		if (m_width < m_height) {
			T value = m_height - m_width;
			T halfValue = value / 2;		
			adjust(0, -halfValue, 0, -(value - halfValue));
		} else if (m_width > m_height){
			T value = m_width - m_height;
			T halfValue = value / 2;		
			adjust(-halfValue, 0, -(value - halfValue), 0);
		}
	}

	T horizontal_dist(const mt_rect_t<T>& other) const {
		mt_rect_t<T> left;
		mt_rect_t<T> right;

		if (m_left < other.m_left) {
			left = *this;
			right = other;
		} else {
			left = other;
			right = *this;
		}

		T distance = right.m_left - left.right();

		if (distance < 0) {
			distance = 0;
		}

		return distance;
	}

	T vertical_dist(const mt_rect_t<T>& other) const {
		mt_rect_t<T> top;
		mt_rect_t<T> bottom;

		if (m_top < other.mTop) {
			top = *this;
			bottom = other;
		} else {
			top = other;
			bottom = *this;
		}

		T distance = bottom.mTop - top.bottom();

		if (distance < 0) {
			distance = 0;
		}

		return distance;
	}

	bool linear_position(const mt_rect_t& other, double high_threshold = 0.75, double low_threshold = 0.45) const {
		T height = min(bottom(), other.bottom()) - max(m_top, other.m_top);
		T min_height = m_height;
		T max_height = other.m_height;

		if (min_height > max_height) {
			T temp = min_height;
			min_height = max_height;
			max_height = temp;
		}

		return height / (double)min_height >= high_threshold || height / (double)max_height >= low_threshold;
	}
	
	mt_rect_t<T> operator*(const mt_point2d& v) {
		mt_rect_t<T> res;
		res.mLeft = (T)(m_left * v.m_x);
		res.mTop = (T)(m_top * v.m_y);
		res.mWidth = (T)(m_width * v.m_x);
		res.mHeight = (T)(m_height * v.m_y);

		return res;
	}

	void operator*=(const mt_point2d& v) {
		m_left = (T)(m_left * v.m_x);
		m_top = (T)(m_top * v.m_y);
		m_width = (T)(m_width * v.m_x);
		m_height = (T)(m_height * v.m_y);
	}

	mt_rect_t<T> operator/(const mt_point2d& v) {
		mt_rect_t<T> res;
		res.mLeft = (T)(m_left / v.m_x);
		res.mTop = (T)(m_top / v.m_y);
		res.mWidth = (T)(m_width / v.m_x);
		res.mHeight = (T)(m_height / v.m_y);

		return res;
	}

	void operator/=(const mt_point2d& v) {
		m_left = (T)(m_left / v.m_x);
		m_top = (T)(m_top / v.m_y);
		m_width = (T)(m_width / v.m_x);
		m_height = (T)(m_height / v.m_y);
	}

	T m_left;
	T m_top;
	T m_width;
	T m_height;
};

typedef mt_rect_t<int> mt_rect;

template<class T>
void write(basicsys::sys_strcombine& str, const mt_rect_t<T>& data) {
	str<<"["<<data.m_left<<","<<data.m_top<<","<<data.m_width<<","<<data.m_height<<"]";
}

}
