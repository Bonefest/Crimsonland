#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -Idependencies -Idependencies/inc -Iinclude
CFLAGS = -O3 -Wall -fexceptions
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -ldl -lGL -lX11 -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf

INC_DEBUG = $(INC) -Idependencies -Idependencies/inc -Iinclude
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS) -ldl -lGL -lX11 -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/Crimsoland

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/Crimsoland

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/Components.o $(OBJDIR_DEBUG)/src/ecs/Registry.o $(OBJDIR_DEBUG)/src/ecs/Component.o $(OBJDIR_DEBUG)/src/ecs/Bitset.o $(OBJDIR_DEBUG)/src/base.o $(OBJDIR_DEBUG)/src/ZombieStates.o $(OBJDIR_DEBUG)/src/Utils.o $(OBJDIR_DEBUG)/src/Systems.o $(OBJDIR_DEBUG)/src/StateController.o $(OBJDIR_DEBUG)/src/StateBase.o $(OBJDIR_DEBUG)/src/PlayerStates.o $(OBJDIR_DEBUG)/src/Message.o $(OBJDIR_DEBUG)/src/Math.o $(OBJDIR_DEBUG)/dependencies/Program.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/dependencies/launch.o $(OBJDIR_DEBUG)/dependencies/glad/glad.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/src/Components.o $(OBJDIR_RELEASE)/src/ecs/Registry.o $(OBJDIR_RELEASE)/src/ecs/Component.o $(OBJDIR_RELEASE)/src/ecs/Bitset.o $(OBJDIR_RELEASE)/src/base.o $(OBJDIR_RELEASE)/src/ZombieStates.o $(OBJDIR_RELEASE)/src/Utils.o $(OBJDIR_RELEASE)/src/Systems.o $(OBJDIR_RELEASE)/src/StateController.o $(OBJDIR_RELEASE)/src/StateBase.o $(OBJDIR_RELEASE)/src/PlayerStates.o $(OBJDIR_RELEASE)/src/Message.o $(OBJDIR_RELEASE)/src/Math.o $(OBJDIR_RELEASE)/dependencies/Program.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/dependencies/launch.o $(OBJDIR_RELEASE)/dependencies/glad/glad.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src
	test -d $(OBJDIR_DEBUG)/src/ecs || mkdir -p $(OBJDIR_DEBUG)/src/ecs
	test -d $(OBJDIR_DEBUG)/dependencies || mkdir -p $(OBJDIR_DEBUG)/dependencies
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/dependencies/glad || mkdir -p $(OBJDIR_DEBUG)/dependencies/glad

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/src/Components.o: src/Components.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Components.cpp -o $(OBJDIR_DEBUG)/src/Components.o

$(OBJDIR_DEBUG)/src/ecs/Registry.o: src/ecs/Registry.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/ecs/Registry.cpp -o $(OBJDIR_DEBUG)/src/ecs/Registry.o

$(OBJDIR_DEBUG)/src/ecs/Component.o: src/ecs/Component.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/ecs/Component.cpp -o $(OBJDIR_DEBUG)/src/ecs/Component.o

$(OBJDIR_DEBUG)/src/ecs/Bitset.o: src/ecs/Bitset.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/ecs/Bitset.cpp -o $(OBJDIR_DEBUG)/src/ecs/Bitset.o

$(OBJDIR_DEBUG)/src/base.o: src/base.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/base.cpp -o $(OBJDIR_DEBUG)/src/base.o

$(OBJDIR_DEBUG)/src/ZombieStates.o: src/ZombieStates.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/ZombieStates.cpp -o $(OBJDIR_DEBUG)/src/ZombieStates.o

$(OBJDIR_DEBUG)/src/Utils.o: src/Utils.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Utils.cpp -o $(OBJDIR_DEBUG)/src/Utils.o

$(OBJDIR_DEBUG)/src/Systems.o: src/Systems.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Systems.cpp -o $(OBJDIR_DEBUG)/src/Systems.o

$(OBJDIR_DEBUG)/src/StateController.o: src/StateController.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/StateController.cpp -o $(OBJDIR_DEBUG)/src/StateController.o

$(OBJDIR_DEBUG)/src/StateBase.o: src/StateBase.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/StateBase.cpp -o $(OBJDIR_DEBUG)/src/StateBase.o

$(OBJDIR_DEBUG)/src/PlayerStates.o: src/PlayerStates.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/PlayerStates.cpp -o $(OBJDIR_DEBUG)/src/PlayerStates.o

