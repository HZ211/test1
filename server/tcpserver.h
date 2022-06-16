#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "thread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include "tcpsocket.h"
#include <pthread.h>

#include <event2/event.h>
#include <event2/buffer_compat.h>

class TcpSocket;
// Tcp服务的基类
class TcpServer
{
    friend class TcpSocket;

public:
    TcpServer(int threadNum = 8);

    int listen(int, const char *ip = "192.168.221.128");

    //服务器开始运行
    void start();

protected:
    //服务器监听回调函数，有客户端连接，会调用这个函数
    static void listenCB(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);

    void listenEvent(evutil_socket_t fd, struct sockaddr_in *clientAddr);

    //-----------------虚函数，处理具体客户端问题-----------------------------------------------
    virtual void connectEvent(TcpSocket *){};  //客户端连接
    virtual void readEvent(TcpSocket *){};      //可读
    virtual void writeEvent(TcpSocket *){};         //可写
    virtual void closeEvent(TcpSocket *, short ){};     //客户端关闭

private:
    int m_threadNum;
    Thread *m_threadPool; //线程池

    int m_nextThread; //下一个线程的下标
    struct event_base *m_base;
    struct evconnlistener *m_listener;
};

#endif // TCPSERVER_H
