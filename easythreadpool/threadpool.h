#ifndef THREADPOOL
#define THREADPOOL


#include <assert.h>
#include <pthread.h>
#include <deque>

using namespace std;

class job
{
public:
	void (*callback)(void* arg);
	void* arg;
};

class threadpool
{
private:
	pthread_t *pth;
	pthread_cond_t busy_cond, ava_cond;
	pthread_mutex_t busy_mutex, ava_mutex;
	deque<pthread_t>busydeque;
	deque<pthread_t>avaliabledeque;
	deque<job>busyjob;
	deque<job>waitjob;
	int poolsize;
public:
	threadpool(){}
	//初始化线程池
	threadpool(int poolsize);
	//向任务队列中添加任务
	void addjob(void(*callback)(void* arg), void* arg);
	//线程池所做的工作
	static void* threadjob(void*arg);
	void dojob();

	//回收
	~threadpool();
};


#endif // !THREADPOOL
