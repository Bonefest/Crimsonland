#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "inc/Framework.h"


SDL_Renderer *g_renderer;
int g_width = 800;
int g_height = 600;

/*
 * structure declarations
 */

class Sprite {
public:
	Sprite():w(0), h(0), tex(nullptr) {}

	int w, h;
	SDL_Texture* tex;
};

FRAMEWORK_API Sprite* createSprite(const char* path)
{
	SDL_Texture* texture = IMG_LoadTexture(g_renderer, path);
	if (!texture) {
		fprintf(stderr, "Couldn't load %s: %s\n", path, SDL_GetError());
		return nullptr;
	}


	Sprite* s = new Sprite();
	if (!s)
	{
		fprintf(stderr, "Not enough memory\n");
		SDL_Quit();
		exit(1);
	}

	SDL_QueryTexture(texture, NULL, NULL, &s->w, &s->h);

	s->tex = texture;

	return s;
}

FRAMEWORK_API void destroySprite(Sprite* s)
{
	SDL_assert(s);

	if (s->tex)
	{
		SDL_DestroyTexture(s->tex);
	}

	delete s;
}

FRAMEWORK_API void getSpriteSize(Sprite* s, int& w, int &h)
{
	SDL_assert(s);
	w = s->w;
	h = s->h;
}

FRAMEWORK_API void drawSprite(Sprite* sprite, int x, int y)
{
	SDL_assert(g_renderer);
	SDL_assert(sprite);

	SDL_Rect r;
	r.w = sprite->w;
	r.h = sprite->h;
	r.x = x;
	r.y = y;
	SDL_RenderCopy(g_renderer, sprite->tex, NULL, &r);
}

FRAMEWORK_API void getScreenSize(int& w, int &h)
{
	SDL_Rect viewport;
	SDL_RenderGetViewport(g_renderer, &viewport);
	w = viewport.w;
	h = viewport.h;
}

FRAMEWORK_API unsigned int getTickCount()
{
	return SDL_GetTicks();
}



