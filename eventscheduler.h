#include"threadpool.h"
#include<sys/time.h>
#include<functional>
#include<queue>
#include<unistd.h>
#include<stdlib.h>
#include<iostream>
#include<unistd.h>
#include <vector>
#include <chrono>
#include <semaphore.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

struct event {

	typedef std::chrono::time_point<std::chrono::system_clock> time_type;
	int eventId;

	void (*function)(void*);
	void * para;
	time_type when_;
};

struct event_less: public std::less<event> {
	bool operator()(const event &e1, const event &e2) const {
		return (e2.when_ < e1.when_);
	}
};



class EventScheduler{
public:
	EventScheduler(size_t maxEvents);
	EventScheduler();
	~EventScheduler();
	int eventSchedule(void evFunction(void *), void * arg, int timeout);
	void eventCancel(int eventId);
	ThreadPool* th;
	void timer();
	int cancelEvents[100];
	int cancelcount;
	bool stopFlag;

	pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

private:
	std::priority_queue<event, std::vector<event>, event_less> event_queue;
	int max_events;
	int eventIdcount;
	sem_t queueprotect;
};

void* control_function(void* arg)
{
	//printf("in control thread!\n");

	while(1){
	//printf("in control thread!\n");
	EventScheduler* eve = (EventScheduler*) arg;
	pthread_mutex_lock(&eve->mutex);
	//wait for turn
	pthread_cond_wait(&eve->cond, &eve->mutex);
	eve->timer();
	//unlock mutex
	pthread_mutex_unlock(&eve->mutex);

	}
	return NULL;
}

EventScheduler::EventScheduler(size_t maxEvents){
	eventIdcount = 10000;
	max_events = maxEvents;
	sem_init(&queueprotect, 0, 1);
	stopFlag = true;
	cancelcount = 0;
	th = new ThreadPool(5);

	pthread_t tid;
	int ret;
	ret = pthread_create(&tid, NULL, control_function, (void*) this);
	//ret = pthread_join(tid, NULL);

}

EventScheduler::EventScheduler(){
	stopFlag = true;
	eventIdcount = 10000;
	max_events = 10;
	cancelcount = 0;
	sem_init(&queueprotect, 0, 1);
	th = new ThreadPool(5);

	pthread_t tid;
	int ret;
	ret = pthread_create(&tid, NULL, control_function, (void*) this);
}



int EventScheduler::eventSchedule(void evFunction(void *), void * arg, int timeout){
	//timeout is in terms of seconds
	if(event_queue.size()>=max_events){
		cout << "event queue full, try again later " << endl;
		return -1;
	}
	struct event newevent;
	eventIdcount++;
	newevent.function = evFunction;
	newevent.para = arg;

	auto now = std::chrono::system_clock::now();
	newevent.when_ = now + std::chrono::seconds(timeout);
	newevent.eventId = eventIdcount;
	sem_wait(&queueprotect);
	event_queue.push(newevent);
	sem_post(&queueprotect);

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_lock(&mutex);

	return eventIdcount;

}

void EventScheduler::eventCancel(int eventId){
	cancelEvents[cancelcount] = eventId;
	cancelcount++;
}

void EventScheduler::timer() {
	while (!event_queue.empty() && (event_queue.top().when_ < (std::chrono::system_clock::now()))) {
		event thisevent = event_queue.top();
		//pthread_t tid;
		//int ret;
		//ret = pthread_create(&tid, NULL, thisevent.function, (void*)thisevent.para);

		int i;
		bool canceled = false;
		for(i=0; i< cancelcount; i++){
			if (cancelEvents[i] == thisevent.eventId) {
				printf("event has been canceled\n");
				canceled = true;
				break;
			}
		}
		//printf("in timer\n");
		//if(!canceled) {thisevent.function(thisevent.para);}
		if(!canceled) {th->dispatch_thread(thisevent.function, thisevent.para);}

		sem_wait(&queueprotect);
		event_queue.pop();
		sem_post(&queueprotect);
	}
}

EventScheduler::~EventScheduler(){
	//printf("in destructor\n");

	stopFlag = true;

	while(!event_queue.empty()){
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_lock(&mutex);
		//sleep(3);
	}

	sleep(10);
}
