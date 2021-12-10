class Player{
public:
	Player(Window& w):window(w){}
	void addCollider(Geo& g){
		colliders.push_back(&g);
	}
	void update(){
		glm::vec2 m=(2.f*window.input.mouse-glm::vec2(window.width,window.height))/float(window.height);
		ObjectToWorld=glm::rotate(glm::mat4(1),-m.x,glm::vec3(0,1,0));
		ObjectToWorld=glm::rotate(ObjectToWorld,-m.y,glm::vec3(1,0,0));

		pos+=vel*dt;
		vel+=glm::vec3(0,-10.,0)*dt;

		glm::vec4 d=glm::vec4(0,1,0,1e10);
		for (int i=0;i<colliders.size();i++){
			glm::vec4 d0=RayTracer::distanceGeo(pos,*colliders[i]);
			if (d0.w<d.w){
				d=d0;
			}
		}
		d.w-=.4f;

		
		glm::vec3 q=glm::vec3(0);bool pressed=false;
		if (glfwGetKey(window.window,GLFW_KEY_W)==GLFW_PRESS){ q.z+=1.;pressed=true; }
		if (glfwGetKey(window.window,GLFW_KEY_S)==GLFW_PRESS){ q.z-=1.;pressed=true; }
		if (glfwGetKey(window.window,GLFW_KEY_A)==GLFW_PRESS){ q.x-=1.;pressed=true; }
		if (glfwGetKey(window.window,GLFW_KEY_D)==GLFW_PRESS){ q.x+=1.;pressed=true; }
		
		bool vpress=glfwGetKey(window.window,GLFW_KEY_V)==GLFW_PRESS;
		if (vpress!=oldv&&vpress){
			fly=!fly; 
		}
		oldv=vpress;

		if (fly){
			vel-=glm::vec3(0,-10.,0)*dt;
			vel+=glm::mat3(ObjectToWorld)*glm::vec3(q.x,q.y,-q.z)*1.5f;
			vel*=.8f;
		}
		else if (d.w<=0.){
			//if (d.y>0.)vel*=0.;
			if (d.y>.6){
				vel.y*=0.f;
				pos+=glm::vec3(0,-d.w/d.y,0);
			}
			else{
				pos+=glm::vec3(d)*-d.w;
			}

			if (glfwGetKey(window.window,GLFW_KEY_SPACE)==GLFW_PRESS){
				vel+=glm::vec3(0,700,0)*dt*glm::dot(glm::vec3(d),glm::vec3(0,1,0));
			}
			if (pressed){
				q=glm::normalize(q);

				glm::vec3 r=glm::vec3(ObjectToWorld[0]);
				glm::vec3 f=-glm::cross(r,glm::vec3(0,1,0));

				vel+=(q.x*r+q.z*f+glm::vec3(0,-.5,0))*3.5f;
			}
			

			vel+=glm::max(0.f,-glm::dot(vel,glm::vec3(d)))*glm::vec3(d);
			vel*=.3;
		}


		ObjectToWorld[3]=glm::vec4(pos+glm::vec3(0,.3,0),1);
	}
	glm::mat4 ObjectToWorld;
	glm::vec3 pos=glm::vec3(0);
	glm::vec3 vel;
private:
	Window& window;
	float dt=1.f/60.f;
	std::vector<Geo*> colliders;
	bool oldv=false;
	bool fly=false;
};