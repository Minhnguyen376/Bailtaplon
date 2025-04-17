#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>
#include <string>

#include "player.h"
#include "entity.h"
#include "ground.h"

//screen size
const float GRAVITY = 0.09f;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 480;
//status nhân vật
const int ALIVE = 0;
const int CURSOR_DEATH = 1;
const int HOLE_DEATH = 2;

// Khởi tạo player và texture
Player::Player(float p_x, float p_y, std::vector<SDL_Texture *> p_tex)
	: Entity{p_x, p_y, p_tex}
{
	// Đặt nhân vật ở giữa màn hình theo trục X, sát mặt đất theo trục Y
	setX(SCREEN_WIDTH / 2 - getWidth() / 2);
	setY(SCREEN_HEIGHT - getHeight() - 64);
}
// Tính dis từ tâm nhân vật đến chuột
float Player::distanceFromCursor()
{
	int mouseX = 0;
	int mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	return sqrt(pow((getX() + getWidth() / 2) - mouseX, 2) + pow((getY() + getHeight() / 2) - mouseY, 2));
}
// Thực hiện nhảy nếu đang đứng trên mặt đất và chuột gần nhân vật
bool Player::jump()
{
	if (distanceFromCursor() < 100)
	{
		if (grounded)
		{
			// Lực nhảy
			velocityY = -(1 / distanceFromCursor() * 200);
			grounded = false;
			return true;
		}
	}
	return false;
}

float Player::clamp(float p_value, float p_min, float p_max)
{
	if (p_value > p_max)
		return p_max;
	if (p_value < p_min)
		return p_min;
	return p_value;
}
// Cập nhật stauts player
void Player::update(Ground &ground)
{
	timer++;
	score = timer / 50; // Tăng điểm theo thời gian sống
	if (score > highscore)
	{
		highscore = score;
	}

	setX(getX() - 1); // autoscroll
	int mouseX = 0;
	int mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	setAnimOffsetY(1, 0);
	setAnimOffsetY(2, 0);

	if (distanceFromCursor() < 100)
	{
		setAnimOffsetY(1, sin(SDL_GetTicks() / 50) * velocityX - 4);
		setAnimOffsetY(2, -sin(SDL_GetTicks() / 50) * velocityX - 4);
		if (mouseX < getX())
		{
			velocityX = 1 / distanceFromCursor() * 100;
		}
		else if (mouseX > getX() + getWidth())
		{
			velocityX = 1 / distanceFromCursor() * -100;
		}
		else
		{
			// Nếu chuột nằm trong vùng nhân vật thì chết
			velocityX = 0;
			if (mouseY > getY() && mouseY < getY() + getHeight())
			{
				dead = CURSOR_DEATH;
			}
		}
	}
	else
	{
		velocityX = 0;
	}
	
	// Cập nhật vị trí theo vận tốc
	setX(getX() + velocityX);
	setY(getY() + velocityY);

	// Kiểm tra có nền bên dưới không
	if (ground.isTileBelow(getX(), getWidth()))
	{
		if (getY() + getHeight() < SCREEN_HEIGHT - 64)
		{
			velocityY += GRAVITY;
			grounded = false;
		}
		else
		{
			// Chạm hẳn mặt đất thì tiếp tục rơi
			if (getY() < SCREEN_HEIGHT - getHeight() - 64 + 20)
			{
				setY(SCREEN_HEIGHT - getHeight() - 64);
				grounded = true;
			}
		}
	}
	else
	{
		// Rơi tự do khi không có nền
		velocityY += GRAVITY;
		grounded = false;
		if (getY() > SCREEN_HEIGHT) // Nếu rơi ra khỏi màn hình thì chết vì lỗ
		{
			dead = HOLE_DEATH;
		}
	}
}

// Trả về điểm số dạng chuỗi để in lên màn hình
const char *Player::getScore()
{
	std::string s = std::to_string(score);
	s = "DISTANCE: " + s;
	return s.c_str();
}

// Trả về điểm cao nhất
const char *Player::getHighscore()
{
	std::string s = std::to_string(highscore);
	s = "BEST: " + s;
	return s.c_str();
}

int Player::getScoreInt()
{
	return score;
}

// status
int Player::isDead()
{	
	return dead;
}
// Reset lại status
void Player::reset()
{
	setX(SCREEN_WIDTH / 2 - getWidth() / 2);
	setY(SCREEN_HEIGHT - getHeight() - 64);
	score = 0;
	timer = 0;
	velocityX = 0;
	velocityY = 0;
	dead = 0;
}
