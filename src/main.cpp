#include <graphics.h>
#include<Windows.h>
#include<SFML/Audio.hpp>
#include<math.h>
#include<string>
#include<vector>
#include<format>
#include<memory>
#include<utility>
#include<chrono>


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

sf::Music music("assets/mus/bgm.mp3");//创建背景音乐对象


//加载透明图片
void putimageAlpha(int x, int y, IMAGE* srcimg);

//动画资源类
class Atlas
{
public:
	Atlas(const std::string& path,int num)
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
	~Atlas() = default;
public:
	std::vector<std::unique_ptr<IMAGE>> frame_list;//动画图片指针数组
	std::pair<int, int> player_res;//图片分辨率
};

std::shared_ptr<Atlas> atlas_player_left = std::make_shared<Atlas>("./assets/img/player_left_", 6);
std::shared_ptr<Atlas> atlas_player_right = std::make_shared<Atlas>("./assets/img/player_right_", 6);
std::shared_ptr<Atlas> atlas_enemy_left = std::make_shared<Atlas>("./assets/img/enemy_left_", 6);
std::shared_ptr<Atlas> atlas_enemy_right = std::make_shared<Atlas>("./assets/img/enemy_right_", 6);

//动画类(png)
class Animation
{
public:
	Animation(const std::shared_ptr<Atlas>& atlas,int interfal)//路径，帧数量，帧间隔时间
	{
		interfal_ms = interfal;
		anim_atlas = atlas;
	}
	~Animation() = default;
	
	//播放动画
	void play(int x, int y, int delta)//坐标x，坐标y，距离播放上一帧的间隔时间
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
	std::pair<int, int> GetRes()
	{
		return anim_atlas->player_res;
	}
private:
	int timer = 0;//动画计时器
	int idx_frame = 0;//动画帧索引
	int interfal_ms = 0;//定义帧间隔时间
	std::shared_ptr<Atlas> anim_atlas;
};


class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, "./assets/img/shadow_player.png");
		anim_left_player = std::make_unique<Animation>(atlas_player_left,45);
		anim_right_player = std::make_unique<Animation>(atlas_player_right,45);
		frame_width = anim_left_player->GetRes().first;
		frame_height = anim_left_player->GetRes().second;
	}
	~Player() = default;

	//处理玩家操作信息
	void ProcessEvent(const ExMessage& msg)
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
	void Draw(int delta)
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
	void Move()
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

	const POINT& GetPosition() const
	{
		return player_point;
	}

	std::pair<int, int> GetSize() const
	{
		return{ frame_width,frame_height };
	}

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

class Bullet
{
public:
	POINT position = { 0,0 };//子弹位置
public:
	Bullet() = default;
	~Bullet() = default;

	//绘制子弹
	void Draw()
	{
		setlinecolor(RGB(65, 105, 225));
		setfillcolor(RGB(65, 105, 225));
		fillcircle(position.x, position.y,RADIUS);
	}

	
private:
	const int RADIUS = 10;//子弹半径
};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, "./assets/img/shadow_player.png");
		anim_left = std::make_unique<Animation>(atlas_enemy_left,45);
		anim_right = std::make_unique<Animation>(atlas_enemy_right,45);
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
	~Enemy() = default;

	//敌人移动逻辑，追踪角色位置移动
	void Move(const Player& player)
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

	void Draw(int delta)
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
	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + frame_height;
		return is_overlap_x && is_overlap_y;
	}
	//判断角色碰撞
	bool CheckPlayerCollision(const Player& player)
	{
		//将敌人的中心位置作为判定点
		POINT check_position = { position.x + frame_width / 2,position.y + frame_height / 2 };
		POINT player_position = player.GetPosition();
		std::pair<int, int> player_size = player.GetSize();

		bool is_overlap_x = check_position.x >= player_position.x && check_position.x<= player_position.x + player_size.first;
		bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + player_size.second;
		return is_overlap_x && is_overlap_y;
	}

	//返回敌人是否存活
	bool CheckAlive()
	{
		return alive;
	}

	//击杀
	void Hurt()
	{
		alive = false;
	}
private:
	const int SPEED = 2;//敌人速度
	const int SHADOW_WIDTH=48;//影子宽度


	IMAGE img_shadow;//阴影图片
	std::unique_ptr<Animation> anim_left;//朝左动画对象
	std::unique_ptr<Animation> anim_right;//朝右动画对象
	POINT position = { 0,0 };//敌人位置
	bool facing_left = false;//是否朝左
	bool alive = true;
	int frame_width;//敌人宽度
	int frame_height;//敌人高度
};
//按钮基类
class Button
{
public:
	Button(RECT rect,
		const std::string& path_img_idle, 
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left,int top)
	{
		region = rect;
		region.left = left,region.right = left+BUTTON_WIDTH, region.top = top, region.bottom = top+BUTTON_HEIGHT;
		

		//加载gui界面的图片
		loadimage(&img_idle, path_img_idle.c_str());
		loadimage(&img_hovered, path_img_hovered.c_str());
		loadimage(&img_pushed, path_img_pushed.c_str());
	}
	~Button()
	{

	}

	//消息处理
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
			{
				status = Status::Hovered;
			}
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
			{
				status = Status::Idle;
			}
			break;
		case WM_LBUTTONDOWN:
			if (status!=Status::Pushed && CheckCursorHit(msg.x, msg.y))
			{
				status = Status::Pushed;
			}
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				Onclick();
			break;
		default:
			break;
		}
	}

	//绘制画面
	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}
protected:
	virtual void Onclick() = 0;
