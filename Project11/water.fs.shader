#version 330 core
in vec4 pos;
in vec2 uv;
in mat3 tbn;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D specular;

uniform sampler2D position;

uniform float time;

in vec4 clip;

layout(location=0)out vec3 dDiffuse;
layout(location=1)out vec3 dNormal;
layout(location=2)out vec3 dAmbient;
layout(location=3)out vec3 dSpecular;
layout(location=4)out vec3 dPosition;

void main(){
	
	vec2 p=clip.xy/clip.w*.5+.5;
	p-=sin(length(uv-.5-time*.002)*2000.)*.002;
	dDiffuse=texture(diffuse,p).xyz;
	dNormal=texture(normal,p).xyz;

	//dNormal.xy=uv;
	dAmbient=texture(ambient,p).xyz*vec3(.3,.3,.3);
	dSpecular=texture(specular,p).xyz;
	dPosition=texture(position,p).xyz;
}