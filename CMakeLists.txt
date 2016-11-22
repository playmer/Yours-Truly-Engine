cmake_minimum_required(VERSION 3.5)

project(Yours-Truly-Engine)

set(Yours_Truly_Engine_Source main.cpp)

add_subdirectory(YTE)


#find_package(constexpr REQUIRED)
#find_package(glm REQUIRED)
#find_package(stb REQUIRED)
#find_package(vulkan REQUIRED)

add_executable(Yours-Truly-Engine ${Yours_Truly_Engine_Source})