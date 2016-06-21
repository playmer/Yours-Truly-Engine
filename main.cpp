//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include <Windows.h>
#include <GL/GL.h>

int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );

  engine.mWindow.CreateOpenGLContext();
  engine.mWindow.MakeOpenGLContextCurrent();

  glbinding::Binding::initialize(); 
  gl::glViewport(0, 0, engine.mWindow.mWidth, engine.mWindow.mHeight);

  float red = 0.0f, green = 0.0f, blue = 0.0f;

  bool decending = false;

  while (engine.mShouldUpdate)
  {
    engine.Update(0.016f);

    if (decending)
    {
      red -= 0.001f;
      green -= 0.001f;
      blue -= 0.001f;
    }
    else
    {
      red += 0.001f;
      green += 0.001f;
      blue += 0.001f;
    }

    if (red >= 1.0f || red <= 0.0f)
    {
      decending = !decending;
    }


    gl::glClearColor(red, green, blue, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT);

    engine.mWindow.SwapBuffers();

    if (engine.mWindow.mKeyboard.IsKeyPressed(YTE::KeyCode::Escape))
    { 
      engine.mShouldUpdate = false;
    }
  }

  return 0;
}