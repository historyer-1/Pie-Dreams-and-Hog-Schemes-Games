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

//������Դ��
class Atlas
{
public:
	Atlas(const std::string& path, int num);
	~Atlas();
public:
	std::vector<std::unique_ptr<IMAGE>> frame_list;//����ͼƬָ������
	std::pair<int, int> player_res;//ͼƬ�ֱ���
};

//������(png)
class Animation
{
public:
	Animation(const std::shared_ptr<Atlas>& atlas, int interfal);//·����֡������֡���ʱ��
	~Animation();

	//���Ŷ���
	void play(int x, int y, int delta);//����x������y�����벥����һ֡�ļ��ʱ��

	//��ȡͼƬ�ֱ���
	std::pair<int, int> GetRes();

private:
	int timer = 0;//������ʱ��
	int idx_frame = 0;//����֡����
	int interfal_ms = 0;//����֡���ʱ��
	std::shared_ptr<Atlas> anim_atlas;
};

extern std::shared_ptr<Atlas> atlas_player_left;
extern std::shared_ptr<Atlas> atlas_player_right;
extern std::shared_ptr<Atlas> atlas_enemy_left;
extern std::shared_ptr<Atlas> atlas_enemy_right;

//��ɫ��
class Player
{
public:
	Player();
	~Player();

	//������Ҳ�����Ϣ
	void ProcessEvent(const ExMessage& msg);

	//���Ŷ���
	void Draw(int delta);

	//��ɫ�ƶ�
	void Move();

	const POINT& GetPosition() const;
	std::pair<int, int> GetSize() const;


private:
	IMAGE img_shadow;	//��ɫ��ӰͼƬ
	const int PLAYER_SPEED = 5;//��ɫ�ƶ��ٶ�
	const int SHADOW_WIDTH = 32;//��ɫ������Ӱ���
	int frame_width;//��ɫ���
	int frame_height;//��ɫ�߶�
	POINT player_point = { 500,500 };//��ɫ��ǰλ��
	//�������Ҷ�������
	std::unique_ptr<Animation> anim_left_player;
	std::unique_ptr<Animation> anim_right_player;
};

class Bullet;

class Enemy
{
public:
	Enemy();
	~Enemy();

	//�����ƶ��߼���׷�ٽ�ɫλ���ƶ�
	void Move(const Player& player);
	//�滭����
	void Draw(int delta);
	//�ж��ӵ���ײ
	bool CheckBulletCollision(const Bullet& bullet);
	//�жϽ�ɫ��ײ
	bool CheckPlayerCollision(const Player& player);

	//���ص����Ƿ���
	bool CheckAlive();

	//��ɱ
	void Hurt();
private:
	const int SPEED = 2;//�����ٶ�
	const int SHADOW_WIDTH = 48;//Ӱ�ӿ��


	IMAGE img_shadow;//��ӰͼƬ
	std::unique_ptr<Animation> anim_left;//���󶯻�����
	std::unique_ptr<Animation> anim_right;//���Ҷ�������
	POINT position = { 0,0 };//����λ��
	bool facing_left = false;//�Ƿ���
	bool alive = true;
	int frame_width;//���˿��
	int frame_height;//���˸߶�
};

class Bullet
{
public:
	POINT position = { 0,0 };//�ӵ�λ��
public:
	Bullet() ;
	~Bullet();

private:
	const int RADIUS = 10;//�ӵ��뾶

public:
	//�����ӵ�
	void Draw();
};