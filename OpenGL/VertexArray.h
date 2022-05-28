#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
class VertexArray{
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    VertexArray(const unsigned int RendererID);
    ~VertexArray();

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
    void Bind() const ;
    void Unbind() const ;
    inline unsigned int GetRenderedID() const {return m_RendererID;}

};