#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUVCoordinates;
layout (location = 1) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
  mat4 mProjectionMatrix;
  mat4 mModelMatrix;
  vec4 mViewPosition;
  float mLevelOfDetailBias;
} uniformBufferObject;

layout (location = 0) out vec2 outUVCoordinates;
layout (location = 1) out float outLevelOfDetailBias;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outViewVector;
layout (location = 4) out vec3 outLightVector;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
  outUVCoordinates = inUVCoordinates;
  outLevelOfDetailBias = uniformBufferObject.mLevelOfDetailBias;

  vec3 worldPosition = vec3 (uniformBufferObject.mModelMatrix * inPosition);

  gl_Position = uniformBufferObject.mProjectionMatrix * 
                uniformBufferObject.mModelMatrix      * 
                inPosition;

  vec4 position = uniformBufferObject.mModelMatrix * inPosition;
  outNormal = mat3(inverse(transpose(uniformBufferObject.mModelMatrix))) * inNormal;
  vec3 lightPosition = vec3(0.0);
  vec3 lPos = mat3(uniformBufferObject.mModelMatrix) * lightPosition.xyz;
  outLightVector = lPos - position.xyz;
  outViewVector = uniformBufferObject.mViewPosition.xyz - position.xyz;
}