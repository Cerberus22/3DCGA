#version 410

uniform float ambientCoeff;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
// uniform float transparency;

uniform vec3 cameraPosition;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform sampler2D normalMap;
uniform float offsetStrength;
uniform bool hasNormalMap;
uniform bool hasSunTexture;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 N = normalize(fragNormal);
    
    vec3 texValue = texture(normalMap, fragTexCoord).xyz;

    if (hasNormalMap) {
        N += offsetStrength * (texValue * 2 - 1);
        N = normalize(N);
    }
    
    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(cameraPosition - fragPosition);


    vec3 diff = vec3(0);
    vec3 spec = vec3(0);
    if (dot(N, L) > 0) {
        diff = dot(N, L) * lightColor * kd;
        spec = pow(
            clamp(dot(V, R), 0, 1), 
            shininess
        ) * lightColor * ks;
    }

    if (hasSunTexture) {
        vec3 darkSunColor = vec3(1, 0.35, 0.0);
        vec3 lightSunColor = vec3(1, 0.8, 0.0);
        vec3 outColor = texValue * lightSunColor + (1 - texValue) * darkSunColor;
        fragColor = vec4(ambientCoeff * outColor, 1.0);
    } else {
        fragColor = vec4(min(ambientCoeff * kd + diff, 1) + spec, 1);
    }
}
