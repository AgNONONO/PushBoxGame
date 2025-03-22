#include "database.h"
#include <mysql.h>
#include <stdio.h>

#define DB_NAME "box_man"
#define DB_HOST "127.0.0.1"
#define DB_PORT "3306"
#define DB_USER "root"
#define DB_USER_PASSWD "123456" // mysql登录密码

static bool connect_db(MYSQL& mysql);

bool debug = false;

bool fetch_level_info(levelInfo& level, int level_id)
{
    MYSQL mysql;
    MYSQL_RES* res;
    MYSQL_ROW row;
    char sqlStmt[256];
    bool ret = false;
    
    if (connect_db(mysql) == false)
        return false;

    //printf("fetch_level_info level_id = %d", level_id);


    snprintf(sqlStmt, 256, "select name, map_row, map_column, map_data, next_level_id from levels where id=%d;",
        level_id);
    ret = mysql_query(&mysql, sqlStmt); // 成功返回0

    if (ret) {
        printf("数据库查询出错，%s 错误原因： %s\n", sqlStmt, mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);
    //printf(row[0]);
    if (row == NULL) {
        //printf("hello\n");

        mysql_free_result(res);
        mysql_close(&mysql);
        return false;
    }
    level.id = level_id;
    level.name = row[0];
    level.map_row = atoi(row[1]);
    level.map_column = atoi(row[2]);
    level.map_data = row[3];
    level.next_level = atoi(row[4]);

    if(debug)
        printf("level id: %d  name: %s map row: %d  map column: %d map data: %s next level: %d\n", 
            level.id, level.name.c_str(), level.map_row, level.map_column, level.map_data.c_str(), level.next_level);

    mysql_free_result(res);
    mysql_close(&mysql);


    return true;
}

/***************************************************
 *功能：通过用户名和密码从数据库获取用户信息
 *输入：
 *      user - 用户信息结构体
 *
 *返回值：
 *       获取成功返回true, 失败false
 ***************************************************/
bool fetch_user_info(userInfo& user)
{
    MYSQL mysql;
    MYSQL_RES* res; // 查询结果集
    MYSQL_ROW row; // 记录结构体
    char sqlStmt[256];
    bool ret = false;

    //1.连接到数据库
    if (!connect_db(mysql))
        return false;

    //2.根据用户名和密码获取用户信息(id, level_id)
    snprintf(sqlStmt,256,"select id,level_id from users where username='%s' and password=md5('%s');",
        user.username.c_str(), user.pwd.c_str());

    ret = mysql_query(&mysql, sqlStmt); //成功返回0

    if (ret) {
        printf("数据库查询出错，%s 错误原因： %s\n", sqlStmt, mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }

    //3.获取结果
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == NULL) {
        mysql_free_result(res); //
        mysql_close(&mysql);
        return false;
    }

    user.id = atoi(row[0]); 
    user.level_id = atoi(row[1]);
    //printf("userid: %d  level_id: %d\n", user.id, user.level_id);  //打印ID

    //释放结果集
    mysql_free_result(res);

    //关闭数据库
    mysql_close(&mysql);

    return true;
}

bool transform_map_db2array(levelInfo& level, int map[ROWS][COLS])
{
    if (level.map_row > ROWS || level.map_column > COLS) {
        printf("地图过大，请重新设置!\n");
        return false;
    }
    if (level.map_data.length() < 1) {
        printf("地图数据有误，请重新设置!\n");
        return false;
    }

    int start = 0, end = 0;
    int row = 0, column = 0;

    // 将地图数据读入到数组中
    do {
        end = level.map_data.find('|', start);

        if (end < 0) {
            end = level.map_data.length();
        }

        if (start >= end) break;

        string line = level.map_data.substr(start, end - start);
        //printf("get line: %s\n", line.c_str());

        //对行地图数据进行解析
        char* next_token = NULL;
        char* item = strtok_s((char*)line.c_str(), ",", &next_token);

        column = 0;

        while (item && column < level.map_column) {
            //printf("%s ", item);
            map[row][column] = atoi(item);
            column++;

            item = strtok_s(NULL, ",", &next_token);
        }

        if (column < level.map_column) {
            printf("地图数据解析出错,终止!\n");
            return false;
        }

        //printf("\n");
        row++;

        if (row >= level.map_row) {
            break;
        }

        start = end + 1;

    } while (1 == 1);

    if (row < level.map_row) {
        printf("地图行数少于设定, %d(need: %d),终止!\n", row, level.map_row);
        return false;
    }

    return true;
}

bool update_user_level(userInfo& user, int next_level_id)
{
    MYSQL mysql;
    MYSQL_RES* res; //查询结果集
    MYSQL_ROW row;  //记录结构体
    char sqlStmt[256];
    bool ret = false;


    //1.连接到数据库
    if (connect_db(mysql) == false) {
        return false;
    }

    //2.根据用户id 更新下一关的level_id
    snprintf(sqlStmt, 256, "update users set level_id = %d where id=%d;", next_level_id, user.id);

    ret = mysql_query(&mysql, sqlStmt);

    if (ret) {
        printf("数据库更新出错，%s 错误原因： %s\n", sqlStmt, mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }

    //关闭数据库
    mysql_close(&mysql);


    return true;
}

bool connect_db(MYSQL& mysql) {
    //1.初始化数据库句柄
    mysql_init(&mysql);

    //2.设置字符编码
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

    //3.连接数据库
    if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWD, DB_NAME, atoi(DB_PORT), NULL, 0) == NULL) {
        printf("数据库连接出错， 错误原因： %s\n", mysql_error(&mysql));
        return false;
    }

    return true;
}
