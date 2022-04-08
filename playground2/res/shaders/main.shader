#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;
out vec2 v_texCoord;

void main() {
	gl_Position = u_mvp * vec4(position, 1);
};


#shader fragment
#version 330 core
layout(location = 0) out vec4 colorOutput;

uniform vec3 color;

in vec2 v_texCoord;
void main() {
	colorOutput = vec4(1);
};