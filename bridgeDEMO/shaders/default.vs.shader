#version 330 core
layout(location=0)in vec3 Position;
layout(location=1)in vec2 Uv;
layout(location=2)in vec3 Normal;
layout(location=3)in vec3 Tangent;

out vec4 pos;
out vec2 uv;
out mat3 tbn;
out vec3 lightspace;
out vec4 clip;

uniform mat4 ObjectToWorld;
uniform mat4 WorldToCamera;
uniform mat4 WorldToLight;

void main(){
	pos=ObjectToWorld*vec4(Position,1);

	uv=Uv;

	tbn[0]=Tangent;
	tbn[1]=cross(Normal,Tangent);
	tbn[2]=Normal;
	tbn=mat3(ObjectToWorld)*tbn;

	lightspace=(WorldToLight*pos).xyz;

	clip=WorldToCamera*pos;
	gl_Position=clip;
}