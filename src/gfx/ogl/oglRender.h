#pragma once

#include <glad/gl.h>

class OGLRender : public Render {
public:
  OGLRender();
  void draw(const Scene &_scene);
};
