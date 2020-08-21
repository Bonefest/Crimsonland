#pragma once


#if defined(_WINDOWS)

    #if defined(FRAMEWORK_PROJECT)
        #define FRAMEWORK_API __declspec(dllexport)
    #else
        #define FRAMEWORK_API __declspec(dllimport)
    #endif
#else
    #define FRAMEWORK_API
#endif


#include <string>
#include <SDL2/SDL.h>

// Bonus for any found bugs in the framework!

class Sprite;

FRAMEWORK_API void setCameraPosition(int x, int y);
FRAMEWORK_API void convertToCameraCoordSystem(int& x, int& y);

FRAMEWORK_API Sprite* createSprite(const std::string& path = "");
FRAMEWORK_API void drawSprite(Sprite*, int x, int y,
                              int alpha = 255,
                              float scale = 1.0f,
                              float angle = 0.0f, bool relativeToCamera = true);

// NOTE(mizofix): Anchor point coords should be in range [0, 1]
// (0, 0) corresponds to top-left corner
FRAMEWORK_API void setSpriteAnchorPoint(Sprite* sprite, float x, float y);

FRAMEWORK_API void getSpriteSize(Sprite* s, int& w, int &h);
FRAMEWORK_API void destroySprite(Sprite* s);

FRAMEWORK_API void setAnimation(Sprite* s, const std::string& animationName, bool repeat = true);
FRAMEWORK_API void updateAnimation(Sprite* s, float deltaTime);
FRAMEWORK_API void setFrozenAnimation(Sprite* s, bool frozen);
FRAMEWORK_API void resetAnimation(Sprite* s);
FRAMEWORK_API void setAnimationFrameDuration(Sprite* s, float duration);
FRAMEWORK_API bool isAnimationFinished(Sprite* s);
FRAMEWORK_API bool loadAnimations(const std::string& path);

FRAMEWORK_API void drawRect(int x, int y, int w, int h,
                            int r, int g, int b, int a,
                            bool relativeToCamera = true);

class Texture;

FRAMEWORK_API Texture* createTexture(int width, int height);
FRAMEWORK_API void drawTexture(Texture* texture, int x, int y, bool relativeToCamera = true);
FRAMEWORK_API void getTextureSize(Texture* texture, int& w, int& h);
FRAMEWORK_API void setTextureAnchorPoint(float x, float y);
FRAMEWORK_API void setTextureAsTarget(Texture* texture);
FRAMEWORK_API void bindTexture(Texture* texture);
FRAMEWORK_API void unbindTexture(Texture* texture);
FRAMEWORK_API void destroyTexture(Texture* texture);

FRAMEWORK_API void drawTestBackground();

FRAMEWORK_API void getScreenSize(int& w, int &h);

// Get the number of milliseconds since library initialization.
FRAMEWORK_API unsigned int getTickCount();

FRAMEWORK_API void showCursor(bool bShow);
FRAMEWORK_API void getCursorPos(int* x, int* y);

FRAMEWORK_API void setDefaultRenderTarget();

enum class FRKey {
	RIGHT,
	LEFT,
	DOWN,
	UP,
	COUNT
};

FRAMEWORK_API bool isKeyPressed(FRKey key);

enum class FRMouseButton {
	LEFT,
	MIDDLE,
	RIGHT,
	COUNT
};

FRAMEWORK_API bool isButtonPressed(FRMouseButton button);


class FRAMEWORK_API Framework {
public:

	// no function calls are available here, this function shuld only return width, height and fullscreen values
	virtual void PreInit(int& width, int& height, bool& fullscreen) = 0;

	// return : true - ok, false - failed, application will exit
	virtual bool Init() = 0;

	virtual void Close() = 0;

	// return value: if true will exit the application
	virtual bool Tick() = 0;

	// param: xrel, yrel: The relative motion in the X/Y direction
	// param: x, y : coordinate, relative to window
	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) = 0;

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) = 0;

    virtual void onMouseWheel(int y) = 0;

	virtual void onKeyPressed(FRKey k) = 0;

	virtual void onKeyReleased(FRKey k) = 0;

	virtual ~Framework() {};

    SDL_Renderer* renderer;
};


FRAMEWORK_API int run(Framework*);
