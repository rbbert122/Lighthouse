#include "lab_m1/lab9/lab9.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab9::Lab9() {}

Lab9::~Lab9() {}

float random(float min, float max) {
  return min + static_cast<float>(rand()) / (RAND_MAX / (max - min));
}

void Lab9::Init() {
  camera = new implemented::Camera();
  camera->Set(glm::vec3(0, 8, 20), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

  const string sourceTextureDir =
      PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9", "textures");

  // Load textures
  {
    auto texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "water.png").c_str(),
                    GL_REPEAT);
    mapTextures["water"] = texture;
  }

  {
    auto texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(),
                    GL_REPEAT);
    mapTextures["crate"] = texture;
  }

  {
    auto texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "moon.jpg").c_str(), GL_REPEAT);
    mapTextures["moon"] = texture;
  }

  {
    auto texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "ground.jpg").c_str(),
                    GL_REPEAT);
    mapTextures["ground"] = texture;
  }

  {
    auto texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "grey.jpg").c_str(), GL_REPEAT);
    mapTextures["grey"] = texture;
  }

  {
    auto mesh = new Mesh("plane50");
    mesh->LoadMesh(
        PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
        "plane50.obj");
    meshes[mesh->GetMeshID()] = mesh;
  }

  {
    auto mesh = new Mesh("sphere");
    mesh->LoadMesh(
        PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
        "sphere.obj");
    meshes[mesh->GetMeshID()] = mesh;
  }

  {
    auto mesh = new Mesh("box");
    mesh->LoadMesh(
        PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
        "box.obj");
    meshes[mesh->GetMeshID()] = mesh;
  }

  {
    auto mesh = new Mesh("cylinder");
    mesh->LoadMesh(
        PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
        "cylinder.obj");
    meshes[mesh->GetMeshID()] = mesh;
  }

  // Create a simple cube
  {
    vector<VertexFormat> vertices{
        VertexFormat(glm::vec3(-1, -1, 1), glm::vec3(0, 1, 1),
                     glm::vec3(0.2, 0.8, 0.2)),
        VertexFormat(glm::vec3(1, -1, 1), glm::vec3(1, 0, 1),
                     glm::vec3(0.9, 0.4, 0.2)),
        VertexFormat(glm::vec3(-1, 1, 1), glm::vec3(1, 0, 0),
                     glm::vec3(0.7, 0.7, 0.1)),
        VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0, 1, 0),
                     glm::vec3(0.7, 0.3, 0.7)),
        VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1),
                     glm::vec3(0.3, 0.5, 0.4)),
        VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1),
                     glm::vec3(0.5, 0.2, 0.9)),
        VertexFormat(glm::vec3(-1, 1, -1), glm::vec3(1, 1, 0),
                     glm::vec3(0.7, 0.0, 0.7)),
        VertexFormat(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1),
                     glm::vec3(0.1, 0.5, 0.8)),
    };

    vector<unsigned int> indices = {
        0, 1, 2, 1, 3, 2, 2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7,
        6, 7, 4, 7, 5, 4, 0, 4, 1, 1, 4, 5, 2, 6, 4, 0, 2, 4,
    };

    CreateMesh("cube", vertices, indices);
  }

  // Create a shader program for drawing face polygon with the color of the
  // normal
  {
    auto shader = new Shader("Tema");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9",
                                "shaders", "TemaVS.glsl"),
                      GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9",
                                "shaders", "TemaFS.glsl"),
                      GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
  }

  {
    auto shader = new Shader("Slider");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9",
                                "shaders", "VertexShader.glsl"),
                      GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab9",
                                "shaders", "FragmentShader.glsl"),
                      GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
  }

  for (auto &wave : waves) {
    const float speed = random(0.1f, 0.5f);
    wave.direction = glm::vec2(random(-1.0f, 1.0f), random(-1.0f, 1.0f));
    wave.amplitude = random(0.1f, 0.3f);
    wave.frequency = random(0.7f, 1.3f);
    wave.phase = speed * wave.frequency;
  }

  island.position = glm::vec3(0, 0, 0);
  island.scale = glm::vec3(7, 5, 7);

  light_house.base.position = glm::vec3(0, 0, 0);
  light_house.base.scale = glm::vec3(1.5f, 6, 1.5f);

  light_house.light.position = glm::vec3(0, 6, 0);
  light_house.light.scale = glm::vec3(1.5f, 1, 1.5f);
  light_house.light.color = glm::vec3(1, 1, 1);

  light_house.top.position = glm::vec3(0, 7, 0);
  light_house.top.scale = glm::vec3(1.5f, 2, 1.5f);

  LightHouseLight.type = 0;
  LightHouseLight.position = glm::vec3(0, 6, 0);
  LightHouseLight.color = light_house.light.color;
  LightHouseLight.direction = glm::vec3(0, 0, 0);
  LightHouseLight.cut_off_angle = 0;

  light_sources.push_back(LightHouseLight);

  for (int i = 0; i < 2; i++) {
    LighHouseRotatingLights[i].type = 1;
    LighHouseRotatingLights[i].position = glm::vec3(0, 6, 0);
    LighHouseRotatingLights[i].color = light_house.light.color;
    float direction = i == 0 ? 1 : -1;
    LighHouseRotatingLights[i].direction = glm::vec3(direction, 0, 0);
    LighHouseRotatingLights[i].cut_off_angle = 30;
    light_sources.push_back(LighHouseRotatingLights[i]);
    rotating_index[i] = light_sources.size() - 1;
  }

  moon.position = glm::vec3(60, 30, -120);
  moon.scale = glm::vec3(30);

  moon_light.type = 2;
  moon_light.position = moon.position;
  moon_light.color = glm::vec3(1, 1, 1);
  moon_light.direction = glm::vec3(0.5, -0.25, -1);
  moon_light.cut_off_angle = 0;

  light_sources.push_back(moon_light);

  for (int i = 0; i < 4; i++) {
    boats[i].base.position = glm::vec3(random(-25, 25), 1.5, random(-25, 25));
    boats[i].base.scale = glm::vec3(2, 0.5, 2);
    boats[i].veil.position = boats[i].base.position;
    boats[i].veil.scale = glm::vec3(0.2, 2, 0.2);
    boats[i].sail.position = boats[i].base.position + glm::vec3(2, 2, 0);
    boats[i].sail.scale = glm::vec3(2, 1, 0.01);
    boat_lights[i].type = 0;
    boat_lights[i].position = boats[i].base.position;
    boat_lights[i].color = glm::vec3(random(0, 1), random(0, 1), random(0, 1));
    boat_lights[i].direction = glm::vec3(0, 0, 0);
    boat_lights[i].cut_off_angle = 0;
    light_sources.push_back(boat_lights[i]);
    boat_indices[i] = light_sources.size() - 1;
    boats[i].direction = i % 2 == 0 ? -1 : 1;
  }

  glm::ivec2 resolution = window->GetResolution();
  miniViewportArea =
      ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

  projectionMatrix =
      glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Lab9::FrameStart() {
  // Clears the color buffer (using the previously set color) and depth buffer
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::ivec2 resolution = window->GetResolution();
  // Sets the screen area where to draw
  glViewport(0, 0, resolution.x, resolution.y);
}

