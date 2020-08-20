#include "base.h"
#include "glad/glad.h"
#include "Utils.h"

#include <GL/gl.h>

#include "ecs/Registry.h"

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


  initECS();

  m_screenTexture = createTexture(m_worldData.windowWidth, m_worldData.windowHeight);

  if(m_screenTexture == nullptr) {
    return false;
  }

  return true;
}

void CrimsonlandFramework::initECS() {
  m_context.registry = &m_registry;
  m_context.data = m_worldData;

  m_systemManager.addSystem(m_context, new PhysicsIntegrationSystem());
  m_systemManager.addSystem(m_context, new PlayerSystem());
  m_systemManager.addSystem(m_context, new ZombieRenderingSystem());
}


bool CrimsonlandFramework::Tick() {

  vec2 acceleration;
  if(isKeyPressed(FRKey::LEFT)) {
    acceleration += vec2(-5.0f, 0.0f);
  }

  if(isKeyPressed(FRKey::RIGHT)) {
    acceleration += vec2( 5.0f, 0.0f);
  }

  // setCameraPosition(int(playerPosition.x), int(playerPosition.y));

  update();
  draw();
  drawToScreen();
  updateTimer();

  return false;
}

void CrimsonlandFramework::update() {
  m_systemManager.updateSystems(m_context, m_deltaTime);
}

void CrimsonlandFramework::draw() {
  setTextureAsTarget(m_screenTexture);

  drawTestBackground();
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for(int x = 0; x < m_worldData.windowWidth; x += 20) {
    SDL_RenderDrawLine(renderer, x, 0, x, m_worldData.windowHeight);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  m_systemManager.drawSystems(m_context);
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

}

void CrimsonlandFramework::onKeyPressed(FRKey k) {

}

void CrimsonlandFramework::Close() {
  destroyTexture(m_screenTexture);
}

void CrimsonlandFramework::collisionSystem() {
  penetrationResolution();
}

void CrimsonlandFramework::penetrationResolution() {

  // // TODO(mizofix): more than one iteration
  // // NOTE(mizofix): warning! extremely slow function
  // for(auto entityAIt = m_entities.begin(); entityAIt != m_entities.end(); entityAIt++) {
  //   for(auto entityBIt = entityAIt + 1; entityBIt != m_entities.end(); entityBIt++) {
  //     real totalSize = (*entityAIt)->getSize() + (*entityBIt)->getSize();
  //     real distance = (*entityAIt)->getPosition().distance((*entityBIt)->getPosition());

  //     if(distance < totalSize) {
  //       vec2 direction = ((*entityBIt)->getPosition() - (*entityAIt)->getPosition());
  //       direction.normalize();
  //       real delta = totalSize - distance;

  //       (*entityAIt)->setPosition((*entityAIt)->getPosition() - direction * delta * 0.5f);
  //       (*entityBIt)->setPosition((*entityBIt)->getPosition() + direction * delta * 0.5f);
  //     }

  //   }
  // }
}
