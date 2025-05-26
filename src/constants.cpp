#include"pch.h"

//窗口分辨率
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//按钮大小
const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;

//游戏是否开始
bool is_game_started = false;
//主循环运行
bool running = true;

//当前移动键状态
bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

sf::Music music;