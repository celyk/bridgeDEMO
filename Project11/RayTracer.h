class RayTracer{
public:
	static glm::vec3 intersectTriangle(glm::vec3 ro,glm::vec3 rd,glm::vec3 a,glm::vec3 b,glm::vec3 c){
		glm::vec3 v=glm::inverse(glm::mat3(-rd,b-a,c-a))*(ro-a);
		if (v.x<0.||v.y<0.||v.z<0.||v.y+v.z>1.)return glm::vec3(1e10,0,0);
		return v;
	}
	template<typename T>
	static T binterp(glm::vec2 uv,T a,T b,T c){
		return a*(1.f-uv.x-uv.y)+b*uv.x+c*uv.y;
	}
	static glm::vec4 intersectGeo(glm::vec3 ro,glm::vec3 rd,std::shared_ptr<Geo> geo){
		glm::vec3 t=glm::vec3(1e10,0,0);int index=0;
		for (int i=0;i<geo->vert.size();i+=3){
			glm::vec3 v=intersectTriangle(ro,rd,geo->vert[i].pos,geo->vert[i+1].pos,geo->vert[i+2].pos);
			if (v.x<t.x){
				t=v;
				index=i;
			}
		}
		Geo::Vertex V;
		V.pos=binterp(glm::vec2(t.y,t.z),geo->vert[index].pos,geo->vert[index+1].pos,geo->vert[index+2].pos);
		V.uv=binterp(glm::vec2(t.y,t.z),geo->vert[index].uv,geo->vert[index+1].uv,geo->vert[index+2].uv);
		V.normal=glm::normalize(glm::cross((geo->vert[index+1].pos-geo->vert[index].pos),(geo->vert[index+2].pos-geo->vert[index].pos)));

		return glm::vec4(V.normal,t.x);
		//V.normal=binterp(glm::vec2(t.y,t.z),geo->vert[index].normal,geo->vert[index+1].normal,geo->vert[index+2].normal);
		//V.tangent=binterp(glm::vec2(t.y,t.z),geo->vert[index].tangent,geo->vert[index+1].tangent,geo->vert[index+2].tangent);
	}
	static float triangleSDF(glm::vec3 p,glm::vec3 v1,glm::vec3 v2,glm::vec3 v3){
		glm::vec3 v21 = v2 - v1; glm::vec3 p1 = p - v1;
		glm::vec3 v32 = v3 - v2; glm::vec3 p2 = p - v2;
		glm::vec3 v13 = v1 - v3; glm::vec3 p3 = p - v3;
		glm::vec3 nor = glm::cross(v21,v13);

		glm::vec3 e1 = v21 * glm::clamp(glm::dot(v21,p1) / glm::dot(v21,v21),0.f,1.f) - p1;
		glm::vec3 e2 = v32 * glm::clamp(glm::dot(v32,p2) / glm::dot(v32,v32),0.f,1.f) - p2;
		glm::vec3 e3 = v13 * glm::clamp(glm::dot(v13,p3) / glm::dot(v13,v32),0.f,1.f) - p3;

		float d= sqrt( // inside/outside test
			(glm::sign(glm::dot(glm::cross(v21,nor),p1)) +
				glm::sign(glm::dot(glm::cross(v32,nor),p2)) +
				glm::sign(glm::dot(glm::cross(v13,nor),p3)) < 2.0)
			?
			// 3 edges
			glm::min(glm::min(glm::dot(e1,e1),glm::dot(e2,e2)),glm::dot(e3,e3))
			:
			// 1 face
			glm::dot(nor,p1) * glm::dot(nor,p1) / glm::dot(nor,nor));

		return d;
	}
	static glm::vec4 distanceGeo(glm::vec3 p,Geo& geo){
		float d=1e10;
		int index=0;
		for (int i=0;i<geo.vert.size();i+=3){
			float d0=triangleSDF(p,geo.vert[i].pos,geo.vert[i+1].pos,geo.vert[i+2].pos);
			if (d0<d){
				d=d0;index=i;
			}
		}
		glm::vec3 n=glm::normalize(glm::cross((geo.vert[index+1].pos)-(geo.vert[index].pos),(geo.vert[index+2].pos)-(geo.vert[index].pos)));
		return glm::vec4(n,d);
	}
};