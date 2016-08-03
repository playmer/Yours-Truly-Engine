#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Vertex attributes
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUVCoordinates;
layout (location = 3) in vec3 inNormal;

// Instanced attributes
layout (location = 4) in vec2 instancePosition;
layout (location = 5) in int instanceTexIndex;

layout (binding = 0) uniform UBO 
{
  mat4 mProjection;
  mat4 mView;
  float mLevelOfDetailBias;
} uniformBufferObject;

layout (location = 0) out vec3 outUVCoordinates;
//layout (location = 0) out vec2 outUVCoordinates;
layout (location = 1) out vec3 outColor;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  outUVCoordinates = vec3(inUVCoordinates, instancePosition);
  //outUVCoordinates = inUVCoordinates;

  outColor = inColor;

  vec4 position = vec4(instancePosition, 0.0f, 0.0f) + inPosition;
  //vec4 position = inPosition;

  gl_Position = uniformBufferObject.mProjection * 
                uniformBufferObject.mView      * 
                position;
}