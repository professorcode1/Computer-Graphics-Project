#pragma once

#include "Renderer.h"
#include <GL/gl.h>
#include <bits/stdc++.h>
struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    unsigned char normalised;
static unsigned int GetSizeOfType(unsigned int type);
    
};
class VertexBufferLayout{
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
public:
    VertexBufferLayout() : m_Stride{0} {}
    template<typename T>
    void Push(unsigned int count){
        // static_assert(false);
    }

    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements;};
    inline unsigned int GetStride() const {return m_Stride;}
};