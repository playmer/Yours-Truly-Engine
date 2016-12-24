Set(Engine_System_Root "${CMAKE_CURRENT_SOURCE_DIR}")

ForEach(engineSystem ${YTE_System_Subdirectories})
    Add_Subdirectory(${engineSystem})
EndForEach()