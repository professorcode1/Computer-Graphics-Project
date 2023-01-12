#pragma once
struct vertex_t{
	float pos[3];
	float nor[3];
	float uv[2];
};
struct index_t{
	int indices[6];
};

#define CREATE_SIMPLE_VERTEX_LAYOUT(x) \
x.Push<float>(3);\
x.Push<float>(3);\
x.Push<float>(2)
	
