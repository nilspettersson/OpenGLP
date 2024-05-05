#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 light;

//1 2 3 4 5 6
//left up right down front back
layout(location = 3) in float normalDirection;

uniform mat4 u_mvp;
out vec2 v_texCoord;
out vec3 v_light;
out float v_normalDirection;

void main() {
	v_light = light;
	v_normalDirection = normalDirection;
	v_texCoord = texCoord;
	gl_Position = u_mvp * vec4(position, 1);
};



#shader fragment
#version 330 core
layout(location = 0) out vec4 colorOutput;

uniform mat4 u_mvp;
uniform vec3 color;
uniform sampler2D sampler[10];

in vec2 v_texCoord;
in vec3 v_light;
in float v_normalDirection;


float linearizeDepth(float depth) {
	float u_near = 0.2f;
	float u_far = 6000.0f;
	float z = depth * 2.0 - 1.0;
	return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

void main() {
	vec4 texture = texture2D(sampler[1], v_texCoord);
	vec4 color = vec4(texture.xyz * v_light.x, 1 + texture.w);
	
	float depth = gl_FragCoord.z;
	float linearDepth = linearizeDepth(depth) / 4000;
	linearDepth = clamp(linearDepth, 0.0, 1);

	vec3 normal;
	int direction = int(v_normalDirection + 0.5);  // Adding 0.5 for proper rounding

	switch (direction) {
		case 0: normal = vec3(-1, 0, 0); break;
		case 1: normal = vec3(0, 1, 0); break;
		case 2: normal = vec3(1, 0, 0); break;
		case 3: normal = vec3(0, -1, 0); break;
		case 4: normal = vec3(0, 0, -1); break;
		case 5: normal = vec3(0, 0, 1); break;
		default: normal = vec3(0, 0, 0); // default normal if out of expected range
	}





	vec3 finalColor = mix(vec3(color.xyz), vec3(0.5, 0.7, 1), linearDepth);
	color = vec4(finalColor, 1 + texture.w);

	//colorOutput = vec4(color);
	colorOutput = vec4(normal, 1);
	
};