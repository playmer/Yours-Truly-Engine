#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D samplerColor;

layout (location = 0) in vec2  inUVCoordinates;
layout (location = 1) in float inLevelOfDetailBias;
layout (location = 2) in vec3  inNormal;
layout (location = 3) in vec3  inViewVector;
layout (location = 4) in vec3  inLightVector;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  outFragColor = texture(samplerColor, inUVCoordinates); 
}