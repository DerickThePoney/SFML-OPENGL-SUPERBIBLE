#sh VERTEX 
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	//uvs stuff
	vec2 uv;
} vs_out;


void main(void)
{
	gl_Position = position;
	vs_out.uv = UV;
}

#sh FRAGMENT 
#version 430 core

in VS_OUT
{
	vec2 uv;
} fs_in;

uniform layout(location=0) sampler2D screenTexture;

layout (location = 0) out vec4 color;

void main(void)
{
	color = texture(screenTexture, fs_in.uv);
}
