#version 450 core

uniform sampler2D tex_object;

in VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
	vec4 color;
	vec2 UV;
} fs_in;

layout (location = 0) out vec4 color;

uniform vec3 diffuse_albedo = vec3(0.8,0.8,0.8);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 8.0;

void main(void)
{
	//normalise input
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	
	//reflected quantity
	vec3 R = reflect(-L, N);
	
	//diffuse and specular quantity
	vec3 colorTexture = texture(tex_object, fs_in.UV).rgb;  
	vec3 diffuse = max(dot(N,L), 0.0) * colorTexture;
	vec3 specular = pow(max(dot(R,V),0.0), specular_power) * colorTexture;
	
	//color = vec4(fs_in.tc, 1.0, 1.0);//texture(tex_object, fs_in.tc * vec2(3.0, 1.0));
	color = fs_in.color * vec4(diffuse + specular, 1.0);
}
