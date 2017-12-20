#sh VERTEX 
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	vec3 N;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} vs_out;

//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;



void main(void)
{
	mat4 mv_matrix = projData.worldViewMatrix * modelToWorldMatrix;
	vec4 P = mv_matrix * position;
	vs_out.N = mat3(mv_matrix) * normal;
	
	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projData.projectionMatrix * P;
	
}

#sh FRAGMENT 
#version 430 core

in VS_OUT
{
	vec3 N;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} fs_in;

uniform layout(location = 1) vec3 diffuse_color = vec3(0.5);
layout (location = 0) out vec4 color;

void main(void)
{	
	float dx = dot(fs_in.N, vec3(1.0,0,0));
	float dy = dot(fs_in.N, vec3(0,1.0,0));
	float dz = dot(fs_in.N, vec3(0,0,1.0));
	
	if(abs(dx) > abs(dy) && abs(dx) > abs(dz))
	{
		if(dx > 0)
		{
			color = vec4(1,0,0,1); // red
		}
		else
		{
			color = vec4(0,1,0,1); // green
		}
	}
	else if(abs(dy) > abs(dx) && abs(dy) > abs(dz))
	{
		if(dy > 0)
		{
			color = vec4(0,0,1,1); // blue
		}
		else
		{
			color = vec4(1,1,0,1); // red + green
		}
	}
	else if(abs(dz) > abs(dx) && abs(dz) > abs(dy))
	{
		if(dz > 0)
		{
			color = vec4(1,0,1,1); // red + blue
		}
		else
		{
			color = vec4(0,1,1,1); // green + blue
		}
	}
	else
	{
		color = vec4(fs_in.N,1);
	}
	
	color = color * fs_in.color * vec4(diffuse_color,1);
}
