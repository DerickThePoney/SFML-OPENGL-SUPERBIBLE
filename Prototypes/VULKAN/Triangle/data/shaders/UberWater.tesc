#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (vertices = 4) out;

layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec3 inTangent[];
layout(location = 3) in vec3 inColor[];
layout(location = 4) in vec2 inTexCoord[];

layout(location = 1) out vec3 outNormal[4];
layout(location = 2) out vec3 outTangent[4];
layout(location = 3) out vec3 outColor[4];
layout(location = 4) out vec2 outTexCoord[4];

#define tess 10
void main()
{
	if(gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = tess;
		gl_TessLevelInner[1] = tess;
		gl_TessLevelOuter[0] = tess;
		gl_TessLevelOuter[1] = tess;
		gl_TessLevelOuter[2] = tess;
		gl_TessLevelOuter[3] = tess;
	}


	gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
	outTangent[gl_InvocationID] = inTangent[gl_InvocationID];
	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outTexCoord[gl_InvocationID] = inTexCoord[gl_InvocationID];
}
