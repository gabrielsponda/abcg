#version 300 es

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time; // Add time uniform

out vec4 fragColor;

void main() {
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);

  // Add sinusoidal movement to y position
  posEyeSpace.y += sin(0.5 * posEyeSpace.x + time) * 0.1;

fragColor = color;

  gl_Position = projMatrix * posEyeSpace;
}