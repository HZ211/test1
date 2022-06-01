#ifndef __MY_SQLITE3_H_
#define __MY_SQLITE3_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

int my_sql_open(const char *, sqlite3 **); //打开数据库
int my_sql_create_list(sqlite3 *);                 //创建数据库

#endif