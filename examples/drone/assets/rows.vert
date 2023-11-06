#version 300 es

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time; // Add time uniform

out vec4 fragColor;

mat4 movingTiles(mat4 _modelMatrix, float _speed){
    float phase = mod(time * _speed, 1.0);
    vec3 movement = vec3(0.0);

    if (mod(floor(_modelMatrix[3].z), 2.0) == 0.0) {
        movement.x = phase * 2.0;
    } else {
        movement.x = -phase * 2.0;
    }

    return _modelMatrix * mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(movement, 1.0)
    );
}

void main() {
  vec4 pos = vec4(inPosition, 1);

  mat4 modelMatrix = movingTiles(modelMatrix, 0.5);
  vec4 posEyeSpace = viewMatrix * modelMatrix * pos;

  float i = 1.0 - (-posEyeSpace.z / 5.0);
  fragColor = vec4(i, i, i, 1) * color;

  gl_Position = projMatrix * posEyeSpace;
}