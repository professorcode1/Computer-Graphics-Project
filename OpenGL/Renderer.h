#pragma once

#include <GL/glew.h>
#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#define ASSERT(x) if(!(x)) __builtin_trap();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))


bool GLLogCall(const char *function, const char* file, int line);
void GLClearError();

class Renderer{
public:
	void Draw(const VertexArray& va, const IndexBuffer& ib,const Shader &shader) const ;
	void Clear() const;
};