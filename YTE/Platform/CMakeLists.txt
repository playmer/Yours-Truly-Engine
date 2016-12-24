include(${Source_Root}/cmake/EngineSystemSetup.cmake)

Get_Filename_Component(engineSystem ${CMAKE_CURRENT_SOURCE_DIR} NAME)
Message(STATUS ${engineSystem})
EngineSystemSetup(${engineSystem})

#AddFile(Yours_Truly_Engine_Source ForwardDeclarations.hpp)
#AddFile(Yours_Truly_Engine_Source Gamepad.hpp)
#AddFile(Yours_Truly_Engine_Source GamepadSystem.hpp)
#AddFile(Yours_Truly_Engine_Source Keyboard.cpp Keyboard.hpp)
#AddFile(Yours_Truly_Engine_Source KeyCodes.hpp)
#AddFile(Yours_Truly_Engine_Source Mouse.hpp)
#AddFile(Yours_Truly_Engine_Source Window.cpp Window.hpp)
#
#add_subdirectory(Windows)