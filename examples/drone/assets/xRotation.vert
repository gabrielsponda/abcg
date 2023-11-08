#version 300 es

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time; // Add time uniform

out vec4 fragColor;

void main() {
  // Calculate rotation angle based on time
  float angle = time;

// Create rotation matrix for x axis
mat4 rotation = mat4(
  1, 0, 0, 0,
  0, cos(angle), -sin(angle), 0,
  0, sin(angle), cos(angle), 0,
  0, 0, 0, 1
);

  // Apply rotation
  vec4 rotatedPosition = rotation * vec4(inPosition, 1);

  vec4 posEyeSpace = viewMatrix * modelMatrix * rotatedPosition;

fragColor = color;

  gl_Position = projMatrix * posEyeSpace;
}