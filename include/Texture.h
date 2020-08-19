#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL2/SDL.h>
#include "Common.h"

class Texture {
public:

  Texture(): m_stexture(nullptr) { }
  ~Texture() {
    if(m_stexture) {
      SDL_DestroyTexture(m_stexture);
      m_stexture = nullptr;
    }
  }

  bool init(SDL_Renderer* renderer, int access, int width, int height) {

    Assert(width > 0 && height > 0);

    SDL_Texture* newTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_RGBA8888,
                                                access, width, height);
    if(newTexture) {

      if(m_stexture != nullptr) {
        SDL_DestroyTexture(m_stexture);
      }

      SDL_SetTextureBlendMode(m_stexture, SDL_BLENDMODE_BLEND);

      m_access = access;
      m_width = width;
      m_height = height;
      m_stexture = newTexture;
      return true;
    } else {
      info("Cannot create a new texture!\n");
    }


    return false;
  }

  void bind() {
    SDL_GL_BindTexture(m_stexture, NULL, NULL);
  }

  void unbind() {
    SDL_GL_UnbindTexture(m_stexture);
  }

  void setAsRenderingTarget(SDL_Renderer* renderer) {
    SDL_SetRenderTarget(renderer, m_stexture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderClear(renderer);
  }

  void setOrigin(real relativeX, real relativeY) {
    Assert(relativeX >= 0.0f && relativeX <= 1.0f);
    Assert(relativeY >= 0.0f && relativeY <= 1.0f);

    m_relativeOriginX = relativeX;
    m_relativeOriginY = relativeY;

    recalculateOrigin();
  }

  void setAngle(real angle) {
    m_angle = angle;
  }

  void draw(SDL_Renderer* renderer) {

    if(m_stexture) {
      SDL_Rect dst = {0, 0, m_width, m_height};

      SDL_RenderCopyEx(renderer, m_stexture, NULL, &dst, m_angle , &m_origin, SDL_FLIP_NONE);
    }
  }

private:
  void recalculateOrigin() {
    m_origin.x = int(m_width * m_relativeOriginX);
    m_origin.y = int(m_height * m_relativeOriginY);
  }

  SDL_Texture* m_stexture;

  int m_access;

  int m_width;
  int m_height;

  real m_relativeOriginX;
  real m_relativeOriginY;

  real m_angle;

  SDL_Point m_origin;

};

#endif
