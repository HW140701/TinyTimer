#ifndef TINY_TIMER_H
#define TINY_TIMER_H

#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>

class TinyTimer
{
public:
	typedef std::shared_ptr<TinyTimer> ptr;
	virtual~TinyTimer() 
	{
		KillTimer();
	}


	TinyTimer() :m_bExpired(true), m_bTryExpired(false), m_bLoopExecute(false)
	{

	}

	TinyTimer(const TinyTimer& t) {
		m_bExpired = t.m_bExpired.load();
		m_bTryExpired = t.m_bTryExpired.load();
	}

public:
	//!
	//! @brief  ���ö�ʱ��
	//!
	//! @param  interval - ��ʱ��ִ�м��
	//! @param  task - ������
	//! @param  bLoop - �Ƿ�ѭ��
	//! @param  async - �Ƿ��첽ִ��
	//!
	//! @return ִ�н��
	//!
	bool SetTimer(int interval, std::function<void()> task, bool bLoop = false, bool async = true)
	{
		if (!m_bExpired || m_bTryExpired)
			return false;

		m_bExpired = false;
		m_bLoopExecute = bLoop;
		m_LoopCount = 0;

		// ������첽ִ��
		if (async)
		{
			if (m_Thread != nullptr)
				m_Thread.reset();

			m_Thread = std::make_shared<std::thread>(
				([this, interval, task]() {
				while (!m_bTryExpired)
				{
					std::unique_lock<std::mutex> lk(m_ThreadMutex);
					if (m_ExpiredConditionVar.wait_for(lk, std::chrono::milliseconds(interval), [this]() {return m_bTryExpired == true; }))
					{
						break;
					}
					else
					{
						task();

						m_LoopCount++;

						if (!m_bLoopExecute)
						{
							break;
						}
					}
				}
				m_bExpired = true;
				m_bTryExpired = false;
			})
			);
			m_Thread->detach();
		}
		// �����ͬ��ִ��
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
			if (!m_bTryExpired) {
				task();
			}
			m_bExpired = true;
			m_bTryExpired = false;
		}

		return true;
	}

	//!
	//! @brief  ɱ����ʱ��
	//!
	void KillTimer()
	{
		if (m_bExpired || m_bTryExpired || m_Thread == nullptr)
		{
			return;
		}

		m_bTryExpired = true;

		m_ExpiredConditionVar.notify_one();
	}

	//!
	//! @brief  �첽�ӳ�ִ��һ�ζ�ʱ��
	//!
	//! @param  interval - ��ʱ��ִ�м��
	//! @param  fun - ������
	//!
	//! @return ִ�н��
	//!
	template<typename callable, typename... arguments>
	bool AsyncOnceExecute(int interval, callable&& fun, arguments&&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));

		return SetTimer(interval, task, false);
	}


	//!
	//! @brief  �첽��ʱѭ��ִ�ж�ʱ��
	//!
	//! @param  interval - ��ʱ��ִ�м��
	//! @param  fun - ������
	//!
	//! @return ִ�н��
	//!
	template<typename callable, typename... arguments>
	bool AsyncLoopExecute(int interval, callable&& fun, arguments&&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...));

		return SetTimer(interval, task, true);
	}

	//!
	//! @brief  ͬ��ִ��һ�ζ�ʱ��
	//!
	//! @param  interval - ��ʱ��ִ�м��
	//! @param  fun - ������
	//!
	//! @return ִ�н��
	//!
	template<typename callable, typename... arguments>
	bool SyncOnceExecute(int interval, callable&& fun, arguments&&... args) {
		std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(fun), std::forward<arguments>(args)...)); //����������lambda��function
		return SetTimer(interval, task, false, false);
	}

private:
	std::atomic_bool m_bExpired;
	std::atomic_bool m_bTryExpired;
	std::atomic_bool m_bLoopExecute;
	std::mutex m_ThreadMutex;
	std::condition_variable m_ExpiredConditionVar;
	std::shared_ptr<std::thread> m_Thread;
	unsigned int m_LoopCount = 0;
};


#endif // !TINY_TIMER_H
