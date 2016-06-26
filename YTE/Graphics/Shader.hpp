//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#pragma once

#ifndef YTE_Shader_hpp
#define YTE_Shader_hpp

#include <stddef.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "YTE/DataStructures/UniqueHandle.hpp"

namespace YTE
{
  using OpenGLHandle = YTE::UniqueHandle<gl::GLuint>;


  void DeleteShader(gl::GLuint aShader);;


  OpenGLHandle CreateShader(const char *aSource, gl::GLenum aType);



  void DeleteShaderProgram(gl::GLuint aShaderProgram);;


  OpenGLHandle CreateShaderProgram(std::vector<OpenGLHandle> &aShaders);


  OpenGLHandle GraphicsInitialize();
}
#endif
