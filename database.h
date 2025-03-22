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
	int id;            //�ؿ���id
	string name;       //�ؿ�������
	int map_row;       //��ͼ������
	int map_column;    //��ͼ������
	string  map_data;  //��ά��ͼ����
	int next_level;    //��һ�ؿ���id
}levelInfo;

bool fetch_level_info(levelInfo& level, int level_id);

bool fetch_user_info(userInfo& user);

bool transform_map_db2array(levelInfo& level, int map[ROWS][COLS]);

bool update_user_level(userInfo& user, int next_level_id);

