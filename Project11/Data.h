#include"Geo.h"

struct FileIO{
	static std::string readFile(const char* filename) {
		std::ifstream in(filename,std::ios::in);
		std::stringstream buffer;
		buffer << in.rdbuf();//std::cout<<buffer.str()<<std::endl;
		return buffer.str();
	}
	static void loadOBJ(const char* filename,Geo& geo){
		std::vector<glm::vec3> pos;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec3> tangent;
		std::vector<long> index;

		std::ifstream in(filename,std::ios::in);
		if (!in)
		{
			std::cerr << "Cannot open " << filename << std::endl; exit(1);
		}
		//std::cout<<"ooo"<<std::endl;

		std::string line;
		while (std::getline(in,line))
		{
			if (line.substr(0,2) == "v ")
			{
				std::istringstream s(line.substr(2));
				glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;
				pos.push_back(v);
			}
			if (line.substr(0,3) == "vt ")
			{
				std::istringstream s(line.substr(3));
				glm::vec2 vt; s >> vt.x; s >> vt.y;

				uv.push_back(vt);
			}
			if (line.substr(0,3) == "vn ")
			{
				std::istringstream s(line.substr(3));
				glm::vec3 vn; s >> vn.x; s >> vn.y; s >> vn.z;
				normal.push_back(vn);
			}
			if (line.substr(0,2)=="f "){
				std::string str=line.substr(2);
				for (size_t t = 0; (t = str.find_first_of("/",t)) != std::string::npos; str[t++] = ' ');
				std::istringstream s(str);

				for (int i=0;i<9;i++){
					long f;
					s>>f;
					index.push_back(f-1L);
				}
			}
		}
		//sort data into geo struct
		for (long i=0;i<index.size();i+=3){	//per vertex
			Geo::Vertex V;
			V.pos=pos[index[i+0]];
			V.uv=uv[index[i+1]];
			V.normal=normal[index[i+2]];

			geo.vert.push_back(V);
			geo.index.push_back(i+0);
			geo.index.push_back(i+1);
			geo.index.push_back(i+2);
		}
		for (long i=0;i<geo.vert.size();i+=3){ //per face
			Geo::calcTangent(geo.vert[i],geo.vert[i+1],geo.vert[i+2]);
		}
	}
	static void loadBMP(const char* filepath,int& width,int& height,std::vector<char>& buffer){
#pragma pack(push, 1)
		struct FileHeader {
			uint16_t file_type{0x4D42};
			uint32_t file_size{0};
			uint16_t r1{0};
			uint16_t r2{0};
			uint32_t offset_data{54};
		};
		struct InfoHeader {
			uint32_t size{0};
			int32_t width{0};
			int32_t height{0};

			uint16_t planes{1};
			uint16_t bit_count{0};
			uint32_t compression{0};
			uint32_t size_image{0};
			int32_t xppm{0};
			int32_t yppm{0};
			uint32_t colors_used{0};
			uint32_t color_important{0};
		};
#pragma pack(pop)

		std::fstream myfile;
		myfile.open(filepath,std::ios::in|std::ios::binary);

		buffer=std::move(std::vector<char>((std::istreambuf_iterator<char>(myfile)),std::istreambuf_iterator<char>()));

		myfile.close();

		FileHeader* FH=(FileHeader*)&buffer[0];
		InfoHeader* IH=(InfoHeader*)&buffer[14];

		width=IH->width;
		height=IH->height;

		buffer.erase(buffer.begin(),buffer.begin()+54);

		/*buffer.resize(height,std::vector< std::vector<char> >(width,{char(0),char(0),char(0)}));

		for (int j=0;j<height;j++)
		for (int i=0;i<width;i++)
		for (int q=0;q<3;q++)
			buffer[j][i][q]=temp[i+j*width+q];*/
	}
};

struct Image{
	int width,height;
	std::vector< char > buffer;
	
	char& operator[](int index)
	{
		return buffer[index];
	}

	Image(int w,int h,int f):width(w),height(h){
		buffer.resize(width*height*3,char(f));
	}
	Image(std::string fp){
		const char* filepath=fp.c_str();
		size_t t=fp.find_last_of(".");
		std::string filetype=fp.substr(t+1);
		if (filetype=="bmp"){
			FileIO::loadBMP(filepath,width,height,buffer);
		}
		else{
			std::cout<<"invalid file '"+fp+"'.";
		}
	}

	char& fetch(int x,int y,int z){
		x=glm::clamp(x,0,width-1);
		y=glm::clamp(y,0,height-1);
		return buffer[(x+y*width)*3+z];
	}
	void flip(){
		for (int i=0;i<height/2;i++){
			/*std::vector< std::vector<char>[3] > c=buffer[i];
			buffer[i]=buffer[height-1-i];
			buffer[height-1-i]=c;*/
		}
	}
	void bumpToNormal(){
		Image tim(width,height,255);
		for(int j=0;j<height;j++)
		for (int i=0;i<width;i++){ //for every pixel
			char k[4]={fetch(i,j+1,0),fetch(i,j-1,2),fetch(i-1,j,2),fetch(i+1,j,2)};
			
			glm::vec3 v=glm::normalize(glm::vec3(k[3]-k[2],-(k[0]-k[1]),2)/2.f)*.5f+.5f;
			tim.fetch(i,j,2)=char(int(v.x*255.));
			tim.fetch(i,j,1)=char(int(v.y*255.));
			tim.fetch(i,j,0)=char(int(v.z*255.));
		}
		buffer=std::move(tim.buffer);
	}
};