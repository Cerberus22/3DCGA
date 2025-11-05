#version 410

uniform sampler2D tex;

in vec2 fragTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(tex, fragTexCoord).xyz, 1);
}
