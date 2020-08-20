#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "inc/Framework.h"
#include "Assert.h"

SDL_Renderer *g_renderer;
int g_width = 800;
int g_height = 600;

struct {

  int centerPosX;
  int centerPosY;

  int topLeftX;
  int topLeftY;

  int viewportW;
  int viewportH;

} g_camera;

FRAMEWORK_API void setCameraPosition(int x, int y) {
  g_camera.centerPosX = x;
  g_camera.centerPosY = y;

  g_camera.topLeftX = x - int(float(g_camera.viewportW) * 0.5f);
  g_camera.topLeftY = y - int(float(g_camera.viewportH) * 0.5f);

}

FRAMEWORK_API void convertToCameraCoordSystem(int& x, int& y) {
  x = x - g_camera.topLeftX;
  y = y - g_camera.topLeftY;
}

#include "animation.cpp"


// NOTE(mizofix): Well.. unordered_map is not the best choice
#include <unordered_map>
std::unordered_map<std::string, SDL_Texture*> loadedTextures;
std::unordered_map<std::string, Animation> loadedAnimations;

static void freeTextures() {
  for(auto textIt: loadedTextures) {
    if(textIt.second != nullptr) {
      SDL_DestroyTexture(textIt.second);
    }
  }
}

#include "json.hpp"
#include <fstream>

static SDL_Texture* loadTexture(const std::string& path) {
  auto textIt = loadedTextures.find(path);
  if(textIt != loadedTextures.end()) {
    return textIt->second;
  }

  SDL_Texture* result = IMG_LoadTexture(g_renderer, path.c_str());
  if(result != nullptr) {
    loadedTextures[path] = result;
  }

  return result;
}

// NOTE(mizofix): to simplify parsing code, we omit checking the parsed data
static bool parseAnimation(nlohmann::json& parser, Animation& animation) {

  SDL_Texture* texture = loadTexture(parser["texture"].get<std::string>());
  if(texture == nullptr) {
    return false;
  }

  int startX = parser["start_frame"]["x"], endX = parser["end_frame"]["x"];
  int startY = parser["start_frame"]["y"], endY = parser["end_frame"]["y"];

  int width = parser["size"]["w"], height = parser["size"]["h"];

  float duration = parser["duration"];
  bool repeat = parser["repeat"];

  Animation result(startX, startY, endX, endY, width, height, duration);
  result.texture = texture;
  result.repeat = repeat;

  animation = result;
  return true;
}

FRAMEWORK_API bool loadAnimations(const std::string& name) {
  std::ifstream file(name);
  if(file.is_open()) {
    nlohmann::json parser;
    file >> parser;

    nlohmann::json animations = parser["animations"];

    for(auto animIt = animations.begin(); animIt != animations.end(); animIt++) {
      Animation newAnimation;
      if(parseAnimation(animIt.value(), newAnimation)) {
        loadedAnimations[animIt.key()] = newAnimation;
      } else {
        return false;
      }
    }

    return true;
  }

  return false;
}


static bool animationIsLoaded(const std::string& name) {
  return loadedAnimations.find(name) != loadedAnimations.end();
}



/*
 * structure declarations
 */


class Sprite {
public:
	Sprite():w(0), h(0),
             anchorX(0.5f), anchorY(0.5f) { }

	int w, h;
    float anchorX, anchorY;
    Animation animation;
};

FRAMEWORK_API Sprite* createSprite(const std::string& animationName)
{

  if(animationName == "") {
    return new Sprite();
  }

  if(!animationIsLoaded(animationName)) {
    return nullptr;
  }
  Sprite* s = new Sprite();
  s->animation = loadedAnimations[animationName];

  return s;
}

FRAMEWORK_API void destroySprite(Sprite* s)
{
	SDL_assert(s);

	delete s;
}

FRAMEWORK_API void getSpriteSize(Sprite* s, int& w, int &h)
{
	SDL_assert(s);
	w = s->w;
	h = s->h;
}

