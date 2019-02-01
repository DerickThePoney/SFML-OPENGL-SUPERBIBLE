#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (vertices = 4) out;

layout(location = 0) in vec3 inNormal[];

layout(location = 0) out vec3 outNormal[4];

#define tess 5
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
}
