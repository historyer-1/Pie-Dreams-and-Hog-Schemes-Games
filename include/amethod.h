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

//加载透明图片
void putimageAlpha(int x, int y, IMAGE* srcimg);
//更新敌人数量
void TryGenerateEnemy(std::vector<std::unique_ptr<Enemy>>& enemy_list);
//更新子弹数量
void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player);
//绘制玩家得分
void DrawPlayerScore(int score);