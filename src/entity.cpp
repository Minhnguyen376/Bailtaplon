#include <SDL.h>
#include <SDL_image.h>
#include <vector>

#include "entity.h"

Entity::Entity(float p_x, float p_y, std::vector<SDL_Texture *> p_tex)
	: x(p_x), y(p_y), tex(p_tex)
{
	// Tạo frame đầu tiên để render
	currentFrame.x = 0;
	currentFrame.y = 0;
	// Lấy kích thước từ texture đầu tiên
	SDL_QueryTexture(p_tex.at(0), NULL, NULL, &currentFrame.w, &currentFrame.h);
	
	// Tạo offset ban đầu cho từng frame
	for (int i = 0; i < getSize(); i++)
	{
		animOffsetsX.push_back(0);
		animOffsetsY.push_back(0);
	}
}

// Khởi tạo Entity với một texture duy nhất
Entity::Entity(float p_x, float p_y, SDL_Texture *p_tex)
	: x(p_x), y(p_y)
{
	// Lưu texture vào vector
	tex.push_back(p_tex);
	
	currentFrame.x = 0;
	currentFrame.y = 0;

	// Lấy kích thước từ texture
	SDL_QueryTexture(p_tex, NULL, NULL, &currentFrame.w, &currentFrame.h);
	for (int i = 0; i < getSize(); i++)
	{
		animOffsetsX.push_back(0);
		animOffsetsY.push_back(0);
	}
}
// Trả vị trí X, Y
float Entity::getX()
{
	return x;
}

float Entity::getY()
{
	return y;
}

// dài rộng của frame
int Entity::getWidth()
{
	return currentFrame.w;
}

int Entity::getHeight()
{
	return currentFrame.h;
}

int Entity::getSize()
{
	return tex.size(); // số texture
}

// reset X, Y
void Entity::setX(float p_x)
{
	x = p_x;
}

void Entity::setY(float p_y)
{
	y = p_y;
}

// trả về offset để gán lại
float Entity::getAnimOffsetX(int p_index)
{
	return animOffsetsX.at(p_index);
}

float Entity::getAnimOffsetY(int p_index)
{
	return animOffsetsY.at(p_index);
}

void Entity::setAnimOffsetX(int p_index, int p_value)
{
	animOffsetsX[p_index] = p_value;
}

void Entity::setAnimOffsetY(int p_index, int p_value)
{
	animOffsetsY[p_index] = p_value;
}

SDL_Texture *Entity::getTex(int p_index)
{
	return tex.at(p_index);
}

//thông tin screen hiện tại 
SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}

void Entity::setTex(SDL_Texture *p_tex)
{
	tex[0] = p_tex;
}
