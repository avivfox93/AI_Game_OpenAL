cmake_minimum_required(VERSION 3.19)
project(Test)

set(CMAKE_CXX_STANDARD 14)

add_executable(Test main.cpp Player.cpp Player.h Point2D.cpp Point2D.h Entity.cpp Entity.h Bullet.cpp Bullet.h Game.cpp Game.h Wall.h Path.h Grenade.cpp Grenade.h Fighter.cpp Fighter.h HealthBox.h AmmoBox.h Support.cpp Support.h AStar.h Random.h DummyGrenade.cpp DummyGrenade.h Random.cpp AudioManager.cpp AudioManager.h AudioUtils.cpp AudioUtils.h)

target_link_libraries(Test -lfreeGLUT -lOpenGL32 -lopenal)
