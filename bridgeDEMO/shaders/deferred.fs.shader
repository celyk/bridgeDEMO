#version 330 core
in vec4 pos;
in vec2 uv;
in mat3 tbn;

uniform vec4 ro;
uniform vec4 lightdir;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D specular;

uniform sampler2D position;
uniform samplerCube sky;

void Grade(inout vec3 x){
	x=max(vec3(0),x);
	x=(x*(2.51*x+.03))/(x*(2.43*x+.59)+.14);    //ACESfilm
	x=pow(x,vec3(1./2.2));
}

out vec4 colour;
void main(){
	/*if (p.x<0.&&p.y>0.)colour.xyz=texture(diffuse,uv).xyz;
	if (p.x>0.&&p.y>0.)colour.xyz=texture(normal,uv).xyz;
	if (p.x<0.&&p.y<0.)colour.xyz=texture(ambient,uv).xyz;
	if (p.x>0.&&p.y<0.)colour.xyz=texture(specular,uv).xyz;
	*/
	vec3 p=texture(position,uv).xyz;
	vec3 rd=normalize(p-ro.xyz);
	vec3 n=texture(normal,uv).xyz;

	vec3 lit=vec3(0);	
	//lit+=vec3(.001);
	lit+=vec3(.0002,.004,.01);
	//lit+=vec3(.001,.008,.02)*(dot(vec3(0,1,0),n)*.5+.5);
	//lit+=vec3(.04,.05,.06)*pow(min(dot(rd,n)+1.,1.),8.);
	vec3 spec=texture(specular,uv).xyz;
	vec3 refn=reflect(rd,n);
	
	vec3 skytint=vec3(1);
	if (length(p)>40.){ refn=n;skytint=vec3(.7,.6,.5); }
	lit+=skytint*spec.x*pow(texture(sky,refn,spec.y).xyz,vec3(2.2))*pow(min(dot(rd,n)+1.,1.),spec.z);
	//lit+=texture(position,uv).xyz;

	lit+=vec3(1.4,.8,.3)*max(0.,dot(normalize(-lightdir.xyz),n))*texture(ambient,uv).xyz;

	//lit+=texture(sky,reflect(rd,n)).xyz*texture(specular);
	//lit+=vec3(1.4,.8,.2)*texture(ambient,uv).xyz;

	//lit=vec3(1)
	//lit*=.1;
	//lit*=texture(ambient,uv).xyz;

	colour=vec4(0);
	colour.xyz=lit*pow(texture(diffuse,uv).xyz,vec3(2.2));
	Grade(colour.xyz);
	colour.w=1.;
}