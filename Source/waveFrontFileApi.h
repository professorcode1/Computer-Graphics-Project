#pragma once
#include "vertex_index_layout.h"
#include "../OpenGL/VertexArray.h" //for the assert macro


void write_to_file(GLuint VBO,GLuint EBO,int div, bool write_normals = true, const char* file_name = "computeShaderResult.OBJ");
void parse_complex_wavefront(const std::string& filename, std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer);