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
  info("Maximal enemies number %u\n", m_worldData.numEnemies);
  info("Initial plants number %u\n", m_worldData.numPlants);
  info("Max health %d\n", int(m_worldData.maxPlayerHealth));
  info("Max stamina %d\n", int(m_worldData.maxPlayerStamina));
  info("Health regen speed %d\n", int(m_worldData.regenSpeed));
  info("Stamina regen speed %d\n", int(m_worldData.staminaRegenSpeed));
  info("Maximal effects number %d\n", int(m_worldData.maxEffectsNumber));
  info("Initial round %d\n", int(m_worldData.roundData.currentRoundNumber));
  info("-------------------------\n");

}

void CrimsonlandFramework::PreInit(int& width, int& height, bool& fullscreen) {
  width = m_worldData.windowWidth;
  height = m_worldData.windowHeight;
  fullscreen = false;
}

bool CrimsonlandFramework::Init() {

  // TODO(mizofix): Get rid of GLAD
  if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    return false;

  }
  if(!m_bumpProgram.generateProgram("shaders/posteffect.vert",
                                    "shaders/bumpeffect.frag")) {
    info("%s\n", m_bumpProgram.getErrorMessage().c_str());
    return false;
  }

  if(!loadAnimations("data/animations.json") ||
     !loadAnimations("data/zombies/zombie.json") ||
     !loadAnimations("data/trees/trees.json")) {
    return false;
  }
  showCursor(false);

  m_background = createSprite("sand");

  m_screenTexture = createTexture(m_worldData.windowWidth, m_worldData.windowHeight);

  if(m_screenTexture == nullptr) {
    return false;
  }

  return initMainPart();
}

bool CrimsonlandFramework::initMainPart() {
  registerMethod<CrimsonlandFramework>(int(MessageType::PLAYER_DEAD),
                                       &CrimsonlandFramework::onPlayerDead,
                                       this);
  m_playerDeadMessageReceived = false;
  m_playerDeadMessageProcessed = false;


  if(!initPlants() || !initECS()) return false;

  m_worldData.zombieCounter = 0;

  return true;
}


bool CrimsonlandFramework::initECS() {
  m_registry = new Registry();
  m_context.registry = m_registry;
  m_context.data = m_worldData;
  if(!m_systemManager.addSystem(m_context, new LevelSystem(), "level_system")) return false;
  if(!m_systemManager.addSystem(m_context, new PhysicsIntegrationSystem(), "integration_system")) return false;
  if(!m_systemManager.addSystem(m_context, new PhysicsCollisionSystem(), "collision_system")) return false;
  if(!m_systemManager.addSystem(m_context, new PenetrationResolutionSystem(), "penetration_system")) return false;
  if(!m_systemManager.addSystem(m_context, new BulletSystem(), "bullet_system")) return false;
  if(!m_systemManager.addSystem(m_context, new FootprintGenerationSystem(), "footprints_system")) return false;
  if(!m_systemManager.addSystem(m_context, new TrailSystem(), "trail_system")) return false;
  if(!m_systemManager.addSystem(m_context, new EffectsSystem(), "effects_system")) return false;
  if(!m_systemManager.addSystem(m_context, new PlayerSystem(), "player_system")) return false;
  if(!m_systemManager.addSystem(m_context, new ZombieSystem(), "zombie_system")) return false;
  if(!m_systemManager.addSystem(m_context, new ModelRenderingSystem(), "model_rendering_system")) return false;

  m_uiSystem = new UIRenderingSystem();
  if(!m_uiSystem->init(m_context)) return false;
  return true;
}

bool CrimsonlandFramework::initPlants() {

  int maximalCount = m_worldData.numPlants;
  for(int i = 0; i < maximalCount; ++i) {
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
  return true;
}

void CrimsonlandFramework::clearMainPart() {
  clearPlants();

  m_systemManager.clear();
  delete m_uiSystem;
  delete m_registry;
}

void CrimsonlandFramework::clearPlants() {
  for(auto bush: m_bushes) {
    destroySprite(bush.first);
  }

  for(auto tree: m_trees) {
    destroySprite(tree.first);
  }

  m_bushes.clear();
  m_trees.clear();
}

bool CrimsonlandFramework::restartGame() {
  clearSubscribers();
  clearMainPart();
  m_worldData.roundData.intermissionActivated = true;
  m_worldData.roundData.elapsedTime = 0.0f;
  m_worldData.roundData.roundTime = 10.0f;
  m_worldData.roundData.currentRoundNumber = 1;

  return initMainPart();
}

bool CrimsonlandFramework::Tick() {

  if(!m_done) {
    update();
    draw();
    drawToScreen();
    updateTimer();
  }

  return m_done;
}

void CrimsonlandFramework::onPlayerDead(Message message) {
  m_playerDeadMessageReceived = true;
  m_playerDeadMessageProcessed = false;
}

void CrimsonlandFramework::processDeadMessage() {
  m_systemManager.removeSystem("level_system");
  m_systemManager.removeSystem("integration_system");
  m_systemManager.removeSystem("penetration_system");
  m_systemManager.removeSystem("collision_system");
  m_systemManager.removeSystem("bullet_system");
  m_systemManager.removeSystem("player_system");
  m_systemManager.removeSystem("zombie_system");

  m_playerDeadMessageProcessed = true;
}

void CrimsonlandFramework::updateTimer() {
  float time = float(getTickCount()) / 1000.0f;
  m_deltaTime = time - m_lastTime;
  m_lastTime = time;
}


void CrimsonlandFramework::update() {
  if(m_playerDeadMessageReceived) {
    m_deltaTime = 0.0f;

    if(!m_playerDeadMessageProcessed) {
      processDeadMessage();
      getCameraPosition(m_deadCameraPosX, m_deadCameraPosY);
    }

    int cameraPosX, cameraPosY;

    cameraPosX = std::cos(m_lastTime) * 17.5f + m_deadCameraPosX;
    cameraPosY = std::sin(m_lastTime) * 35.0f + m_deadCameraPosY;

    setCameraPosition(cameraPosX, cameraPosY);

    if(isKeyPressed(FRKey::ACTION)) {
      if(!restartGame()) {
        m_done = true;
        return;
      }

      m_playerDeadMessageReceived = false;
      m_playerDeadMessageProcessed = false;
    }

  }

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
  clearMainPart();

  destroySprite(m_background);
  destroyTexture(m_screenTexture);
}
