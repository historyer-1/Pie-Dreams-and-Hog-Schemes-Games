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

sf::Music music("assets/mus/bgm.mp3");//�����������ֶ���


//����͸��ͼƬ
void putimageAlpha(int x, int y, IMAGE* srcimg);

//������Դ��
class Atlas
{
public:
	Atlas(const std::string& path,int num)
	{
		std::string path_file;
		//�����ڴ�洢֡������ָ�������������ʹ������
		for (int i = 0; i < num; i++)
		{
			path_file = std::format("{}{}.png", path, i);

			frame_list.emplace_back(std::make_unique<IMAGE>());
			auto& frame = frame_list.back();
			loadimage(frame.get(), path_file.c_str());

		}

		//ͼƬ�ֱ���
		player_res.first = frame_list[0]->getwidth();
		player_res.second = frame_list[0]->getheight();
	}
	~Atlas() = default;
public:
	std::vector<std::unique_ptr<IMAGE>> frame_list;//����ͼƬָ������
	std::pair<int, int> player_res;//ͼƬ�ֱ���
};

std::shared_ptr<Atlas> atlas_player_left = std::make_shared<Atlas>("./assets/img/player_left_", 6);
std::shared_ptr<Atlas> atlas_player_right = std::make_shared<Atlas>("./assets/img/player_right_", 6);
std::shared_ptr<Atlas> atlas_enemy_left = std::make_shared<Atlas>("./assets/img/enemy_left_", 6);
std::shared_ptr<Atlas> atlas_enemy_right = std::make_shared<Atlas>("./assets/img/enemy_right_", 6);

//������(png)
class Animation
{
public:
	Animation(const std::shared_ptr<Atlas>& atlas,int interfal)//·����֡������֡���ʱ��
	{
		interfal_ms = interfal;
		anim_atlas = atlas;
	}
	~Animation() = default;
	
	//���Ŷ���
	void play(int x, int y, int delta)//����x������y�����벥����һ֡�ļ��ʱ��
	{
		timer += delta;
		if (timer >= interfal_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}

		putimageAlpha(x, y, anim_atlas->frame_list[idx_frame].get());
	}

	//��ȡͼƬ�ֱ���
	std::pair<int, int> GetRes()
	{
		return anim_atlas->player_res;
	}
private:
	int timer = 0;//������ʱ��
	int idx_frame = 0;//����֡����
	int interfal_ms = 0;//����֡���ʱ��
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

	//������Ҳ�����Ϣ
	void ProcessEvent(const ExMessage& msg)
	{

			//����������߼���wasd��
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

			//����̧���߼�
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

	//���Ŷ���
	void Draw(int delta)
	{
		int player_width = frame_width;//��ɫ���
		int player_height = frame_height;//��ɫ�߶�
		int player_shadow_x = player_point.x + (player_width / 2 - SHADOW_WIDTH / 2);//��Ӱ������
		int player_shadow_y = player_point.y + (player_height - 8);//��Ӱ������
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
	
	//��ɫ�ƶ�
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
		//������Խ����
		if (player_point.y < 0) player_point.y = 0;
		else if (player_point.y > WINDOW_HEIGHT - frame_height)
			player_point.y = WINDOW_HEIGHT - frame_height;

		//������Խ����
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

class Bullet
{
public:
	POINT position = { 0,0 };//�ӵ�λ��
public:
	Bullet() = default;
	~Bullet() = default;

	//�����ӵ�
	void Draw()
	{
		setlinecolor(RGB(65, 105, 225));
		setfillcolor(RGB(65, 105, 225));
		fillcircle(position.x, position.y,RADIUS);
	}

	
private:
	const int RADIUS = 10;//�ӵ��뾶
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

		//�������ɱ߽�ö��
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

	//�����ƶ��߼���׷�ٽ�ɫλ���ƶ�
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
	//�ж��ӵ���ײ
	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + frame_width;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + frame_height;
		return is_overlap_x && is_overlap_y;
	}
	//�жϽ�ɫ��ײ
	bool CheckPlayerCollision(const Player& player)
	{
		//�����˵�����λ����Ϊ�ж���
		POINT check_position = { position.x + frame_width / 2,position.y + frame_height / 2 };
		POINT player_position = player.GetPosition();
		std::pair<int, int> player_size = player.GetSize();

		bool is_overlap_x = check_position.x >= player_position.x && check_position.x<= player_position.x + player_size.first;
		bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + player_size.second;
		return is_overlap_x && is_overlap_y;
	}

	//���ص����Ƿ���
	bool CheckAlive()
	{
		return alive;
	}

	//��ɱ
	void Hurt()
	{
		alive = false;
	}
private:
	const int SPEED = 2;//�����ٶ�
	const int SHADOW_WIDTH=48;//Ӱ�ӿ��


	IMAGE img_shadow;//��ӰͼƬ
	std::unique_ptr<Animation> anim_left;//���󶯻�����
	std::unique_ptr<Animation> anim_right;//���Ҷ�������
	POINT position = { 0,0 };//����λ��
	bool facing_left = false;//�Ƿ���
	bool alive = true;
	int frame_width;//���˿��
	int frame_height;//���˸߶�
};
//��ť����
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
		

		//����gui�����ͼƬ
		loadimage(&img_idle, path_img_idle.c_str());
		loadimage(&img_hovered, path_img_hovered.c_str());
		loadimage(&img_pushed, path_img_pushed.c_str());
	}
	~Button()
	{

	}

