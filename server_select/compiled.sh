#!/bin/bash
PATH=/home/edison/bin:/home/edison/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/work/tools/gcc-3.4.5-glibc-2.3.6/bin
export PATH

gcc server.c ./my_sqlite3/my_sqlite3.c ./tcp/tcp_net_socket.c ./list_server.c -o server -lsqlite3