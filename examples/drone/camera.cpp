#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

void Camera::dolly(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};

  // Move eye and center forward (speed > 0) or backward (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};
  // Compute vector to the left
  auto const left{glm::cross(m_up, forward)};

  // Move eye and center to the left (speed < 0) or to the right (speed > 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

void Camera::pan(float speed) {
  glm::mat4 transform{1.0f};

  // Rotate camera around its local y axis
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}

void Camera::roll(float speed) {
  // Compute forward vector (view direction)
  auto const forward{glm::normalize(m_at - m_eye)};

  // Rotate camera around its local z axis
  glm::mat4 transform{1.0f};
  transform = glm::rotate(transform, speed, forward);

  m_up = transform * glm::vec4(m_up, 1.0f);

  computeViewMatrix();
}

void Camera::tilt(float speed) {
  // Compute right vector
  auto const right{glm::normalize(glm::cross(m_up, m_at - m_eye))};

  // Rotate camera around its local x axis
  glm::mat4 transform{1.0f};
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, speed, right);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  transform = glm::mat4(1.0f);
  transform = glm::rotate(transform, speed, right);

  m_up = transform * glm::vec4(m_up, 1.0f);

  computeViewMatrix();
}

void Camera::pedestal(float speed) {
  // Move eye and center up (speed > 0) or down (speed < 0)
  m_eye += m_up * speed;
  m_at += m_up * speed;

  computeViewMatrix();
}