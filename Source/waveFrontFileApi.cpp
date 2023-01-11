
#include <math.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <vector>
#include "waveFrontFileApi.h"
void write_to_file(GLuint VBO,GLuint EBO,int div, bool write_normals, const char* file_name){
	using namespace std;
    vertex_t* data_buffer = new vertex_t[( div + 1 ) * ( div + 1 )];
	index_t* indices_buffer = new index_t[ div * div ];
	glGetNamedBufferSubData(VBO, 0, ( div + 1 ) * ( div + 1 ) * sizeof(struct vertex_t), data_buffer);
	glGetNamedBufferSubData(EBO, 0, div * div * sizeof(struct index_t), indices_buffer);
	std::ofstream file(file_name);
	int progress = -1;
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) ; iter_i++){
		if(ceil((float) iter_i * 33.3f / (( div + 1 ) * ( div + 1 ))) > progress){
			progress++;
			printf("%d \n", progress);
		}
		file<<"v "<<data_buffer[iter_i].pos[0]<<" "<<data_buffer[iter_i].pos[1]<<" "<<data_buffer[iter_i].pos[2]<<'\n';
	}
	for(int iter_i = 0 ; iter_i < ( div + 1 ) * ( div + 1 ) * write_normals ; iter_i++){
		if(ceil(33.3f + (float)iter_i * 33.3f / (( div + 1 ) * ( div + 1 ))) > progress){
			progress++;
			printf("%d \n", progress);
		}
		file<<"vn "<<data_buffer[iter_i].nor[0]<<" "<<data_buffer[iter_i].nor[1]<<" "<<data_buffer[iter_i].nor[2]<<'\n';
	}

	for(int iter_i = 0 ; iter_i < div * div ; iter_i++){
		if(ceil(66.6 + 33 * (float) iter_i  / (div*div)) > progress){
			progress++;
			printf("%d \n", progress);
		}

		file<<"f "<<indices_buffer[iter_i].indices[0] + 1 <<" "<<indices_buffer[iter_i].indices[1] + 1 <<" "<<indices_buffer[iter_i].indices[2] + 1 <<'\n';
		file<<"f "<<indices_buffer[iter_i].indices[3] + 1 <<" "<<indices_buffer[iter_i].indices[4] + 1 <<" "<<indices_buffer[iter_i].indices[5] + 1 <<'\n';
	}
	file.close();
}

static void string_split( std::string &line, std::vector<std::string> &split, std::string delimiter ){
	size_t pos = 0;
    while ((pos = line.find(delimiter)) != std::string::npos) {
        split.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
    }
	split.push_back(line);
}

static void wavefront_parse_triangle_and_push(
	std::vector<std::string> &split, std::vector<float> &vertex_buffer,
	std::vector<float> &texture_buffer,std::vector<float> &normal_buffer,
	std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer,
	unsigned int &index_count
){
	std::vector<std::string> split_split;
	vertex_t hlpr_ver;
	// std::cout<<"line\t"<<line<<std::endl;
	// std::cout<<split[1]<<'\t' <<split[2]<<'\t'<<split[3]<<'\t'<<std::endl;
	string_split(split[1], split_split, "/");
	hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
	hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
	hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
	
	hlpr_ver.uv[0] = texture_buffer.at(2 * (std::stoi(split_split[1]) - 1) + 0);
	hlpr_ver.uv[1] = texture_buffer.at(2 * (std::stoi(split_split[1]) - 1) + 1);
	
	hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
	hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
	hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
	vertices.push_back(hlpr_ver);
	index_buffer.push_back(index_count);
	index_count++;
	split_split.clear();
	
	string_split(split[2], split_split, "/");
	hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
	hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
	hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
	
	hlpr_ver.uv[0] = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 0);
	hlpr_ver.uv[1] = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 1);
	
	hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
	hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
	hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
	vertices.push_back(hlpr_ver);
	index_buffer.push_back(index_count);
	index_count++;
	split_split.clear();
	string_split(split[3], split_split, "/");
	hlpr_ver.pos[0] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 0);
	hlpr_ver.pos[1] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 1);
	hlpr_ver.pos[2] = vertex_buffer.at(3 * (std::stoi(split_split[0]) - 1 ) + 2);
	
	hlpr_ver.uv[0] = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 0);
	hlpr_ver.uv[1] = texture_buffer.at(2 * (std::stoi(split_split[1])- 1) + 1);
	
	hlpr_ver.nor[0] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 0);
	hlpr_ver.nor[1] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 1);
	hlpr_ver.nor[2] = normal_buffer.at(3 * (std::stoi(split_split[2]) - 1 ) + 2);
	vertices.push_back(hlpr_ver);
	index_buffer.push_back(index_count);
	index_count++;
}


void parse_complex_wavefront(const std::string& filename, std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer){
	std::vector<float> vertex_buffer;
	std::vector<float> texture_buffer;
	std::vector<float> normal_buffer;
	std::string line;
	std::ifstream file(filename);
	std::vector<std::string> split;
	unsigned int index_count = 0;
	while (getline (file, line)) {
		// std::cout<<line<<std::endl;
		split.clear();
		if(line.size() < 2)
			continue;
		string_split(line, split," ");
		if(split[0] == "v"){
			vertex_buffer.push_back(std::stof(split[1]));
			vertex_buffer.push_back(std::stof(split[2]));
			vertex_buffer.push_back(std::stof(split[3]));
		}else if(split[0] == "vn"){
			normal_buffer.push_back(std::stof(split[1]));
			normal_buffer.push_back(std::stof(split[2]));
			normal_buffer.push_back(std::stof(split[3]));
		}else if(split[0] == "vt"){
			texture_buffer.push_back(std::stof(split[1]));
			texture_buffer.push_back(std::stof(split[2]));
		}else if(split[0] == "f"){
			for(int i=3 ; i<split.size(); i++){
				std::vector<std::string> triangle({"f", split.at(1), split.at( i - 1 ), split.at( i )});
				wavefront_parse_triangle_and_push(
					triangle, vertex_buffer, texture_buffer, normal_buffer, vertices, index_buffer, index_count);
			}
		}
	}
	file.close();

}