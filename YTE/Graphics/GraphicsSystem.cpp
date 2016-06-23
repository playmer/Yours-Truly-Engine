#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

namespace YTE
{
  GraphicsSystem::GraphicsSystem(Engine *aEngine) : mEngine(aEngine)
  {
  }

  GraphicsSystem::~GraphicsSystem()
  {
  }

  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
    aWindow->CreateOpenGLContext();
    aWindow->MakeOpenGLContextCurrent();
  }

  void GraphicsSystem::Initialize()
  {
    glbinding::Binding::initialize();
  }

  void GraphicsSystem::Update(float aDt)
  {
    for (auto &window : mEngine->mWindows)
    {
      gl::glViewport(0, 0, window->mWidth, window->mHeight);

      gl::glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      gl::glClear(gl::GL_COLOR_BUFFER_BIT);

      window->SwapBuffers();
    }
  }
}
