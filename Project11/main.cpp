#include"stdafx.h"

#include"Data.h"
#include"RayTracer.h"
#include"Window.h"
#include"OGL.h"
#include"Player.h"

struct RenderObject{
	OGL::VAO vao;
	OGL::VO vbo=OGL::VO(GL_ARRAY_BUFFER);
	OGL::VO vio=OGL::VO(GL_ELEMENT_ARRAY_BUFFER);

	RenderObject(Geo& geo){
		vao.bind();
		vbo.load(geo.vert);
		vio.load(geo.index);
		vao.setAttribute(0,3,0);
		vao.setAttribute(1,2,0+3);
		vao.setAttribute(2,3,0+3+2);
		vao.setAttribute(3,3,0+3+2+3);
	}
};
struct Canvas{
	OGL::FBO fbo;
	std::shared_ptr<OGL::Texture> fbo_textures[5];
	Canvas(int width,int height){
		for (int i=0;i<5;i++){
			fbo_textures[i]=std::make_shared<OGL::Texture>(GL_TEXTURE_2D);
			fbo_textures[i]->internalformat=GL_RGBA16F;
			fbo_textures[i]->format=GL_RGBA;
			fbo_textures[i]->type=GL_FLOAT;
			fbo_textures[i]->generate_mips=false;
			fbo_textures[i]->load(width,height,nullptr);
			fbo.attachTexture(fbo_textures[i],GL_COLOR_ATTACHMENT0+i);
		}
		std::shared_ptr<OGL::Texture> fbo_depth=std::make_shared<OGL::Texture>(GL_TEXTURE_2D);
		fbo_depth->internalformat=GL_DEPTH_COMPONENT32F;
		fbo_depth->format=GL_DEPTH_COMPONENT;
		fbo_depth->type=GL_UNSIGNED_INT;
		fbo_depth->generate_mips=false;
		fbo_depth->load(width,height,nullptr);
		fbo.attachTexture(fbo_depth,GL_DEPTH_ATTACHMENT);
		fbo.bind();
		unsigned int attachments[5] ={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4};
		glDrawBuffers(5,attachments);
		fbo.unbind();
	}
	void bind(){
		fbo.bind();
	}
	void bindTextures(){
		for (int i=0;i<5;i++){
			glActiveTexture(GL_TEXTURE0+i);
			fbo_textures[i]->bind();
		}
	}
	static void unbind(){
		OGL::FBO::unbind();
	}
};

using namespace std;
using namespace glm;

