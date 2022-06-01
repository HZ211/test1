#include "server.h"
#include "./my_sqlite3/my_sqlite3.h"
#include "./tcp/tcp_net_socket.h"
#include "../debug_msg/debug.h"

sqlite3 *db;
int RUN = 1;
int fd[1024];
fd_set allset;

int main(int argc, char **argv)
{
    pthread_t tid, udtid;
    int listenfd, connfd, udpfd;
    struct online pp;
    int maxfd;
    fd_set readset;
    int ret;

    for (int i = 0; i < 1024; i++)
    {
        fd[i] = -1;
    }
    // freopen("./file/debug.txt", "a", stdout);  /将服务器运行记录导入到文件中

    Line *head;
    init_list(&head); //初始化在线链表

    signal(SIGINT, my_exit);

    srand((unsigned)time(NULL));
    bzero(&pp, sizeof(pp));

    my_sql_open("./my_sqlite3/database.db", &db);
    my_sql_create_list(db); //初始化数据库部分

    // bind服务器的ip地址和端口。
    listenfd = tcp_init("192.168.221.128", SERVER_PORT, 100); //这里服务器地址采用的是本地回环地址，所以ip地址这里没有意义
    pp.head = head;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;
    while (RUN)
    {
        printf("select\n");
        for (int i = 0; i < 1024; i++)
        {
            maxfd = (fd[i] > maxfd ? fd[i] : maxfd);
        }
        memcpy(&readset, &allset, sizeof(allset));

        ret = select(maxfd + 1, &readset, NULL, NULL, NULL);
        {
            if (FD_ISSET(listenfd, &readset))
            {
                printf("Waiting client to connect server...\n");
                connfd = tcp_accept(listenfd, &pp);
                pp.connfd = connfd;

                maxfd = connfd > maxfd ? connfd : maxfd; //最大值
                printf("ip:%s  port:%d successfull! connfd = %d\n", pp.ip, pp.port, pp.connfd);
                for (int i = 0; i < 1024; i++)
                {
                    if (fd[i] == -1)
                    {
                        fd[i] = connfd;
                        FD_SET(connfd, &allset);
                        break;
                    }
                }
                --ret;
            }
            for (int i = 0; i < 1024; i++)
            {
                if (FD_ISSET(fd[i], &readset))
                {
                    debug_msg("%d\n", __LINE__);
                    pp.connfd = fd[i];
                    my_func(pp);
                    --ret;
                }
                if (ret == 0)
                {
                    break;
                }
            }
        }
    }

    close(listenfd);
    for (int i = 0; i < 1024; i++)
    {
        if (fd[i] != -1)
        {
            close(fd[i]);
        }
    }
    sqlite3_close(db);
    return 0;
}

void my_func(struct online pp)
{
    int recv_len;
    struct msg_node msg, r_msg, w_msg;
    int SEL;
    int fd;

    Line *temp;
    temp = (pp.head)->next;
    while (temp != NULL)
    {
        if (temp->connfd == pp.connfd)
        {
            strcpy(pp.name, temp->name);
            break;
        }
        temp = temp->next;
    }

    printf("进入func界面\n");
    SEL = -1;
    memset(&msg, 0, sizeof(msg));
    if ((recv_len = read(pp.connfd, &msg, sizeof(msg))) == 0)
    {
        server_offline(pp); //客户端下线
        printf("exit\n");
        return;
    }
    SEL = msg.mode;
    switch (SEL)
    {
    case 1: //登陆
        printf("进入登陆界面\n");
        struct sqlite_node sqnode;
        server_land(pp, &sqnode);
        strcpy(pp.name, sqnode.name); //登陆账号个人信息的姓名
        break;
    case 2: //注册
        printf("进入注册界面\n");
        server_log(pp); //对数据库进行操作，如果姓名没有，则注册成功。
        break;
    case 3: //私聊
        printf("进入私聊界面\n");
        if (NO_SPEEK == server_speek_state(pp))
        {
            break;
        }
        else
        {
            server_speek2one(pp, msg);
        }
        break;
    case 4: //群聊
        printf("进入群聊界面\n");
        if (NO_SPEEK == server_speek_state(pp))
        {
            break;
        }
        else
        {
            msg.mode = 4;
            server_speek2all(pp, msg);
        }
        break;
    case 5: // 查看在线人数
        printf("进入查看在线人数界面\n");
        look_online(pp);
        break;
    case 6: //发送文件
        printf("进入到接收文件\n");
        fd = server_recv_file(pp, msg);
        printf("进入到发送文件\n");
        server_send_file(pp, msg, fd);
        break;
    case 11:
        printf("进入到禁言界面\n");
        server_no_speek(pp, msg);
        break;
    case 12:
        printf("进入到解除禁言界面\n");
        server_can_speek(pp, msg);
        break;
    case 13:
        printf("进入到踢人界面\n");
        server_Kick_people(pp, msg);
        break;
    case 999: //客户端退出
        break;
    default:
        break;
    }
}

