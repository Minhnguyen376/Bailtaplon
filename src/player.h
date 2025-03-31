#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>

#include "entity.h"
#include "ground.h"

class Player : public Entity
{
public:
	Player(float p_x, float p_y, std::vector<SDL_Texture *> p_tex);
	float distanceFromCursor();
	bool jump();
	void update(Ground &ground);
	const char *getScore();
	const char *getHighscore();
	int getScoreInt();
	int isDead();
	void reset();
	int highscore = 0;
	int score = 0;

private:
	float velocityX, velocityY;
	bool grounded;
	float clamp(float p_value, float p_min, float p_max);

	int timer = 0;
	int dead = 0;
};
