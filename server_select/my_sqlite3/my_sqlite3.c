#include "my_sqlite3.h"

extern sqlite3 *db;

int my_sql_open(const char *filename, sqlite3 **db) //
{
    if (sqlite3_open(filename, db) != SQLITE_OK)
    {
        perror("sqlite open!");
        return SQLITE_FAIL;
    }
    //printf("sqlite open successfully\n");
    return SQLITE_OK;
}

int my_sql_create_list(sqlite3 *db)
{
    char *zErrMsg;
    char *sql = "create table if not exists user_list(\
        ppid VARCHAR(12) NOT NULL,\
        name  VARCHAR(20) NOT NULL,\
        password VARCHAR(12) NOT NULL,\
        vip VARCHAR(1));";

    if ((SQLITE_OK != sqlite3_exec(db, sql, NULL, NULL, &zErrMsg)))
    {
        perror("create table");
        return SQLITE_FAIL;
    }
    // printf("create table successfully\n");
    return SQLITE_OK;
}