FRAMEWORK_API void drawSprite(Sprite* sprite, int x, int y, int alpha,
                              float scale, float angle, bool relativeToCamera)
{
	SDL_assert(g_renderer);
	SDL_assert(sprite);

	SDL_Rect dst;
    if(relativeToCamera) {
      dst.x = x - g_camera.topLeftX;
      dst.y = y - g_camera.topLeftY;
    } else {
      dst.x = x;
      dst.y = y;
    }

    SDL_Rect src = sprite->animation.getSourceRect();


    dst.w = int(round(float(src.w) * scale));
    dst.h = int(round(float(src.h) * scale));

    dst.x -= int(sprite->anchorX * float(dst.w));
    dst.y -= int(sprite->anchorY * float(dst.h));

    if(dst.x < sprite->w || dst.y < sprite->h ||
       dst.x > g_width + sprite->w || dst.y > g_height + sprite->h) {
      return;
    }

    Uint8 previousAlpha;
    SDL_GetTextureAlphaMod(sprite->animation.texture, &previousAlpha);
    SDL_SetTextureAlphaMod(sprite->animation.texture, alpha);
	SDL_RenderCopyEx(g_renderer, sprite->animation.texture, &src, &dst, angle, NULL, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(sprite->animation.texture, previousAlpha);
}

FRAMEWORK_API void setSpriteAnchorPoint(Sprite* sprite, float x, float y) {
  Assert(x >= 0.0f && y >= 0.0f && x <= 1.0f && y <= 1.0f);

  sprite->anchorX = x;
  sprite->anchorY = y;
}

FRAMEWORK_API void setAnimation(Sprite* sprite, const std::string& name, bool repeat) {
  if(animationIsLoaded(name)) {
    sprite->animation = loadedAnimations[name];
    sprite->animation.repeat = repeat;
  }
}

FRAMEWORK_API void updateAnimation(Sprite* sprite, float deltaTime) {
  sprite->animation.update(deltaTime);
}

FRAMEWORK_API void setFrozenAnimation(Sprite* sprite, bool frozen) {
  sprite->animation.frozen = frozen;
}

FRAMEWORK_API void resetAnimation(Sprite* sprite) {
  sprite->animation.reset();
}

FRAMEWORK_API void setAnimationFrameDuration(Sprite* sprite, float duration) {
  sprite->animation.setFrameDuration(duration);
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

FRAMEWORK_API void getCursorPos(int* x, int* y) {
  SDL_GetMouseState(x, y);
}

FRAMEWORK_API void setDefaultRenderTarget() {
  SDL_SetRenderTarget(g_renderer, NULL);
}


bool GKeyState[(int)FRKey::COUNT] = {};

FRAMEWORK_API bool isKeyPressed(FRKey key) {
    return GKeyState[(int)key];
}

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
    g_camera.viewportW = g_width;
    g_camera.viewportH = g_height;

    flags = SDL_WINDOW_HIDDEN | SDL_RENDERER_TARGETTEXTURE;
	if (fullscreen) {
		SDL_ShowCursor(0);
        //flags |= SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_VIDEO_OPENGL) == -1) {
        fprintf(stderr, "SDL_Init(SDL_INIT_VIDEO) failed: %s\n", SDL_GetError());
        return(2);
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

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


    // if (SDL_CreateWindowAndRenderer(0, 0, flags | SDL_WINDOW_OPENGL, &window, &g_renderer) < 0) {
    //     fprintf(stderr, "SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
    //     return(2);
    // } else {
    //     GFramework->renderer = g_renderer;
    // }

    window = SDL_CreateWindow("Rendering to a texture!", SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, g_width, g_height, 0);
	g_renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    GFramework->renderer = g_renderer;

    SDL_GLContext glContext = SDL_GL_CreateContext(window);


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
    freeTextures();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(window);

    /* We're done! */
    SDL_Quit();
    return(0);
}


struct Texture {

  Texture(): texture(nullptr),
             anchorX(0.5f),
             anchorY(0.5f) { }

  SDL_Texture* texture;
  float        anchorX;
  float        anchorY;

};

Texture* createTexture(int width, int height) {
  Assert(width >= 0 && height >= 0);
  Texture* text = new Texture();
  if(text != nullptr) {
    // TODO(mizofix): make this function call robust
    text->texture = SDL_CreateTexture(g_renderer,
                                      SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET,
                                      width,
                                      height);

    if(text->texture == nullptr) {
      delete text;
      text = nullptr;
    } else {

      SDL_SetTextureBlendMode(text->texture, SDL_BLENDMODE_BLEND);

    }
  }

  return text;
}


void drawTexture(Texture* texture, int x, int y, bool relativeToCamera) {

  int tw, th;
  getTextureSize(texture, tw, th);

  SDL_Rect dst;
  dst.w = tw;
  dst.h = th;
  dst.x = x;
  dst.y = y;

  if(relativeToCamera) {
    dst.x = x - g_camera.topLeftX;
    dst.y = y - g_camera.topLeftY;
  }

  dst.x -= int(float(tw) * texture->anchorX);
  dst.y -= int(float(th) * texture->anchorY);

  SDL_RenderCopy(g_renderer, texture->texture, NULL, &dst);

}

void getTextureSize(Texture* texture, int& w, int& h) {

  Uint32 format;
  int tw, th, access;
  SDL_QueryTexture(texture->texture, &format, &access, &tw, &th);

  w = th;
  h = th;

}

void setTextureAnchorPoint(Texture* texture, float anchorX, float anchorY) {
  texture->anchorX = anchorX;
  texture->anchorY = anchorY;
}

void setTextureAsTarget(Texture* texture) {
  SDL_SetRenderTarget(g_renderer, texture->texture);
  SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 0);
  SDL_RenderClear(g_renderer);
}

void bindTexture(Texture* texture) {
  SDL_GL_BindTexture(texture->texture, NULL, NULL);
}

void unbindTexture(Texture* texture) {
  SDL_GL_UnbindTexture(texture->texture);
}

void destroyTexture(Texture* texture) {
  if(texture) {
    if(texture->texture) {
      SDL_DestroyTexture(texture->texture);
      texture->texture = nullptr;
    }

    delete texture;
  }
}
