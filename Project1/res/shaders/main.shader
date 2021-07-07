#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_mvp;

out vec2 v_texCoord;

void main() {
	v_texCoord = texCoord;
	gl_Position = u_mvp * position;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 colorOutput;

uniform vec3 color;
uniform float test;
uniform sampler2D u_texture;

in vec2 v_texCoord;
void main() {
	vec4 texture = texture(u_texture, v_texCoord);
	colorOutput = vec4(color, 1);
};