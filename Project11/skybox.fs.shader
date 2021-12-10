#version 330 core
in vec4 pos;
in vec2 uv;
in mat3 tbn;
in vec3 lightspace;
in vec4 clip;

uniform samplerCube sky;

uniform vec4 ro;

layout(location=0)out vec3 dDiffuse;
layout(location=1)out vec3 dNormal;
layout(location=2)out vec3 dAmbient;
layout(location=3)out vec3 dSpecular;
layout(location=4)out vec3 dPosition;

mat2 rot(float a){ float s=sin(a),c=cos(a);return mat2(c,s,-s,c); }

void main(){
	vec3 rd=normalize(pos.xyz);
	dDiffuse=vec3(1);
	//dDiffuse=vec3(0);
	vec3 n=-normalize(pos.xyz-ro.xyz);
	n.xz=rot(.7)*n.xz;
	dNormal=n;
	dAmbient=vec3(0);
	dSpecular=vec3(1,0,0);
	dPosition=pos.xyz;
	
}