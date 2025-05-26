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

//按钮基类
class Button
{
public:
	Button(RECT rect,
		const std::string& path_img_idle,
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left, int top);
	~Button();

	//消息处理
	void ProcessEvent(const ExMessage& msg);

	//绘制画面
	void Draw();
protected:
	virtual void Onclick() = 0;
private:
	bool CheckCursorHit(int x, int y);
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
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed, left, top){ }
	~StartGameButton()=default;
protected:
	void Onclick();
};

//退出游戏按钮
class QuitGameButton :public Button
{
public:
	QuitGameButton(RECT rect,
		const std::string& path_img_idle,
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left, int top)
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed, left, top){ }
	~QuitGameButton()=default;
protected:
	void Onclick()
	{
		running = false;
	}
};