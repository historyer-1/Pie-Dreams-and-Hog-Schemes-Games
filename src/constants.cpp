#include"pch.h"

//���ڷֱ���
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//��ť��С
const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;

//��Ϸ�Ƿ�ʼ
bool is_game_started = false;
//��ѭ������
bool running = true;

//��ǰ�ƶ���״̬
bool is_move_up = false;
bool is_move_down = false;
bool is_move_left = false;
bool is_move_right = false;

sf::Music music;