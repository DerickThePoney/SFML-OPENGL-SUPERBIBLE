#sh VERTEX 
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 UV;

out VS_OUT
{
	//light stuff
	vec3 N;
	vec3 L;
	vec3 V;
	
	//color stuff
	vec4 color;
	
	//texture stuff
	vec2 UV;
} vs_out;

//uniforms
uniform layout(location = 0) mat4 modelToWorldMatrix;
uniform layout(location = 1) vec3 lightPosition = vec3(0,0,10);

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
	vs_out.L = lightPosition - P.xyz;
	vs_out.V = -P.xyz;

	vs_out.UV = UV;
	vs_out.color = color;
	gl_Position = projData.projectionMatrix * P;
	
}

#sh FRAGMENT 
#version 430 core

in VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
	vec4 color;
	vec2 UV;
} fs_in;

layout (location = 0) out vec4 color;

uniform layout(location = 3) vec3 ambiant_light = vec3(0.2,0.2,0.2);
uniform layout(location = 4) vec3 diffuse_albedo = vec3(0.8,0.8,0.8);
uniform layout(location = 5) vec3 specular_albedo = vec3(0.7);
uniform layout(location = 6) float specular_power = 8.0;

void main(void)
{
	//normalise input
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	
	//reflected quantity
	vec3 R = reflect(-L, N);
	
	//diffuse and specular quantity
	vec3 colorTexture = diffuse_albedo;//vec3(1.0,1.0,1.0);//texture(tex_object, fs_in.UV).rgb;  
	vec3 diffuse = max(dot(N,L), 0.2) * colorTexture;
	vec3 specular = pow(max(dot(R,V),0.0), specular_power) * specular_albedo;
	
	vec3 light = clamp(ambiant_light + diffuse + specular,0,1);
	
	//color = vec4(fs_in.tc, 1.0, 1.0);//texture(tex_object, fs_in.tc * vec2(3.0, 1.0));
	color = fs_in.color * vec4(light, 1.0);
}
