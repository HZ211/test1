#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include "tcpserver.h"

class TcpServer;
//通信类
class TcpSocket
{
public:
    TcpSocket(TcpServer *, struct bufferevent *, char *, u_int16_t);

    static void read_cb(struct bufferevent *bev, void *ctx);
    static void write_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short what, void *ctx);

    char *getIp();
    u_int16_t getPort();

    //从客户端读数据 Data
    int readData(void *data,int size);
    //向客户端写数据
    int writeData(void *data,int size);

private:
    static TcpServer *m_TcpServer; //服务器对象
    struct bufferevent *m_bev;     //与客户端通信的句柄
    char *m_ip;                    //客户端的ip地址
    u_int16_t m_port;              //客户端使用的端口
};

#endif