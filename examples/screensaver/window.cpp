#include "window.hpp"

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() { 
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es
  
    precision mediump float;

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  abcg::glEnable(GL_BLEND);
  abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_positions[16] = {-1.0f, -1.0f};
  m_positions[17] = {-1.0f, 1.0f};
  m_positions[18] = {1.0f, 1.0f};
  m_positions[19] = {1.0f, -1.0f};

  m_dx = m_dx / m_magnitude * (1.0 / m_speed);
  m_dy = m_dy / m_magnitude * (1.0 / m_speed);
}

void Window::onPaint() {
  if (m_timer.elapsed() < 1.0 / m_speed)
    return;
  m_timer.restart();

  setupModel();

  auto minSize{std::min(m_viewportSize.x, m_viewportSize.y)};
  auto x{(m_viewportSize.x - minSize) / 2};
  auto y{(m_viewportSize.y - minSize) / 2};
  abcg::glViewport(x, y, minSize, minSize);

  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);

  if (!m_allowTrail) {
    abcg::glClear(GL_COLOR_BUFFER_BIT);
  }
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, 16);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const widgetSize{ImVec2(250, 120)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5,
                                   m_viewportSize.y - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto windowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);

    ImGui::PushItemWidth(185);
    ImGui::SliderFloat("Radius", &m_radius, 0.01, 0.5, "%.2f");
    ImGui::SliderInt("Speed", &m_speed, 1, 100);
    ImGui::Checkbox("Allow trail", &m_allowTrail);
    if (m_allowTrail) {
      ImGui::SliderFloat("Trail", &m_trail, 10.0f, 0.0f, "%.1f");
    }
    ImGui::PopItemWidth();

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::setupModel() {
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  float angle = 0.0f;
  float angleIncrement = 2.0f * glm::pi<float>() / 16;

  for (int i = 0; i < 16; ++i) {
    float x = m_center.x + m_radius * glm::cos(angle);
    float y = m_center.y + m_radius * glm::sin(angle);
    m_positions[i] = {x, y};
    angle += angleIncrement;
  }

  m_colors[16] = {0.0f, 0.0f, 0.0f, m_trail / m_speed};
  m_colors[17] = {0.0f, 0.0f, 0.0f, m_trail / m_speed};
  m_colors[18] = {0.0f, 0.0f, 0.0f, m_trail / m_speed};
  m_colors[19] = {0.0f, 0.0f, 0.0f, m_trail / m_speed};

  m_center.x += m_dx;
  m_center.y += m_dy;

  if (m_center.x + m_radius > 1.0f || m_center.x - m_radius < -1.0f) {
    m_dx = -m_dx;
  }

  if (m_center.y + m_radius > 1.0f || m_center.y - m_radius < -1.0f) {
    m_dy = -m_dy;
  }

  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  abcg::glGenVertexArrays(1, &m_VAO);

  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindVertexArray(0);
}