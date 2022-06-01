#ifndef __TCP_NET_SOCKET_C_
#define __TCP_NET_SOCKET_C_

#include "tcp_net_socket.h"

int tcp_init(const char *ip, int port, int n)
{
    int sockfd;
    struct sockaddr_in s_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("sever socket!");
        close(sockfd);
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    // s_addr.sin_addr.s_addr = inet_addr(ip);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(sockfd, (struct sockaddr *)&s_addr, sizeof(s_addr))) == -1)
    {
        perror("sever bind!");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, n) == -1)
    {
        perror("sever listen!");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

int tcp_accept(int sockfd, struct online *pp)
{
    int c_sockfd;
    int addrlen;
    struct sockaddr_in c_addr;

    addrlen = sizeof(struct sockaddr);
    bzero(&c_addr, sizeof(c_addr));

    if ((c_sockfd = accept(sockfd, (struct sockaddr *)&c_addr, &addrlen)) == -1)
    {
        perror("sever accept!");
        // return -1;
        exit(1);
    }
    char *p = inet_ntoa(c_addr.sin_addr);
    printf("p = %s\n", p);
    strcpy(pp->ip, inet_ntoa(c_addr.sin_addr));
    pp->port = ntohs(c_addr.sin_port);
    // printf("ip:%s  port:%d success connect...\n", pp->ip, pp->port);
    // printf("ip:%s  port:%d success connect...\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));

    return c_sockfd;
}

int tcp_connect(const char *ip, int port)
{
    struct sockaddr_in severaddr;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        perror("client socket!");
        close(sfd);
        exit(1);
    }

    memset(&severaddr, 0, sizeof(struct sockaddr_in));
    severaddr.sin_family = AF_INET;
    severaddr.sin_port = htons(port);
    severaddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sfd, (struct sockaddr *)&severaddr, sizeof(severaddr)) == -1)
    {
        perror("connect!");
        close(sfd);
        exit(1);
    }

    return sfd;
}

int udp_init(const char *ip, int port)
{
    int sockfd;
    struct sockaddr_in s_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = inet_addr(ip);
    // s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(sockfd, (struct sockaddr *)&s_addr, sizeof(s_addr))) == -1)
    {
        perror("sever bind!");
        close(sockfd);
        exit(1);
    }

    return sockfd;
}

#endif