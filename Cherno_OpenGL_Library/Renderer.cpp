#include "Renderer.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include <iostream>
bool GLLogCall(const char *function, const char* file, int line){
    bool isErrorFree = true;
    while(GLenum error = glGetError()){
		std::cout<<"[OpenGL Error](0x" <<std::hex << error<<")"<<std::dec<<std::endl;
		std::cout<<"Function call :: "<< function << std::endl;
		std::cout<<file<<" : "<<line<<std::endl;
		isErrorFree = false;
	}
	return isErrorFree;
}
void GLClearError(){
	while(glGetError() != GL_NO_ERROR)std::cout<<"The error buffer being cleared has an error"<<std::endl;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib,const Shader &shader) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
void Renderer::Clear() const {
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
