#include "threadpool.h"
using namespace std;

ThreadPool::ThreadPool(){
    ThreadPool(DEFAULT_SIZE);
}
//Set threadCount
ThreadPool::ThreadPool(size_t threadCount){
    this->threadCount = threadCount;
    this->availableThreads = threadCount;
    threads = new pthread_t[threadCount];
    for(unsigned int i = 0;i < threadCount;i++){
        pthread_create(&threads[i], NULL, &execute, (void*)this);
    }
}

ThreadPool::~ThreadPool(){
    cvLock.lock();
    terminate = true;
    cv.notify_all();
    cvLock.unlock();
    //printf("Joining\n");
    for(unsigned int i = 0;i < threadCount;i++){
        //printf("Joining thread: %d\n", i);
        pthread_join(threads[i], NULL);
    }
    delete[] threads;
}

//Push a job on the the job queue
int ThreadPool::dispatch_thread(void dispatch_function(void*), void* arg){
    cvLock.lock();
    //printf("%zu: Dispatch: Arg %d Queue size: %lu\n",syscall(SYS_gettid), *((int*) arg), jobs.size());
    jobs.push_back(new job(dispatch_function, arg));
    //printf("%zu: Dispatch success. Queue size: %lu\n", syscall(SYS_gettid), jobs.size());
    cv.notify_one();
    cvLock.unlock();
    return 0;
}

bool ThreadPool::thread_avail(){
    bool result;
    cvLock.lock();
    result = availableThreads > 0;
    cvLock.unlock();
    return result;
}

//Function run by the threads
void* ThreadPool::execute(void* tp){
    ThreadPool* pool = (ThreadPool*) tp;
    list<job*> *jobQueue = &(pool->jobs);
    while(!pool->terminate){
        //Use a condition variable to avoid busy waiting
        job* myJob;
        unique_lock<mutex> uLock(pool->cvLock);
        bool empty = jobQueue->empty();
        //printf("%zu: Queue size: %lu\n", syscall(SYS_gettid), jobQueue->size());
        if(empty){
            //printf("%zu: Waiting\n",syscall(SYS_gettid));
            pool->cv.wait(uLock);
            //printf("%zu: Woken\n",syscall(SYS_gettid));
            //When notified circle around again and try to get a job
        }else{
            pool->availableThreads--;
            myJob = jobQueue->front();
            //printf("%zu: Popping\n", syscall(SYS_gettid));
            jobQueue->pop_front();
            uLock.unlock();
            //printf("%zu: Executing with arg %d\n",syscall(SYS_gettid), *((int*)myJob->arg));
            myJob->function(myJob->arg);
            pool->cvLock.lock();
            pool->availableThreads++;
            pool->cvLock.unlock();
            delete(myJob);
        }
    }
    //Meaningless return
    return (void*) 0;
}