	//��Ϣ����
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

	//���ƻ���
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

	RECT region;//�����������
	IMAGE img_idle;//���û����ͣ
	IMAGE img_hovered;//�����ͣ
	IMAGE img_pushed;//��갴��
	Status status = Status::Idle;//��ť״̬ö��
};
//��ʼ��Ϸ��ť
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
		music.setLooping(true);//ѭ������
		music.play();//��ʼ����
	}
};
//�˳���Ϸ��ť
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


//����͸��ͼƬ
void putimageAlpha(int x, int y,IMAGE* srcimg)
{
	int w = srcimg->getwidth();
	int h = srcimg->getheight();

	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(srcimg), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}

//���µ�������
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

//�����ӵ�����
void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045; //�����ٶȣ��뾶�仯�ٶȣ�
	const double TANGENT_SPEED = 0.0055; //�����ٶȣ���ת�ٶȣ�
	double radian_interfal = 2 * 3.14159 / bullet_list.size();//�ӵ�֮��Ļ��ȼ��
	POINT player_position = player.GetPosition();
	auto player_res = player.GetSize();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);//�뾶��������ʱ�������Ա���ÿһ�ε�������
	for (int i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interfal * i;//ÿ���ӵ��Ļ���
		bullet_list[i].position.x = player_position.x + player_res.first/2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_position.y + player_res.second/2 + (int)(radius * sin(radian));
	}
}

//������ҵ÷�
void DrawPlayerScore(int score)
{
	std::string score_text = std::format("{}{}", "��ǰ��ҵ÷֣�", score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, score_text.c_str());
}



int main()
{
	int score=0;//�÷�
	ExMessage msg;//��Ϣ
	Player myplayer;
	IMAGE image_background;//��Ϸ����
	IMAGE img_menu;//ҳ�汳��

	std::vector<std::unique_ptr<Enemy>> enemy_list;//����ָ���б�
	std::vector<Bullet> bullet_list(3);

	
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);//��ʼ������
	loadimage(&image_background, ("assets/img/background.png"));//���ر���ͼƬ��IMAGE
	loadimage(&img_menu, ("assets/img/menu.png"));
	sf::SoundBuffer hit("assets/mus/hit.wav");//�����ܻ����ֶ���
	sf::Sound sound(hit);

	//���ð�ť
	RECT region_start_game = { 0,0,0,0 }, region_quit_game = { 0,0,0,0 };
	//������ť
	StartGameButton btn_start_game(region_start_game,
		("assets/img/ui_start_idle.png"), ("assets/img/ui_start_hovered.png"), ("assets/img/ui_start_pushed.png"),
		(WINDOW_WIDTH-BUTTON_WIDTH)/2,430);
	QuitGameButton btn_quit_game(region_quit_game,
		("assets/img/ui_quit_idle.png"), ("assets/img/ui_quit_hovered.png"), ("assets/img/ui_quit_pushed.png"),
		(WINDOW_WIDTH - BUTTON_WIDTH) / 2, 550);


	//������̬����
	BeginBatchDraw();

	//��Ϸ��ѭ��
	while (running)
	{
		//�ж�����ʱ��
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
			myplayer.Move();//��ɫ�ƶ�
			for (auto& enemy : enemy_list)//�����ƶ�
			{
				enemy->Move(myplayer);
			}
			UpdateBullets(bullet_list, myplayer);

			//�жϵ��˺ͽ�ɫ����ײ
			for (auto& enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollision(myplayer))
				{
					static TCHAR text[64];
					_stprintf_s(text, _T("���յ÷�Ϊ��%d !"), score);
					MessageBox(GetHWnd(), text, _T("��Ϸ����"), MB_OK);
					running = false;
					break;
				}
			}

			//�жϵ������ӵ�����ײ
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


			//ɾ����ȥ�ĵ���
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
		

		//����
		cleardevice();

		if (is_game_started)
		{
			//Ҫ���ص���Ϸ����
			putimage(0, 0, &image_background);
			myplayer.Draw(1000 / 144);
			//���Ƶ���
			for (auto& enemy : enemy_list)
			{
				enemy->Draw(1000 / 144);
			}
			//�����ӵ�
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

		//������ѭ��ʱ��
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