/* Draw a Gimpish background pattern to show transparency in the image */
static void draw_background(SDL_Renderer *renderer, int w, int h)
{
    SDL_Color col[2] = {
        { 0x66, 0x66, 0x66, 0xff },
        { 0x99, 0x99, 0x99, 0xff },
    };
    int i, x, y;
    SDL_Rect rect;

    rect.w = 8;
    rect.h = 8;
    for (y = 0; y < h; y += rect.h) {
        for (x = 0; x < w; x += rect.w) {
            /* use an 8x8 checkerboard pattern */
            i = (((x ^ y) >> 3) & 1);
            SDL_SetRenderDrawColor(renderer, col[i].r, col[i].g, col[i].b, col[i].a);

            rect.x = x;
            rect.y = y;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}


FRAMEWORK_API void drawTestBackground()
{
	SDL_Rect viewport;
	SDL_RenderGetViewport(g_renderer, &viewport);
	return draw_background(g_renderer, viewport.w, viewport.h);
}

FRAMEWORK_API void showCursor(bool bShow)
{
	SDL_ShowCursor(bShow?1:0);
}

bool GKeyState[(int)FRKey::COUNT] = {};

FRAMEWORK_API int run(Framework* framework)
{
    SDL_Window *window;
    Uint32 flags;
    int done;
    SDL_Event event;

	for (int i = 0; i < (int)FRKey::COUNT; ++i)
	{
		GKeyState[i] = false;
	}

	Framework* GFramework = framework;

	bool fullscreen;
	GFramework->PreInit(g_width, g_height, fullscreen);
	
    flags = SDL_WINDOW_HIDDEN;
	if (fullscreen) {
		SDL_ShowCursor(0);
        //flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
        return(2);
    }

	bool bModeFound = false;
	int num_displays = SDL_GetNumVideoDisplays();
	for(int displayIndex=0; displayIndex < num_displays; ++displayIndex)
	{
		int num_modes = SDL_GetNumDisplayModes(displayIndex);
		for(int modeIndex = 0; modeIndex < num_modes; ++modeIndex)
		{
			SDL_DisplayMode mode;
			int index = SDL_GetDisplayMode(displayIndex, modeIndex, &mode);
			if(mode.w == g_width && mode.h == g_height)
			{
				bModeFound = true;
				break;
			}
		}
	}

	if(!bModeFound)
	{
        fprintf(stderr, "Desired window size: %d x %d is not suported\n", g_width, g_height);
		return 1;
	}

    if (SDL_CreateWindowAndRenderer(0, 0, flags, &window, &g_renderer) < 0) {
        fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
        return(2);
    }

	{
        /* Show the window */
        SDL_SetWindowTitle(window, "crimsonland");
        SDL_SetWindowSize(window, g_width, g_height);
		SDL_DisplayMode displayMode = { SDL_PIXELFORMAT_UNKNOWN, g_width, g_height, 0, 0 };
		if(SDL_SetWindowDisplayMode(window, &displayMode) < 0)
		{
 	    	fprintf(stderr, "SDL_SetWindowDisplayMode() failed: %s\n", SDL_GetError());
			return 1;
		}
		if(fullscreen)
		{
			if(SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP)<0)
			{
				fprintf(stderr, "SDL_SetWindowFullscreen() failed: %s\n", SDL_GetError());
				return 1;
			}
		}
        SDL_ShowWindow(window);


		if (!GFramework->Init())
		{
			fprintf(stderr, "Framework::Init failed\n");
			SDL_Quit();
			exit(1);
		}

        done = 0;
        while ( ! done ) {
            while ( SDL_PollEvent(&event) ) {
                switch (event.type) {
                    case SDL_KEYUP:
                        switch (event.key.keysym.sym) {
                            case SDLK_RIGHT:
                            case SDLK_LEFT:
                            case SDLK_DOWN:
                            case SDLK_UP:
							{
								int key_index = (event.key.keysym.sym - SDLK_RIGHT);
								if (GKeyState[key_index])
								{
									GFramework->onKeyReleased((FRKey)key_index);
									GKeyState[key_index] = false;
								}
								break;
							}
                            case SDLK_ESCAPE:
								done = 1;
                            break;
	                        default:
		                        break;
                        }
                        break;
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym) {
						case SDLK_RIGHT:
						case SDLK_LEFT:
						case SDLK_DOWN:
						case SDLK_UP:
						{
							int key_index = (event.key.keysym.sym - SDLK_RIGHT);
							if (!GKeyState[key_index])
							{
								GFramework->onKeyPressed((FRKey)key_index);
								GKeyState[key_index] = true;
							}
						}
							break;

						default:
							break;
						}
						break;
                    case SDL_MOUSEBUTTONDOWN:
						if (event.button.button <= SDL_BUTTON_RIGHT) {
							GFramework->onMouseButtonClick((FRMouseButton)(event.button.button - SDL_BUTTON_LEFT), false);
						}
						break;
                    case SDL_MOUSEBUTTONUP:
						if (event.button.button <= SDL_BUTTON_RIGHT) {
							GFramework->onMouseButtonClick((FRMouseButton)(event.button.button - SDL_BUTTON_LEFT), true);
						}
                        break;
					case SDL_MOUSEMOTION:
						GFramework->onMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                        break;
                    case SDL_QUIT:
                        done = 1;
                        break;
                    default:
                        break;
                }
            }

			SDL_RenderClear(g_renderer);

			SDL_Rect viewport;
			SDL_RenderGetViewport(g_renderer, &viewport);

			/* Draw a gray background */
			SDL_SetRenderDrawColor(g_renderer, 0xA0, 0xA0, 0xA0, 0xFF);
			SDL_RenderClear(g_renderer);

			done |= GFramework->Tick() ? 1 : 0;

            SDL_RenderPresent(g_renderer);

            SDL_Delay(1);
        }
    }

	GFramework->Close();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);

    /* We're done! */
    SDL_Quit();
    return(0);
}
