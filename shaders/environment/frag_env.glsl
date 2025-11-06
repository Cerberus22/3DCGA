#version 410

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform sampler2D envMap;

uniform vec3 cameraPosition;

layout(location = 0) out vec4 fragColor;

float PI = 3.14159;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPosition - fragPosition);
    vec3 R = normalize(reflect(-V, N));

    float r = length(R);
    float theta = acos(R.y / r) / PI;
    float phi   = atan(R.z, R.x) / (2.0 * PI);
    
    vec3 color = texture(envMap, vec2(phi, theta)).rgb;

    fragColor = vec4(color, 1.0);
}