#include"pch.h"


Atlas::Atlas(const std::string& path, int num)
{
	std::string path_file;
	//开辟内存存储帧，并用指针数组来保存和使用他们
	for (int i = 0; i < num; i++)
	{
		path_file = std::format("{}{}.png", path, i);

		frame_list.emplace_back(std::make_unique<IMAGE>());
		auto& frame = frame_list.back();
		loadimage(frame.get(), path_file.c_str());

	}

	//图片分辨率
	player_res.first = frame_list[0]->getwidth();
	player_res.second = frame_list[0]->getheight();
}
Atlas::~Atlas() = default;

std::shared_ptr<Atlas> atlas_player_left = std::make_shared<Atlas>("./assets/img/player_left_", 6);
std::shared_ptr<Atlas> atlas_player_right = std::make_shared<Atlas>("./assets/img/player_right_", 6);
std::shared_ptr<Atlas> atlas_enemy_left = std::make_shared<Atlas>("./assets/img/enemy_left_", 6);
std::shared_ptr<Atlas> atlas_enemy_right = std::make_shared<Atlas>("./assets/img/enemy_right_", 6);


Animation::Animation(const std::shared_ptr<Atlas>& atlas, int interfal)//路径，帧数量，帧间隔时间
{
	interfal_ms = interfal;
	anim_atlas = atlas;
}
Animation::~Animation() = default;

//播放动画
void Animation::play(int x, int y, int delta)//坐标x，坐标y，距离播放上一帧的间隔时间
{
	timer += delta;
	if (timer >= interfal_ms)
	{
		idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
		timer = 0;
	}

	putimageAlpha(x, y, anim_atlas->frame_list[idx_frame].get());
}

//获取图片分辨率
std::pair<int, int> Animation::GetRes()
{
	return anim_atlas->player_res;
}

Player::Player()
{
	loadimage(&img_shadow, "./assets/img/shadow_player.png");
	anim_left_player = std::make_unique<Animation>(atlas_player_left, 45);
	anim_right_player = std::make_unique<Animation>(atlas_player_right, 45);
	frame_width = anim_left_player->GetRes().first;
	frame_height = anim_left_player->GetRes().second;
}

Player::~Player() = default;

//处理玩家操作信息
void Player::ProcessEvent(const ExMessage& msg)
{

	//方向键按下逻辑（wasd）
	if (msg.message == WM_KEYDOWN)
	{
		switch (msg.vkcode)
		{
		case'W':
			is_move_up = true;
			break;
		case'S':
			is_move_down = true;
			break;
		case'A':
			is_move_left = true;
			break;
		case'D':
			is_move_right = true;
			break;
		}
	}

	//按键抬起逻辑
	if (msg.message == WM_KEYUP)
	{
		switch (msg.vkcode)
		{
		case'W':
			is_move_up = false;
			break;
		case'S':
			is_move_down = false;
			break;
		case'A':
			is_move_left = false;
			break;
		case'D':
			is_move_right = false;
			break;
		}
	}

}

//播放动画
void Player::Draw(int delta)
{
	int player_width = frame_width;//角色宽度
	int player_height = frame_height;//角色高度
	int player_shadow_x = player_point.x + (player_width / 2 - SHADOW_WIDTH / 2);//阴影横坐标
	int player_shadow_y = player_point.y + (player_height - 8);//阴影纵坐标
	putimageAlpha(player_shadow_x, player_shadow_y, &img_shadow);

	static bool facing_left = false;
	int dir_x = is_move_right - is_move_left;
	if (dir_x > 0) facing_left = false;
	else if (dir_x < 0)facing_left = true;

	if (facing_left)
	{
		anim_left_player->play(player_point.x, player_point.y, delta);
	}
	else
	{
		anim_right_player->play(player_point.x, player_point.y, delta);
	}
}

