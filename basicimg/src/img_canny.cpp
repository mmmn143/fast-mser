#include "stdafx.h"

#include "img_canny.h"
#include "img_img.h"

img_canny::~img_canny() {
	basicsys_delete_array(m_buffer);
}

void img_canny::clear_memory_cache() {
	if (m_inner_gradient_x) {
		m_gradient_x = mt_mat();
	}

	if (m_inner_gradient_y) {
		m_gradient_y = mt_mat();
	}

	vector<u8*> temp;
	m_stack.swap(temp);
	basicsys_delete_array(m_buffer);
	m_buffer_size = 0;
}

mt_mat img_canny::edge_detection(const mt_mat& src, const mt_mat& gradient_x /* = mt_mat() */, const mt_mat& gradient_y /* = mt_mat() */, const mt_mat& gradient_mag /* = mt_mat() */) {
	mt_mat edge;
	edge_detection(edge, src, gradient_x, gradient_y, gradient_mag);

	return edge;
}

void img_canny::edge_detection(mt_mat& edge, const mt_mat& src, const mt_mat& gradient_x /* = mt_mat() */, const mt_mat& gradient_y /* = mt_mat() */, const mt_mat& gradient_mag /* = mt_mat() */) {
	if (gradient_x.is_empty()) {
		img_img::sobel(m_gradient_x, src, mt_F32, 1, 0, m_apeture_size);
		m_inner_gradient_x = sys_true;
	} else {
		m_gradient_x = gradient_x;
		m_inner_gradient_x = sys_false;
	}

	if (gradient_y.is_empty()) {
		img_img::sobel(m_gradient_y, src, mt_F32, 0, 1, m_apeture_size);
		m_inner_gradient_y = sys_true;
	} else {
		m_gradient_y = gradient_y;
		m_inner_gradient_y = sys_false;
	}

	if (gradient_mag.is_empty()) {
		img_img::Gradient_Mag_Type gradient_mag_type = img_img::Gradient_Mag_Type_L1;

		if (m_l2_gradient) {
			gradient_mag_type = img_img::Gradient_Mag_Type_L2;
		}

		 img_img::generate_gradient_mag(m_gradient_mag, m_gradient_x, m_gradient_y, gradient_mag_type);
		 m_inner_gradient_mag = sys_true;
	} else {
		m_gradient_mag = gradient_mag;
		m_inner_gradient_mag = sys_false;
	}
	
	canny_impl(edge);

	if (m_enable_memory_cache) {
		clear_memory_cache();
	}
}

