#version 410

uniform vec3 light = vec3(0.2,0.6,0.83);
uniform vec3 dark = vec3(0, 0.3, 0.7);

uniform float time;
uniform float a;

uniform float ax;
uniform float az;
uniform float a1;
uniform float a2;


in vec3 fragPosition;
in vec3 fragNormal;

layout(location = 0) out vec4 fragColor;


void main()
{
	float y = fragPosition.y;

	float maxA = ax + az + a1 + a2;
	float dx = (y/maxA + 1)/2;

	fragColor = vec4(
		clamp(dx * light + (1-dx)*dark, 0, 1)
	,1);
}