//角色移动
void Player::Move()
{
	int dir_x = is_move_right - is_move_left;
	int dir_y = is_move_down - is_move_up;
	double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
	if (len_dir)
	{
		double normalized_x = dir_x / len_dir;
		double normalized_y = dir_y / len_dir;

		player_point.x += (int)(PLAYER_SPEED * normalized_x);

		player_point.y += (int)(PLAYER_SPEED * normalized_y);
	}
	//纵坐标越界检查
	if (player_point.y < 0) player_point.y = 0;
	else if (player_point.y > WINDOW_HEIGHT - frame_height)
		player_point.y = WINDOW_HEIGHT - frame_height;

	//横坐标越界检查
	if (player_point.x < 0) player_point.x = 0;
	else if (player_point.x > WINDOW_WIDTH - frame_width)
		player_point.x = WINDOW_WIDTH - frame_width;
}

//返回角色位置
const POINT& Player::GetPosition() const
{
	return player_point;
}

//返回角色图片分辨率
std::pair<int, int> Player::GetSize() const
{
	return{ frame_width,frame_height };
}



Enemy::Enemy()
{
	loadimage(&img_shadow, "./assets/img/shadow_player.png");
	anim_left = std::make_unique<Animation>(atlas_enemy_left, 45);
	anim_right = std::make_unique<Animation>(atlas_enemy_right, 45);
	frame_width = anim_left->GetRes().first;
	frame_height = anim_left->GetRes().second;

	//敌人生成边界枚举
	enum class SpawnEage
	{
		Up = 0,
		Down,
		Left,
		Right
	};

	SpawnEage edge = (SpawnEage)(rand() % 4);
	switch (edge)
	{
	case SpawnEage::Up:
		position.x = rand() % WINDOW_WIDTH;
		position.y = -frame_height;
		break;
	case SpawnEage::Down:
		position.x = rand() % WINDOW_WIDTH;
		position.y = WINDOW_HEIGHT;
		break;
	case SpawnEage::Left:
		position.x = -frame_width;
		position.y = rand() % WINDOW_HEIGHT;
		break;
	case SpawnEage::Right:
		position.x = WINDOW_WIDTH;
		position.y = rand() % WINDOW_HEIGHT;
		break;
	default:
		break;
	}
}

Enemy::~Enemy() = default;

//敌人移动逻辑，追踪角色位置移动
void Enemy::Move(const Player& player)
{
	const POINT& player_position = player.GetPosition();
	int dir_x = player_position.x - position.x;
	int dir_y = player_position.y - position.y;
	double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
	if (len_dir)
	{
		double normalized_x = dir_x / len_dir;
		double normalized_y = dir_y / len_dir;
		position.x += (int)(SPEED * normalized_x);
		position.y += (int)(SPEED * normalized_y);
	}
}

//绘画敌人
void Enemy::Draw(int delta)
{
	int img_shadow_x = position.x + (frame_width / 2 - SHADOW_WIDTH / 2);
	int img_shadow_y = position.y + frame_height - 35;
	putimageAlpha(img_shadow_x, img_shadow_y, &img_shadow);
	if (facing_left)
	{
		anim_left->play(position.x, position.y, delta);
	}
	else
	{
		anim_right->play(position.x, position.y, delta);
	}
}

//判断子弹碰撞
bool Enemy::CheckBulletCollision(const Bullet& bullet)
{
	bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + frame_width;
	bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + frame_height;
	return is_overlap_x && is_overlap_y;
}

//判断角色碰撞
bool Enemy::CheckPlayerCollision(const Player& player)
{
	//将敌人的中心位置作为判定点
	POINT check_position = { position.x + frame_width / 2,position.y + frame_height / 2 };
	POINT player_position = player.GetPosition();
	std::pair<int, int> player_size = player.GetSize();

	bool is_overlap_x = check_position.x >= player_position.x && check_position.x <= player_position.x + player_size.first;
	bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + player_size.second;
	return is_overlap_x && is_overlap_y;
}

//返回敌人是否存活
bool Enemy::CheckAlive()
{
	return alive;
}

//击杀
void Enemy::Hurt()
{
	alive = false;
}


Bullet::Bullet() = default;
Bullet::~Bullet() = default;

//绘制子弹
void Bullet::Draw()
{
	setlinecolor(RGB(65, 105, 225));
	setfillcolor(RGB(65, 105, 225));
	fillcircle(position.x, position.y, RADIUS);
}