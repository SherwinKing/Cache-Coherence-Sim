cmake_minimum_required(VERSION 3.9)
project(project_csim)

set(CMAKE_CXX_STANDARD 11)

add_executable(project_csim main.cpp src/CacheController.h src/Cache.cpp src/Cache.h src/Statistics.cpp src/Statistics.h src/Response.cpp src/Response.h src/Request.cpp src/Request.h src/SnoopingCacheController.cpp src/SnoopingCacheController.h src/Simulator.cpp src/Simulator.h src/CacheSet.cpp src/CacheSet.h src/Interconnection.cpp src/Interconnection.h src/main.cpp)