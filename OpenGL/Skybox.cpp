#include "Skybox.hpp"

Skybox::Skybox(
    const std::vector<std::string> &images_name,
    const std::string &vertex_shader_file,
    const std::string &frgmnt_shader_file
):
shader(vertex_shader_file, frgmnt_shader_file)
{
    float skyboxVertices[] =
        {
        	//   Coordinates
        	-1.0f, -1.0f,  1.0f,//        7--------6
        	 1.0f, -1.0f,  1.0f,//       /|       /|
        	 1.0f, -1.0f, -1.0f,//      4--------5 |
        	-1.0f, -1.0f, -1.0f,//      | |      | |
        	-1.0f,  1.0f,  1.0f,//      | 3------|-2
        	 1.0f,  1.0f,  1.0f,//      |/       |/
        	 1.0f,  1.0f, -1.0f,//      0--------1
        	-1.0f,  1.0f, -1.0f
        };

    unsigned int skyboxIndices[] =
        {
        	// Right
        	1, 2, 6,
        	6, 5, 1,
        	// Left
        	0, 4, 7,
        	7, 3, 0,
        	// Top
        	4, 5, 6,
        	6, 7, 4,
        	// Bottom
        	0, 3, 2,
        	2, 1, 0,
        	// Back
        	0, 1, 5,
        	5, 4, 0,
        	// Front
        	3, 7, 6,
        	6, 2, 3
        };
    std::cout<<"In Constructor of skybox"<<std::endl;
    std::cout<<"1"<<std::endl;
    this->vbo = new VertexBuffer(skyboxVertices, sizeof(float) *  6 );
    this->ibo = new IndexBuffer(skyboxIndices, 12);
    std::cout<<"2"<<std::endl;
    VertexBufferLayout layout;
    layout.Push<float>(3);
    vao.AddBuffer(*vbo, layout);
    vao.AddElementBuffer(*ibo);
    std::cout<<"3"<<std::endl;
    tex = new Texture(images_name);
    std::cout<<"4"<<std::endl;
}

void Skybox::render(glm::mat4 view, const glm::mat4 &projecton) {
    glDepthFunc(GL_LEQUAL);
    view = glm::mat4(glm::mat3(view));
    shader.Bind();
    shader.SetUniformMat4f("view", view);
    shader.SetUniformMat4f("projection", projecton);
    shader.SetUniform1i("skybox", 0);
    vao.Bind();
    tex->Bind( 0, GL_TEXTURE_CUBE_MAP);
    GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount() , GL_UNSIGNED_INT, nullptr));
    vao.Unbind();
    tex->Unbind();
    shader.Unbind();
    glDepthFunc(GL_LESS);
}


