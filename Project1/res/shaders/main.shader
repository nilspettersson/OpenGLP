#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_mvp;

out vec2 v_texCoord;
out float v_textureId;

void main() {
	//v_textureId = textureId;
	v_texCoord = texCoord;
	gl_Position = u_mvp * vec4(position, 1);
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 colorOutput;

uniform vec3 color;
uniform float test;
uniform sampler2D sampler[20];

in vec2 v_texCoord;
in float v_textureId;
void main() {
	//int id = int(ceil(v_textureId));
	//vec4 texture = texture2D(sampler[id], v_texCoord);
	colorOutput = vec4(v_texCoord, 0, 1);
};