void Lab9::Update(float deltaTimeSeconds) {
  glm::ivec2 resolution = window->GetResolution();
  glViewport(0, 0, resolution.x, resolution.y);

  projectionMatrix =
      glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

  auto modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, glm::vec3(0, -1, 0));
  modelMatrix = scale(modelMatrix, glm::vec3(2));
  RenderSimpleMesh(meshes["plane50"], shaders["Tema"], modelMatrix,
                   mapTextures["water"], true);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, island.position);
  modelMatrix = scale(modelMatrix, island.scale);
  RenderSimpleMesh(meshes["sphere"], shaders["Tema"], modelMatrix,
                   mapTextures["ground"]);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, light_house.base.position);
  modelMatrix = scale(modelMatrix, light_house.base.scale);
  RenderSimpleMesh(meshes["cylinder"], shaders["Tema"], modelMatrix,
                   mapTextures["grey"]);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, light_house.light.position);
  modelMatrix = scale(modelMatrix, light_house.light.scale);
  RenderSimpleMesh(meshes["cylinder"], shaders["Tema"], modelMatrix, nullptr,
                   false, true, true);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, light_house.top.position);
  modelMatrix = scale(modelMatrix, light_house.top.scale);
  RenderSimpleMesh(meshes["cylinder"], shaders["Tema"], modelMatrix,
                   mapTextures["grey"]);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, moon.position);
  modelMatrix = scale(modelMatrix, moon.scale);
  RenderSimpleMesh(meshes["sphere"], shaders["Tema"], modelMatrix,
                   mapTextures["moon"], false, true, false);

  light_house_rotation += deltaTimeSeconds;
  int index1 = rotating_index[0];
  int index2 = rotating_index[1];
  light_sources[index1].direction =
      glm::vec3(cos(light_house_rotation), 0, sin(light_house_rotation));
  light_sources[index2].direction =
      glm::vec3(-cos(light_house_rotation), 0, -sin(light_house_rotation));

  boat_rotation += deltaTimeSeconds;
  float boat_speed = 0.2f;
  for (int i = 0; i < 4; i++) {
    glm::vec3 position = boats[i].base.position;
    glm::vec3 island_pos = island.position;

    float x = position.x - island_pos.x;
    float z = position.z - island_pos.z;
    float angle = atan2(z, x);
    float radius = sqrt(x * x + z * z);
    float new_angle =
        angle + boats[i].direction * boat_speed * deltaTimeSeconds;
    float new_x = radius * cos(new_angle);
    float new_z = radius * sin(new_angle);
    boats[i].base.position =
        glm::vec3(new_x + island_pos.x, position.y, new_z + island_pos.z);
    boats[i].veil.position = boats[i].base.position;
    // the sail is facing the direction of the boat
    boats[i].sail.position =
        boats[i].base.position + glm::vec3(boats[i].direction * 2, 2, 0);
    modelMatrix = glm::mat4(1);
    modelMatrix = translate(modelMatrix, boats[i].base.position);
    modelMatrix = scale(modelMatrix, boats[i].base.scale);
    modelMatrix = rotate(modelMatrix, -angle, glm::vec3(0, 1.5f, 0));
    RenderSimpleMesh(meshes["box"], shaders["Tema"], modelMatrix,
                     mapTextures["crate"]);
    modelMatrix = glm::mat4(1);
    modelMatrix = translate(modelMatrix, boats[i].veil.position);
    modelMatrix = scale(modelMatrix, boats[i].veil.scale);
    RenderSimpleMesh(meshes["cylinder"], shaders["Tema"], modelMatrix,
                     mapTextures["grey"]);
    // rotate sail aroung the veil position so it faces the direction the boat
    // is moving
    modelMatrix = glm::mat4(1);
    modelMatrix = translate(modelMatrix, boats[i].base.position);
    modelMatrix = scale(modelMatrix, boats[i].sail.scale);
    modelMatrix = rotate(modelMatrix, -angle, glm::vec3(0, 1.5f, 0));
    RenderSimpleMesh(meshes["box"], shaders["Tema"], modelMatrix,
                     mapTextures["grey"]);

    int index = boat_indices[i];
    light_sources[index].position =
        boats[i].base.position + glm::vec3(0, 0.5, 0);
  }

  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width,
             miniViewportArea.height);

  projectionMatrix = glm::ortho(-10.f, 10.f, -10.f, 10.f, -200.0f, 200.0f);

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, glm::vec3(-12, 8, 10));
  // calculate scale based on light_house.light.color
  // when color.r is 0 scaleX = 0, when color.r is 1 scaleX = 22
  float scaleX = light_house.light.color.r * 22;
  modelMatrix = scale(modelMatrix, glm::vec3(scaleX, 0, 5));
  RenderSliderMesh(meshes["cube"], shaders["Slider"], modelMatrix,
                   glm::vec3(1, 0, 0));

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, glm::vec3(-12, 4, 10));
  scaleX = light_house.light.color.g * 22;
  modelMatrix = scale(modelMatrix, glm::vec3(scaleX, 0, 5));
  RenderSliderMesh(meshes["cube"], shaders["Slider"], modelMatrix,
                   glm::vec3(0, 1, 0));

  modelMatrix = glm::mat4(1);
  modelMatrix = translate(modelMatrix, glm::vec3(-12, 0, 10));
  scaleX = light_house.light.color.b * 22;
  modelMatrix = scale(modelMatrix, glm::vec3(scaleX, 0, 5));
  RenderSliderMesh(meshes["cube"], shaders["Slider"], modelMatrix,
                   glm::vec3(0, 0, 1));
}

