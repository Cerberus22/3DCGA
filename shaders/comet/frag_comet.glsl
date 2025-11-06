#version 410 core

out vec4 fragColor;

uniform vec3 emissiveColor;

void main()
{
    fragColor = vec4(emissiveColor, 1.0);
}