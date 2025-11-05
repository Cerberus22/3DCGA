#version 410

uniform vec3 waterColor = vec3(0,0.3,0.7);

uniform float time;
uniform float a;

in vec3 fragPosition;
in vec3 fragNormal;

layout(location = 0) out vec4 fragColor;


void main()
{
	float y = fragPosition.y;
	fragColor = vec4(
		clamp(waterColor * ((y / (4*a)) + 0.75), 0, 1)
	,1);
}
