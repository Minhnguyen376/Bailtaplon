#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <stdlib.h>
#include <string.h>

//class
#include "renderwindow.h"
#include "entity.h"
#include "player.h"
#include "ground.h"

//screen size
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 480;

//player status
const int ALIVE = 0;
const int CURSOR_DEATH = 1;
const int HOLE_DEATH = 2;

const Uint8 *keyState; // Trạng thái phím

RenderWindow window; // Tạo cửa sổ

//texture, res
std::vector<SDL_Texture *> playerTex;
SDL_Texture *groundTex[4][4];
SDL_Texture *background[4];
SDL_Texture *arrow;
SDL_Texture *highscoreBox;

//font
TTF_Font *font32;
TTF_Font *font32_outline;
TTF_Font *font24;
TTF_Font *font16;

//màu chữ
SDL_Color white = {255, 255, 255};
SDL_Color black = {0, 0, 0};

// Sound
Mix_Chunk *jumpSfx;
Mix_Chunk *fallSfx;
Mix_Chunk *hitSfx;
Mix_Chunk *clickSfx;
Mix_Music *music;
// Điều khiển game status
bool gameRunning = true;
bool playedDeathSFX = false;
bool mainMenu = true;

//bắt đầu khởi tạo game
bool init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	window.create("Jump-Jump", SCREEN_WIDTH, SCREEN_HEIGHT);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	srand((unsigned)time(0));
	
	// Load texture player (3 layers)
	playerTex.push_back(window.loadTexture("res/textures/player/player_2.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_3.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_4.png"));

	// Load các bộ texture nền (3 nền 1 lỗ)
	groundTex[0][0] = window.loadTexture("res/textures/ground/left (1).png");
	groundTex[0][1] = window.loadTexture("res/textures/ground/center (1).png");
	groundTex[0][2] = window.loadTexture("res/textures/ground/right (1).png");
	groundTex[0][3] = window.loadTexture("res/textures/ground/hole (1).png");
	groundTex[1][0] = window.loadTexture("res/textures/ground/left (2).png");
	groundTex[1][1] = window.loadTexture("res/textures/ground/center (2).png");
	groundTex[1][2] = window.loadTexture("res/textures/ground/right (2).png");
	groundTex[1][3] = window.loadTexture("res/textures/ground/hole (2).png");
	groundTex[2][0] = window.loadTexture("res/textures/ground/left (3).png");
	groundTex[2][1] = window.loadTexture("res/textures/ground/center (3).png");
	groundTex[2][2] = window.loadTexture("res/textures/ground/right (3).png");
	groundTex[2][3] = window.loadTexture("res/textures/ground/hole (3).png");

	// bg và giao diện
	background[0] = window.loadTexture("res/bg/bg (1).png");
	background[1] = window.loadTexture("res/bg/bg (2).png");
	background[2] = window.loadTexture("res/bg/bg (5).png");
	background[3] = window.loadTexture("res/bg/CC.png");
	arrow = window.loadTexture("res/textures/arrow.png");
	highscoreBox = window.loadTexture("res/textures/highscore_box.png");

	//load font 
	font32 = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font32_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font24 = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
	font16 = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
	TTF_SetFontOutline(font32_outline, 3);

	// Load âm thanh
	jumpSfx = Mix_LoadWAV("res/sounds/jump.wav");
	fallSfx = Mix_LoadWAV("res/sounds/fall.wav");
	hitSfx = Mix_LoadWAV("res/sounds/hit.wav");
	clickSfx = Mix_LoadWAV("res/sounds/click.wav");
	music = Mix_LoadMUS("res/sounds/music (1).wav");
	Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME);
	return true;
}

bool load = init();
// Tạo đối tượng chính
Player player(0, 0, playerTex);
Ground ground;

void reset() // Reset lại game status khi chết/ đổi nền
{
	player.reset();
	ground.reset();
}
int id = 0;
bool paused = false;

struct Button
{
	int x, y, w, h;
	Button(int _x = 0, int _y = 0, int _w = 0, int _h = 0)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	}
};
// Menu
Button playButton(400, 210, 180, 24);
Button MuteButtom(400, 300, 64, 24);
Button UnmuteButtom(400, 300, 100, 24);
Button UpBottom(570, 300, 32, 32);
Button DownBottom(540, 300, 32, 32);
Button quitButton(400, 330, 50, 24);

