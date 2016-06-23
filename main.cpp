//////////////////////////////////////////////
// Author: Joshua T. Fisher
//////////////////////////////////////////////
#include <stdio.h>
#include <iostream>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/Windows/WindowData_Windows.hpp"

#include <Windows.h>
#include <GL/GL.h>

#include <memory>

struct Color
{
  Color() : mRed(0.0f), mGreen(0.0f), mBlue(0.0f), mAlpha(1.0f) {};

  float mRed;
  float mGreen;
  float mBlue;
  float mAlpha;
};

template <typename HandleType, typename DestructionFunction = void(*)(HandleType)>
class unique_handle
{
  public:
  unique_handle(HandleType &aHandle, DestructionFunction aDestructor)
    : mHandle(aHandle), mDestructor(aDestructor)
  {

  }

  unique_handle& operator=(const unique_handle &) = delete;
  unique_handle(const unique_handle &) = delete;

  unique_handle(unique_handle &&rhs)
  {
    // Release whatever we're holding.
    release();

    mDestructor = rhs.mDestructor;
    mDestructor = rhs.mDestructor;

    rhs.mDestructor = nullptr;
    rhs.mHandle = HandleType();
  }

  HandleType get()
  {
    return mHandle;
  }

  void release()
  {
    if (nullptr != mDestructor)
    {
      mDestructor(mHandle);
      mDestructor = nullptr;
      mHandle = HandleType();
    }
  }

  ~unique_handle()
  {
    mDestructor(mHandle);
  }

  private:
  HandleType mHandle;
  DestructionFunction mDestructor;
};

void DeleteShader(gl::GLuint aShader)
{
  if (aShader != 0)
  {
    std::cout << "Deleting shader: " << aShader << std::endl;
    gl::glDeleteShader(aShader);
  }
};


using OpenGLHandle = unique_handle<gl::GLuint>;

