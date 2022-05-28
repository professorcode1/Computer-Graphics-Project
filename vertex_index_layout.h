#pragma once
#include "OpenGL/VertexBufferLayout.h"
struct vertex_t{
	float pos[3];
	float u;
	float nor[3];
	float v;
};
struct index_t{
	int indices[6];
};

	