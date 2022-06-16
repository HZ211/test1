#include "thread.h"

Thread::Thread()
{
    m_base = event_base_new();
    if (!m_base)
    {
        fprintf(stderr, "Con't create an event! \n");
        exit(-1);
    }

    //创建管道
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(-1);
    }

    m_pipeReadFd = fd[0];
    m_pipeWriteFd = fd[1];

    //让管道事件监听管道的读端
    //如果监听到， 管道的读端有数据可读  //永久有效
    event_set(&m_pipeEvent, m_pipeReadFd, EV_READ | EV_PERSIST,
              pipeRead, this);

    //将事件添加到base集合中
    event_base_set(m_base, &m_pipeEvent);
    event_add(&m_pipeEvent, 0); //开始事件的监听，0是事件超时时间设置
}

//参数
void Thread::pipeRead(evutil_socket_t, short, void *)
{
}

void Thread::start() //创建一个线程
{
    pthread_create(&m_threadId, NULL, worker, this); //创建一个线程

    pthread_detach(m_threadId);
}

void *Thread::worker(void *arg)
{
    Thread *p = (Thread *)arg;
    p->run();

    return NULL;
}

void Thread::run()
{
    // printf("%d\n",m_threadId);
    //监听base事件集合
    // event_base_dispatch 死循环  用来处理事件  类似于qt的exec；
    //如果 m_base 是空，则event_base_dispatch不运行
    //初始化时需要给 base 添加一个事件。让 m_base 不为空
    event_base_dispatch(m_base);
    event_base_free(m_base);

    printf("done\n");
}

struct event_base *Thread::getBase()
{
    return m_base;
}