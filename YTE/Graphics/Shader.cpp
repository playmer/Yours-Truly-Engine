#include <iostream>

#include "YTE/Graphics/Shader.hpp"
#include "Shader.hpp"

namespace YTE
{
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

  void DeleteShader(gl::GLuint aShader)
  {
    if (aShader != 0)
    {
      gl::glDeleteShader(aShader);
    }
  }

  OpenGLHandle CreateShader(const char * aSource, gl::GLenum aType)
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
  }

  OpenGLHandle CreateShaderProgram(std::vector<OpenGLHandle>& aShaders)
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

  OpenGLHandle YTE::GraphicsInitialize()
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
}