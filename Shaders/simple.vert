#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inUVCoordinates;
layout (location = 2) in vec3 inNormal;


layout (location = 3) in vec3 inInstanceTranslation;
layout (location = 4) in vec3 inInstanceScale;
layout (location = 5) in vec3 inInstanceRotation;
layout (location = 6) in vec3 inInstanceColor;
layout (location = 7) in uint inTextureId;
layout (location = 8) in mat4 inInstanceTransformation;

layout (binding = 0) uniform UBO 
{
  mat4 mProjectionMatrix;
  mat4 mModelMatrix;
  vec3 mViewPosition;
  float mLevelOfDetailBias;
} uniformBufferObject;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUVCoordinates;
layout (location = 2) out flat uint outTextureId;

out gl_PerVertex 
{
    vec4 gl_Position;
};



// From glm
mat4 translate(mat4 m, vec3 v)
{
  mat4 Result = m;
  Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
  return Result;
}

// From glm
mat4 scale(mat4 m, vec3 v)
{
  mat4 Result;
  Result[0] = m[0] * v[0];
  Result[1] = m[1] * v[1];
  Result[2] = m[2] * v[2];
  Result[3] = m[3];
  return Result;
}

// From glm
mat4 rotate(mat4 m, float angle, vec3 v)
{
  float a = angle;
  float c = cos(a);
  float s = sin(a);

  vec3 axis = normalize(v);
  vec3 temp = (1.0 - c) * axis;

  mat4 Rotate;
  Rotate[0][0] = c + temp[0] * axis[0];
  Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
  Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

  Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
  Rotate[1][1] = c + temp[1] * axis[1];
  Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

  Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
  Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
  Rotate[2][2] = c + temp[2] * axis[2];

  mat4 Result;
  Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
  Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
  Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
  Result[3] = m[3];
  return Result;
}

// From glm
float radians(float degrees)
{
  return degrees * 0.01745329251994329576923690768489;
}


void main() 
{
  outTextureId = inTextureId;
  outColor = inInstanceColor;
  outUVCoordinates = inUVCoordinates;

  mat4 temp = mat4(1.0);
  
  mat4 objectToWorld = translate(temp, inInstanceTranslation);
  
  objectToWorld = rotate(objectToWorld, inInstanceRotation.x, vec3(1.0, 0.0, 0.0));
  objectToWorld = rotate(objectToWorld, inInstanceRotation.y, vec3(0.0, 1.0, 0.0));
  objectToWorld = rotate(objectToWorld, inInstanceRotation.z, vec3(0.0, 0.0, 1.0));

  objectToWorld = scale(objectToWorld, inInstanceScale);
  

  gl_Position = uniformBufferObject.mProjectionMatrix * 
                uniformBufferObject.mModelMatrix      * 
                objectToWorld *
                inPosition;
                
                
  //gl_Position = uniformBufferObject.mProjectionMatrix * 
  //              uniformBufferObject.mModelMatrix      * 
  //              inInstanceTransformation *
  //              inPosition;
}