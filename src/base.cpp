#include "base.h"
#include <cstring>


#include "glad/glad.h"
#include <GL/gl.h>

const static char* line = "-------------------------\n";
const static char* error_header = "[error]";
const static char* warning_header = "[warning]";
const static char* note_header = "[note]";

const static int MIN_SCREENW = 320;
const static int MIN_SCREENH = 240;

const static int MIN_AMMO = 5;
const static int MAX_AMMO = 20;

const static int MIN_ENEMIES = 5;
const static int MAX_ENEMIES = 20;

// NOTE(mizofix): current version is not cross-platform
WorldData parseCommands(int argc, char** commands) {

  auto parseSizeArgument = [](const char* argument, int* value1, int* value2) {
    *value1 = atoi(argument);
    const char* argument2 = strchr(argument, 'x');
    if(argument2 != nullptr) {
      *value2 = atoi(argument2 + 1);
    }
  };

  WorldData result {
    MIN_ENEMIES, MIN_AMMO,
    MIN_SCREENW, MIN_SCREENH,
    MIN_SCREENW, MIN_SCREENH
  };

  int i = 1;
  while(i < argc) {
    bool isNotLast = (argc - i) > 1;
    // TODO(mizofix): move strcasecmp to platfrom-independent layer
    if(strcasecmp(commands[i], "-help") == 0) {
      printf("\n%s", line);
      printf("    Help\n");
      printf("%s\n", line);
      printf(" -map [Width]x[Height] - to set map size equal to Width times Height pixels\n");
      printf(" -window [Width]x[Height] - to set window size equal to Width times Height pixels\n"
             "  (minimal width %d, minimal height %d)\n", MIN_SCREENW, MIN_SCREENH);
      printf(" -num_ammo [num] - to set desired maximal number of ammo\n"
             "  (minimal %d maximal %d)\n", MIN_AMMO, MAX_AMMO);
      printf(" -num_enemies [num] - to set desired maximal number of enemies\n"
             "  (minimal %d maximal %d)\n", MIN_ENEMIES, MAX_ENEMIES);

      exit(0);
    }
    else if(strcasecmp(commands[i], "-window") == 0 && isNotLast) {
      char* windowData = commands[i + 1];

      int width, height;
      parseSizeArgument(windowData, &width, &height);
      if(width < MIN_SCREENW || height < MIN_SCREENH) {
        info("%s window size is too small! Window size set to %dx%d.\n",
             warning_header, MIN_SCREENW, MIN_SCREENH);

        width = MIN_SCREENW;
        height = MIN_SCREENH;
      }

      result.windowWidth = width;
      result.windowHeight = height;

      if(result.mapWidth < result.windowWidth) {
        result.mapWidth = result.windowWidth;
        info("%s map width was stretched to screen width, since it too small!\n", note_header);
      }
      if(result.mapHeight < result.windowHeight) {
        result.mapHeight = result.windowHeight;
        info("%s map height was stretched to screen height, since int too small!\n", note_header);
      }

      i += 2;
    }

    else if(strcasecmp(commands[i], "-map") == 0 && isNotLast) {
      char* mapData = commands[i + 1];

      int width, height;
      parseSizeArgument(mapData, &width, &height);
      if(width < result.windowWidth && height < result.windowHeight) {
        info("%s map size is too small! Map size set to %dx%d.\n",
                warning_header, result.windowWidth, result.windowHeight);

        width = result.windowWidth;
        height = result.windowHeight;
      }

      result.mapWidth = (real)width;
      result.mapHeight = (real)height;
      i += 2;
    }

    else if(strcasecmp(commands[i], "-num_enemies") == 0 && isNotLast) {
      result.numEnemies = clamp(atoi(commands[i + 1]), MIN_ENEMIES, MAX_ENEMIES);
      i += 2;
    }

    else if(strcasecmp(commands[i], "-num_ammo") == 0 && isNotLast) {
      result.numAmmo = clamp(atoi(commands[i + 1]), MIN_AMMO, MAX_AMMO);
      i += 2;
    }

    else {
      info("%s command '%s' is undefined.\n", error_header, commands[i]);
      i += 1;
    }

  }

  return result;
}

CrimsonlandFramework::CrimsonlandFramework(int argc, char** commands) {
  m_worldData = parseCommands(argc, commands);

  info(line);
  info("final world data values are:\n", note_header);
  info("Window size (%d, %d)\n", m_worldData.windowWidth, m_worldData.windowHeight);
  info("Map size(%.1f, %.1f)\n", m_worldData.mapWidth, m_worldData.mapHeight);
  info("Maximal enemies number %d\n", m_worldData.numEnemies);
  info("Maximal ammo number %d\n", m_worldData.numAmmo);
  info(line);

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


  return true;
}

bool CrimsonlandFramework::Tick() {

  float time = float(SDL_GetTicks()) / 1000.0f;

  m_screenTexture.setAsRenderingTarget(renderer);

  drawTestBackground();
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for(int x = 0; x < m_worldData.windowWidth; x += 20) {
    //    SDL_RenderDrawLine(renderer, x, 0, x, m_worldData.windowHeight);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

  drawSprite(m_testSprite, 200, 240);
  drawSprite(m_testSprite, 320, 150);
  drawSprite(m_testSprite, 320, 300);
  drawSprite(m_testSprite, 440, 240);

  drawSprite(m_testSprite, 320 + std::sin(time) * 240, 240);

  deactivateTextureRendering();

  SDL_RenderClear(renderer);

  m_screenTexture.bind();
  glUseProgram(m_bumpProgram.getProgramID());

  glUniform1f(glGetUniformLocation(m_bumpProgram.getProgramID(), "time"), time);
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

  return false;
}

void CrimsonlandFramework::deactivateTextureRendering() {
  SDL_SetRenderTarget(renderer, NULL);
}

void CrimsonlandFramework::Close() {
  destroySprite(m_testSprite);
}
