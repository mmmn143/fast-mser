#pragma once



namespace basicmath {

	/**
	@ thread safety
	*/
	class mt_mat_cache {
	public:

		mt_mat_cache();
		~mt_mat_cache();

		static void enable_cache(b8 enable, i64 mmeory_size = 1024 * 1024 * 1024);

		mt_mat get_as(const mt_mat& src);

		mt_mat get(const vector<i32>& sizes, mt_Depth_Channel depth_channel);

		mt_mat get(i32 dim, const i32* sizes, mt_Depth_Channel depth_channel);

	protected:

		void statistic_memory();

		vector<mt_mat> m_caches;
		void* m_mutex;
		b8 m_enable;
		i64 m_memory_size;
	};


	extern mt_mat_cache s_mat_cache;
}