#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 light;

//0 1 2 3 4 5
//left up right down front back
layout(location = 3) in float normalDirection;

uniform vec3 u_camera; // Camera position in world space
uniform mat4 u_model; // Model Matrix
uniform mat4 u_mv; // Model-View Matrix
uniform mat4 u_p; // Projection Matrix

out vec2 v_texCoord;
out vec3 v_light;
out float v_normalDirection;
out vec3 v_cameraViewVector;

void main() {
	v_light = light;
	v_normalDirection = normalDirection;
	v_texCoord = texCoord;


	vec4 worldPosition = u_model * vec4(position, 1);
	v_cameraViewVector = normalize(u_camera + worldPosition.xyz);

	gl_Position = u_p * u_mv * vec4(position, 1.0);
};



#shader fragment
#version 330 core
layout(location = 0) out vec4 colorOutput;


uniform float u_time;
uniform mat4 u_mvp;
uniform vec3 color;
uniform sampler2D sampler[10];

in vec2 v_texCoord;
in vec3 v_light;
in float v_normalDirection;
in vec3 v_cameraViewVector;


float linearizeDepth(float depth) {
	float u_near = 0.2f;
	float u_far = 6000.0f;
	float z = depth * 2.0 - 1.0;
	return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

void main() {
	vec4 texture = texture2D(sampler[1], v_texCoord);
	
	float depth = gl_FragCoord.z;
	float linearDepth = linearizeDepth(depth) / 4000;
	linearDepth = clamp(linearDepth, 0.0, 1);

	vec3 normal;
	int direction = int(v_normalDirection + 0.5);

	switch (direction) {
		case 0: normal = vec3(-1, 0, 0); break;
		case 1: normal = vec3(0, 1, 0); break;
		case 2: normal = vec3(1, 0, 0); break;
		case 3: normal = vec3(0, -1, 0); break;
		case 4: normal = vec3(0, 0, -1); break;
		case 5: normal = vec3(0, 0, 1); break;
		default: normal = vec3(0, 0, 0);
	}


	vec3 color = vec3(texture.xyz /* v_light.x*/);

	//ambient light
	vec3 ambientColor = vec3(0.7, 0.8, 1);
	float ambientIntensity = v_light.x * 0.8f;
	vec3 ambient = ambientColor * ambientIntensity;


	//Sun
	vec3 sunDirection = vec3(cos(u_time), sin(u_time), 0.0f);
	//vec3 sunDirection = vec3(0.5f, -0.5f, 0.0f);
	float sunDot = max(dot(normalize(-sunDirection), normalize(normal)), 0);
	float sunIntensity = 1.0f;
	vec3 sunColor = vec3(0.9f, 0.9f, 0.5f);

	//If sun is bellow horizon reduce strength
	if (sunDirection.y > 0) {
		sunIntensity *= 1.01f - sunDirection.y;
		ambient *= 1.01f - sunDirection.y / 1.2f;
	}

	//diffuse color from sun
	vec3 diffuse = sunIntensity * sunColor * sunDot;

	vec3 sunReflection = reflect(normalize(-sunDirection), normal);
	float specularStrength = pow(max(0.0, dot(v_cameraViewVector, sunReflection)), 8) * 0.7;

	color = (ambient + diffuse + specularStrength) * color;

	vec3 finalColor = mix(vec3(color.xyz), vec3(0.5, 0.7, 1), linearDepth);
	colorOutput = vec4(finalColor, 1);
	
};