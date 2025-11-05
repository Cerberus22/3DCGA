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

uniform mat4 mvpMatrix;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;


layout(location = 0) out vec4 fragColor;

void main()
{
    vec3 lightPosition;
	vec3 lightColor;

	vec3 color = vec3(0);

	for (int i = 0; i < 2; i++) {
		if (i == 0) {
			lightPosition = light1Position;
			lightColor = light1Color;
		} else {
			lightPosition = light2Position;
			lightColor = light2Color;
		}

        vec3 L = normalize(lightPosition - fragPosition);
        vec3 N = normalize(fragNormal);
        vec3 T = normalize(fragTangent);
        vec3 B = -normalize(cross(N, T));
        
        mat3 TBN = mat3(T, B, N);

        vec4 texValue = texture(normalMap, fragTexCoord);
        vec3 normalValue = ((texValue - 0.5) * 2).xyz;
        N = normalize(TBN * normalValue);
        
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

        color += min(ambientCoeff * kd + diff, 1) + spec;
    }

    fragColor = vec4(color, 1);
}