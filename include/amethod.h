#pragma once

#include <graphics.h>
#include<Windows.h>
#include<SFML/Audio.hpp>
#include<math.h>
#include<string>
#include<vector>
#include<format>
#include<memory>
#include<utility>
#include"anim.h"

//����͸��ͼƬ
void putimageAlpha(int x, int y, IMAGE* srcimg);
//���µ�������
void TryGenerateEnemy(std::vector<std::unique_ptr<Enemy>>& enemy_list);
//�����ӵ�����
void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player);
//������ҵ÷�
void DrawPlayerScore(int score);