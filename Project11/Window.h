class Window{
public:
	int width,height;
	const char* title;
	Window(int w,int h,const char* t):width(w),height(h),title(t){ init(); }
	void init(){
		if (!glfwInit()){
			throw std::runtime_error("glfw failed to initialize");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,1);glfwWindowHint(GLFW_SAMPLES,4);
		glfwWindowHint(GLFW_RESIZABLE,0);

		window=glfwCreateWindow(width,height,title,nullptr,nullptr);
		glfwMakeContextCurrent(window);

		glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

		glewExperimental=GL_TRUE;
		if (glewInit()!=GLEW_OK){
			throw std::runtime_error("glew failed to initialize");
		}

		glfwSetFramebufferSizeCallback(window,resize_callback);
	}
	bool shouldClose(){ return glfwWindowShouldClose(window); }
	void pollEvents(){ glfwPollEvents(); }//input.update();

	void swapBuffers(){ glfwSwapBuffers(window);input.update(this); }
	void cleanup(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	class Input{
	public:
		float time=0.;
		glm::vec2 mouse;

		struct Button{
			bool down = false;
			bool changed = false;
		};

		void getButton(int code, Button& b);

		void getMousePos(int p[2]);
		void getNDC(int p[2], float f[2]);

		struct MouseState{
			int pos[2];
			int drag_pos[2];
			Button L;
			Button R;
		};
		void getMouse(MouseState* state);

		bool mouse_trap = false;
	private:
		friend Window;
		void update(Window* window){
			double mx,my;
			glfwGetCursorPos(window->window,&mx,&my);
			mouse=glm::vec2(mx,my);
			time=glfwGetTime();
		}
	};
	Input input;

	//private:
	GLFWwindow* window=nullptr;
private:
	static void resize_callback(GLFWwindow* win,int w,int h){
		glViewport(0,0,w,h);
	}
};