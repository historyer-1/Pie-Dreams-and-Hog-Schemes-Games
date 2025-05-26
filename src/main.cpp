#include"pch.h"


int main()
{


	int score=0;//�÷�
	ExMessage msg;//��Ϣ
	Player myplayer;
	IMAGE image_background;//��Ϸ����
	IMAGE img_menu;//ҳ�汳��

	std::vector<std::unique_ptr<Enemy>> enemy_list;//����ָ���б�
	std::vector<Bullet> bullet_list(3);
	music.openFromFile("assets/mus/bgm.mp3");
	
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