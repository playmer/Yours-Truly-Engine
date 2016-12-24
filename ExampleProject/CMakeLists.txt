Include_Directories(${Source_Root})
Include_Directories(${YTE_Root})

Include_Directories(${ExampleProject_Root})

File(GLOB_RECURSE projectSource *.cpp)
File(GLOB_RECURSE projectHeaders *.h *.hpp)

Add_Executable(ExampleProject ${projectSource} 
                              ${Dependencies_Root}/stb/stb_image.cpp
                              ${Dependencies_Root}/vulkan/vulkan/vkel.c
                              ${projectHeaders})

ForEach(engineSystem ${YTE_System_Subdirectories})
  Target_Link_Libraries( ExampleProject ${engineSystem} )
EndForEach()