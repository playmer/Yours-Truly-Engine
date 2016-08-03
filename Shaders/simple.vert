#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUVCoordinates;
layout (location = 3) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
  mat4 mProjectionMatrix;
  mat4 mModelMatrix;
  vec4 mViewPosition;
  float mLevelOfDetailBias;
} uniformBufferObject;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUVCoordinates;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  outColor = inColor;
  outUVCoordinates = inUVCoordinates;

  gl_Position = uniformBufferObject.mProjectionMatrix * 
                uniformBufferObject.mModelMatrix      * 
                inPosition;
}