#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(binding = 0) uniform ScreenSize {
    vec2 screenSize;
	vec2 position;
	vec2 scale;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	//create the matrix
	vec3 x = vec3(ubo.scale.x,0,0);
	vec3 y = vec3(ubo.scale.y,0,0);
	vec3 z = vec3(ubo.position,0);
	mat3 mvp = mat3(x,y,z);

	//convert from NDC to screen
	vec3 pos = (inPosition + 1) * 0.5; 
	pos = vec3(pos.x * ubo.screenSize.x, pos.y * ubo.screenSize.y, 0);
	pos = mvp * pos;
	pos.x = pos.x/ubo.screenSize.x;
	pos.y = pos.y/ubo.screenSize.y;

	gl_Position = vec4(pos, 1.0);
	outTexCoord = inTexCoord;
}