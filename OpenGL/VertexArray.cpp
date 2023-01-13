#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
VertexArray::VertexArray(){
    GLCall(glGenVertexArrays(1, &m_RendererID));
}
VertexArray::VertexArray(const unsigned int RendererID):m_RendererID(RendererID){}
VertexArray::~VertexArray(){
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
    std::cout<<"Deleted "<<m_RendererID<<"as a render id"<<std::endl;
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout){
    GLCall(glBindVertexArray(m_RendererID));
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int ofset = 0;
    for(unsigned int i=0 ; i< elements.size() ; i++){
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count , element.type, element.normalised, layout.GetStride(), (const void*)ofset));
        ofset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}
VertexArray::VertexArray(VertexArray &&other) noexcept {
    if(&other == this){
        return ;
    }
    this->m_RendererID = other.m_RendererID;
    other.m_RendererID = 0;
}
void VertexArray::AddElementBuffer(const IndexBuffer &ib) const {
    GLCall(glBindVertexArray(m_RendererID));
    ib.Bind();
}

void VertexArray::Bind()const {
    std::cout<<"Bind called for"<<m_RendererID<<std::endl;
    GLCall(glBindVertexArray(m_RendererID));
}
void VertexArray::Unbind()const {
    GLCall(glBindVertexArray(0));
}