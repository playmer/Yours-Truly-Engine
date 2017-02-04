#pragma once

class Renderer
{
public:
  virtual ~Renderer() {};
  virtual void AddTexture() {};
  virtual void AddMesh() {};
  virtual void AddInstance() {};
  virtual void AddModel() {};
  virtual void AddShader() {};
  virtual void AddMaterial() {};
};