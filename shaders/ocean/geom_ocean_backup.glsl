#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 256) out;

in vec3 geomPosition[];
in vec3 geomNormal[];

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 mvpMatrix;

uniform int subdivisions = 0;
uniform float time;
uniform float a;
uniform float kx;
uniform float kz;
uniform float w;

float PI = 3.14159;

struct Triangle {
    vec3 p0;
    vec3 p1;
    vec3 p2;
};

Triangle triangles0[256]; // read
Triangle triangles1[256]; // write

int triangleCount = 1;

void EmitVertixWith(vec3 pos, vec3 norm) {
    fragPosition = pos;
    fragNormal   = norm;

    gl_Position = mvpMatrix * vec4(pos, 1);
    EmitVertex();
}

vec3 computeNormal(float x, float z) {
    float t = time * 2 * PI;

    float dy_dx = a * kx * cos(kx * x + w * t);
    float dy_dz = a * kz * cos(kz * z + w * t);
    return normalize(vec3(-dy_dx, 1.0, -dy_dz));
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

void EmitTriangle(Triangle t) {
    vec3 na = computeNormal(t.p0.x, t.p0.z);
    vec3 nb = computeNormal(t.p1.x, t.p1.z);
    vec3 nc = computeNormal(t.p2.x, t.p2.z);

    EmitVertixWith(modifyY(t.p0), na);
    EmitVertixWith(modifyY(t.p1), nb);
    EmitVertixWith(modifyY(t.p2), nc);
    EndPrimitive();
}

void splitTriangles() {
    for (int i = 0; i < triangleCount; i++) {
        Triangle t = triangles0[i];
        
        vec3 m0 = (t.p0 + t.p1) / 2;
        vec3 m1 = (t.p1 + t.p2) / 2;
        vec3 m2 = (t.p2 + t.p0) / 2;

        Triangle t0, t1, t2, t3;
        t0.p0 = t.p0;
        t0.p1 = m0;
        t0.p2 = m2;

        t1.p0 = m0;
        t1.p1 = m1;
        t1.p2 = m2;

        t2.p0 = t.p1;
        t2.p1 = m0;
        t2.p2 = m1;

        t3.p0 = t.p2;
        t3.p1 = m1;
        t3.p2 = m2;

        triangles1[i*4 + 0] = t0;
        triangles1[i*4 + 1] = t1;
        triangles1[i*4 + 2] = t2;
        triangles1[i*4 + 3] = t3;
    }
    
    triangleCount *= 4;
    
    for (int i = 0; i < triangleCount; i++) {
        triangles0[i] = triangles1[i];
    }
}

void main() {
    Triangle orig;

    orig.p0 = geomPosition[0];
    orig.p1 = geomPosition[1];
    orig.p2 = geomPosition[2];

    triangles0[0] = orig;

    for (int i = 0; i < subdivisions; i++) {
        splitTriangles();
    }

    for (int i = 0; i < triangleCount; i++) {
        EmitTriangle(triangles0[i]);
    }
}