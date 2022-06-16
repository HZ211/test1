#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include "tcpserver.h"

class TcpServer;
//ͨ����
class TcpSocket
{
public:
    TcpSocket(TcpServer *, struct bufferevent *, char *, u_int16_t);

    static void read_cb(struct bufferevent *bev, void *ctx);
    static void write_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short what, void *ctx);

    char *getIp();
    u_int16_t getPort();

    //�ӿͻ��˶����� Data
    int readData(void *data,int size);
    //��ͻ���д����
    int writeData(void *data,int size);

private:
    static TcpServer *m_TcpServer; //����������
    struct bufferevent *m_bev;     //��ͻ���ͨ�ŵľ��
    char *m_ip;                    //�ͻ��˵�ip��ַ
    u_int16_t m_port;              //�ͻ���ʹ�õĶ˿�
};

#endif