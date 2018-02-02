#sh VERTEX 
#version 430 core
//vertex attributes
layout (location = 0) in vec4 position;

layout(std140, binding = 0) uniform ProjectionData
{
	mat4 worldViewMatrix;
	mat4 projectionMatrix;
} projData;

out vec3 TexCoords;

void main(void)
{
	TexCoords = mat3(projData.worldViewMatrix) * vec3(position);	
	gl_Position = position;
}

#sh FRAGMENT 
#version 430 core 
out vec4 FragColor;

in vec3 TexCoords;

uniform layout(binding=0) samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}