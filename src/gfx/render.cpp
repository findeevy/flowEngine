#include "render.h"
#include "ogl/oglRender.h"
// #include "vlk/vulkanRender.h"

std::unique_ptr<Render> Render::create(RendererBackend backend) {
  switch (backend) {
  case RendererBackend::OpenGL:
    return std::make_unique<OGLRender>();
  default:
    throw std::runtime_error("Unknown backend");
  }
}