void my_exit(int sig)
{
    if (sig == SIGINT)
    {
        printf("\nCtrl + c\nserver closing...\n");
        RUN = 0;
        exit(1);
    }
}

void server_land(struct online pp, struct sqlite_node *sqnode) //登陆
{
    int connfd = pp.connfd;
    int flag = 1;
    int recv_len;
    int nnum, lnum;

    char **result;
    char *zErrMsg;
    char *sql;
    char buffer[1024];
    char file_buff[100] = {0};

    Line *temp;
    debug_msg("%s,%d\n", __func__, __LINE__);
    struct msg_node msg;
    do
    {
        flag = 1;
        recv_len = read(connfd, &msg, sizeof(msg)); //接收客户端的id和password
        if (recv_len == 0)
        {
            printf("客户端登陆时下线!\n");
            return; //判断客户端是否在连接，不在连接就下线
        }
        sprintf(buffer, "select ppid,password,name from user_list where ppid = '%u';", msg.id);
        sqlite3_get_table(db, buffer, &result, &lnum, &nnum, &zErrMsg);
        debug_msg("%s,%d\n", __func__, __LINE__);
        // printf("lnum = %d  nnum = %d\n", lnum, nnum);
        printf("ppid: %s  password:%s  name = %s\n", result[nnum + 0], result[nnum + 1], result[nnum + 2]);
        //这里打印有问题，如果客户端输入错误会导致服务器死机。先放这
        //  if(nnum == 2); //判断返回的是不是只有一个账号
        if (lnum != 1) //数据库返回值不唯一或者没有，发送客户端错误，重新输入
        {
            msg.mode = 203;
            write(connfd, &msg, sizeof(msg));
            continue;
        }
        debug_msg("%s,%d\n", __func__, __LINE__);
        if (1 == lnum)
        {
            if ((atoi(result[nnum + 0]) == msg.id) && (strcmp(result[nnum + 1], msg.password) == 0))
            {
                temp = (pp.head)->next;
                while (temp != NULL)
                {
                    if (strcmp(temp->name, result[nnum + 2]) == 0)
                    {
                        flag = 0;
                        msg.mode = 204;
                        write(connfd, &msg, sizeof(msg));
                        break;
                    }
                    temp = temp->next;
                }
                if (flag == 0)
                {
                    continue;
                }
                printf("OK\n");
                msg.mode = 1;
                write(connfd, &msg, sizeof(msg));
                break;
            }
            else
            { //密码或者账号id错误
                msg.mode = 203;
                write(connfd, &msg, sizeof(msg));
                continue;
            }
        }
    } while (1);
    debug_msg("%s,%d\n", __func__, __LINE__);
    // printf("ppid: %s  password:%s  name = %s\n", result[nnum + 0], result[nnum + 1], result[nnum + 2]);
    strcpy(sqnode->name, result[nnum + 2]);

    //在线链表操作
    Line *linenode = (Line *)malloc(sizeof(Line));
    is_malloc_ok(linenode);
    linenode->connfd = pp.connfd;         //套接口
    strcpy(linenode->name, sqnode->name); //姓名
    printf("name = %s上线\n", sqnode->name);
    linenode->state = 1; // 1表示没有被禁言
    linenode->next = (pp.head)->next;
    (pp.head)->next = linenode;

    if (strcmp(sqnode->name, "admin") == 0)
    {
        strcpy(msg.name, "admin");
        strcpy(msg.msg_buff, "admin上线\n");
        server_speek2all(pp, msg);
    }

    int fd = open("./file/land.txt", O_CREAT | O_APPEND | O_WRONLY, 0644);
    sprintf(file_buff, "%s  %s|name:%s id:%s\n", __DATE__, __TIME__, sqnode->name, result[nnum + 0]);
    write(fd, file_buff, strlen(file_buff));
    close(fd);

    return;
}

void look_online(struct online pp) //查看在线人数
{
    int connfd = pp.connfd;
    struct msg_node msg;
    bzero(&msg, sizeof(msg));
    // strcat(msg.msg_buff,"目前在线的人有:");
    Line *temp;
    temp = (pp.head)->next;
    while (temp != NULL)
    {
        printf("%d   ", temp->state);
        printf("%s  ", temp->name);
        strcat(msg.msg_buff, temp->name);
        strcat(msg.msg_buff, " ");
        printf("%d\n", temp->connfd);
        temp = temp->next;
    }
    // strcat(msg.msg_buff,"\n");
    msg.mode = 5; //发送在线人数名单
    write(connfd, &msg, sizeof(msg));

    return;
}

