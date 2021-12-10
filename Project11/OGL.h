class OGL{
public:
	static void errorCheck(){
		for (GLenum Error = glGetError(); (GL_NO_ERROR != Error); Error = glGetError()){
			std::cout<<"ERROR:"<<std::endl;
			switch (Error)
			{
			case GL_INVALID_ENUM:      printf("\n%s\n\n","GL_INVALID_ENUM"); break;
			case GL_INVALID_VALUE:     printf("\n%s\n\n","GL_INVALID_VALUE"); break;
			case GL_INVALID_OPERATION: printf("\n%s\n\n","GL_INVALID_OPERATION"); break;
			case GL_STACK_OVERFLOW:    printf("\n%s\n\n","GL_STACK_OVERFLOW"); break;
			case GL_STACK_UNDERFLOW:   printf("\n%s\n\n","GL_STACK_UNDERFLOW"); break;
			case GL_OUT_OF_MEMORY:     printf("\n%s\n\n","GL_OUT_OF_MEMORY"); break;
			default:                   printf("\n%s\n\n","idk");                  break;
			}
		}
	}

	struct Component{
		GLuint id;
	};
	struct VAO:Component{
		VAO(){
			glGenVertexArrays(1,&id);
		}
		~VAO(){ glDeleteVertexArrays(1,&id);std::cout<<"vao deleted"<<std::endl; }
		void bind(){
			glBindVertexArray(id);
		}
		static void unbind(){ glBindVertexArray(0); }
		void setAttribute(int index,GLint size,int offset){
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,size,GL_FLOAT,GL_FALSE,sizeof(Geo::Vertex),(void*)(offset*sizeof(GLfloat)));
		}
	};
	struct VO:Component{
		GLenum buffertype;
		VO(GLenum b):buffertype(b) //GL_ARRAY_BUFFER for vertex, GL_ELEMENT_ARRAY_BUFFER for indices
		{
			glGenBuffers(1,&id);
		}
		~VO(){ glDeleteBuffers(1,&id);std::cout<<"vo deleted"<<std::endl; }
		void bind(){
			glBindBuffer(buffertype,id);
		}
		template<typename T>
		void load(std::vector<T>& data){
			bind();
			glBufferData(buffertype,data.size()*sizeof(T),data.data(),GL_STATIC_DRAW);
		}
	};
	struct Shader:Component{
		Shader(){
			id=glCreateProgram();
		}
		~Shader(){ glDeleteProgram(id);std::cout<<"shader deleted"<<std::endl; }
		void compileSubShader(GLenum type,const char* s){
			//std::cout<<s<<"\n\n";
			GLuint subid=glCreateShader(type);

			glShaderSource(subid,1,&s,NULL);
			glCompileShader(subid);

			GLint isCompiled = 0;
			glGetShaderiv(subid,GL_COMPILE_STATUS,&isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(subid,GL_INFO_LOG_LENGTH,&maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(subid,maxLength,&maxLength,&errorLog[0]);

				// Provide the infolog in whatever manor you deem best.
				// Exit with failure.
				glDeleteShader(subid); // Don't leak the shader.
				std::cout<<&errorLog[0]<<std::endl;
			}
			glAttachShader(id,subid);
		}
		void load(){
			glLinkProgram(id);
		}
		void use(){
			glUseProgram(id);
		}
		void update(){
			for (auto& U:uniforms)U.update();
		}		
		void setData(const char* label,float& v){
			uniforms.push_back(Uniform(*this,label,0,&v));
		}
		void setData(const char* label,glm::vec4& v){
			uniforms.push_back(Uniform(*this,label,3,&v[0]));
		}
		void setData(const char* label,glm::mat4& M){
			uniforms.push_back(Uniform(*this,label,4,&M[0][0]));
		}
		void setData(const char* label,int& ip){
			uniforms.push_back(Uniform(*this,label,5,&ip));
		}
	//private:
		struct Uniform:Component{
			Uniform(Shader& shader,const char* l,int type,void* p):v(p){
				id=glGetUniformLocation(shader.id,l);
				switch (type){
				case 0: func=vec1;break;
				case 1: func=vec2;break;
				case 2: func=vec3;break;
				case 3: func=vec4;break;
				case 4: func=mat4;break;
				case 5: func=int1;break;
				}
			}
			void update(){
				func(id,v);
			}
			GLuint id;void* v;
		private:
			void (*func)(GLuint up0,void* v0)=nullptr;
			static void vec1(GLuint id0,void* v0){ glUniform1fv(id0,1,(const GLfloat*)v0); }
			static void vec2(GLuint id0,void* v0){ glUniform2fv(id0,1,(const GLfloat*)v0); }
			static void vec3(GLuint id0,void* v0){ glUniform3fv(id0,1,(const GLfloat*)v0); }
			static void vec4(GLuint id0,void* v0){ glUniform4fv(id0,1,(const GLfloat*)v0); }
			static void mat4(GLuint id0,void* v0){ glUniformMatrix4fv(id0,1,false,(const GLfloat*)v0); }
			static void int1(GLuint id0,void* v0){ glUniform1i(id0,*(const int*)v0); }
		};
		std::vector<Uniform> uniforms;
	};
	struct Texture:Component{
		Texture(GLenum t):target(t)
		{
			glGenTextures(1,&id);
		}
		~Texture(){ glDeleteTextures(1,&id);std::cout<<"texture deleted"<<std::endl; }
		void bind(){
			glBindTexture(target,id);
		}
		void unbind(){
			glBindTexture(target,0);
		}
		void load(GLsizei width,GLsizei height,const char* pixels){
			bind();

			if (target==GL_TEXTURE_2D){
				glTexImage2D(GL_TEXTURE_2D,0,internalformat,width,height,0,format,type,pixels);
				GLint param=GL_LINEAR;
				if (generate_mips){ glGenerateMipmap(GL_TEXTURE_2D);param=GL_LINEAR_MIPMAP_LINEAR; }
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,param);//GL_LINEAR_MIPMAP_LINEAR
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			}

			unbind();
		}

		void loadCube(GLsizei width,GLsizei height,std::shared_ptr<Image> img[6]){
			bind();
			if (target==GL_TEXTURE_CUBE_MAP){
				for (int i=0;i<6;i++){
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
					//mipmap levels
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_BASE_LEVEL,0);
					glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAX_LEVEL,5);
					for (int i=0;i<6;i++){
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGBA8,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,img[i]->buffer.data());
					}


					glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				}
			}

			unbind();
		}
		GLenum target;
		GLint internalformat=GL_RGBA8;
		GLenum format=GL_BGR;
		GLenum type=GL_UNSIGNED_BYTE;
		bool generate_mips=true;
	};

	struct FBO:Component{
		FBO(){
			glGenFramebuffers(1,&id);
		}
		~FBO(){
			glDeleteFramebuffers(1,&id);std::cout<<"FBO deleted"<<std::endl;
		}
		void bind(){
			glBindFramebuffer(GL_FRAMEBUFFER,id);
		}
		static void unbind(){
			glBindFramebuffer(GL_FRAMEBUFFER,0);
		}
		void attachTexture(std::shared_ptr<Texture> T,GLenum attachment){
			textures.push_back(T);
			bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,GL_TEXTURE_2D,T->id,0);
			unbind();
		}
		std::vector< std::shared_ptr<Texture> > textures;
	};

	static std::shared_ptr<Shader> createShader(const char* fragS,const char* vertS){
		std::string fss=FileIO::readFile(fragS);
		std::string vss=FileIO::readFile(vertS);
		std::shared_ptr<Shader> shader=std::make_shared<Shader>();
		shader->compileSubShader(GL_FRAGMENT_SHADER,fss.c_str());
		shader->compileSubShader(GL_VERTEX_SHADER,vss.c_str());
		shader->load();
		//shader.use();
		return shader;
	}
};