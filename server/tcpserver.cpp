#include "tcpserver.h"
#include <cstdlib>

TcpServer::TcpServer(int threadNum) : m_nextThread(0)
{
    if (threadNum <= 0)
    {
        printf("threadNum <= 0\n");
        exit(-1);
    }

    //创建一个线程数组
    m_threadNum = threadNum;
    m_threadPool = new Thread[threadNum];
    if (m_threadPool == NULL)
    {
        printf("threadpool = NULL\n");
        exit(-1);
    }

    m_base = event_base_new();
    if (m_base == NULL)
    {
        printf("event_base_new\n");
        exit(-1);
    }
}

int TcpServer::listen(int port, const char *ip)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (ip != NULL)
    {
        inet_aton(ip, &sin.sin_addr);
    }

    m_listener = evconnlistener_new_bind(m_base, listenCB, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                         (struct sockaddr *)&sin, sizeof(sin));
    if (!m_listener)
    {
        printf("evconnlistener_new_bind\n");
        return -1;
    }
    //开启线程池
    for (int i = 0; i < m_threadNum; i++)
    {
        m_threadPool[i].start();
        printf("thread %d build   id = %lu\n", i + 1, m_threadPool[i].getId());
    }

    return 0;
}

void TcpServer::listenCB(struct evconnlistener *, evutil_socket_t fd,
                         struct sockaddr *clientAddr, int socklen, void *data)
{
    printf("listenCB\n");

    TcpServer *p = (TcpServer *)data;
    p->listenEvent(fd, (struct sockaddr_in *)clientAddr);
}

void TcpServer::start()
{
    event_base_dispatch(m_base);

    evconnlistener_free(m_listener);
    event_base_free(m_base);

    delete[] m_threadPool;
    printf("done\n");
}

void TcpServer::listenEvent(evutil_socket_t fd, struct sockaddr_in *clientAddr)
{
    char *ip = inet_ntoa(clientAddr->sin_addr);  //客户端的ip地址
    uint16_t port = ntohs(clientAddr->sin_port); //客户端的端口

    printf("%s  %d\n", ip, port);

    //从线程池中选择一个线程区处理客户端的请求
    //轮询的方式选择
    struct event_base *base = m_threadPool[m_nextThread].getBase();
    m_nextThread = (m_nextThread + 1) % m_threadNum;

    struct bufferevent *bev; //通信接口
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        printf("bufferevent_socket_new\n");
        // event_loop_break();
        return;
    }

    //创建一个通信对象
    TcpSocket *s = new TcpSocket(this, bev, ip, port);

    bufferevent_setcb(bev, s->read_cb, s->write_cb, s->event_cb, s);
    bufferevent_enable(bev, EV_WRITE);  //打开写开关
    bufferevent_enable(bev, EV_READ);   //打开读开关
    bufferevent_enable(bev, EV_SIGNAL); //信号

    // bufferevent_write(bev, MESSAGE, strlen(MESSAGE));

    connectEvent(s);
}
