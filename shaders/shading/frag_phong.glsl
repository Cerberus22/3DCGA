#version 410

uniform vec3 ks;
uniform float shininess;
// uniform float transparency;

uniform vec3 cameraPosition;

/*uniform */vec3 lightPosition = vec3(0);
/*uniform */vec3 lightColor = vec3(1);

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 N = normalize(fragNormal);
    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(cameraPosition - fragPosition);

    vec3 spec = vec3(0);
    if (dot(N, L) > 0) {
        spec = pow(
            clamp(dot(V, R), 0, 1), 
            shininess
        ) * lightColor * ks;
    }
    fragColor = vec4(spec, 1);
}
