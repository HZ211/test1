#pragma once
#ifndef _SERVER_H
#define _SERVER_H
// #pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr("192.168.221.128");不加头文件会报警
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/time.h>
#include <sqlite3.h>
#include <time.h> //用于调用rand生成pp_id

#define SERVER_PORT 8888

#define NO_SPEEK -1
#define OK_SPEEK 0

#define MALLOC_OK 1

extern sqlite3 *db;

typedef struct line_list //在线链表
{
    int state; //状态1，可以发言， 为0，被禁言状态
    int connfd;
    unsigned char name[20];
    struct line_list *next;
} Line;

struct online // people 保存登陆 账号个人信息
{
    char ip[15];
    int port;
    int connfd;
    unsigned char name[20]; //此处姓名是用来标注登陆者姓名 //unsigned
    Line *head;
};

struct msg_node
{
    //消息模式，私聊 / 群聊
    int mode; // mode表  1、登陆

    unsigned int id;
    char password[20];
    //如果私聊，私聊的对象  //或者是发送的对方的对象
    unsigned char name[20];

    //消息
    char msg_buff[100];
};

struct sqlite_node
{
    char ppid[12];
    unsigned char name[20]; // unsigned
    char password[20];
    char vip;
};

// int is_malloc_ok(Line *node);

extern void my_exit(int);
extern void my_func(struct online);
extern int is_malloc_ok(Line *node);
extern void init_list(Line **);
extern void server_land(struct online, struct sqlite_node *);      
extern void server_log(struct online);                             
extern void server_speek2one(struct online, struct msg_node);     
extern void server_speek2all(struct online, struct msg_node);      
extern void server_offline(struct online);                       
extern void look_online(struct online);                            
extern int server_recv_file(struct online, struct msg_node);       
extern void server_send_file(struct online, struct msg_node, int); 
extern int server_speek_state(struct online);                     
extern void server_no_speek(struct online, struct msg_node);       
extern void server_can_speek(struct online, struct msg_node);      
extern void server_Kick_people(struct online, struct msg_node);    

#endif