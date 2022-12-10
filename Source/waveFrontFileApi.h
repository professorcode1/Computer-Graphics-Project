#pragma once

void parse_simple_wavefront(const std::string& filename, std::vector<vertex_t> &vertices, std::vector< unsigned int> &index_buffer);
void write_to_file(GLuint VBO,GLuint EBO,int div, bool write_normals = true, const char* file_name = "computeShaderResult.OBJ");
