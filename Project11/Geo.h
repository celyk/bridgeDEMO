struct Geo{
	struct Vertex{
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec3 tangent;
	};

	std::vector<Vertex> vert;
	std::vector<GLuint> index;

	void invertWinding(){
		for (int i=0;i<vert.size();i+=3){
			Vertex V;
			V=vert[i+1];
			vert[i+1]=vert[i+2];
			vert[i+2]=V;
		}
	}
	static glm::vec3 rotateab(glm::vec3 p,glm::vec3 a,glm::vec3 b){
		glm::vec3 n=glm::cross(a,b);
		float l=length(n);
		if (l>0.f){
			glm::vec3 i=dot(p,n)/l/l*n;
			return i+(p-i)*glm::cos(glm::asin(l))+glm::cross(n,p);
		}
		else return p;
	}
	static void calcTangent(Vertex& tri0,Vertex& tri1,Vertex& tri2){
		glm::vec3 E1=(tri1.pos-tri0.pos);
		glm::vec3 E2=(tri2.pos-tri0.pos);
		glm::vec3 n=glm::normalize(glm::cross(E1,E2));
		glm::mat2x3 E(E1,E2);
		glm::mat2 U(glm::vec2(tri1.uv-tri0.uv),glm::vec2(tri2.uv-tri0.uv));

		glm::mat2x3 X=E*glm::inverse(U);

		glm::vec3 tan=glm::normalize(X[0]);

		tri0.tangent=rotateab(tan,n,tri0.normal);
		tri1.tangent=rotateab(tan,n,tri1.normal);
		tri2.tangent=rotateab(tan,n,tri2.normal);
	}
};
