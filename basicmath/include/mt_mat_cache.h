#pragma once



namespace basicmath {
	
	/**
	@ thread safety
	*/
	class mt_mat_cache {
	public:

		mt_mat_cache();
		~mt_mat_cache();

		static void set_memory_size(i64 memory_size);
		static i64 get_memory_size();
		static void output_cache_statistic();

		mt_mat get_as(const mt_mat& src);

		mt_mat get(const vector<i32>& sizes, mt_Depth_Channel depth_channel);

		mt_mat get(i32 dim, const i32* sizes, mt_Depth_Channel depth_channel);

	protected:

		static void enable_cache(b8 enable);

		void statistic_memory();

		struct thread_data {
			b8 m_enable;
		};

		map<u64, thread_data> m_thread_data;

		i64 m_memory_size;
		vector<mt_mat> m_caches;

		i32 m_get_number;
		i32 m_cache_number;

		void* m_mutex;

		friend class mt_auto_mat_cache;
	};

	class mt_auto_mat_cache {
	public:
		mt_auto_mat_cache();

		~mt_auto_mat_cache();
	};

	extern mt_mat_cache s_mat_cache;
}