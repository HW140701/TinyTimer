# TinyTimer
TinyTimer是一个只有单个头文件的简单的支持C++11特性的定时器，
该类包括以下特征：

- 支持同步/异步执行定时器任务；
- 支持中途中断定时器任务，可参考Windows Api的SetTimer和KillTimer;

# 代码演示
## 1 静态/全局作为任务函数

```cpp
#include <iostream>
#include "conio.h"

#include "TinyTimer.hpp"

using namespace std;

void Print()
{
	std::cout << "定时器全局函数Print执行" << std::endl;
}


int main()
{
	TinyTimer::ptr tinyTimer = std::make_shared<TinyTimer>();

	// 延迟1秒执行1次静态/全局函数
	tinyTimer->AsyncOnceExecute(1000, Print);
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
```

## 2 Lambda表达式作为任务函数

```cpp
#include <iostream>
#include "conio.h"

#include "TinyTimer.hpp"

using namespace std;

void Print()
{
	std::cout << "定时器全局函数Print执行" << std::endl;
}

int main()
{
	TinyTimer::ptr tinyTimer = std::make_shared<TinyTimer>();

	// 延迟1秒执行lambda函数表达式
	tinyTimer->AsyncOnceExecute(1000, []() {
		std::cout << "Lambda函数执行" << std::endl;
	});
	tinyTimer->KillTimer();

	getchar();
	return 0;
}
```

## 3 类成员函数做为任务函数

```cpp
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

	// 延迟1秒执行1次类成员函数
	Task taskObj;
	std::function<void(void)> taskRunFunc = std::bind(&Task::TaskRun, taskObj);
	tinyTimer->AsyncOnceExecute(1000, taskRunFunc);
	tinyTimer->KillTimer();
	
	getchar();
	return 0;
}
```
