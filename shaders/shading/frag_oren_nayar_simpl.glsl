#version 410

uniform float ambientCoeff;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
uniform float rho = 1.0; // albedo
uniform float sigma = 0; // roughness

float PI = 3.14159;

uniform vec3 cameraPosition;

vec3 lightPosition = vec3(2,2,4);
vec3 lightColor = vec3(1);

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 E0 = kd;
    
    vec3 L = normalize(lightPosition - fragPosition);
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPosition - fragPosition);

    vec3 cross_component = vec3(1,0,0);
    if (dot(N, cross_component) > 0.95) {
        cross_component = vec3(0,1,0);
    }

    vec3 X = normalize(cross(N, cross_component));
    vec3 Y = normalize(cross(N, X));

    float distV = dot(V, N);
    float distL = dot(L, N);

    vec3 Vproj = normalize(V - distV * N);
    vec3 Lproj = normalize(L - distL * N);

    // Clamp dot products to [-1, 1] before acos()
    float NL = clamp(dot(N, L), -1.0, 1.0);
    float NV = clamp(dot(N, V), -1.0, 1.0);

    float theta_i = acos(NL);
    float theta_r = acos(NV);

    float alpha = max(theta_i, theta_r);
    float beta  = min(theta_i, theta_r);
    
    float s2 = sigma * sigma;

    float phi_diff = atan(dot(X, Lproj), dot(Y, Lproj)) - atan(dot(X, Vproj), dot(Y, Vproj));
    float cos_phi  = cos(phi_diff);

    float A = 1.0 - 0.5 * (s2 / (s2 + 0.33));
    float B = 0.45 * (s2 / (s2 + 0.09));

    // Clamp tan(beta) to prevent blow-up near 0°
    float tBeta = tan(beta);
    tBeta = clamp(tBeta, -10.0, 10.0);

    vec3 Lr = (rho / PI) * E0 * NL * (A + B * max(0.0, cos_phi) * sin(alpha) * tBeta);

    fragColor = vec4(Lr, 1);
}
