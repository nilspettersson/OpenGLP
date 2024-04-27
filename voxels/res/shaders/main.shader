#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;
out vec2 v_texCoord;
out vec3 v_normal;

void main() {
	v_normal = normal;
	v_texCoord = texCoord;
	gl_Position = u_mvp * vec4(position, 1);
};


#shader fragment
#version 330 core
layout(location = 0) out vec4 colorOutput;

uniform vec3 color;
uniform sampler2D sampler[10];

in vec2 v_texCoord;
in vec3 v_normal;

float linearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0; // Back to NDC form
	return (2.0 * 0.1f * 6000) / (6000.0f + 0.1f - z * (6000.0f - 0.1f));
}

void main() {
	vec4 texture = texture2D(sampler[1], v_texCoord);
	vec4 color = vec4(texture.xyz * v_normal.x, 1 + texture.w);
	//colorOutput = vec4(texture.xyz * v_normal.x, 1 + texture.w);

	
	float linearDepth = linearizeDepth(gl_FragCoord.z) / 3500;

	linearDepth = clamp(linearDepth, 0.0, 1.0);

	vec3 finalColor = mix(vec3(color.xyz), vec3(0.5, 0.7, 1), linearDepth);
	color = vec4(finalColor, 1 + texture.w);

	colorOutput = vec4(color);
	
};