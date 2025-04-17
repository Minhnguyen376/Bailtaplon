#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "ground.h"
#include "groundtile.h"
#include <iostream>
void Ground::add(SDL_Texture *left, SDL_Texture *center, SDL_Texture *right, SDL_Texture *hole)
{
	// 4 status: trái, giữa, phải, lỗ
	groundTiles.clear();
	groundTex[0] = left;
	groundTex[1] = center;
	groundTex[2] = right;
	groundTex[3] = hole;

	for (int i = 0; i < 18; i++)
	{
		groundTiles.push_back(GroundTile(groundTex[1], i)); // 18 tile khởi tạo, center default
	}
}
// Trả về tile tại chỉ số p_index
GroundTile &Ground::getTile(int p_index)
{
	return groundTiles.at(p_index);
}

int Ground::getStatus(int p_index)
{
	return groundTiles[p_index].getStatus();
}

int Ground::getLength()
{
	return groundTiles.size(); // Số lượng tile hiện có (default 18)
}

// check tồn tại tile dưới player (ngang)
bool Ground::isTileBelow(float x, int width)
{
	for (int i = 0; i < getLength(); i++)
	{
		switch (getStatus(i))
		{
		case 0:
			if (x + width > groundTiles[i].getX() + 24 && x < groundTiles[i].getX() + 64)
			{
				return true;
			}
			break;
		case 1:
			if (x + width > groundTiles[i].getX() && x < groundTiles[i].getX() + 64)
			{
				return true;
			}
			break;
		case 2:
			if (x + width > groundTiles[i].getX() && x < groundTiles[i].getX() + 40)
			{
				return true;
			}
			break;
		}
	}
	return false;
}

// Reset tile status
void Ground::reset()
{
	for (int i = 0; i < 18; i++)
	{
		groundTiles[i].setStatus(1, groundTex); // mặc định là tile giữa
		groundTiles[i].setX(i * 64.0f);		// đặt vị trí theo hàng ngang
	}
	lastStatus = 1;
	holeCount = 0;
}
// Cập nhật tile loca theo time và logic tạo lỗ
float randomFloat(float min, float max)
{
	return (rand() / (float)RAND_MAX) * (max - min) + min;
}
void Ground::update(int score)
{
	float speed = 2.5f; //nền speed
	for (int i = 0; i < getLength(); i++)
	{
		groundTiles[i].setX(groundTiles[i].getX() - speed); // Di chuyển tile sang trái theo speed
		//tái sd tile sau khi ra khỏi màn (qua phải)
		if (groundTiles[i].getX() + 64 < 0)
		{
			groundTiles[i].setX(64 * (getLength() - 1) - speed); //Đặt xuống cuối
			switch (lastStatus) //tile mới theo last status
			{
			case 0: //trái - giữa
			{
				groundTiles[i].setStatus(1, groundTex);
				lastStatus = 1;
				holeCount = 0;
				break;
			}
			case 1: // giữa - phải /trái
			{
				int randomInt = rand() % 2 + 1;
				groundTiles[i].setStatus(randomInt, groundTex);
				lastStatus = randomInt;
				holeCount = 0;
				break;
			}
			case 2: // phải - lỗ
			{
				groundTiles[i].setStatus(3, groundTex);
				lastStatus = 3;
				holeCount = 0;
				break;
			}
			case 3: // lỗ - lỗ/ trái
			{
				int randomInt = rand() % 2;
				if (randomInt == 1)
				{
					randomInt = 3;
					holeCount++;
				}
				else
				{
					holeCount = 0;
				}
				// Limit số lỗ liên tiếp theo điểm
				if ((holeCount > 4 && score > 99) || (holeCount > 3 && score < 100))
				{
					randomInt = 0;
					holeCount = 0;
				}
				groundTiles[i].setStatus(randomInt, groundTex);
				lastStatus = randomInt;
				break;
			}
			}
		}
	}
}
