#pragma once




namespace basicsys {

	class sys_thread_lock {
	public:

		static void* create_mutex();
		static void release_mutex(void*& mutex);

		sys_thread_lock(void* mutex);
		~sys_thread_lock();

	protected:

		void* m_mutex;
	};

}