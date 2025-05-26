#include"pch.h"

Button::Button(RECT rect,
	const std::string& path_img_idle,
	const std::string& path_img_hovered,
	const std::string& path_img_pushed,
	int left, int top)
{
	region = rect;
	region.left = left, region.right = left + BUTTON_WIDTH, region.top = top, region.bottom = top + BUTTON_HEIGHT;


	//加载gui界面的图片
	loadimage(&img_idle, path_img_idle.c_str());
	loadimage(&img_hovered, path_img_hovered.c_str());
	loadimage(&img_pushed, path_img_pushed.c_str());
}

Button::~Button() = default;

//消息处理
void Button::ProcessEvent(const ExMessage& msg)
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
		if (status != Status::Pushed && CheckCursorHit(msg.x, msg.y))
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
void Button::Draw()
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

bool Button::CheckCursorHit(int x, int y)
{
	return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
}


void StartGameButton::Onclick()
{
	is_game_started = true;
	music.setLooping(true);//循环播放
	music.play();//开始播放
}