$(OBJDIR_DEBUG)/src/Message.o: src/Message.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Message.cpp -o $(OBJDIR_DEBUG)/src/Message.o

$(OBJDIR_DEBUG)/src/Math.o: src/Math.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Math.cpp -o $(OBJDIR_DEBUG)/src/Math.o

$(OBJDIR_DEBUG)/dependencies/Program.o: dependencies/Program.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c dependencies/Program.cpp -o $(OBJDIR_DEBUG)/dependencies/Program.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/dependencies/launch.o: dependencies/launch.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c dependencies/launch.cpp -o $(OBJDIR_DEBUG)/dependencies/launch.o

$(OBJDIR_DEBUG)/dependencies/glad/glad.o: dependencies/glad/glad.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c dependencies/glad/glad.c -o $(OBJDIR_DEBUG)/dependencies/glad/glad.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/src
	rm -rf $(OBJDIR_DEBUG)/src/ecs
	rm -rf $(OBJDIR_DEBUG)/dependencies
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/dependencies/glad

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src
	test -d $(OBJDIR_RELEASE)/src/ecs || mkdir -p $(OBJDIR_RELEASE)/src/ecs
	test -d $(OBJDIR_RELEASE)/dependencies || mkdir -p $(OBJDIR_RELEASE)/dependencies
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/dependencies/glad || mkdir -p $(OBJDIR_RELEASE)/dependencies/glad

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/src/Components.o: src/Components.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Components.cpp -o $(OBJDIR_RELEASE)/src/Components.o

$(OBJDIR_RELEASE)/src/ecs/Registry.o: src/ecs/Registry.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/ecs/Registry.cpp -o $(OBJDIR_RELEASE)/src/ecs/Registry.o

$(OBJDIR_RELEASE)/src/ecs/Component.o: src/ecs/Component.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/ecs/Component.cpp -o $(OBJDIR_RELEASE)/src/ecs/Component.o

$(OBJDIR_RELEASE)/src/ecs/Bitset.o: src/ecs/Bitset.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/ecs/Bitset.cpp -o $(OBJDIR_RELEASE)/src/ecs/Bitset.o

$(OBJDIR_RELEASE)/src/base.o: src/base.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/base.cpp -o $(OBJDIR_RELEASE)/src/base.o

$(OBJDIR_RELEASE)/src/ZombieStates.o: src/ZombieStates.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/ZombieStates.cpp -o $(OBJDIR_RELEASE)/src/ZombieStates.o

$(OBJDIR_RELEASE)/src/Utils.o: src/Utils.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Utils.cpp -o $(OBJDIR_RELEASE)/src/Utils.o

$(OBJDIR_RELEASE)/src/Systems.o: src/Systems.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Systems.cpp -o $(OBJDIR_RELEASE)/src/Systems.o

$(OBJDIR_RELEASE)/src/StateController.o: src/StateController.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/StateController.cpp -o $(OBJDIR_RELEASE)/src/StateController.o

$(OBJDIR_RELEASE)/src/StateBase.o: src/StateBase.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/StateBase.cpp -o $(OBJDIR_RELEASE)/src/StateBase.o

$(OBJDIR_RELEASE)/src/PlayerStates.o: src/PlayerStates.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/PlayerStates.cpp -o $(OBJDIR_RELEASE)/src/PlayerStates.o

$(OBJDIR_RELEASE)/src/Message.o: src/Message.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Message.cpp -o $(OBJDIR_RELEASE)/src/Message.o

$(OBJDIR_RELEASE)/src/Math.o: src/Math.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Math.cpp -o $(OBJDIR_RELEASE)/src/Math.o

$(OBJDIR_RELEASE)/dependencies/Program.o: dependencies/Program.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c dependencies/Program.cpp -o $(OBJDIR_RELEASE)/dependencies/Program.o

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/dependencies/launch.o: dependencies/launch.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c dependencies/launch.cpp -o $(OBJDIR_RELEASE)/dependencies/launch.o

$(OBJDIR_RELEASE)/dependencies/glad/glad.o: dependencies/glad/glad.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c dependencies/glad/glad.c -o $(OBJDIR_RELEASE)/dependencies/glad/glad.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)/src
	rm -rf $(OBJDIR_RELEASE)/src/ecs
	rm -rf $(OBJDIR_RELEASE)/dependencies
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/dependencies/glad

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

