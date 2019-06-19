#pragma once


namespace basicmath {

#define basicmath_block_memory_get(bm, data, T)	\
	if (bm.m_reuse_number > 0) { \
		data = (T*)((u8*)bm.m_begin + bm.m_reuse_indexes[bm.m_reuse_number - 1]); \
	} else { \
		data = bm.m_cur_data; \
	}\
	//bm.assert_addr(data);

#define basicmath_block_memory_add(bm)	\
	if (bm.m_reuse_number == 0) { \
		++bm.m_cur_data;\
		++bm.m_element_number;\
		if (bm.m_cur_data >= bm.m_cur_block_end) {\
			bm.next_block();\
		}	\
	} else {\
		--bm.m_reuse_number;\
	}
	
#define basicmath_block_memory_visit(bm, bp) \
	if (bp.m_visit_number >= bm.m_element_number) { \
		bp.m_data = NULL; \
		bp.m_block_index = -1; \
		bp.m_index = -1; \
		break;\
	} else { \
		if (bp.m_index == bm.m_block_size) { \
			++bp.m_block_index; \
			bp.m_index = 0; \
		} \
		\
		if (bp.m_index == 0) { \
			bp.m_data = bm.m_datas[bp.m_block_index]; \
		} else { \
			++bp.m_data; \
		} \
		\
		++bp.m_index; \
		++bp.m_visit_number; \
	}

#define basicmath_block_memory_reuse(bm, ptr)	\
	if (bm.m_reuse_number < bm.m_resue_max_number) {\
		bm.m_reuse_indexes[bm.m_reuse_number++] = ((u8*)ptr - (u8*)bm.m_begin);\
	}

#define basicmath_block_memory_in(bm, ptr)	\
	bm_in = sys_false;

	template<class T>
	class mt_block_memory {
	public:
		class block_pos {
		public:

			block_pos(i32 block_index = 0, i32 index = 0) {
				m_data = NULL;
				m_block_index = block_index;
				m_index = index;
				m_visit_number = 0;
			}

			T* m_data;
			i32 m_block_index;
			i32 m_index;
			i32 m_visit_number;
		};


		mt_block_memory(i32 block_size = -1, b8 use_new = sys_false, i32 max_reuse = 0) {
			default_param();

			if (block_size >= 0) {
				init(block_size, use_new, max_reuse);
			}
		}

		~mt_block_memory() {
			clear();
		}

		void clear() {
			for (i32 i = 0; i < (i32)m_datas.size(); ++i) {
				if (m_use_new) {
					delete[] m_datas[i];
				} else {
					free(m_datas[i]);
				}
			}

			m_datas.clear();

			if (m_reuse_indexes != NULL) {
				free(m_reuse_indexes);
			}

			default_param();
		}

		void reset() {
			init(m_block_size, m_use_new, m_resue_max_number);
		}

		void init(i32 block_size, b8 use_new = sys_false, i32 max_reuse = 0) {
			basiclog_assert2(block_size > 0);

			if (block_size == m_block_size && m_use_new == use_new) {
				m_cur_block = 0;
				m_cur_data = m_datas.front();
				m_cur_block_end = m_cur_data + m_block_size;
			} else {
				for (i32 i = 0; i < (i32)m_datas.size(); ++i) {
					if (m_use_new) {
						delete[] m_datas[i];
					} else {
						free(m_datas[i]);
					}
				}

				m_datas.clear();

				m_block_size = block_size;
				m_use_new = use_new;
				m_cur_block = -1;
				next_block();

				m_begin = m_datas.front();
			}

			if (m_resue_max_number != max_reuse) {
				m_resue_max_number = max_reuse;

				if (m_reuse_indexes != NULL) {
					free(m_reuse_indexes);
				}

				if (m_resue_max_number > 0) {
					m_reuse_indexes = (i64*)malloc(sizeof(i64) * m_resue_max_number);
				} else {
					m_reuse_indexes = NULL;
				}
			}

			m_element_number = 0;
			m_reuse_number = 0;
		}

		T& at(i32 index) {
			return m_datas[index / m_block_size][index % m_block_size];
		}

		const T& at(i32 index) const {
			return m_datas[index / m_block_size][index % m_block_size];
		}

		void assert_addr(T* data) {
			b8 valid = sys_false;
			for (i32 i = 0; i <= m_cur_block; ++i) {
				if (data >= m_datas[i] && data < m_datas[i] + m_block_size) {
					valid = sys_true;
					break;
				}
			}

			basiclog_assert2(valid);
		}

		b8 in_block(T* data) const {
			if (m_cur_block == -1 || data < m_begin || data >= m_cur_data) {
				return sys_false;
			}

			if (data >= m_datas[m_cur_block] && data < m_cur_data) {
				return sys_true;
			}

			for (i32 i = 0; i < m_cur_block; ++i) {
				if (data >= m_datas[i] && data < m_datas[i] + m_block_size) {
					return sys_false;
				}
			}

			return sys_true;
		}

		void next_block() {
			++m_cur_block;

			if (m_cur_block > (i32)m_datas.size() - 1) {
				T* d = NULL;
				if (m_use_new) { 
					d = new T[m_block_size]; 
				} else { 
					d = (T*)malloc(sizeof(T) * m_block_size); 
				} 

				m_datas.push_back(d); 
				m_memory_size += m_block_size; 
				m_cur_data = d; 
			} else {
				m_cur_data = m_datas[m_cur_block];
			}

			m_cur_block_end = m_cur_data + m_block_size; 
		}

		T* m_begin;
		T* m_cur_data;
		T* m_cur_block_end;
		i32 m_cur_block;

		i32 m_element_number;
		
		i32 m_memory_size;

		i32 m_block_size;
		b8 m_use_new;
		i64* m_reuse_indexes;
		i32 m_resue_max_number;
		i32 m_reuse_number;
		

		vector<T*> m_datas;

	protected:
		void default_param() {
			m_block_size = 0;
			m_cur_block = -1;
			m_use_new = sys_false;
			m_resue_max_number = 0;

			m_memory_size = 0;
			m_element_number = 0;
			m_reuse_number = 0;

			m_begin = NULL;
			m_cur_data = NULL;
			m_cur_block_end = NULL;
			m_reuse_indexes = NULL;
		}


	};

	template<class T>
	void write(basicsys::sys_strcombine& str, const mt_block_memory<T>& data) {
		str<<L"[";

		for (i32 i = 0; i < data.m_element_number; ++i) {
			str<<data.at(i);

			if (i != data.m_element_number - 1) {
				str<<L",";
			}
		}

		str<<L"]";
	}
}