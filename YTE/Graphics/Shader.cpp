#include "YTE/Graphics/Shader.hpp"

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