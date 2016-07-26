#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2DArray samplerColor;

layout (location = 0) in vec3 inUVCoordinates;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  outFragColor = texture(samplerColor, inUVCoordinates) * vec4(inColor, 1.0);  
}