private:
	bool CheckCursorHit(int x, int y)
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
private:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

	RECT region;//界面矩形区域
	IMAGE img_idle;//鼠标没有悬停
	IMAGE img_hovered;//鼠标悬停
	IMAGE img_pushed;//鼠标按下
	Status status = Status::Idle;//按钮状态枚举
};
//开始游戏按钮
class StartGameButton :public Button
{
public:
	StartGameButton(RECT rect,
		const std::string& path_img_idle,
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left, int top)
		:Button(rect,path_img_idle,path_img_hovered,path_img_pushed,left,top){ }
	~StartGameButton() = default;
protected:
	void Onclick()
	{
		is_game_started = true;
		music.setLooping(true);//循环播放
		music.play();//开始播放
	}
};
//退出游戏按钮
class QuitGameButton :public Button
{
public:
	QuitGameButton(RECT rect,
		const std::string& path_img_idle,
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left,int top)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed,left,top) {
	}
	~QuitGameButton() = default;
protected:
	void Onclick()
	{
		running = false;
	}
};


//加载透明图片
void putimageAlpha(int x, int y,IMAGE* srcimg)
{
	int w = srcimg->getwidth();
	int h = srcimg->getheight();

	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(srcimg), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//更新敌人数量
void TryGenerateEnemy(std::vector<std::unique_ptr<Enemy>>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
	{
		enemy_list.emplace_back(std::make_unique<Enemy>());
	}
	//printf("enemy count: %zu\n", enemy_list.size());

}

//更新子弹数量
void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045; //径向速度（半径变化速度）
	const double TANGENT_SPEED = 0.0055; //切向速度（旋转速度）
	double radian_interfal = 2 * 3.14159 / bullet_list.size();//子弹之间的弧度间隔
	POINT player_position = player.GetPosition();
	auto player_res = player.GetSize();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);//半径，依托于时间连续性保持每一次调用连贯
	for (int i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interfal * i;//每颗子弹的弧度
		bullet_list[i].position.x = player_position.x + player_res.first/2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_position.y + player_res.second/2 + (int)(radius * sin(radian));
	}
}

//绘制玩家得分
void DrawPlayerScore(int score)
{
	std::string score_text = std::format("{}{}", "当前玩家得分：", score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, score_text.c_str());
}



int main()
{
	int score=0;//得分
	ExMessage msg;//消息
	Player myplayer;
	IMAGE image_background;//游戏背景
	IMAGE img_menu;//页面背景

	std::vector<std::unique_ptr<Enemy>> enemy_list;//敌人指针列表
	std::vector<Bullet> bullet_list(3);

	
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);//初始化窗口
	loadimage(&image_background, ("assets/img/background.png"));//加载背景图片到IMAGE
	loadimage(&img_menu, ("assets/img/menu.png"));
	sf::SoundBuffer hit("assets/mus/hit.wav");//创建受击音乐对象
	sf::Sound sound(hit);

	//设置按钮
	RECT region_start_game = { 0,0,0,0 }, region_quit_game = { 0,0,0,0 };
	//两个按钮
	StartGameButton btn_start_game(region_start_game,
		("assets/img/ui_start_idle.png"), ("assets/img/ui_start_hovered.png"), ("assets/img/ui_start_pushed.png"),
		(WINDOW_WIDTH-BUTTON_WIDTH)/2,430);
	QuitGameButton btn_quit_game(region_quit_game,
		("assets/img/ui_quit_idle.png"), ("assets/img/ui_quit_hovered.png"), ("assets/img/ui_quit_pushed.png"),
		(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 550);


	//开启动态缓冲
	BeginBatchDraw();

	//游戏主循环
	while (running)
	{
		//判断运行时间
		auto start_time = std::chrono::system_clock::now();
		TryGenerateEnemy(enemy_list);

		while (peekmessage(&msg))
		{
			if (is_game_started)
			{
				myplayer.ProcessEvent(msg);
			}
			else
			{
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
			}
		}

		if (is_game_started)
		{
			myplayer.Move();//角色移动
			for (auto& enemy : enemy_list)//敌人移动
			{
				enemy->Move(myplayer);
			}
			UpdateBullets(bullet_list, myplayer);

			//判断敌人和角色的碰撞
			for (auto& enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollision(myplayer))
				{
					static TCHAR text[64];
					_stprintf_s(text, _T("最终得分为：%d !"), score);
					MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
					running = false;
					break;
				}
			}

			//判断敌人与子弹的碰撞
			for (auto& enemy : enemy_list)
			{
				for (auto& bullet : bullet_list)
				{
					if (enemy->CheckBulletCollision(bullet))
					{
						enemy->Hurt();
						score++;
						sound.play();
						break;
					}
				}
			}


			//删除死去的敌人
			for (int i = 0; i < enemy_list.size(); i++)
			{
				auto& enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
				}
			}
		}
		

		//清屏
		cleardevice();

		if (is_game_started)
		{
			//要加载的游戏画面
			putimage(0, 0, &image_background);
			myplayer.Draw(1000 / 144);
			//绘制敌人
			for (auto& enemy : enemy_list)
			{
				enemy->Draw(1000 / 144);
			}
			//绘制子弹
			for (auto& bullet : bullet_list)
			{
				bullet.Draw();
			}
			DrawPlayerScore(score);
		}
		else
		{
			putimage(0, 0, &img_menu);
			btn_start_game.Draw();
			btn_quit_game.Draw();
		}
		

		FlushBatchDraw();

		//计算主循环时长
		auto end_time = std::chrono::system_clock::now();
		auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();


		if (delta_time < 1000 / 144)
		{
			Sleep((1000 / 144) - delta_time);

		}
	}

	EndBatchDraw();
	return 0;
}