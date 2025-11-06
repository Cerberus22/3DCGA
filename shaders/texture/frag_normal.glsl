#version 410

uniform float ambientCoeff;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
// uniform float transparency;

uniform vec3 cameraPosition;

uniform vec3 light1Position;
uniform vec3 light1Color;

uniform vec3 light2Position;
uniform vec3 light2Color;

uniform sampler2D normalMap;
uniform bool useNormalMap;

uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;

uniform mat4 lightMVP1;
uniform mat4 lightMVP2;

uniform mat4 modelMatrix;

uniform bool useShadows;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;


layout(location = 0) out vec4 fragColor;

vec3 compute(vec3 lightPosition, vec3 lightColor, int i) {
    float mapDepth;
    float viewDepth;
    if (i == 0) {
        vec4 lPos = (lightMVP1 * inverse(modelMatrix) * vec4(fragPosition, 1));
        vec3 projPos = lPos.xyz / lPos.w;
        projPos = projPos * 0.5 + 0.5;

        viewDepth = projPos.z;
        mapDepth = texture(shadowMap1, projPos.xy).x;
    } else {
        vec4 lPos = (lightMVP2 * inverse(modelMatrix) * vec4(fragPosition, 1));
        vec3 projPos = lPos.xyz / lPos.w;
        projPos = projPos * 0.5 + 0.5;

        viewDepth = projPos.z;
        mapDepth = texture(shadowMap2, projPos.xy).x;
    }


    if (viewDepth - 0.00005 > mapDepth && useShadows) {
        return vec3(0);
    }

    vec3 L = normalize(lightPosition - fragPosition);
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(cross(N, T));
        
    mat3 TBN = mat3(T, B, N);

    vec4 texValue = texture(normalMap, fragTexCoord);
    vec3 normalValue = ((texValue - 0.5) * 2).xyz;
        
    if (useNormalMap) {
        N = normalize(TBN * normalValue);
    }

    vec3 R = normalize(reflect(-L, N));
    vec3 V = normalize(cameraPosition - fragPosition);

    vec3 diff = vec3(0);
    vec3 spec = vec3(0);
    if (dot(fragNormal, L) > 0) {
        diff = dot(N, L) * lightColor * kd;
        spec = pow(
            clamp(dot(V, R), 0, 1), 
            shininess
        ) * lightColor * ks;
    }

    return min(ambientCoeff * kd + diff, 1) + spec;
}

void main()
{
    vec3 color = vec3(0);
    color += compute(light1Position, light1Color, 0);
    color += compute(light2Position, light2Color, 1);

    fragColor = vec4(color, 1);
}