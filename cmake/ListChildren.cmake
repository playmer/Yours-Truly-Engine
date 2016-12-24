MACRO(ListDirectories result currentDirectory)
  File(GLOB children RELATIVE ${currentDirectory} ${currentDirectory}/*)
  Set(dirlist "")
  
  ForEach(child ${children})
    If(IS_DIRECTORY ${currentDirectory}/${child})
      List(APPEND dirlist ${child})
    EndIf()
  EndForEach()
  
  Set(${result} ${dirlist})
ENDMACRO()

MACRO(ListFiles result currentDirectory)
  File(GLOB children ${currentDirectory} 
          ${currentDirectory}/*.cpp ${currentDirectory}/*.c
          ${currentDirectory}/*.hpp ${currentDirectory}/*.h)
  Set(dirlist "")
  
  ForEach(child ${children})
    If(NOT IS_DIRECTORY ${currentDirectory}/${child})
      List(APPEND dirlist ${child})
    EndIf()
  EndForEach()
  
  Set(${result} ${dirlist})
ENDMACRO()