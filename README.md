ENGINE STILL IN VERY EARLY DEV!

- C++ and OpenGL
- PBR
- Editor and Player Model
- Easy Mapping Tools


layout logic:

if it's loaded from disk automatically (texture, mesh, glsl shader, etc) put it into gfx, if it is generated via engine command put it in scene (material, gameObject).

!(FlowEngine.png)


src/
  gfx/ (generic graphics structs, etc)
    ogl/ (opengl specific code)
    vlk/ (vulkan specific code)
  gui/ (imgui code)
  scene/ (game engine scene objects/logic)
  net/ (networking...)
  utils/ (generic utils used across all parts of engine)
  core/ (front facing app logic, window interaction, etc)
  events/ (engine wide event queue and processing, the switchboard of the engine)
