#include "Texture.h"
#include <GL/gl.h>
#include "stb_image.h"



Texture::Texture(const std::string& path) : m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0){
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
    std::cout<<"Height::"<<m_Height<<"\t Width::"<<m_Width<<std::endl;
    GLCall(glGenTextures( 1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    // std::cout<<path.c_str()<<m_RendererID<<std::endl;
    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
    else{
        std::cout<<"unable to load image for "<<m_FilePath<<std::endl;
    }

}


Texture::Texture(const std::vector<std::string>& paths){
    std::cout<<"calling glGenTextures"<<std::endl;
    GLCall(glGenTextures(1, &m_RendererID));
    std::cout<<"calling glBindTexture"<<std::endl;
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(0);
    for (unsigned int i = 0; i < paths.size(); i++)
    {
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // std::cout<<width <<" "<<height<<std::endl;
            std::cout<<"calling glTexImage2D"<<std::endl;
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            ));
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << paths[i] << std::endl;
            assert(false);
            stbi_image_free(data);
        }
    }
    std::cout<<"calling glTexParameteri"<<std::endl;
    
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

Texture::Texture(Texture &&other) noexcept:
    m_FilePath{std::move(other.m_FilePath)}
{
    if(&other == this){
        return ;
    }
    this->m_RendererID = other.m_RendererID;
    this->m_Width = other.m_Width;
    this->m_Height = other.m_Height;
    this->m_BPP = other.m_BPP;
    other.m_RendererID = 0;
}


Texture::~Texture(){
    // std::cout<<"Deleting Texture \t"<<this->m_FilePath<<this->m_RendererID<<std::endl;
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    // std::cout<<"m_RendererID"<<m_RendererID<<std::endl;
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}


void Texture::Bind(unsigned int slot, uint32_t texture_type) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(texture_type, m_RendererID));
}

void Texture::Unbind() const {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

// Texture::Texture(uint32_t SSBO_id){
//     GLCall(glGenTextures(1, &m_RendererID));
//     GLCall(glBindTexture(GL_TEXTURE_BUFFER, m_RendererID));
//     // GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, SSBO_id)); 
// }