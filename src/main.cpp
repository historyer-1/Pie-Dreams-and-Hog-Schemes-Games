#include"pch.h"


int main()
{


	int score=0;//得分
	ExMessage msg;//消息
	Player myplayer;
	IMAGE image_background;//游戏背景
	IMAGE img_menu;//页面背景

	std::vector<std::unique_ptr<Enemy>> enemy_list;//敌人指针列表
	std::vector<Bullet> bullet_list(3);
	music.openFromFile("assets/mus/bgm.mp3");
	
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