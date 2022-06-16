##########################################################################
# File Name: complied.sh
# Author: 王晖
# mail: 614833201@qq.com
# Created Time: 2022年06月16日 星期四 12时31分33秒
#########################################################################
#!/bin/zsh
PATH=/home/edison/bin:/home/edison/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/work/tools/gcc-3.4.5-glibc-2.3.6/bin
export PATH

g++ main.cpp tcpserver.cpp thread.cpp tcpsocket.cpp -levent -lpthread 
