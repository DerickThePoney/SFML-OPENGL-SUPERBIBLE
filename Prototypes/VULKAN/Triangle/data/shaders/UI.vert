#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

layout(binding = 0) uniform ScreenSize {
    vec2 screenSize;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	//convert from NDC to screen
	vec4 p = vec4(inPosition, 1.0);
	//p.xy /= ubo.screenSize.xy;
	gl_Position = p;

	outTexCoord = inTexCoord;
}