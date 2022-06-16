#include "tcpsocket.h"

TcpServer *TcpSocket::m_TcpServer = NULL;

TcpSocket::TcpSocket(TcpServer *tcpserver, struct bufferevent *bev, char *ip, u_int16_t port)
{
    m_TcpServer = tcpserver;
    m_bev = bev;
    m_ip = ip;
    m_port = port;
}

void TcpSocket::read_cb(struct bufferevent *bev, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->readEvent(s);
}

void TcpSocket::write_cb(struct bufferevent *bev, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->writeEvent(s);
}

void TcpSocket::event_cb(struct bufferevent *bev, short what, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->closeEvent(s, what);

    delete s;
}

char *TcpSocket::getIp()
{
    return m_ip;
}

u_int16_t TcpSocket::getPort()
{
    return m_port;
}

int TcpSocket::readData(void *data, int size)
{
    return bufferevent_read(m_bev, data, size);
}

int TcpSocket::writeData(void *data, int size)
{
    return bufferevent_write(m_bev, data, size);
}