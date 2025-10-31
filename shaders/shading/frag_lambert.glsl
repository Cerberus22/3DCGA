#version 410

uniform vec3 kd;

uniform vec3 lightPosition = vec3(0);
uniform vec3 lightColor = vec3(1);

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 N = normalize(fragNormal);

    vec3 diff = max(0, dot(N, L)) * lightColor * kd;

    fragColor = vec4(diff, 1);
}
