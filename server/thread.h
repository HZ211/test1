#ifndef THREAD_H
#define THREAD_H

#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <pthread.h>
#include <stdlib.h>
#include <event2/event.h>
#include <event2/event_compat.h>
#include <event2/event_struct.h>

class Thread
{
public:
    Thread();
    void start(); //线程运行

    //获取事件集合
    struct event_base *getBase(); //

    pthread_t getId()
    {
        return m_threadId;
    }

protected:
    static void *worker(void *); //线程工作函数

protected:
    void run(); //线程逻辑处理函数

private:
    static void pipeRead(evutil_socket_t, short, void *);

    struct event_base *m_base; //事件集合
    pthread_t m_threadId;      //线程id

    int m_pipeReadFd;  //管道读
    int m_pipeWriteFd; //管道写

    struct event m_pipeEvent;
};

#endif // THREAD_H