void server_log(struct online pp) //注册
{
    struct msg_node r_msg, msg;

    int recv_len;
    int flag;
    int nnum, lnum;

    char *zErrMsg;
    char **result;
    char *sql;
    char buffer[1024];

    sql = "select name from user_list;";
    do
    {
        flag = 0;
        memset(&r_msg, 0, sizeof(r_msg));
        if ((recv_len = read(pp.connfd, &r_msg, sizeof(r_msg))) == 0)
        {
            return; //退出注册界面;
        }
        printf("name = %s\n", r_msg.name);
        sqlite3_get_table(db, sql, &result, &lnum, &nnum, &zErrMsg);
        for (int i = 0; i <= lnum; i++)
        {
            for (int j = 0; j < nnum; j++)
            {
                if (strcmp(result[i * nnum + j], r_msg.name) == 0)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
            {
                break;
            }
        }
        if (flag == 1)
        {
            msg.mode = 201;
            write(pp.connfd, &msg, sizeof(msg));
            continue;
        }
        else if (flag == 0)
        {
            msg.mode = 2;
            write(pp.connfd, &msg, sizeof(msg));
            break;
        }
    } while (flag);
    unsigned int a = rand() % 900000000 + 100000000; //这里未对id做重复判断。
    printf("a= %u\n", a);
    read(pp.connfd, &r_msg, sizeof(r_msg));
    r_msg.id = a;
    r_msg.mode = 202;
    write(pp.connfd, &r_msg, sizeof(r_msg));
    printf("password = %s  name = %s  id = %u\n", r_msg.password, r_msg.name, r_msg.id);
    sprintf(buffer, "insert into user_list (password,name,ppid) values ('%s','%s','%u');", r_msg.password, r_msg.name, r_msg.id);
    sqlite3_exec(db, buffer, NULL, NULL, NULL);

    int fd = open("./file/log.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    printf("fd = %d\n", fd);
    sprintf(buffer, "time:%s  %s|name:%s id:%u password:%s\n", __DATE__, __TIME__, r_msg.name, r_msg.id, r_msg.password);
    write(fd, buffer, strlen(buffer));
    close(fd);
    return;
}

void server_speek2one(struct online pp, struct msg_node msg) //私聊
{
    int connfd = pp.connfd;
    int flag = 1;
    int recv_len;
    int fd;
    Line *temp;
    char buffer[200] = {0};
    temp = (pp.head)->next;

    while (temp != NULL)
    {
        if ((strcmp(msg.name, temp->name) == 0) && (strcmp(msg.name, pp.name) != 0))
        {
            msg.mode = 3;
            strcpy(msg.name, pp.name);
            write(temp->connfd, &msg, sizeof(msg));

            fd = open("./file/chat record.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
            // printf("fd = %d\n", fd);
            sprintf(buffer, "%s %s |%s send to %s:%s\n", __DATE__, __TIME__,
                    pp.name, temp->name, msg.msg_buff);
            write(fd, buffer, strlen(buffer));
            close(fd);

            return;
        }
        temp = temp->next;
    }
    msg.mode = 205;
    write(connfd, &msg, sizeof(msg));

    return;
}

void server_speek2all(struct online pp, struct msg_node msg) //群聊
{
    Line *head = pp.head;
    char buffer[100];

    msg.mode = 4;
    strcpy(msg.name, pp.name);

    Line *temp;
    temp = (pp.head)->next;

    while (temp != NULL)
    {
        if (strcmp(temp->name, pp.name) == 0)
        {
            temp = temp->next;
            continue; //这里跳过，忘了往后迁移指针，坑！！！
        }
        write(temp->connfd, &msg, sizeof(msg));
        temp = temp->next;
    }
    char buff[100] = {0};
    strncpy(buff, msg.msg_buff, strlen(msg.msg_buff));
    int fd = open("./file/broadcast.txt", O_CREAT | O_APPEND | O_WRONLY, 0644);
    sprintf(buffer, "%s %s|%s:%s\n", __DATE__, __TIME__, msg.name, buff);
    write(fd, buffer, strlen(buffer));
    close(fd);

    pp.head = head;
    return;
}

void server_offline(struct online pp) //下线
{
    Line *temp = (pp.head)->next;
    Line *p = (pp.head);
    struct msg_node msg;
    bzero(&msg, sizeof(msg));

    while (temp != NULL) //操作在线链表
    {
        if (temp->connfd == pp.connfd)
        {
            printf("%s下线\n", pp.name);
            p->next = temp->next;
            close(temp->connfd);
            free(temp);
            break;
        }
        p = temp;
        temp = temp->next;
    }

    msg.mode = 100;
    strcpy(msg.name, pp.name);
    printf("msg = %s  pp = %s  \n", msg.name, pp.name);
    temp = (pp.head)->next;
    while (temp != NULL)
    {
        debug_msg("%s,%d\n", __func__, __LINE__);
        write(temp->connfd, &msg, sizeof(msg));
        temp = temp->next;
    }

    for (int i = 0; i < 1024; i++)
    {
        if (fd[i] == pp.connfd)
        {
            fd[i] = -1;
            FD_CLR(pp.connfd, &allset);
            close(pp.connfd);
            break;
        }
    }

    char buffer[100] = {0};
    int fd = open("./file/offline.txt", O_CREAT | O_APPEND | O_WRONLY, 0644);
    sprintf(buffer, "%s %s|%s\n", __DATE__, __TIME__, msg.name); // id还要访问数据库，太麻烦,就不加id了
    write(fd, buffer, strlen(buffer));
    close(fd);
    return;
}

int server_recv_file(struct online pp, struct msg_node msg) //从客户端接收文件
{
    printf("%s  %s \n", msg.msg_buff, msg.name);
    char filename[100];
    int n;
    char buffer[1024];
    // strcpy(filename, msg.msg_buff);
    printf("B\n");
    int fd = open(msg.msg_buff, O_CREAT | O_RDWR | O_TRUNC, 0644);
    while (1)
    {
        n = read(pp.connfd, buffer, sizeof(buffer));
        // n = strlen(buffer);
        printf("%d\n", n);
        write(fd, buffer, n);
        if (n < 1024)
        {
            break;
        }
    }
    return fd;
}

void server_send_file(struct online pp, struct msg_node msg, int fd) //向客户端发送文件
{
    lseek(fd, SEEK_SET, 0);
    printf("C\n");
    msg.mode = 6;
    Line *temp = (pp.head)->next;
    int n;
    char buffer[1024];
    while (temp != NULL)
    {
        if (strcmp(temp->name, msg.name) == 0)
        {
            strcpy(msg.name, pp.name);
            write(temp->connfd, &msg, sizeof(msg));
            // sleep(1);
            while (1)
            {
                bzero(buffer, 1024);
                n = read(fd, buffer, sizeof(buffer));
                write(temp->connfd, buffer, n);
                if (n < 1024)
                {
                    break;
                }
            }
            break;
        }
        temp = temp->next;
    }
    close(fd);
    sprintf(buffer, "rm %s", msg.msg_buff);
    // printf("buffer = %s\n",buffer);
    system(buffer); //服务器删除缓存文件
}

int server_speek_state(struct online pp) //在线账号状态，为 statew == 0时，为禁言状态。
{
    Line *temp = (pp.head)->next;
    struct msg_node msg;
    bzero(&msg, sizeof(msg));

    while (temp != NULL)
    {
        if (pp.connfd == temp->connfd)
        {
            if (temp->state == 0)
            {
                msg.mode = 206;
                write(temp->connfd, &msg, sizeof(msg));
                return NO_SPEEK;
            }
        }
        temp = temp->next;
    }
    return OK_SPEEK;
}

void server_no_speek(struct online pp, struct msg_node msg) //禁言
{
    Line *temp = (pp.head)->next;

    while (temp != NULL)
    {
        if (strcmp(temp->name, msg.name) == 0)
        {
            msg.mode = 206;
            temp->state = 0;
            write(temp->connfd, &msg, sizeof(msg));
            return;
        }
        temp = temp->next;
    }

    return;
}

void server_can_speek(struct online pp, struct msg_node msg) //解除发言
{
    Line *temp = (pp.head)->next;

    while (temp != NULL)
    {
        if (strcmp(temp->name, msg.name) == 0)
        {
            msg.mode = 207;
            temp->state = 1;
            write(temp->connfd, &msg, sizeof(msg));
            return;
        }
        temp = temp->next;
    }

    return;
}

void server_Kick_people(struct online pp, struct msg_node msg) //踢人
{
    Line *temp = (pp.head)->next;
    Line *p = (pp.head);

    while (temp != NULL)
    {
        if (strcmp(temp->name, msg.name) == 0)
        {
            printf("%s已被管理员admin踢下线\n", temp->name);
            p->next = temp->next;
            msg.mode = 208;
            write(temp->connfd, &msg, sizeof(msg));
            close(temp->connfd);
            free(temp);
            break;
        }
        p = temp;
        temp = temp->next;
    }

    msg.mode = 100;
    temp = (pp.head)->next;
    while (temp != NULL)
    {
        write(temp->connfd, &msg, sizeof(msg));
        temp = temp->next;
    }

    return;
}
