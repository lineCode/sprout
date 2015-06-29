#pragma once

#include <functional>
#include <vector>
#include <condition_variable>
#include <thread>

namespace thread {

class Pool {
public:

	typedef std::function<void(uint32_t)> Parallel_program;
	typedef std::function<void(uint32_t, uint32_t)> Range_program;

	Pool(uint32_t num_threads);
	~Pool();

	uint32_t num_threads() const;

	void run(Parallel_program program);
	void run_range(Range_program program, uint32_t begin, uint32_t end);

private:

	void wake_all(uint32_t begin, uint32_t end);
	void wait_all();

	struct Unique {
		uint32_t begin;
		uint32_t end;
		std::condition_variable done_signal;
		std::mutex mutex;
		bool wake;
	};

	struct Shared {
		Parallel_program parallel_program;
		Range_program    range_program;
		std::condition_variable wake_signal;
		std::mutex mutex;
		bool end;
	};

	uint32_t num_threads_;

	Shared shared_;

	std::vector<Unique> uniques_;
	std::vector<std::thread> threads_;

	static void loop(uint32_t id, Unique& unique, Shared& shared);
};

}
