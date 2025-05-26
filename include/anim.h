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

//动画资源类
class Atlas
{
public:
	Atlas(const std::string& path, int num);
	~Atlas();
public:
	std::vector<std::unique_ptr<IMAGE>> frame_list;//动画图片指针数组
	std::pair<int, int> player_res;//图片分辨率
};

//动画类(png)
class Animation
{
public:
	Animation(const std::shared_ptr<Atlas>& atlas, int interfal);//路径，帧数量，帧间隔时间
	~Animation();

	//播放动画
	void play(int x, int y, int delta);//坐标x，坐标y，距离播放上一帧的间隔时间

	//获取图片分辨率
	std::pair<int, int> GetRes();

private:
	int timer = 0;//动画计时器
	int idx_frame = 0;//动画帧索引
	int interfal_ms = 0;//定义帧间隔时间
	std::shared_ptr<Atlas> anim_atlas;
};

extern std::shared_ptr<Atlas> atlas_player_left;
extern std::shared_ptr<Atlas> atlas_player_right;
extern std::shared_ptr<Atlas> atlas_enemy_left;
extern std::shared_ptr<Atlas> atlas_enemy_right;

//角色类
class Player
{
public:
	Player();
	~Player();

	//处理玩家操作信息
	void ProcessEvent(const ExMessage& msg);

	//播放动画
	void Draw(int delta);

	//角色移动
	void Move();

	const POINT& GetPosition() const;
	std::pair<int, int> GetSize() const;


private:
	IMAGE img_shadow;	//角色阴影图片
	const int PLAYER_SPEED = 5;//角色移动速度
	const int SHADOW_WIDTH = 32;//角色脚下阴影宽度
	int frame_width;//角色宽度
	int frame_height;//角色高度
	POINT player_point = { 500,500 };//角色当前位置
	//派梦左右动画对象
	std::unique_ptr<Animation> anim_left_player;
	std::unique_ptr<Animation> anim_right_player;
};

class Bullet;

class Enemy
{
public:
	Enemy();
	~Enemy();

	//敌人移动逻辑，追踪角色位置移动
	void Move(const Player& player);
	//绘画敌人
	void Draw(int delta);
	//判断子弹碰撞
	bool CheckBulletCollision(const Bullet& bullet);
	//判断角色碰撞
	bool CheckPlayerCollision(const Player& player);

	//返回敌人是否存活
	bool CheckAlive();

	//击杀
	void Hurt();
private:
	const int SPEED = 2;//敌人速度
	const int SHADOW_WIDTH = 48;//影子宽度


	IMAGE img_shadow;//阴影图片
	std::unique_ptr<Animation> anim_left;//朝左动画对象
	std::unique_ptr<Animation> anim_right;//朝右动画对象
	POINT position = { 0,0 };//敌人位置
	bool facing_left = false;//是否朝左
	bool alive = true;
	int frame_width;//敌人宽度
	int frame_height;//敌人高度
};

class Bullet
{
public:
	POINT position = { 0,0 };//子弹位置
public:
	Bullet() ;
	~Bullet();

private:
	const int RADIUS = 10;//子弹半径

public:
	//绘制子弹
	void Draw();
};