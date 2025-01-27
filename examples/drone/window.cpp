#include "window.hpp"

#include <SDL_keycode.h>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_rollSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_rollSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_LEFT)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_UP)
      m_tiltSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN)
      m_tiltSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_SPACE)
      m_pedestalSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_LCTRL)
      m_pedestalSpeed = -1.0f;

    if (event.key.keysym.sym == SDLK_0)
      loadShader("original.vert", "original.frag");
    if (event.key.keysym.sym == SDLK_1)
      loadShader("rows.vert", "rows.frag");
    if (event.key.keysym.sym == SDLK_2)
      loadShader("columns.vert", "columns.frag");
    if (event.key.keysym.sym == SDLK_3)
      loadShader("sheet.vert", "sheet.frag");
    if (event.key.keysym.sym == SDLK_4)
      loadShader("xRotation.vert", "xRotation.frag");
    if (event.key.keysym.sym == SDLK_5)
      loadShader("zRotation.vert", "zRotation.frag");
    if (event.key.keysym.sym == SDLK_6)
      loadShader("wave.vert", "wave.frag");
    if (event.key.keysym.sym == SDLK_7)
      loadShader("cubeWave.vert", "cubeWave.frag");
    if (event.key.keysym.sym == SDLK_t) {
      loadShader("original.vert", "original.frag");
      m_togleSize = 1;
    }
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_w) && m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_s) && m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_a) && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_d) && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_rollSpeed < 0)
      m_rollSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_rollSpeed > 0)
      m_rollSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT) && m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT) && m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_UP) && m_tiltSpeed > 0)
      m_tiltSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN) && m_tiltSpeed < 0)
      m_tiltSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_SPACE) && m_pedestalSpeed > 0)
      m_pedestalSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LCTRL) && m_pedestalSpeed < 0)
      m_pedestalSpeed = 0.0f;
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "original.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "original.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_program, m_cubeSize);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
  m_timeLocation = abcg::glGetUniformLocation(m_program, "time");

  // Load model
  loadModelFromFile(assetsPath + "sphere.obj");

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void Window::onPaint() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Update the time uniform
  glUniform1f(m_timeLocation, m_time);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Set model
  glm::mat4 model{1.0f};

  // Draw red sphere
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-2.0f, 1.5f, 0.0f));
  model = glm::scale(model, glm::vec3(0.01f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Draw green sphere
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
  model = glm::scale(model, glm::vec3(0.01f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  // Draw blue sphere
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(2.0f, 1.5f, 0.0f));
  model = glm::scale(model, glm::vec3(0.01f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);

  if (m_togleSize == 1) {
    m_ground.destroy();
    m_program =
        abcg::createOpenGLProgram({{.source = assetsPath + "original.vert",
                                    .stage = abcg::ShaderStage::Vertex},
                                   {.source = assetsPath + "original.frag",
                                    .stage = abcg::ShaderStage::Fragment}});
    if (m_cubeSize == 0.5)
      m_cubeSize = 0.15;
    else
      m_cubeSize = 0.5;
    m_ground.create(m_program, m_cubeSize);
    m_togleSize = 0;
  } else {
    // Draw ground
    m_ground.paint();
  }
  abcg::glUseProgram(0);
}

void Window::onPaintUI() { abcg::OpenGLWindow::onPaintUI(); }

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.roll(m_rollSpeed * deltaTime);
  m_camera.tilt(m_tiltSpeed * deltaTime);
  m_camera.pedestal(m_pedestalSpeed * deltaTime);

  m_time += deltaTime;
}

void Window::loadShader(std::string vertexShaderPath,
                        std::string fragmentShaderPath) {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  // Destroy the old program
  abcg::glDeleteProgram(m_program);

  // Create a new program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + vertexShaderPath,
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + fragmentShaderPath,
                                  .stage = abcg::ShaderStage::Fragment}});

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");
  m_timeLocation = abcg::glGetUniformLocation(m_program, "time");
}