#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <string>
#include <chrono>

int main(int argumentCount, char * arguments[])
{
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();
	}
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
	SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "1");
	unsigned window_width = 200;
	unsigned window_height = 50;
	SDL_Window* window = SDL_CreateWindow("Latency test!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI | SDL_WindowFlags::SDL_WINDOW_FULLSCREEN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	bool quit = false;
	SDL_Event e;
	TTF_Font* font = TTF_OpenFont("LiberationSans.ttf", 24);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = window_width;
	rect.h = window_height;

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		SDL_JoystickOpen(i);
	}

	auto clok = std::chrono::steady_clock();
	auto startStamp = clok.now();

	SDL_Rect messageRect;
	messageRect.x = 0;
	messageRect.y = 0;

	SDL_Color Green = { 0, 255, 0 };
	SDL_Color Red = { 255, 0, 0 };

	bool click = false;

	char message[20];
	auto lastStamp = startStamp;

	while (!quit) {
		//Handle input/events
		while (SDL_PollEvent(&e) != 0){
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_JOYBUTTONDOWN:
				click = true;
				break;
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONUP:
			case SDL_JOYBUTTONUP:
				click = false;
				break;
			}
		}

		auto now = clok.now();
		long ms = std::chrono::duration<double, std::milli>(now - startStamp).count();
		SDL_ltoa(ms, message, 10);

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, message, (click?Green:Red));
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		
		TTF_SizeText(font, message, &messageRect.w, &messageRect.h);

		SDL_SetRenderDrawColor(renderer, (!click ? Green : Red).r, (!click ? Green : Red).g, (!click ? Green : Red).b, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderCopy(renderer, Message, NULL, &messageRect);
		SDL_RenderPresent(renderer);

		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);
		//long frametime = std::chrono::duration<double, std::micro>(now - lastStamp).count();
		//SDL_Log("%ld us", frametime);
		//lastStamp = now;
	}

	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}