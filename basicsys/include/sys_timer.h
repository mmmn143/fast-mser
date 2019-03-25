#pragma once



namespace basicsys {
	class sys_timer {
	public:

		sys_timer(const wstring& tag, b8 auto_out_cost = sys_true, b8 debug_output = sys_true);
		~sys_timer();

		void begin();
		void end();
		void out(b8 debug_output = sys_true);

		i32 get_cost() const;

		void reset() {
			m_cost = 0;
			m_count = 0;
		}

		static i64 get_tick_cout();	

	private:

		wstring m_name;
		i32 m_cost;
		i32 m_count;
		b8 m_auto_out_cost;
		b8 m_debug_auto_out;

		i64 m_freq;
		i64 m_begin;
	};

}