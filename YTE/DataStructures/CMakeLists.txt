include(${Source_Root}/cmake/EngineSystemSetup.cmake)

Get_Filename_Component(engineSystem ${CMAKE_CURRENT_SOURCE_DIR} NAME)
Message(STATUS ${engineSystem})
EngineSystemSetup(${engineSystem})

#AddFile(Yours_Truly_Engine_Source IntrusiveList.hpp)
#AddFile(Yours_Truly_Engine_Source UniqueHandle.hpp)