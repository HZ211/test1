#ifndef __TCP_NET_SOCKET_H_
#define __TCP_NET_SOCKET_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr("192.168.221.128");不加头文件会报警
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sqlite3.h>
#include <time.h> //用于调用rand生成pp_id
#include <signal.h>
#include "../server.h"

// n 为listen第二个参数，同时连接服务器的个数
extern int tcp_init(const char *ip, int port, int n);

extern int tcp_accept(int sockfd, struct online *);

extern int tcp_connect(const char *ip, int port);

extern int udp_init(const char *ip,int port);

#endif