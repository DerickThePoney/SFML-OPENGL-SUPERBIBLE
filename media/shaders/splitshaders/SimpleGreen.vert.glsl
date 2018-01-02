#version 450 core

layout (location = 0) in vec4 position;
uniform mat4 proj_matrix;

void main(void)
{
	
	gl_Position = proj_matrix * position;
	
}
