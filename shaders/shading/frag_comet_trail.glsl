#version 330 core
in float fragAlpha;
out vec4 outColor;

void main() {
    vec3 baseColor = vec3(1.0, 0.8, 0.6); // comet color
    outColor = vec4(baseColor, fragAlpha);
}