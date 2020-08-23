#include "Utils.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

const static char* line = "-------------------------\n";
const static char* error_header = "[error]";
const static char* warning_header = "[warning]";

const static int MIN_SCREENW = 320;
const static int MIN_SCREENH = 240;

const static int MIN_AMMO = 5;
const static int MAX_AMMO = 20;

const static int MIN_ENEMIES = 5;
const static int MAX_ENEMIES = 200;

const static int MAX_PLANTS = 100;
const static int MIN_PLANTS = 1;

const static int MAX_POINTS = 10000;
const static int MIN_POINTS = 1;

const static int MIN_REGEN = 0;
const static int MAX_REGEN = 10000;

const static int MIN_EFFECTS = 0;
const static int MAX_EFFECTS = 10000;

const static int MIN_ROUND = 1;
const static int MAX_ROUND = 10;


static int strCaseCmp(const char* strA, const char* strB) {
  const char* ptrA = strA, *ptrB = strB;
  for(;(tolower(*ptrA) == tolower(*ptrB)) && *ptrA; ptrA++, ptrB++);

  return (*ptrA == *ptrB) ?  0 :
         (*ptrA < *ptrB)  ? -1 :
                             1;
}

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
    MIN_ENEMIES, 50,
    MIN_SCREENW, MIN_SCREENH,
    MIN_SCREENW, MIN_SCREENH,

    100, 100, 100, 5.0f, 6.0f,

    10000

  };

  result.roundData = {};
  result.roundData.roundTime = 10.0f;
  result.roundData.currentRoundNumber = 1;
  result.roundData.intermissionActivated = true;

  int i = 1;
  while(i < argc) {
    bool isNotLast = (argc - i) > 1;
    // TODO(mizofix): move strcasecmp to platfrom-independent layer
    if(strCaseCmp(commands[i], "-help") == 0) {
      printf("\n%s", line);
      printf("    Help\n");
      printf("%s\n", line);
      printf(" -map [Width]x[Height] - to set map size equal to Width times Height pixels\n");
      printf(" -window [Width]x[Height] - to set window size equal to Width times Height pixels\n"
             "  (minimal width %d, minimal height %d)\n", MIN_SCREENW, MIN_SCREENH);
      printf(" -num_enemies [num] - to set desired maximal number of enemies\n"
             "  (minimal %d maximal %d)\n", MIN_ENEMIES, MAX_ENEMIES);
      printf(" -num_plants [num] - to set desired maximal number of plants\n"
             "  (minimal %d maximal %d)\n", MIN_PLANTS, MAX_PLANTS);
      printf(" -max_health [num] - to set desired max player health\n"
             "  (minimal %d maximal %d)\n", MIN_POINTS, MAX_POINTS);
      printf(" -max_stamina [num] - to set desired max player stamina\n"
             "  (minimal %d maximal %d)\n", MIN_POINTS, MAX_POINTS);
      printf(" -hp_regen [num] - to set player health regeneration speed\n"
             "  (minimal %d maximal %d)\n", MIN_REGEN, MAX_POINTS);
      printf(" -stamina_regen [num] - to set player stamina regeneration speed\n"
             "  (minimal %d maximal %d)\n", MIN_REGEN, MAX_REGEN);
      printf(" -num_effects [num] - to set desired maximal number of effects\n"
             "  (minimal %d maximal %d)\n", MIN_EFFECTS, MAX_EFFECTS);
      printf(" -start_round [num] - to set initial round number\n"
             "  (minimal %d maximal %d)\n", MIN_ROUND, MAX_ROUND);

      exit(0);
    }
    else if(strCaseCmp(commands[i], "-window") == 0 && isNotLast) {
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
        //info("%s map width was stretched to screen width, since it too small!\n", note_header);
      }
      if(result.mapHeight < result.windowHeight) {
        result.mapHeight = result.windowHeight;
        //info("%s map height was stretched to screen height, since int too small!\n", note_header);
      }

      i += 2;
    }

    else if(strCaseCmp(commands[i], "-map") == 0 && isNotLast) {
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

    else if(strCaseCmp(commands[i], "-num_enemies") == 0 && isNotLast) {
      result.numEnemies = clamp(atoi(commands[i + 1]), MIN_ENEMIES, MAX_ENEMIES);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-num_plants") == 0 && isNotLast) {
      result.numPlants = clamp(atoi(commands[i + 1]), MIN_PLANTS, MAX_PLANTS);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-max_health") == 0 && isNotLast) {
      result.maxPlayerHealth = clamp(atoi(commands[i + 1]), MIN_POINTS, MAX_POINTS);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-max_stamina") == 0 && isNotLast) {
      result.maxPlayerStamina = clamp(atoi(commands[i + 1]), MIN_POINTS, MAX_POINTS);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-hp_regen") == 0 && isNotLast) {
      result.regenSpeed = clamp(atoi(commands[i + 1]), MIN_REGEN, MAX_REGEN);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-stamina_regen") == 0 && isNotLast) {
      result.staminaRegenSpeed = clamp(atoi(commands[i + 1]), MIN_REGEN, MAX_REGEN);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-num_effects") == 0 && isNotLast) {
      result.maxEffectsNumber = clamp(atoi(commands[i + 1]), MIN_EFFECTS, MAX_EFFECTS);
      i += 2;
    }
    else if(strCaseCmp(commands[i], "-start_round") == 0 && isNotLast) {
      result.roundData.currentRoundNumber = clamp(atoi(commands[i + 1]), MIN_ROUND, MAX_ROUND);
      i += 2;
    }

    else {
      info("%s command '%s' is undefined.\n", error_header, commands[i]);
      i += 1;
    }

  }

  return result;
}

real randomReal(real start, real end) {
  return drand48() * (end - start) + start;
}
