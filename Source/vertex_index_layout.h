#pragma once
struct vertex_t{
	float pos[3];
	float u;
	float nor[3];
	float v;
};
struct index_t{
	int indices[6];
};

#define CREATE_VERTEX_LAYOUT(x) \
x.Push<float>(3);\
x.Push<float>(1);\
x.Push<float>(3);\
x.Push<float>(1)
	