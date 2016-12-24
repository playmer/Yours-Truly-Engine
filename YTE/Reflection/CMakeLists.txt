include(${Source_Root}/cmake/EngineSystemSetup.cmake)

Get_Filename_Component(engineSystem ${CMAKE_CURRENT_SOURCE_DIR} NAME)
Message(STATUS ${engineSystem})
EngineSystemSetup(${engineSystem})

#AddFile(Yours_Truly_Engine_Source Reflectable.hpp)
#AddFile(Yours_Truly_Engine_Source ReflectionSystem.hpp)
#AddFile(Yours_Truly_Engine_Source Type.hpp)