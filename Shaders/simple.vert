#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 2) in vec2 inUVCoordinates;
layout (location = 3) in vec3 inNormal;


layout (location = 4) in vec3 inInstanceTranslation;
layout (location = 5) in vec3 inInstanceScale;
layout (location = 6) in vec3 inInstanceRotation;
layout (location = 7) in vec3 inInstanceColor;

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
  outColor = inInstanceColor;
  outUVCoordinates = inUVCoordinates;

  gl_Position = uniformBufferObject.mProjectionMatrix * 
                uniformBufferObject.mModelMatrix      * 
                //vec4(inPosition.xyz, 1.0);
                vec4(inPosition.xyz + inInstanceTranslation, 1.0);
}