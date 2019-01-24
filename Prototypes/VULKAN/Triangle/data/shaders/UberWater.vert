#version 450
#extension GL_ARB_separate_shader_objects : enable


out gl_PerVertex {
    vec4 gl_Position;
};


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inColor;
layout(location = 4) in vec2 inTexCoord;

layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outTangent;
layout(location = 3) out vec3 outColor;
layout(location = 4) out vec2 outTexCoord;

void main() 
{
	//standard suff
    gl_Position = vec4(inPosition , 1.0);

	outNormal = inNormal;
	outTangent = inTangent;
	outColor = inColor;
	outTexCoord = inTexCoord;
}