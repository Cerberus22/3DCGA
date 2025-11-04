#version 410

uniform sampler2D minimapTexture;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(minimapTexture, fragTexCoord).xyz, 1);
}
