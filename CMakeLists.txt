CMake_Minimum_Required(VERSION 3.6 FATAL_ERROR)

Project(Yours_Truly_Engine CXX C)

Set(Source_Root ${CMAKE_CURRENT_SOURCE_DIR})
Set(YTE_Root ${Source_Root}/YTE)
Set(Dependencies_Root ${Source_Root}/Dependencies)
Set(ExampleProject_Root ${Source_Root}/ExampleProject)
Set(CMake_Include ${Source_Root}/cmake/)

Include(${CMake_Include}/SourceGroupSetup.cmake)
Include(${CMake_Include}/ListChildren.cmake)
Include(${CMake_Include}/EngineSystemSetup.cmake)

Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

ListDirectories(YTE_System_Subdirectories ${YTE_Root})

Include_Directories(${Dependencies_Root}/constexpr)
Include_Directories(${Dependencies_Root}/glm)
Include_Directories(${Dependencies_Root}/stb)
Include_Directories(${Dependencies_Root}/vulkan)


Include_Directories(${Source_Root})

Add_Subdirectory(YTE)

Add_Subdirectory(ExampleProject)

Set_Target_Properties(ExampleProject PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Source_Root})