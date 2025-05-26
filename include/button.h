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

//��ť����
class Button
{
public:
	Button(RECT rect,
		const std::string& path_img_idle,
		const std::string& path_img_hovered,
		const std::string& path_img_pushed,
		int left, int top);
	~Button();

	//��Ϣ����
	void ProcessEvent(const ExMessage& msg);

	//���ƻ���
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
		:Button(rect, path_img_idle, path_img_hovered, path_img_pushed, left, top){ }
	~StartGameButton()=default;
protected:
	void Onclick();
};

//�˳���Ϸ��ť
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