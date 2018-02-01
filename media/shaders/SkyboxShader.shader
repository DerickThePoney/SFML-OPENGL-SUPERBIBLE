#sh VERTEX 
#version 430 core
//vertex attributes
layout (location = 0) in vec4 position;


layout (location = 0) uniform mat4 worldViewMatrix;
layout (location = 1) uniform mat4 projectionMatrix;


out vec3 TexCoords;

void main(void)
{
	TexCoords = vec3(position);
	vec4 pos = projectionMatrix * worldViewMatrix * position;	
	gl_Position = pos.xyww;
}

#sh FRAGMENT 
#version 430 core 
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}