void Lab9::FrameEnd() {}

Mesh *Lab9::CreateMesh(const char *name,
                       const std::vector<VertexFormat> &vertices,
                       const std::vector<unsigned int> &indices) {
  unsigned int VAO = 0;
  // Create the VAO and bind it
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Create the VBO and bind it
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Send vertices data into the VBO buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
               &vertices[0], GL_STATIC_DRAW);

  // Create the IBO and bind it
  unsigned int IBO;
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

  // Send indices data into the IBO buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
               &indices[0], GL_STATIC_DRAW);

  // ========================================================================
  // This section demonstrates how the GPU vertex shader program
  // receives data.

  // TODO(student): If you look closely in the `Init()` and `Update()`
  // functions, you will see that we have three objects which we load
  // and use in three different ways:
  // - LoadMesh   + LabShader (this lab's shader)
  // - CreateMesh + VertexNormal (this shader is already implemented)
  // - CreateMesh + LabShader (this lab's shader)
  // To get an idea about how they're different from one another, do the
  // following experiments. What happens if you switch the color pipe and
  // normal pipe in this function (but not in the shader)? Now, what happens
  // if you do the same thing in the shader (but not in this function)?
  // Finally, what happens if you do the same thing in both places? Why?

  // Set vertex position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                        nullptr);

  // Set vertex normal attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                        (void *)(sizeof(glm::vec3)));

  // Set texture coordinate attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                        (void *)(2 * sizeof(glm::vec3)));

  // Set vertex color attribute
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
                        (void *)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
  // ========================================================================

  // Unbind the VAO
  glBindVertexArray(0);

  // Check for OpenGL errors
  CheckOpenGLError();

  // Mesh information is saved into a Mesh object
  meshes[name] = new Mesh(name);
  meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
  meshes[name]->vertices = vertices;
  meshes[name]->indices = indices;
  return meshes[name];
}

