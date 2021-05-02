#shader vertex
#version 330 core

uniform mat4 u_mvp;


layout(location = 0) in vec4 position;
void main() {
	gl_Position = u_mvp * position;
};


#shader fragment
#version 330 core
uniform vec4 color;

layout(location = 0) out vec4 colorOutput;
void main() {
	colorOutput = color;
};