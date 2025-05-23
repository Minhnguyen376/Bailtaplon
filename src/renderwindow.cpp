#include <SDL.h>
#include <SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "renderwindow.h"
#include "entity.h"

//screen size
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 480;

RenderWindow::RenderWindow()
{
}
//tạo cửa sổ và render
void RenderWindow::create(const char *p_title, int p_w, int p_h)
{
	//dùng SDL
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);

	if (window == NULL)
		std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
	// Tạo renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}
// lấy ảnh
SDL_Texture *RenderWindow::loadTexture(const char *p_filePath)
{
	SDL_Texture *texture = NULL;
	texture = IMG_LoadTexture(renderer, p_filePath);

	if (texture == NULL)
		std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;

	return texture;
}

// reset nền
void RenderWindow::clear()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //đen
	SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity &p_entity) //vẽ entity 
{

	for (int i = p_entity.getSize() - 1; i >= 0; i--)
	{
		SDL_Rect src;
		src.x = p_entity.getCurrentFrame().x;
		src.y = p_entity.getCurrentFrame().y;
		src.w = p_entity.getCurrentFrame().w;
		src.h = p_entity.getCurrentFrame().h;

		SDL_Rect dst;

		dst.x = p_entity.getX() + p_entity.getAnimOffsetX(i);
		dst.y = p_entity.getY() + p_entity.getAnimOffsetY(i);
		dst.w = p_entity.getCurrentFrame().w;
		dst.h = p_entity.getCurrentFrame().h;
		if (i == 1)
		{
			dst.x -= 20;
			dst.y -= 32;
		}
		if (i == 2)
		{
			dst.x += 18;
			dst.y -= 32;
		}

		// Đẩy texture lên renderer
		SDL_RenderCopy(renderer, p_entity.getTex(i), &src, &dst); 
	}
}
// Vẽ texture đơn (không dùng Entity) tại toạ độ p_x, p_y
void RenderWindow::render(float p_x, float p_y, SDL_Texture *p_tex)
{
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	SDL_QueryTexture(p_tex, NULL, NULL, &src.w, &src.h);

	SDL_Rect dst;
	dst.x = p_x;
	dst.y = p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, p_tex, &src, &dst);
}
// Vẽ toàn màn hình texture
void RenderWindow::render(SDL_Texture *p_tex)
{
	render(0, 0, p_tex);
}
// hiện chữ từ string tại vị trí (p_x, p_y)
void RenderWindow::render(float p_x, float p_y, const std::string p_text, TTF_Font *font, SDL_Color textColor)
{
	char *cstr = new char[p_text.length() + 1];
	strcpy(cstr, p_text.c_str());
	SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, cstr, textColor); //surface từ text
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); // texture từ surface

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = p_x;
	dst.y = p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst); //Hiện chữ lên màn
	SDL_FreeSurface(surfaceMessage); // Dọn dẹp bộ nhớ
	SDL_DestroyTexture(message); 
}
// Hiện chữ từ const char* tại vị trí (p_x, p_y)
void RenderWindow::render(float p_x, float p_y, const char *p_text, TTF_Font *font, SDL_Color textColor)
{
	SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, p_text, textColor);
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = p_x;
	dst.y = p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}
// Hiện chữ ở chính giữa màn hình, thêm offset
void RenderWindow::renderCenter(float p_x, float p_y, const std::string p_text, TTF_Font *font, SDL_Color textColor)
{
	char *cstr = new char[p_text.length() + 1];
	strcpy(cstr, p_text.c_str());
	SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, cstr, textColor);
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = SCREEN_WIDTH / 2 - src.w / 2 + p_x;
	dst.y = SCREEN_HEIGHT / 2 - src.h / 2 + p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}
// Hiện chữ ở chính giữa từ const char*
void RenderWindow::renderCenter(float p_x, float p_y, const char *p_text, TTF_Font *font, SDL_Color textColor)
{
	SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, p_text, textColor);
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	src.w = surfaceMessage->w;
	src.h = surfaceMessage->h;

	SDL_Rect dst;
	dst.x = SCREEN_WIDTH / 2 - src.w / 2 + p_x;
	dst.y = SCREEN_HEIGHT / 2 - src.h / 2 + p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, message, &src, &dst);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message);
}
// Display đã render ra màn hình
void RenderWindow::display()
{
	SDL_RenderPresent(renderer);
}
// clear res
void RenderWindow::cleanUp()
{
	SDL_DestroyWindow(window);
}
// Đổi bg color cho renderer
void RenderWindow::Color(int r, int g, int b, int opacity)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, opacity);
}
