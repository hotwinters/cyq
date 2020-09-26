#include "threadpool.h"
#include <assert.h>


void* threadpool::threadjob(void *arg) {
	threadpool* p =(threadpool*)arg;
	cout << endl << endl;
	cout << "begin to do someworking" << endl;
	p->dojob();
	delete p;
}
void threadpool::dojob()
{
	job* d_job = NULL;
	while (1)
	{
		cout << "working!!!!!" << endl;
		pthread_mutex_lock(&busy_mutex);
		if (busyjob.size() == 0) {
			pthread_cond_wait(&busy_cond, &busy_mutex);
		}
		d_job = &busyjob.front();
		busyjob.pop_front();
		if (busyjob.size() < poolsize) {
			pthread_cond_signal(&ava_cond);
		}
		pthread_mutex_unlock(&busy_mutex);
		(*(d_job->callback))(d_job->arg);
		cout << "jobs are already done!" << endl;
		d_job = NULL;
	}
}



threadpool::threadpool(int poolsize1) {
	poolsize = poolsize1;
	int ret = 0;
	ret = pthread_mutex_init(&busy_mutex, NULL);
	assert(ret == 0);
	ret = pthread_mutex_init(&ava_mutex, NULL);
	assert(ret == 0);
	ret = pthread_cond_init(&busy_cond, NULL);
	assert(ret == 0);
	ret = pthread_cond_init(&ava_cond, NULL);
	assert(ret == 0);
	pth = new pthread_t[poolsize];
	for (int i = 0; i < poolsize; ++i) {
		pthread_create(&pth[i], NULL, threadjob, this);
		avaliabledeque.push_back(pth[i]);
		delete[]pth;
		cout << "thread created finished!" << endl;
	}
}


void threadpool::addjob(void(*callback)(void* arg), void* arg) {
	pthread_mutex_lock(&busy_mutex);
	if (busydeque.size() >= poolsize) {
		pthread_cond_wait(&ava_cond, &busy_mutex);
	}
	job m_job;
	m_job.callback = callback;
	m_job.arg = arg;
	busyjob.push_back(m_job);
	if (busyjob.size() != 0) {
		pthread_cond_signal(&busy_cond);
	}
	pthread_mutex_unlock(&busy_mutex);
	cout << "addjob finished " << endl;
}

threadpool::~threadpool() {
	for (int i = 0; i < poolsize;++i) {
		pthread_join(pth[i], NULL);
	}
	pthread_cond_destroy(&busy_cond);
	pthread_cond_destroy(&ava_cond);
	pthread_mutex_destroy(&busy_mutex);
	pthread_mutex_destroy(&ava_mutex);
	delete[]pth;
	cout << "join the thread" << endl;
}
