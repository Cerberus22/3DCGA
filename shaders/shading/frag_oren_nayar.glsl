#version 410

uniform vec3 kd;
uniform float rho;	 // albedo
uniform float sigma; // roughness
uniform float ambientCoeff;
// uniform float transparency;

float PI = 3.14159;

uniform vec3 cameraPosition;

/*uniform */vec3 lightPosition = vec3(2,2,4);
/*uniform */vec3 lightColor = vec3(1);

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

	vec3 X = cross(N, cross_component);
	vec3 Y = cross(N, X);

	float distV = dot(V, N);
	float distL = dot(L, N);

	vec3 Vproj = V - distV * N;
	vec3 Lproj = L - distL * N;

	float phi_i = acos(dot(Vproj, Lproj));
	float phi_r = 0;

	float theta_i = acos(dot(N,L));
	float theta_r = acos(dot(N,V));

	float alpha = max(theta_i, theta_r); 
	float beta = min(theta_i, theta_r);
	
	float s2 = sigma * sigma;

	float C1 = 1 - 0.5 * (s2 / (s2 + 0.33));

	float C2 = 0;
	if (cos(phi_i - phi_r) >= 0) {
		C2 = 0.45 * (s2 / (s2 + 0.09)) * sin(alpha);
	} else {
		C2 = 0.45 * (s2 / (s2 + 0.09)) * (sin(alpha) - pow(2 * beta / PI, 3)); 
	}
	
	float C3 = 0.125 * (s2 / (s2 + 0.09)) * pow(4 * alpha * beta / (PI * PI), 2);

	vec3 L1 = (rho / PI) * E0 * cos(theta_i) * 
		(C1 + C2 * cos(phi_i - phi_r) * clamp(tan(beta),-10,10) + C3 * (1 - abs(cos(phi_i - phi_r))) * clamp(tan((alpha + beta) / 2),-10,10));

	vec3 L2 = 0.17 * (rho * rho / PI) * E0 * cos(theta_i) * (s2 / (s2 + 0.13)) * (1 - cos(phi_i - phi_r) * pow(2 * beta / PI, 2));

	vec3 diff = L1 + L2;
	
	vec3 spec = vec3(0);

	fragColor = vec4(ambientCoeff * kd + diff, 1);
}