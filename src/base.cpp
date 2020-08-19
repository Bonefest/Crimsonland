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

  if(!loadAnimations("data/zombie_animations.json")) {
    return false;
  }

  m_testSprite = createSprite("player_walk");
  m_screenTexture = createTexture(m_worldData.windowWidth, m_worldData.windowHeight);

  if(m_testSprite == nullptr || m_screenTexture == nullptr) {
    return false;
  }


  m_spritePosX = 320;
  m_spritePosY = 240;

  return true;
}

bool CrimsonlandFramework::Tick() {
 if(isKeyPressed(FRKey::LEFT)) {
    m_spritePosX -= 100.0f * m_deltaTime;
  }

  if(isKeyPressed(FRKey::RIGHT)) {
    m_spritePosX += 100.0f * m_deltaTime;
  }

  setCameraPosition(m_spritePosX, m_spritePosY);

  setTextureAsTarget(m_screenTexture);
  updateAnimation(m_testSprite, m_deltaTime);
  draw();
  drawToScreen();
  updateTimer();

  return false;
}

void CrimsonlandFramework::draw() {
  drawTestBackground();
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for(int x = 0; x < m_worldData.windowWidth; x += 20) {
    SDL_RenderDrawLine(renderer, x, 0, x, m_worldData.windowHeight);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  drawSprite(m_testSprite, round(m_spritePosX), round(m_spritePosY), m_angle);
  drawSprite(m_testSprite, 320, 150, false);
  drawSprite(m_testSprite, 320, 300);
  drawSprite(m_testSprite, 440, 240);
  drawSprite(m_testSprite, 320 + std::sin(m_lastTime) * 240, 240);
}

void CrimsonlandFramework::updateTimer() {
  float time = float(getTickCount()) / 1000.0f;
  m_deltaTime = time - m_lastTime;
  m_lastTime = time;
}

void CrimsonlandFramework::drawToScreen() {

  setDefaultRenderTarget();

  SDL_RenderClear(renderer);

  drawTexture(m_screenTexture, m_worldData.windowWidth, m_worldData.windowHeight, false);

  bindTexture(m_screenTexture);

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

}

void CrimsonlandFramework::onMouseMove(int x, int y, int xrelative, int yrelative) {
  info("%d %d\n", x, y);



  float angle = atan2(float(y) - 0.5f * m_worldData.windowHeight, float(x) - 0.5f * m_worldData.windowWidth) / (6.28) * 360.0f;
  m_angle = angle;
}

void CrimsonlandFramework::onKeyPressed(FRKey k) {

}

void CrimsonlandFramework::Close() {
  destroySprite(m_testSprite);
  destroyTexture(m_screenTexture);
}
