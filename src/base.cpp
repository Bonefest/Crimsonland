#include "base.h"
#include "glad/glad.h"
#include "Utils.h"

#include <GL/gl.h>



CrimsonlandFramework::CrimsonlandFramework(int argc, char** commands): m_lastTime(0.0f) {
  m_worldData = parseCommands(argc, commands);

  info("-------------------------\n");
  info("final world data values are:\n");
  info("Window size (%d, %d)\n", m_worldData.windowWidth, m_worldData.windowHeight);
  info("Map size(%.1f, %.1f)\n", m_worldData.mapWidth, m_worldData.mapHeight);
  info("Maximal enemies number %d\n", m_worldData.numEnemies);
  info("Maximal ammo number %d\n", m_worldData.numAmmo);
  info("-------------------------\n");

}

void CrimsonlandFramework::PreInit(int& width, int& height, bool& fullscreen) {
  width = m_worldData.windowWidth;
  height = m_worldData.windowHeight;
  fullscreen = false;
}

bool CrimsonlandFramework::Init() {

  if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    return false;

  }

  if(!m_bumpProgram.generateProgram("shaders/posteffect.vert",
                                    "shaders/bumpeffect.frag")) {
    info("%s\n", m_bumpProgram.getErrorMessage().c_str());
    return false;
  }

  // if(!m_noiseProgram.generateProgram("shaders/posteffect.vert",
  //                                    "shaders/noiseeffect.frag")) {
  //   info("%s\n", m_noiseProgram.getErrorMessage().c_str());
  //   return false;
  // }

  m_testSprite = createSprite("data/circle.tga");
  if(!m_screenTexture.init(renderer, SDL_TEXTUREACCESS_TARGET,
                           m_worldData.windowWidth, m_worldData.windowHeight)) {
    return false;
  }

  if(!m_noiseTexture.init(renderer, SDL_TEXTUREACCESS_STREAMING,
                          m_worldData.windowWidth, m_worldData.windowHeight)) {
    return false;
  }

  m_spritePosX = 320;
  m_spritePosY = 240;

  return true;
}

bool CrimsonlandFramework::Tick() {

  if(isKeyPressed(FRKey::LEFT)) {
    m_spritePosX -= 100.0f * m_deltaTime;
    info("%f\n", m_spritePosX);
  }

  if(isKeyPressed(FRKey::RIGHT)) {
    m_spritePosX += 100.0f * m_deltaTime;
    info("%f\n", m_spritePosX);
  }

  setCameraPosition(m_spritePosX, m_spritePosY);

  m_screenTexture.setAsRenderingTarget(renderer);

  drawTestBackground();
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for(int x = 0; x < m_worldData.windowWidth; x += 20) {
    //    SDL_RenderDrawLine(renderer, x, 0, x, m_worldData.windowHeight);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  drawSprite(m_testSprite, round(m_spritePosX), round(m_spritePosY));
  drawSprite(m_testSprite, 320, 150);
  drawSprite(m_testSprite, 320, 300);
  drawSprite(m_testSprite, 440, 240);

  drawSprite(m_testSprite, 320 + std::sin(m_lastTime) * 240, 240);

  deactivateTextureRendering();

  SDL_RenderClear(renderer);

  m_screenTexture.bind();
  glUseProgram(m_bumpProgram.getProgramID());

  glUniform1f(glGetUniformLocation(m_bumpProgram.getProgramID(), "time"), m_lastTime);
  glUniform2f(glGetUniformLocation(m_bumpProgram.getProgramID(), "screenSize"),
              m_worldData.windowWidth, m_worldData.windowHeight);

  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0.0f, 0.0f);

  glTexCoord2f( 1.0f, 0.0f);
  glVertex2f(m_worldData.windowWidth, 0.0f);

  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0.0f, m_worldData.windowHeight);

  glTexCoord2f( 1.0f, 1.0f);
  glVertex2f(m_worldData.windowWidth, m_worldData.windowHeight);

  glEnd();
  //  drawTextureToScreen();

  float time = float(getTickCount()) / 1000.0f;
  m_deltaTime = time - m_lastTime;
  m_lastTime = time;

  return false;
}

void CrimsonlandFramework::onKeyPressed(FRKey k) {

}

void CrimsonlandFramework::deactivateTextureRendering() {
  SDL_SetRenderTarget(renderer, NULL);
}

void CrimsonlandFramework::Close() {
  destroySprite(m_testSprite);
}
