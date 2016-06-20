//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef TargetDefinitions_h
#define TargetDefinitions_h

#ifdef _WIN32
  // If _WIN32 is defined, we're on Windows.
  #define Windows

  // If _WIN64 is defined, we're on x64.
  #ifdef _WIN64
    #define x64

  // Else we're on 32 bit.
  #else
   #define x86
  #endif
#endif

#endif