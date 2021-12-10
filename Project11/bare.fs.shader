#version 330 core
in vec4 pos;
in vec2 uv;
in mat3 tbn;

void main(){
	color=vec4(uv,0,1);
}