int main(){
	Window window(1440,720,"hi");
	
	Player player(window);
	player.pos=vec3(-2,.2,3);

	mat4 MI=mat4(1);
	mat4 persp=perspective(.3f*3.14159f,float(window.width)/float(window.height),0.3f,100.f);
	mat4 ObjectToWorld=mat4(1);
	mat4 WorldToCamera=mat4(1);
	mat4 waterM=translate(scale(mat4(1),vec3(30)),vec3(-10,10,0));

	auto quad_obj=std::make_shared<Geo>();
	FileIO::loadOBJ("models/quad.obj",*quad_obj);
	auto scene_obj=std::make_shared<Geo>();
	FileIO::loadOBJ("models/underbridge4.obj",*scene_obj);
	auto skybox_obj=std::make_shared<Geo>();
	FileIO::loadOBJ("models/cube.obj",*skybox_obj);
	skybox_obj->invertWinding();

	player.addCollider(*scene_obj);

	auto bumpy=make_shared<Image>("textures/bump - Copy.bmp");
	bumpy->bumpToNormal();

	shared_ptr<Image> object_images[4]={make_shared<Image>(4,4,255),
										bumpy,
										make_shared<Image>(4,4,255),
										make_shared<Image>(4,4,255)};
	shared_ptr<OGL::Texture> object_textures[4];
	for (int i=0;i<4;i++){
		object_textures[i]=make_shared<OGL::Texture>(GL_TEXTURE_2D);
		object_textures[i]->load(object_images[i]->width,object_images[i]->height,object_images[i]->buffer.data());
	}


	shared_ptr<Image> skybox_img[6]={
		make_shared<Image>("textures/cubemap/nx.bmp"),
		make_shared<Image>("textures/cubemap/px.bmp"),
		make_shared<Image>("textures/cubemap/ny.bmp"),
		make_shared<Image>("textures/cubemap/py.bmp"),
		make_shared<Image>("textures/cubemap/nz.bmp"),
		make_shared<Image>("textures/cubemap/pz.bmp")
	};

	auto skybox_texture=make_shared<OGL::Texture>(GL_TEXTURE_CUBE_MAP);
	skybox_texture->loadCube(skybox_img[0]->width,skybox_img[0]->height,skybox_img);

	//RENDERING INIT
	//offscreen framebuffer
	/*OGL::FBO fbo;
	shared_ptr<OGL::Texture> fbo_textures[5];
	for (int i=0;i<5;i++){
		fbo_textures[i]=make_shared<OGL::Texture>(GL_TEXTURE_2D);
		fbo_textures[i]->internalformat=GL_RGBA16F;
		fbo_textures[i]->format=GL_RGBA;
		fbo_textures[i]->type=GL_FLOAT;
		fbo_textures[i]->generate_mips=false;
		fbo_textures[i]->load(window.width,window.height,nullptr);
		fbo.attachTexture(fbo_textures[i],GL_COLOR_ATTACHMENT0+i);
	}
	shared_ptr<OGL::Texture> fbo_depth=make_shared<OGL::Texture>(GL_TEXTURE_2D);
	fbo_depth->internalformat=GL_DEPTH_COMPONENT32F;
	fbo_depth->format=GL_DEPTH_COMPONENT;
	fbo_depth->type=GL_UNSIGNED_INT;
	fbo_depth->load(window.width,window.height,nullptr);
	fbo.attachTexture(fbo_depth,GL_DEPTH_ATTACHMENT);
	fbo.bind();
	unsigned int attachments[5] ={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5,attachments);
	fbo.unbind();*/
	//Canvas shadowcanvas(window.width,window.height);
	mat4 LightToWorld=scale(mat4(1),vec3(8));
	LightToWorld[2]*=-1.;
	LightToWorld=translate(mat4(1),vec3(-1,0,-6))*rotate(rotate(LightToWorld,-3.14159f*.02f,vec3(0,0,1)),3.14159f*.9f,vec3(0,1,0));
	//LightToWorld=rotate(LightToWorld,.4f,vec3(1,1,0));
	mat4 WorldToLight=inverse(LightToWorld);
	//WorldToLight=ortho(-1.f,1.f,-1.f,1.f,3.7f,5.f);


	OGL::FBO shadow_fbo;
	std::shared_ptr<OGL::Texture> shadow_fbo_depth=std::make_shared<OGL::Texture>(GL_TEXTURE_2D);
	shadow_fbo_depth->internalformat=GL_DEPTH_COMPONENT32F;
	shadow_fbo_depth->format=GL_DEPTH_COMPONENT;
	shadow_fbo_depth->type=GL_UNSIGNED_INT;
	shadow_fbo_depth->generate_mips=false;
	shadow_fbo_depth->load(4096,4096,nullptr);
	shadow_fbo.attachTexture(shadow_fbo_depth,GL_DEPTH_ATTACHMENT);
	shadow_fbo.bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	shadow_fbo.unbind();

	Canvas canvas0(window.width,window.height);
	Canvas canvas1(window.width,window.height);

	//shadow shader
	auto shadow_shader=OGL::createShader("shaders/shadowmap.fs.shader","shaders/shadowmap.vs.shader");
	shadow_shader->use();
	shadow_shader->setData("ObjectToWorld",MI);
	shadow_shader->setData("WorldToLight",WorldToLight);


	//water shader
	auto water_shader=OGL::createShader("shaders/water.fs.shader","shaders/water.vs.shader");
	water_shader->use();
	water_shader->setData("ObjectToWorld",waterM);
	water_shader->setData("WorldToCamera",WorldToCamera);
	water_shader->setData("time",window.input.time);
	glUniform1i(glGetUniformLocation(water_shader->id,"diffuse"),0);
	glUniform1i(glGetUniformLocation(water_shader->id,"normal"),1);
	glUniform1i(glGetUniformLocation(water_shader->id,"ambient"),2);
	glUniform1i(glGetUniformLocation(water_shader->id,"specular"),3);
	glUniform1i(glGetUniformLocation(water_shader->id,"position"),4);

	//object shader
	auto object_shader=OGL::createShader("shaders/default.fs.shader","shaders/default.vs.shader");
	object_shader->use();
	object_shader->setData("ObjectToWorld",ObjectToWorld);
	object_shader->setData("WorldToCamera",WorldToCamera);
	object_shader->setData("WorldToLight",WorldToLight);
	object_shader->setData("plane",waterM);
	vec4 planen;
	object_shader->setData("planen",planen);

	glUniform1i(glGetUniformLocation(object_shader->id,"diffuse"),0);
	glUniform1i(glGetUniformLocation(object_shader->id,"normal"),1);
	glUniform1i(glGetUniformLocation(object_shader->id,"ambient"),2);
	glUniform1i(glGetUniformLocation(object_shader->id,"specular"),3);
	glUniform1i(glGetUniformLocation(object_shader->id,"shadow"),4);

	//skybox shader
	auto skybox_shader=OGL::createShader("shaders/skybox.fs.shader","shaders/default.vs.shader");
	skybox_shader->use();
	OGL::errorCheck();
	mat4 SkyBoxToWorld=mat4(1);
	skybox_shader->setData("ObjectToWorld",SkyBoxToWorld);
	skybox_shader->setData("WorldToCamera",WorldToCamera);
	skybox_shader->setData("WorldToLight",WorldToLight);
	skybox_shader->setData("ro",player.ObjectToWorld[3]);

	glUniform1i(glGetUniformLocation(skybox_shader->id,"sky"),5);


	//deferred shader
	auto deferred_shader=OGL::createShader("shaders/deferred.fs.shader","shaders/default.vs.shader");
	deferred_shader->use();
	deferred_shader->setData("ObjectToWorld",MI);
	deferred_shader->setData("WorldToCamera",MI);
	deferred_shader->setData("ro",player.ObjectToWorld[3]);
	deferred_shader->setData("lightdir",LightToWorld[2]);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"diffuse"),0);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"normal"),1);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"ambient"),2);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"specular"),3);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"position"),4);
	glUniform1i(glGetUniformLocation(deferred_shader->id,"sky"),5);

	//water vao
	RenderObject water(*quad_obj);

	//object vao
	RenderObject scene(*scene_obj);
	//skybox vao
	RenderObject skybox(*skybox_obj);
	
	//deffered vao
	RenderObject quad(*quad_obj);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	wglSwapIntervalEXT(1);

	while (!window.shouldClose()){
		window.pollEvents();
		//input/updates
		player.update();
		waterM=rotate(scale(translate(mat4(1),vec3(-10,-3.3,1.)),vec3(50)),-3.14159f*.5f,vec3(1,0,0));
		planen=waterM[2];
		//WorldToLight=rotate(rotate(ortho(-8.f,8.f,-8.f,8.f,-8.f,4.f),.6f+window.input.time*.04f,vec3(0,1,0)),1.f,vec3(1,0,0));
		//waterM=scale(inverse(player.ObjectToWorld),vec3(1,-1,1));



		//RENDER
		glCullFace(GL_FRONT);
		glViewport(0,0,4096,4096);
		shadow_fbo.bind();

		shadow_shader->use();
		shadow_shader->update();
		scene.vao.bind();

		glClear(GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES,scene_obj->vert.size(),GL_UNSIGNED_INT,0);



		glViewport(0,0,window.width,window.height);
		canvas1.bind();

		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		planen.w=1.;
		mat4 WorldToView=inverse(player.ObjectToWorld)*waterM*scale(mat4(1),vec3(1,1,-1))*inverse(waterM);
		SkyBoxToWorld=scale(translate( mat4(1),vec3(player.ObjectToWorld[3])),vec3(50));
		WorldToCamera=persp*WorldToView;

		//render sky reflection

		glActiveTexture(GL_TEXTURE0+5);
		skybox_texture->bind();

		skybox_shader->use();
		skybox_shader->update();
		skybox.vao.bind();

		glDrawElements(GL_TRIANGLES,skybox_obj->vert.size(),GL_UNSIGNED_INT,0);

		//render reflection

		for (int i=0;i<4;i++){
			glActiveTexture(GL_TEXTURE0+i);
			object_textures[i]->bind();
		}
		glActiveTexture(GL_TEXTURE0+4);
		shadow_fbo_depth->bind();

		object_shader->use();
		object_shader->update();
		scene.vao.bind();


		glDrawElements(GL_TRIANGLES,scene_obj->vert.size(),GL_UNSIGNED_INT,0);


		canvas0.bind();
		//render scene
		glCullFace(GL_BACK);

		planen.w=-1.;
		WorldToView=inverse(player.ObjectToWorld);
		WorldToCamera=persp*inverse(player.ObjectToWorld);//WorldToCamera=WorldToLight;
		object_shader->update();

		glClearColor(1,1,1,1);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		glDrawElements(GL_TRIANGLES,scene_obj->vert.size(),GL_UNSIGNED_INT,0);
		
		//render sky

		glActiveTexture(GL_TEXTURE0+5);
		skybox_texture->bind();

		skybox_shader->use();
		skybox_shader->update();
		skybox.vao.bind();

		glDrawElements(GL_TRIANGLES,skybox_obj->vert.size(),GL_UNSIGNED_INT,0);

		//canvas0.bind();
		canvas1.bindTextures();
		//render water

		water_shader->use();
		water_shader->update();
		water.vao.bind();

		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

		//render deferred
		OGL::FBO::unbind();

		canvas0.bindTextures();
		glActiveTexture(GL_TEXTURE0+5);
		skybox_texture->bind();

		deferred_shader->use();
		deferred_shader->update();
		quad.vao.bind();

		glClearColor(.01,.02,.07,.7);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES,quad_obj->vert.size(),GL_UNSIGNED_INT,0);

		window.swapBuffers();
	}
}