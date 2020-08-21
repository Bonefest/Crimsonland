#include "base.h"
#include "glad/glad.h"
#include "Utils.h"

#include <GL/gl.h>

#include "ecs/Registry.h"

CrimsonlandFramework::CrimsonlandFramework(int argc, char** commands): m_lastTime(0.0f) {
  m_worldData = parseCommands(argc, commands);
  m_worldData.maxEffectsNumber = 10;
  m_worldData.maxPlayerSpeed = 25.0f;

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

  if(!loadAnimations("data/animations.json") || !loadAnimations("data/trees/trees.json")) {
    return false;
  }


  m_background = createSprite("sand");
  initPlants();
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
  m_systemManager.addSystem(m_context, new TrailSystem());
  m_systemManager.addSystem(m_context, new EffectsSystem());
  m_systemManager.addSystem(m_context, new PlayerSystem());
  m_systemManager.addSystem(m_context, new ZombieRenderingSystem());

  m_uiSystem = new UIRenderingSystem();
  m_uiSystem->init(m_context);
}

void CrimsonlandFramework::initPlants() {

  int maximalCount = 40;
  for(int i = 0; i < 40; ++i) {
    int rnd = rand() % 100;

    char name[32];
    int plantNumber = rand() % 46 + 1;
    sprintf(name, "tree_%d", plantNumber);

    Sprite* plantSprite = createSprite(name);

    vec2 position(float(rand() % int(m_worldData.mapWidth * 1.25f)) - m_worldData.mapWidth * 0.75f,
                  float(rand() % int(m_worldData.mapHeight * 1.25f)) - m_worldData.mapHeight * 0.75f
                  );

    if(rnd < 50) {
      m_bushes.emplace_back(plantSprite, position);
    } else {
      m_trees.emplace_back(plantSprite, position);
    }
  }

}

bool CrimsonlandFramework::Tick() {

  update();
  draw();
  drawToScreen();
  updateTimer();

  return false;
}


void CrimsonlandFramework::updateTimer() {
  float time = float(getTickCount()) / 1000.0f;
  m_deltaTime = time - m_lastTime;
  m_lastTime = time;
}


void CrimsonlandFramework::update() {
  m_systemManager.updateSystems(m_context, m_deltaTime);
  m_uiSystem->update(m_context, m_deltaTime);
}

void CrimsonlandFramework::draw() {
  setTextureAsTarget(m_screenTexture);

  drawTestBackground();
  drawBackground();
  drawPlants(m_bushes);
  m_systemManager.drawSystems(m_context);
  drawPlants(m_trees);
}

void CrimsonlandFramework::drawBackground() {

  int tilesX = ceil((m_worldData.mapWidth + m_worldData.windowWidth) / 1000.0f);
  int tilesY = ceil((m_worldData.mapHeight + m_worldData.windowHeight) / 1000.0f);

  for(int y = 0; y < tilesY; y++) {
    for(int x = 0; x < tilesX; x++) {
      int posX = int((real(x) * 1000.0f) - m_worldData.mapWidth * 0.5f);
      int posY = int((real(y) * 1000.0f) - m_worldData.mapHeight * 0.5f);

      drawSprite(m_background, posX, posY);

    }
  }

}

void CrimsonlandFramework::drawPlants(std::vector<std::pair<Sprite*, vec2>>& plants) {
  for(auto plant: plants) {
    drawSprite(plant.first, round(plant.second.x), round(plant.second.y));
  }
}

void CrimsonlandFramework::drawToScreen() {

  setDefaultRenderTarget();

  SDL_RenderClear(renderer);

  drawTexture(m_screenTexture, m_worldData.windowWidth, m_worldData.windowHeight, false);

  bindTexture(m_screenTexture);

  GLint oldProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);

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

  glUseProgram(oldProgram);

  m_uiSystem->draw(m_context);
}

void CrimsonlandFramework::onMouseMove(int x, int y, int xrelative, int yrelative) {

}

void CrimsonlandFramework::onKeyPressed(FRKey k) {

}

void CrimsonlandFramework::onMouseWheel(int y) {
  Message msg;
  msg.type = int(MessageType::ON_MOUSE_WHEEL);
  msg.wheel.y = y;
  notify(msg);

}

void CrimsonlandFramework::Close() {
  destroySprite(m_background);
  destroyTexture(m_screenTexture);
  delete m_uiSystem;
}
