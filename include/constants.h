#pragma once
#include<SFML/Audio.hpp>

//窗口分辨率
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

//按钮大小
extern const int BUTTON_WIDTH;
extern const int BUTTON_HEIGHT;

//游戏是否开始
extern bool is_game_started;
//主循环运行
extern bool running;

//当前移动键状态
extern bool is_move_up;
extern bool is_move_down;
extern bool is_move_left;
extern bool is_move_right;

extern sf::Music music;//创建背景音乐对象