// Kiểm tra chuột có nằm trong nút không
bool isMouseIn(SDL_Event e, Button button)
{
	int mouseX, mouseY;
	int x, y, w, h;
	SDL_GetMouseState(&mouseX, &mouseY);
	if (mouseX < button.x)
		return false;
	if (mouseY < button.y)
		return false;
	if (mouseX > button.x + button.w)
		return false;
	if (mouseY > button.y + button.h)
		return false;
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		return true;
	}
}
// Sound control
bool mute = false;
const int VOLUME_music_max = MIX_MAX_VOLUME / 2;
int VOLUME_music = MIX_MAX_VOLUME / 2;
int VOLUME_chuck = MIX_MAX_VOLUME / 2;

//Loop game
void gameLoop()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
		{
			gameRunning = false;
			break;
		}
		case SDL_KEYDOWN:
		{
			if (event.key.keysym.sym == SDLK_p)
			{
				paused = true;
			}
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				mainMenu = true;
			}

			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			if (mainMenu) // Xử lý nút menu
			{
				if (isMouseIn(event, quitButton))
				{
					gameRunning = false;
					break;
				}
				if (!mute && isMouseIn(event, MuteButtom))
				{
					mute = true;
					Mix_VolumeMusic(0);
					Mix_VolumeChunk(jumpSfx, 0);
					Mix_VolumeChunk(jumpSfx, 0);
					Mix_VolumeChunk(fallSfx, 0);
					Mix_VolumeChunk(hitSfx, 0);
					Mix_VolumeChunk(clickSfx, 0);
					break;
				}
				if (mute && isMouseIn(event, UnmuteButtom))
				{
					mute = false;
					Mix_VolumeMusic(VOLUME_music);
					Mix_VolumeChunk(jumpSfx, VOLUME_chuck);
					Mix_VolumeChunk(fallSfx, VOLUME_chuck);
					Mix_VolumeChunk(hitSfx, VOLUME_chuck);
					Mix_VolumeChunk(clickSfx, VOLUME_chuck);
					break;
				}
				if (isMouseIn(event, UpBottom))
				{
					VOLUME_music += 10;
					mute = false;
					if (VOLUME_music > VOLUME_music_max)
						VOLUME_music = VOLUME_music_max;
					Mix_VolumeMusic(VOLUME_music);
				}
				if (isMouseIn(event, DownBottom))
				{
					VOLUME_music -= 10;
					if (VOLUME_music < 0)
					{
						VOLUME_music = 0;
						Mix_VolumeMusic(0);
						Mix_VolumeChunk(jumpSfx, 0);
						Mix_VolumeChunk(jumpSfx, 0);
						Mix_VolumeChunk(fallSfx, 0);
						Mix_VolumeChunk(hitSfx, 0);
						Mix_VolumeChunk(clickSfx, 0);
						mute = true; 
						// tắt hết âm
					}
					Mix_VolumeMusic(VOLUME_music);
				}
				if (isMouseIn(event, playButton) && SDL_GetTicks() > 2500)
				{
					mainMenu = false;
					Mix_PlayChannel(-1, clickSfx, 0);
				}
			}
			else
			{
				if (event.button.button == SDL_BUTTON_LEFT && player.isDead() == ALIVE)
				{
					if (player.jump())
					{
						Mix_PlayChannel(-1, jumpSfx, 0);
					}
				}
				else if (player.isDead() != ALIVE)
				{
					id++;
					if (id == 3)
						id = 0;
					ground.add(groundTex[id][0], groundTex[id][1], groundTex[id][2], groundTex[id][3]);
					Mix_PlayChannel(-1, clickSfx, 0);
					reset();
					playedDeathSFX = false;
				}
			}
			break;
		}
		}
	}

	// Hiển thị
	if (mainMenu)
	{
		// Intro splash
		if (SDL_GetTicks() < 2500)
		{

			window.clear();
			window.render(background[3]);
			window.renderCenter(0, sin(SDL_GetTicks() / 100) * 2 - 4, "Jump - Jump", font24, white);
		}
		else
		{
			window.clear();
			window.render(background[id]);
			window.render(400, 210 + sin(SDL_GetTicks() / 100) * 2, "Click to start", font24, white);
			window.render(400, 250 + sin(SDL_GetTicks() / 100) * 2, "Highscore score: " + std::to_string(player.highscore), font24, white);
			if (mute)
			{
				window.render(400, 290 + sin(SDL_GetTicks() / 100) * 2, "Unmute", font24, white);
			}
			else
			{
				window.render(400, 290 + sin(SDL_GetTicks() / 100) * 2, "Mute", font24, white);
			}
			window.render(535, 280 + sin(SDL_GetTicks() / 100) * 2, "-  +", font32, white);
			window.render(400, 330 + sin(SDL_GetTicks() / 100) * 2, "Quit", font24, white);
			for (int i = 0; i < ground.getLength(); i++)
			{
				window.render(ground.getTile(i));
			}
		}
	}
	else
	{
		// Cập nhật game
		if (player.isDead() != CURSOR_DEATH)
		{
			ground.update(player.getScoreInt());
		}

		if (player.isDead() == ALIVE)
		{
			player.update(ground);
		}
		else if (!playedDeathSFX)
		{
			if (player.isDead() == CURSOR_DEATH)
			{
				Mix_PlayChannel(-1, hitSfx, 0);
			}
			else if (player.isDead() == HOLE_DEATH)
			{
				Mix_PlayChannel(-1, fallSfx, 0);
			}

			playedDeathSFX = true;
		}
		// Render gameplay
		window.clear();
		window.render(-90, 0, background[id]);
		window.render(player);
		for (int i = 0; i < ground.getLength(); i++)
		{
			window.render(ground.getTile(i));
		}
		window.render(25, 30, arrow);
		window.render(62, 20, player.getScore(), font32_outline, black);
		window.render(65, 23, player.getScore(), font32, white);
		window.render(0, 65, highscoreBox);
		window.render(65, 64, player.getHighscore(), font16, white);

		// Hiển thị thông báo chết
		if (player.isDead() != ALIVE)
		{
			window.renderCenter(0, -75, "High score: " + std::to_string(player.highscore), font24, white);
			window.renderCenter(0, -120, "Score: " + std::to_string(player.score), font24, white);

			if (player.isDead() == CURSOR_DEATH)
			{
				window.renderCenter(0, -24, "The cursor is deadly to the player...", font24, white);
			}
			else if (player.isDead() == HOLE_DEATH)
			{
				window.renderCenter(0, -24, "The hole had no bottom...", font24, white);
			}

			window.renderCenter(0, 12, "Click to retry.", font16, white);
		}
	}
	// Pause
	if (paused)
	{
		window.renderCenter(0, -24, "Pause", font24, white);
		window.display();
		while (true)
		{
			if (SDL_WaitEvent(&event) != 0 && (event.type == SDL_KEYDOWN))
				if (event.key.keysym.sym == SDLK_p)
				{
					paused = false;
					break;
				}
		}
	}
	window.display(); // Xuất hình lên màn hình
}

int main(int argc, char *args[])
{

	FILE *f = fopen("storage.dat", "r"); // Đọc file lưu âm lượng và highscore
	if (!f)
		std::cerr << ":(( on r";
	fscanf(f, "%d", &VOLUME_music);
	fscanf(f, "%d", &player.highscore);
	fclose(f);
	Mix_VolumeMusic(VOLUME_music);
	if (VOLUME_music == 0)
		mute = true;
	ground.add(groundTex[id][0], groundTex[id][1], groundTex[id][2], groundTex[id][3]); // Khởi tạo nền đất ban đầu

	// Bắt đầu vòng lặp game
	while (gameRunning)
	{
		gameLoop();
		SDL_Delay(20);
	}
	// Ghi lại highscore và âm lượng khi thoát
	FILE *fp = fopen("storage.dat", "w");
	fprintf(fp, "%d %d", VOLUME_music, player.highscore);
	fclose(fp);

	// Dọn dẹp
	window.cleanUp();
	TTF_CloseFont(font32);
	TTF_CloseFont(font32_outline);
	TTF_CloseFont(font24);
	TTF_CloseFont(font16);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