void Lab9::RenderSliderMesh(Mesh *mesh, Shader *shader,
                            const glm::mat4 &modelMatrix, glm::vec3 color) {
  if (!mesh || !shader || !shader->GetProgramID())
    return;

  // Render an object using the specified shader and the specified position
  glUseProgram(shader->program);
  glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                     value_ptr(camera->GetViewMatrix()));
  glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                     value_ptr(projectionMatrix));
  glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                     value_ptr(modelMatrix));

  // Bind model matrix
  GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
  glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, value_ptr(modelMatrix));

  // Bind view matrix
  int loc_view_matrix = glGetUniformLocation(shader->program, "View");
  glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE,
                     value_ptr(camera->GetViewMatrix()));

  // Bind projection matrix
  int loc_projection_matrix =
      glGetUniformLocation(shader->program, "Projection");
  glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE,
                     value_ptr(projectionMatrix));

  int color_loc = glGetUniformLocation(shader->program, "color");
  glUniform3f(color_loc, color.x, color.y, color.z);

  // Draw the object
  glBindVertexArray(mesh->GetBuffers()->m_VAO);
  glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()),
                 GL_UNSIGNED_INT, nullptr);
}

void Lab9::RenderSimpleMesh(Mesh *mesh, Shader *shader,
                            const glm::mat4 &modelMatrix, Texture2D *texture,
                            bool isWater, bool isLightSource,
                            bool isLightHouse) {
  if (!mesh || !shader || !shader->GetProgramID())
    return;

  // Render an object using the specified shader and the specified position
  glUseProgram(shader->program);
  glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
                     value_ptr(camera->GetViewMatrix()));
  glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE,
                     value_ptr(projectionMatrix));
  glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE,
                     value_ptr(modelMatrix));

  // Bind model matrix
  GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
  glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, value_ptr(modelMatrix));

  // Bind view matrix
  int loc_view_matrix = glGetUniformLocation(shader->program, "View");
  glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE,
                     value_ptr(camera->GetViewMatrix()));

  // Bind projection matrix
  int loc_projection_matrix =
      glGetUniformLocation(shader->program, "Projection");
  glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE,
                     value_ptr(projectionMatrix));

  // TODO(student): Set any other shader uniforms that you need
  int time_loc = glGetUniformLocation(shader->program, "time");
  glUniform1f(time_loc, Engine::GetElapsedTime());

  glm::vec3 eyePosition = camera->GetWorldPosition();
  int eye_position = glGetUniformLocation(shader->program, "eye_position");
  glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

  int is_water_loc = glGetUniformLocation(shader->program, "is_water");
  glUniform1i(is_water_loc, isWater);

  int num_lights_loc = glGetUniformLocation(shader->program, "num_lights");
  glUniform1i(num_lights_loc, light_sources.size());

  int is_lighthouse_loc =
      glGetUniformLocation(shader->program, "is_lighthouse");
  glUniform1i(is_lighthouse_loc, isLightHouse);

  int light_house_color_loc =
      glGetUniformLocation(shader->program, "light_house_color");
  glUniform3f(light_house_color_loc, light_house.light.color.x,
              light_house.light.color.y, light_house.light.color.z);

  // Send the wave struct to the shader
  for (int i = 0; i < MAX_WAVES; i++) {
    std::string name =
        std::string("waves[") + std::to_string(i) + std::string("].amplitude");
    GLint location = glGetUniformLocation(shader->program, name.c_str());
    glUniform1f(location, waves[i].amplitude);
    name =
        std::string("waves[") + std::to_string(i) + std::string("].frequency");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform1f(location, waves[i].frequency);
    name = std::string("waves[") + std::to_string(i) + std::string("].phase");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform1f(location, waves[i].phase);
    name =
        std::string("waves[") + std::to_string(i) + std::string("].direction");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform2f(location, waves[i].direction.x, waves[i].direction.y);
  }

  for (int i = 0; i < light_sources.size(); i++) {
    std::string name =
        std::string("lights[") + std::to_string(i) + std::string("].type");
    GLint location = glGetUniformLocation(shader->program, name.c_str());
    glUniform1i(location, light_sources[i].type);
    name =
        std::string("lights[") + std::to_string(i) + std::string("].position");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform3f(location, light_sources[i].position.x,
                light_sources[i].position.y, light_sources[i].position.z);
    name = std::string("lights[") + std::to_string(i) + std::string("].color");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform3f(location, light_sources[i].color.x, light_sources[i].color.y,
                light_sources[i].color.z);
    name =
        std::string("lights[") + std::to_string(i) + std::string("].direction");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform3f(location, light_sources[i].direction.x,
                light_sources[i].direction.y, light_sources[i].direction.z);
    name = std::string("lights[") + std::to_string(i) +
           std::string("].cut_off_angle");
    location = glGetUniformLocation(shader->program, name.c_str());
    glUniform1f(location, light_sources[i].cut_off_angle);
  }

  if (isLightSource) {
    int ke_loc = glGetUniformLocation(shader->program, "ke");
    glUniform1f(ke_loc, 0.8f);
  } else {
    int ke_loc = glGetUniformLocation(shader->program, "ke");
    glUniform1f(ke_loc, 0.0f);
  }

  if (texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
  }

  // Draw the object
  glBindVertexArray(mesh->GetBuffers()->m_VAO);
  glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()),
                 GL_UNSIGNED_INT, nullptr);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Lab9::OnInputUpdate(float deltaTime, int mods) {
  if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
    auto up = glm::vec3(0, 1, 0);
    glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
    glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
    forward = normalize(glm::vec3(forward.x, 0, forward.z));
  }

  if (window->KeyHold(GLFW_KEY_1)) {
    light_house.light.color.r += 0.5f * deltaTime;
    light_house.light.color.r =
        glm::clamp(light_house.light.color.r, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }
  if (window->KeyHold(GLFW_KEY_2)) {
    light_house.light.color.r -= 0.5f * deltaTime;
    light_house.light.color.r =
        glm::clamp(light_house.light.color.r, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }
  if (window->KeyHold(GLFW_KEY_3)) {
    light_house.light.color.g += 0.5f * deltaTime;
    light_house.light.color.g =
        glm::clamp(light_house.light.color.g, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }
  if (window->KeyHold(GLFW_KEY_4)) {
    light_house.light.color.g -= 0.5f * deltaTime;
    light_house.light.color.g =
        glm::clamp(light_house.light.color.g, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }
  if (window->KeyHold(GLFW_KEY_5)) {
    light_house.light.color.b += 0.5f * deltaTime;
    light_house.light.color.b =
        glm::clamp(light_house.light.color.b, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }
  if (window->KeyHold(GLFW_KEY_6)) {
    light_house.light.color.b -= 0.5f * deltaTime;
    light_house.light.color.b =
        glm::clamp(light_house.light.color.b, 0.0f, 1.0f);
    light_sources[0].color = light_house.light.color;
    light_sources[rotating_index[0]].color = light_house.light.color;
    light_sources[rotating_index[1]].color = light_house.light.color;
  }

  if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
    float cameraSpeed = 2.0f;

    if (window->KeyHold(GLFW_KEY_W)) {
      // TODO(student): Translate the camera forward
      camera->TranslateForward(deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_A)) {
      // TODO(student): Translate the camera to the left
      camera->TranslateRight(-deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_S)) {
      // TODO(student): Translate the camera backward
      camera->TranslateForward(-deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_D)) {
      // TODO(student): Translate the camera to the right
      camera->TranslateRight(deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
      // TODO(student): Translate the camera downward
      camera->TranslateUpward(-deltaTime * cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
      // TODO(student): Translate the camera upward
      camera->TranslateUpward(deltaTime * cameraSpeed);
    }
  }
}

void Lab9::OnKeyPress(int key, int mods) {
  // Add key press event
}

void Lab9::OnKeyRelease(int key, int mods) {
  // Add key release event
}

void Lab9::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
  // Add mouse move event
  if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (window->GetSpecialKeyState() == 0) {
      // TODO(student): Rotate the camera in first-person mode around
      // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
      // variables for setting up the rotation speed.
      camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
      camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
    }
  }
}

void Lab9::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
  // Add mouse button press event
}

void Lab9::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
  // Add mouse button release event
}

void Lab9::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Lab9::OnWindowResize(int width, int height) {}
