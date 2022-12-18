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
void main() {
	vec4 texture = texture2D(sampler[1], v_texCoord);
	colorOutput = vec4(texture.xyz * v_normal.x, 1);
};