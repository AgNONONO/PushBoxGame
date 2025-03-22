#pragma once

#include <string>

using namespace std;

#define ROWS 9
#define COLS 12

typedef struct _userInfo {
	int id;
	string username;
	string pwd;
	int level_id;
}userInfo;

typedef struct _levelInfo {
	int id;            //关卡的id
	string name;       //关卡的名字
	int map_row;       //地图总行数
	int map_column;    //地图总列数
	string  map_data;  //二维地图数据
	int next_level;    //下一关卡的id
}levelInfo;

bool fetch_level_info(levelInfo& level, int level_id);

bool fetch_user_info(userInfo& user);

bool transform_map_db2array(levelInfo& level, int map[ROWS][COLS]);

bool update_user_level(userInfo& user, int next_level_id);

