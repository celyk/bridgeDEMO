#version 330 core
in vec4 pos;
in vec2 uv;
in mat3 tbn;
in vec3 lightspace;
in vec4 clip;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D specular;

uniform sampler2D shadow;

layout(location=0)out vec3 dDiffuse;
layout(location=1)out vec3 dNormal;
layout(location=2)out vec3 dAmbient;
layout(location=3)out vec3 dSpecular;
layout(location=4)out vec3 dPosition;

uniform mat4 plane;
uniform vec4 planen;

float rand(vec2 p){ return fract(sin(dot(p.xy,vec2(12.9898,4.1414)))*43758.5453); }

float stest(vec2 v){
	return step(0.0,texture(shadow,(lightspace.xy*.5+.5)+v/4096.).x-(lightspace.z*.5+.5));
}

void main(){
	dDiffuse=texture(diffuse,uv).xyz;
	//dDiffuse=texture(normal,uv).xyz*2.-1.;
	//dDiffuse=vec3(0,0,1);
	//dDiffuse=vec3(1);
	//dDiffuse=lightspace;

	dNormal=tbn*(texture(normal,uv).xyz*2.-1.);
	//dNormal=tbn[2].xyz;

	float shad=0.;
	vec2 p=clip.xy/clip.w*.5+.5;
	for (int j=-2;j<=2;j++){
		for (int i=-2;i<=2;i++){
			 shad+=stest( vec2(i,j) );
		}
	}
	shad/=9.;

	//dAmbient=vec3( step(lightspace.z,1.)*step(max(lightspace.x,lightspace.y),1.)*shad );
	
	dAmbient=vec3(1);
	//dAmbient=vec3( step(0.,-max(max(v.x,v.y),v.z)+1.)*.5+.5 );
	vec3 v=abs(lightspace);
	float db=max(max(v.x,v.y),v.z);
	if (db<1.)
		dAmbient*=mix(1.,shad,smoothstep(0.,-.1,db-1.));
	//else dAmbient*=vec3(1,0,1);

	dSpecular=texture(specular,uv).xyz*vec3(.08,5,5);
	dPosition=pos.xyz;

	float f=dot(pos.xyz-plane[3].xyz,planen.xyz);
	if (f<0.&&planen.w>0.)discard;
}