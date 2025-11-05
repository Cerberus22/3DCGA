#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 256) out;

in vec3 geomPosition[];
in vec3 geomNormal[];

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 mvpMatrix;

uniform float time;
uniform float a;
uniform float kx;
uniform float kz;
uniform float w;

uniform bool doSubdivide;

float PI = 3.14159;

vec3 computeNormal(vec3 pos) {
    float x = pos.x;
    float z = pos.z;

    float t = time * 2 * PI;

    float dy_dx = a * kx * cos(kx * x + w * t);
    float dy_dz = a * kz * cos(kz * z + w * t);
    return normalize(vec3(-dy_dx, 1.0, -dy_dz));
}

void EmitVertixWith(vec3 pos) {
    fragPosition = pos;
    fragNormal   = computeNormal(pos);

    gl_Position = mvpMatrix * vec4(pos, 1);
    EmitVertex();
}

vec3 modifyY(vec3 pos) {
    float x = pos.x;
    float z = pos.z;
    float t = time * 2 * PI;

    return vec3(
        x,
        a * (sin(kx * x + w*t) + sin(kz * z + w*t)),
        z
    );
}

void EmitTriangle(vec3 a, vec3 b, vec3 c) {
    EmitVertixWith(modifyY(a));
    EmitVertixWith(modifyY(b));
    EmitVertixWith(modifyY(c));
    EndPrimitive();
}


void main() {
    vec3 p0 = geomPosition[0];
    vec3 p1 = geomPosition[1];
    vec3 p2 = geomPosition[2];
    
    if (!doSubdivide) {
        EmitTriangle(p0, p1, p2);
        return;
    }

    vec3 m0 = (p0 + p1) / 2;
    vec3 m1 = (p1 + p2) / 2;
    vec3 m2 = (p2 + p0) / 2;

    EmitTriangle(p0, m0, m2);
    EmitTriangle(p1, m0, m1);
    EmitTriangle(p2, m1, m2);
    EmitTriangle(m0, m1, m2);
}