void img_canny::canny_impl(mt_mat& edge) {
	edge.create(m_gradient_x.size()[0], m_gradient_x.size()[1], mt_U8C1);
	
	// Copy from OpenCV
	i32 high = m_high_threshold;
	i32 low = m_low_threshold;

	i32 rows = m_gradient_x.size()[0];
	i32 cols = m_gradient_x.size()[1];

	i32 mapstep = m_gradient_x.size()[1] + 2;
	i32 buffer_size = (m_gradient_x.size()[0] + 2) * (cols + 2) + mapstep * 3 * sizeof(i32);
	
	if (m_buffer_size != buffer_size) {
		basicsys_delete_array(m_buffer);

		m_buffer_size = buffer_size;
		m_buffer = new u8[m_buffer_size];
	}
	
    i32* mag_buf[3];
    mag_buf[0] = (i32*)(uchar*)m_buffer;
    mag_buf[1] = mag_buf[0] + mapstep;
    mag_buf[2] = mag_buf[1] + mapstep;
    memset(mag_buf[0], 0, mapstep*sizeof(i32));

    u8* map = (u8*)(mag_buf[2] + mapstep);
    memset(map, 1, mapstep);
    memset(map + mapstep*(m_gradient_x.size()[0] + 1), 1, mapstep);

    int maxsize = max(1 << 10, cols * m_gradient_x.size()[0] / 10);
	
	if ((i32)m_stack.size() > maxsize * 4) {
		vector<u8*> temp;
		m_stack.swap(temp);
	}
	
	m_stack.resize(maxsize);

    u8 **stack_top = &m_stack[0];
    u8 **stack_bottom = &m_stack[0];

    /* sector numbers
       (Top-Left Origin)

        1   2   3
         *  *  *
          * * *
        0*******0
          * * *
         *  *  *
        3   2   1
    */

    #define CANNY_PUSH(d)    *(d) = uchar(2), *stack_top++ = (d)
    #define CANNY_POP(d)     (d) = *--stack_top

    // calculate magnitude and angle of gradient, perform non-maxima supression.
    // fill the map with one of the following values:
    //   0 - the pixel might belong to an edge
    //   1 - the pixel can not belong to an edge
    //   2 - the pixel does belong to an edge
    for (i32 i = 0; i <= rows; i++) {
        i32* _norm = mag_buf[(i > 0) + 1] + 1;
        if (i < rows) {
			const f32* value = m_gradient_mag.ptr<f32>(i, 0);
			
            //for (int j = 0; j < dx.cols; j++)
            //   _norm[j] = int(_dx[j])*_dx[j] + int(_dy[j])*_dy[j];
			for (i32 j = 0; j < cols; j++)
			   _norm[j] = (i32)value[j];

            _norm[-1] = _norm[cols] = 0;
        }
        else
            memset(_norm-1, 0, mapstep*sizeof(i32));

        // at the very beginning we do not have a complete ring
        // buffer of 3 magnitude rows for non-maxima suppression
        if (i == 0)
            continue;

        uchar* _map = map + mapstep*i + 1;
        _map[-1] = _map[cols] = 1;

        i32* _mag = mag_buf[1] + 1; // take the central row
        i64 magstep1 = mag_buf[2] - mag_buf[1];
        i64 magstep2 = mag_buf[0] - mag_buf[1];

        const f32* _x = m_gradient_x.ptr<f32>(i-1, 0);
        const f32* _y = m_gradient_y.ptr<f32>(i-1, 0);

        if ((stack_top - stack_bottom) + cols > maxsize) {
            i32 sz = (i32)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            m_stack.resize(maxsize);
            stack_bottom = &m_stack[0];
            stack_top = stack_bottom + sz;
        }

        i32 prev_flag = 0;
        for (i32 j = 0; j < cols; j++) {
            #define CANNY_SHIFT 15
            const i32 TG22 = (i32)(0.4142135623730950488016887242097*(1<<CANNY_SHIFT) + 0.5);

            int m = _mag[j];

            if (m > low)
            {
                i32 xs = (i32)_x[j];
                i32 ys = (i32)_y[j];
                i32 x = std::abs(xs);
                i32 y = std::abs(ys) << CANNY_SHIFT;

                i32 tg22x = x * TG22;

                if (y < tg22x)
                {
                    if (m > _mag[j-1] && m >= _mag[j+1]) goto __ocv_canny_push;
                }
                else
                {
                    int tg67x = tg22x + (x << (CANNY_SHIFT+1));
                    if (y > tg67x)
                    {
                        if (m > _mag[j+magstep2] && m >= _mag[j+magstep1]) goto __ocv_canny_push;
                    }
                    else
                    {
                        int s = (xs ^ ys) < 0 ? -1 : 1;
                        if (m > _mag[j+magstep2-s] && m > _mag[j+magstep1+s]) goto __ocv_canny_push;
                    }
                }
            }
            prev_flag = 0;
            _map[j] = uchar(1);
            continue;
__ocv_canny_push:
            if (!prev_flag && m > high && _map[j-mapstep] != 2)
            {
                CANNY_PUSH(_map + j);
                prev_flag = 1;
            }
            else
                _map[j] = 0;
        }

        // scroll the ring buffer
        _mag = mag_buf[0];
        mag_buf[0] = mag_buf[1];
        mag_buf[1] = mag_buf[2];
        mag_buf[2] = _mag;
    }

    // now track the edges (hysteresis thresholding)
    while (stack_top > stack_bottom)
    {
        uchar* m;
        if ((stack_top - stack_bottom) + 8 > maxsize)
        {
            int sz = (int)(stack_top - stack_bottom);
            maxsize = maxsize * 3/2;
            m_stack.resize(maxsize);
            stack_bottom = &m_stack[0];
            stack_top = stack_bottom + sz;
        }

        CANNY_POP(m);

        if (!m[-1])         CANNY_PUSH(m - 1);
        if (!m[1])          CANNY_PUSH(m + 1);
        if (!m[-mapstep-1]) CANNY_PUSH(m - mapstep - 1);
        if (!m[-mapstep])   CANNY_PUSH(m - mapstep);
        if (!m[-mapstep+1]) CANNY_PUSH(m - mapstep + 1);
        if (!m[mapstep-1])  CANNY_PUSH(m + mapstep - 1);
        if (!m[mapstep])    CANNY_PUSH(m + mapstep);
        if (!m[mapstep+1])  CANNY_PUSH(m + mapstep + 1);
    }

    // the final pass, form the final image
    const u8* pmap = map + mapstep + 1;
    u8* pdst = edge.data();
    for (int i = 0; i < rows; i++, pmap += mapstep, pdst += edge.step()[0])
    {
        for (int j = 0; j < cols; j++)
            pdst[j] = (u8)-(pmap[j] >> 1);
    }
}