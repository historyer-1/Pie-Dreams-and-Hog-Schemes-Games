#include"pch.h"

//����͸��ͼƬ
void putimageAlpha(int x, int y, IMAGE* srcimg)
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
		bullet_list[i].position.x = player_position.x + player_res.first / 2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_position.y + player_res.second / 2 + (int)(radius * sin(radian));
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