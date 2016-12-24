Macro(EngineSystemSetup engineSystem)
  File( GLOB_RECURSE EngineSystemSource *.cpp *.c)
  File( GLOB_RECURSE EngineSystemHeaders *.hpp *.h)
  
  EngineSystemSetupDirectory(${engineSystem} ${CMAKE_CURRENT_SOURCE_DIR}
                             "${EngineSystemSource}" "${EngineSystemHeaders}")
EndMacro()

Function(EngineSystemSetupDirectory engineSystem currentDirectory sourceFiles headerFiles)
  #Include_Directories(${currentDirectory} ${Engine_System_Root})
  
  # Make source groups
  SourceGroupSetup(${currentDirectory} " " FALSE)
  
  If (sourceFiles STREQUAL "")
    Add_Library( ${engineSystem} INTERFACE)
    target_sources(${engineSystem} INTERFACE ${headerFiles})
  Else()
    Add_Library( ${engineSystem} ${sourceFiles} ${headerFiles})
    set_property( TARGET ${engineSystem} PROPERTY FOLDER "YTE")
  EndIf()
  

  #If (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  #  Target_Compile_Options(${engineSystem} PUBLIC "/MT$<$<CONFIG:Debug>:d>")
  #EndIf()

    # External Dependencies
  Target_Link_Libraries(${engineSystem} ${LIBCMT_LIBRARIES} ${StaticLibraries})

EndFunction()
