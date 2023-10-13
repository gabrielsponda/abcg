#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  GLuint m_program{};

  abcg::Timer m_timer;

  glm::vec2 m_center = {0.0f, 0.0f};

  int m_speed = 50;

  float m_dx = (rand() % 200 - 100) / 100.0f;
  float m_dy = (rand() % 200 - 100) / 100.0f;
  float m_magnitude = sqrt(m_dx * m_dx + m_dy * m_dy);

  float m_radius = 0.1f;

  bool m_allowTrail = 0;
  float m_trail = 10.0f;

  std::array<glm::vec2, 20> m_positions;

  std::array<glm::vec4, 20> m_colors{
      {{1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {1.00f, 1.00f, 1.00f, 1},    {1.00f, 1.00f, 1.00f, 1},
       {0.00f, 0.00f, 0.00f, 0.25}, {0.00f, 0.00f, 0.00f, 0.1},
       {0.00f, 0.00f, 0.00f, 0.25}, {0.00f, 0.00f, 0.00f, 0.1}}};

  void setupModel();
};

#endif