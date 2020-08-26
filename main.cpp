#include <iostream>
#include "conio.h"

#include "TinyTimer.hpp"

using namespace std;

void Print()
{
	std::cout << "定时器全局函数Print执行" << std::endl;
}


class Task
{
public:
	Task() {

	};

	~Task() {

	};

	void TaskRun()
	{
		std::cout << "Task::TaskRun()函数执行" << std::endl;
	};
};


int main()
{
	TinyTimer::ptr tinyTimer = std::make_shared<TinyTimer>();

	// 延迟1秒执行1次静态/全局函数
	tinyTimer->AsyncOnceExecute(1000, Print);
	tinyTimer->KillTimer();

	// 延迟1秒执行lambda函数表达式
	tinyTimer->AsyncOnceExecute(1000, []() {
		std::cout << "Lambda函数执行" << std::endl;
	});
	tinyTimer->KillTimer();

	// 延迟1秒执行1次类成员函数
	Task taskObj;
	std::function<void(void)> taskRunFunc = std::bind(&Task::TaskRun, taskObj);
	tinyTimer->AsyncOnceExecute(1000, taskRunFunc);
	tinyTimer->KillTimer();

	// 循环执行静态/全局函数，按Q键杀死定时器
	int a = 0;
	while (true)
	{
		// 在此处填入需要循环的代码
		if (a == 0)
		{
			tinyTimer->AsyncLoopExecute(1, Print);
		}

		if (_kbhit()) // 如果有按键被按下
		{
			if (_getch() == 'q') //如果按下了q键则跳出循环
			{
				tinyTimer->KillTimer();
				break;
			}

		}
	}

	getchar();
	return 0;
}