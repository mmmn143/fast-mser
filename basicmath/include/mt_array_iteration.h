/**
@file ml_array_iteration.h

This file implement the accession of an array of arbitrarily dimension (possibly be discontinuous).

@note the array_iteration is a common operation for the data any dimensions (even in-continuous), hence the performance maybe terrible!
*/

#pragma once


namespace basicmath {
	class mt_array_iteration {
	public:

		/** Get the continuous dim of the array. If the array is entirely continuous, the return is 0. If the array is not continues in the last dim, the return is the dimensions.
		The data in the dimension is continuous (either memory increase or decrease).
		*/
		static int get_continuous_dim(int ndims, const int* sizes, const int* steps, int element_size);
		static void array_copy(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size);
		static b8 array_cmp(const u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_steps, int element_size);

		template<class T>
		static void array_assign(basicsys::u8* data, const T& value, basicsys::i32 size, basicsys::i32 step = 0) {
			if (step == 0) {
				step = sizeof(T);
			}
			
			for (basicsys::i32 i = 0; i < size; ++i) {
				*(T*)data = value;

				data += step;
			}
		}

	private:

		static b8 array_iteration(u8* dst, const u8* src, int ndims, const int* sizes, const int* dst_steps, const int* src_stepse, int element_size, int typ);
	};

	class mt_array_index_iterator {
	public:
		mt_array_index_iterator();
		mt_array_index_iterator(int dim, const i32* sizes);

		void init(int dim, const i32* sizes);

		b8 next();

		const vector<int>& position() {
			return m_cur_position;
		}

		int index() const {
			return m_access_number - 1;
		}

		i32 size() const;

		void all_index(vector<vector<i32>>& all_indexes) const;

	protected:

		const i32* m_sizes;
		vector<int> m_cur_position;
		int m_dims;
		int m_total_number;
		int m_access_number;
	};

	class mt_array_element_const_iterator {
	public:

		mt_array_element_const_iterator(const u8* data, int ndims, const int* sizes, const int* steps, int element_size);
		mt_array_element_const_iterator(const mt_mat& mat);

		virtual ~mt_array_element_const_iterator() {}

		const u8* data();

		int index() const {
			return m_accessed_count - 1;
		}

		const vector<int>& position() const {
			return m_cur_position;
		}

		int element_number() const {
			return m_element_number;
		}

		int element_size() const {
			return m_element_size;
		}

	protected:

		void init_construct(const u8* data, int ndims, const int* sizes, const int* steps, int element_size);

		mt_array_element_const_iterator() {}

		friend class mt_array_memory_block_const_iterator;

		int m_dims;
		
		const int* m_sizes;
		const int* m_int_steps;

		vector<const u8*> m_ptr_dim_datas;
		vector<int> m_cur_position;
		int m_accessed_count;
		int m_element_number;
		int m_element_size;
	};

	class mt_array_element_iterator : public mt_array_element_const_iterator {
	public:

		mt_array_element_iterator(u8* data, int ndims, const int* sizes, const int* steps, int element_size);
		mt_array_element_iterator(mt_mat& mat);

		u8* data();

	protected:


	};

	class mt_array_memory_block_const_iterator {
	public:

		mt_array_memory_block_const_iterator(const u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size);
		mt_array_memory_block_const_iterator(const mt_mat& mat);

		virtual ~mt_array_memory_block_const_iterator() {}

		/**
		@param ordered if the step[dim] < 0, then the return data point to the end of the data in dim.
			When we want to use memcpy function to copy data, we need to set ordered be true to make the data point to front side.
		*/
		const u8* data();

		const u8* memory_start();

		int block_number() const;

		int block_size() const;

		/**
		@note It does not consider the channel.
		*/
		int block_element_number() const;

		int block_element_step() const {
			return m_element_step;
		}

		int index() const {
			return m_element_iterator.index();
		}

		const vector<int>& position() const {
			return m_element_iterator.position();
		}

	private:
		
		void init_construct(const u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size);

		mt_array_element_const_iterator m_element_iterator;
		int m_element_step;
		int m_block_element_number;
	};

	class mt_array_memory_block_iterator : public mt_array_memory_block_const_iterator {
	public:

		mt_array_memory_block_iterator(u8* data, int ndims, const int* sizes, const int* steps, int dim, int element_size);
		mt_array_memory_block_iterator(mt_mat& mat);

		u8* data();

		u8* memory_start();
	};
}