#pragma once
//
//  Sync_queue.h
//  adapted from an example from Bjarne Stroustrup
//

#include <mutex>
#include <condition_variable>
#include <list>
#include <future>
#include <thread>

namespace server::connection {
	template<typename T>
	class Sync_queue {
	public:
		void put(const T& val)
		{
			std::lock_guard<std::mutex> lck{ mtx };
			q.push_back(val);
			cond.notify_one();
		}

		T get()
		{
			std::unique_lock<std::mutex> lck{ mtx };
			cond.wait(lck, [this] { return !q.empty(); });
			T val{ q.front() };
			q.pop_front();
			return val;
		}

	private:
		std::mutex mtx;
		std::condition_variable cond;
		std::list<T> q;
	};
}

