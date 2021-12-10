#version 330 core
layout(location=0)in vec3 Position;
layout(location=1)in vec2 Uv;
layout(location=2)in vec3 Normal;
layout(location=3)in vec3 Tangent;

out vec4 pos;
out vec2 uv;
out mat3 tbn;

uniform mat4 ObjectToWorld;
uniform mat4 WorldToCamera;

out vec4 clip;
out vec3 planepos;
out vec3 planen;
void main(){
	pos=ObjectToWorld*vec4(Position,1);
	uv=Uv;
	tbn=mat3(ObjectToWorld)*tbn;
	
	planepos=ObjectToWorld[3].xyz;
	planen=ObjectToWorld[2].xyz;

	clip=WorldToCamera*pos;
	gl_Position=clip;
}