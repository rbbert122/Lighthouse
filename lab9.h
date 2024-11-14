#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/lab5/lab_camera.h"

#define MAX_WAVES 64
#define MAX_LIGHTS 32

namespace m1 {
class Lab9 : public gfxc::SimpleScene {
public:
  struct ViewportArea {
    ViewportArea() : x(0), y(0), width(1), height(1) {}
    ViewportArea(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}
    int x;
    int y;
    int width;
    int height;
  };

  struct Wave {
    glm::vec2 direction;
    float amplitude;
    float frequency;
    float phase;
  };

  struct light_source {
    int type;
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 direction;
    float cut_off_angle;
  };

  struct Island {
    glm::vec3 position;
    glm::vec3 scale;
  };

  struct light_house_part {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color = glm::vec3(1, 1, 1);
  };

  struct LightHouse {
    light_house_part base;
    light_house_part light;
    light_house_part top;
  };

  struct Moon {
    glm::vec3 position;
    glm::vec3 scale;
  };

  struct boat_part {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color = glm::vec3(1, 1, 1);
  };

  struct Boat {
    boat_part base;
    boat_part veil;
    boat_part sail;
    int direction;
  };

  Lab9();
  ~Lab9() override;

  void Init() override;

private:
  void FrameStart() override;
  void Update(float deltaTimeSeconds) override;
  void FrameEnd() override;

  void RenderSimpleMesh(Mesh *mesh, Shader *shader,
                        const glm::mat4 &modelMatrix,
                        Texture2D *texture = nullptr, bool isWater = false,
                        bool isLightSource = false, bool isLightHouse = false);
  void RenderSliderMesh(Mesh *mesh, Shader *shader,
                        const glm::mat4 &modelMatrix, glm::vec3 color);

  Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices,
                   const std::vector<unsigned int> &indices);

  void OnInputUpdate(float deltaTime, int mods) override;
  void OnKeyPress(int key, int mods) override;
  void OnKeyRelease(int key, int mods) override;
  void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
  void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
  void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
  void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
  void OnWindowResize(int width, int height) override;

  std::unordered_map<std::string, Texture2D *> mapTextures;
  struct Wave waves[MAX_WAVES];
  std::vector<light_source> light_sources;
  struct Island island;
  struct LightHouse light_house;
  struct light_source LightHouseLight;
  struct light_source LighHouseRotatingLights[2];
  int rotating_index[2] = {0, 0};
  float light_house_rotation = 0;
  struct Moon moon;
  struct light_source moon_light;
  struct Boat boats[4];
  struct light_source boat_lights[4];
  float boat_rotation = 0;
  int boat_indices[4] = {0, 0, 0, 0};
  ViewportArea miniViewportArea;
  implemented::Camera *camera;
  glm::mat4 projectionMatrix;
};
} // namespace m1
