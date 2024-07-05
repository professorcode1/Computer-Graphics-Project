#include "Skybox.hpp"

Skybox::Skybox(
    const std::vector<std::string> &images_name,
    const std::string &vertex_shader_file,
    const std::string &frgmnt_shader_file
):
tex(images_name),
shader(vertex_shader_file, frgmnt_shader_file)
{
    std::vector<float> skyboxVertices =
        {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

    this->vbo = new VertexBuffer(skyboxVertices.data(), skyboxVertices.size() * sizeof(float) );
    VertexBufferLayout layout;
    layout.Push<float>(3);
    vao.AddBuffer(*vbo, layout);
}

void Skybox::render(glm::mat4 view, const glm::mat4 &projecton) {
    // std::cout<<"render called"<<std::endl;
    glDepthFunc(GL_LEQUAL);
    view = glm::mat4(glm::mat3(view));
    shader.Bind();
    shader.SetUniformMat4f("view", view);
    shader.SetUniformMat4f("projection", projecton);
    shader.SetUniform1i("skybox", 0);
    vao.Bind();
    tex.Bind( 0, GL_TEXTURE_CUBE_MAP);        
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vao.Unbind();
    tex.Unbind();
    shader.Unbind();
    glDepthFunc(GL_LESS);
}


