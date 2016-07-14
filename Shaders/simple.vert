#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;

layout (binding = 0) uniform UBO 
{
  mat4 mProjectionMatrix;
  mat4 mModelMatrix;
  mat4 mViewMatrix;
} uniformBufferObject;

layout (location = 0) out vec4 outColor;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  outColor = inColor;
  gl_Position = uniformBufferObject.mProjectionMatrix * 
                uniformBufferObject.mModelMatrix      * 
                uniformBufferObject.mViewMatrix       * 
                inPosition;
}