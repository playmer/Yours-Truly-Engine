#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 1) uniform sampler2D samplerColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUVCoordinates;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  //outFragColor = vec4(inColor, 1.0);
  //outFragColor = texture(samplerColor, inUVCoordinates) * vec4(inColor, 1.0);
  vec4 textureColor = texture(samplerColor, inUVCoordinates);
  
  vec4 colorFilledVec = vec4(step(0.0, inColor.x),
                             step(0.0, inColor.y),
                             step(0.0, inColor.z),
                             1.0);
  
  float colorFilled = ceil(max(max(max(0.0,  colorFilledVec.x), colorFilledVec.y), colorFilledVec.z));
  
  outFragColor = vec4(mix(textureColor.x, inColor.x, colorFilled),
                      mix(textureColor.y, inColor.y, colorFilled),
                      mix(textureColor.z, inColor.z, colorFilled),
                      mix(textureColor.w, 1.0, colorFilled));

  //outFragColor = vec4(1.0, 1.0, 1.0, 1.0);
}