#version 330 core
layout(location=0)in vec3 Position;
layout(location=1)in vec2 Uv;
layout(location=2)in vec3 Normal;
layout(location=3)in vec3 Tangent;

uniform mat4 ObjectToWorld;
uniform mat4 WorldToLight;

void main(){
	gl_Position=WorldToLight*ObjectToWorld*vec4(Position,1);
}