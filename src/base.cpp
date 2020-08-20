#include "base.h"
#include "glad/glad.h"
#include "Utils.h"

#include <GL/gl.h>

#include "ecs/Registry.h"

CrimsonlandFramework::CrimsonlandFramework(int argc, char** commands): m_lastTime(0.0f) {
  m_worldData = parseCommands(argc, commands);
  m_worldData.maxEffectsNumber = 10;
  m_worldData.maxPlayerSpeed = 100.0f;

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

  m_background = createSprite("grass");
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
  m_systemManager.addSystem(m_context, new EffectsSystem());
  m_systemManager.addSystem(m_context, new PlayerSystem());
  m_systemManager.addSystem(m_context, new ZombieRenderingSystem());

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
}

void CrimsonlandFramework::draw() {
  setTextureAsTarget(m_screenTexture);

  drawTestBackground();
  drawSprite(m_background, int(m_worldData.windowWidth * 0.5f),
             int(m_worldData.windowHeight * 0.5f),
             255,
             1.0f,
             0.0f,
             false);

  drawPlants(m_bushes);

  m_systemManager.drawSystems(m_context);
  drawPlants(m_trees);
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
  destroySprite(m_background);
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
