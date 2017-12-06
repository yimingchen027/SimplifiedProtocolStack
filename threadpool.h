#ifndef __THREADPOOL_H
#define __THREADPOOL_H
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
#include <mutex>
#include <list>
#include <condition_variable>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SIZE 10
using namespace std;

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    int dispatch_thread(void dispatch_function(void*), void *arg);
    bool thread_avail();
private:
    class job{
        public:
            job(void function(void*), void* arg){
                this->function = function;
                this->arg = arg;
            }
            void (*function)(void*);
            void* arg;
    };
    pthread_t* threads;
    bool terminate = false;
    static void* execute(void* tp);
    list<job*> jobs;
    size_t threadCount;
    mutex cvLock;
    condition_variable cv;
    size_t availableThreads;
};
#endif