bool PrintShaderError(gl::GLuint aShader, gl::GLenum aType)
{
  gl::GLint success;
  gl::GLchar infoLog[512];
  gl::glGetShaderiv(aShader, gl::GL_COMPILE_STATUS, &success);

  if (!success)
  {
    gl::glGetShaderInfoLog(aShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::";

    switch (aType)
    {
      case gl::GL_VERTEX_SHADER:
      {
        std::cout << "VERTEX";
        break;
      }
      case gl::GL_GEOMETRY_SHADER:
      {
        std::cout << "GEOMETRY";
        break;
      }
      case gl::GL_COMPUTE_SHADER:
      {
        std::cout << "COMPUTE";
        break;
      }
      case gl::GL_FRAGMENT_SHADER:
      {
        std::cout << "FRAGMENT";
        break;
      }
      case gl::GL_TESS_CONTROL_SHADER:
      {
        std::cout << "FRAGMENT";
        break;
      }
      case gl::GL_TESS_EVALUATION_SHADER:
      {
        std::cout << "FRAGMENT";
        break;
      }
    }

    std::cout << "::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  return success == 0;
}

OpenGLHandle CreateShader(const char *aSource, gl::GLenum aType)
{
  gl::GLuint shader;
  shader = gl::glCreateShader(aType);

  gl::glShaderSource(shader, 1, &aSource, NULL);
  gl::glCompileShader(shader);


  if (!PrintShaderError(shader, aType))
  {
    OpenGLHandle shaderPtr(shader, DeleteShader);

    return std::move(shaderPtr);
  }
  else
  {
    gl::GLuint nullShader;
    OpenGLHandle shaderPtr(nullShader, DeleteShader);
    return std::move(shaderPtr);
  }
}



void DeleteShaderProgram(gl::GLuint aShaderProgram)
{
  if (aShaderProgram != 0)
  {
    std::cout << "Deleting shader: " << aShaderProgram << std::endl;
    gl::glDeleteProgram(aShaderProgram);
  }
};


OpenGLHandle CreateShaderProgram(std::vector<OpenGLHandle> &aShaders)
{
  gl::GLuint shaderProgram;
  shaderProgram = gl::glCreateProgram();

  for (auto &shader : aShaders)
  {
    gl::glAttachShader(shaderProgram, shader.get());
  }

  gl::glLinkProgram(shaderProgram);

  for (auto &shader : aShaders)
  {
    gl::glDetachShader(shaderProgram, shader.get());
  }

  gl::GLint success;
  gl::glGetProgramiv(shaderProgram, gl::GL_LINK_STATUS, &success);

  if (!success) 
  {
    gl::GLchar infoLog[512];

    gl::glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  if (success)
  {
    OpenGLHandle shaderPtr(shaderProgram, DeleteShaderProgram);

    return std::move(shaderPtr);
  }
  else
  {
    gl::GLuint nullShader;
    OpenGLHandle shaderPtr(nullShader, DeleteShaderProgram);
    return std::move(shaderPtr);
  }
}


OpenGLHandle GraphicsInitialize()
{
  auto vertexShaderSource = R"(
      #version 330 core
  
      layout(location = 0) in vec3 position;
  
      void main()
      {
        gl_Position = vec4(position.x, position.y, position.z, 1.0);
      })";
  
  auto vertexShader = CreateShader(vertexShaderSource, gl::GL_VERTEX_SHADER);
  
  auto fragmentShaderSource = R"(
      #version 330 core
  
      out vec4 color;
  
      void main()
      {
        color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
      })";

  auto fragmentShader = CreateShader(fragmentShaderSource, gl::GL_FRAGMENT_SHADER);

  std::vector<OpenGLHandle> shaders;
  shaders.emplace_back(std::move(vertexShader));
  shaders.emplace_back(std::move(fragmentShader));

  auto shaderProgram = CreateShaderProgram(shaders);
  
  gl::glUseProgram((gl::GLuint)shaderProgram.get());

  gl::glVertexAttribPointer(0, 3, gl::GL_FLOAT, gl::GL_FALSE, 3 * sizeof(gl::GLfloat), (gl::GLvoid*)0);
  gl::glEnableVertexAttribArray(0);


  return shaderProgram;
}


const float cColorCreep = 0.0025f;

void RenderGraphics(Color &aColor, bool aDescending)
{
  if (aDescending)
  {
    aColor.mRed -= cColorCreep;
    aColor.mGreen -= cColorCreep;
    aColor.mBlue -= cColorCreep;
  }
  else
  {
    aColor.mRed += cColorCreep;
    aColor.mGreen += cColorCreep;
    aColor.mBlue += cColorCreep;
  }

  gl::glClearColor(aColor.mRed, aColor.mGreen, aColor.mBlue, 1.0f);
  gl::glClear(gl::GL_COLOR_BUFFER_BIT);
}

int main(int aArgumentNumber, char **Arguments)
{
  YTE::Engine engine;

  // This is how to create SubWindows
  //YTE::Window subWindow1( &engine, "subWindow1", nullptr, nullptr, 246, 360, &engine.mWindow );

  engine.mWindow.CreateOpenGLContext();
  engine.mWindow.MakeOpenGLContextCurrent();

  glbinding::Binding::initialize(); 
  gl::glViewport(0, 0, engine.mWindow.mWidth, engine.mWindow.mHeight);

  auto program = GraphicsInitialize();

  Color color;

  bool descending = false;

  while (engine.mShouldUpdate)
  {
    engine.Update(0.016f);

    if ((color.mRed >= 1.0f && !descending) || (color.mRed <= 0.0f && descending))
    {
      descending = !descending;
    }

    RenderGraphics(color, descending);

    engine.mWindow.SwapBuffers();

    if (engine.mWindow.mKeyboard.IsKeyPressed(YTE::KeyCode::Escape))
    { 
      engine.mShouldUpdate = false;
    }
  }

  return 0;
}