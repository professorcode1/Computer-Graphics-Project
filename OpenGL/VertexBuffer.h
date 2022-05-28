#pragma once
class VertexBuffer{
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    VertexBuffer(const unsigned int m_RendererID);
    ~VertexBuffer();
    void Bind()const ;
    void Unbind()const ;
    inline unsigned int GetRenderedID() const {return m_RendererID;}

};