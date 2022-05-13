#include "main.h"
#include "Cherno_OpenGL_Library/Camera.h"
#include "Cherno_OpenGL_Library/Renderer.h"
#include "Cherno_OpenGL_Library/Shader.h"
#include "Cherno_OpenGL_Library/VertexBuffer.h"
#include "Cherno_OpenGL_Library/VertexBufferLayout.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/trigonometric.hpp>

int main(){
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    constexpr int width = 640, height = 480;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        std::cout<<"Could not Create window"<<std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if(glewInit() != GLEW_OK)
        std::cout<<"Error"<<std::endl;  

    std::cout<<glGetString(GL_VERSION) << std::endl;
    GLCall(glClearColor(0.5f, 0.6f, 0.7f, 0.0f));    
    Camera camera(width, height, glm::vec3(0.0f, 1000.0f, 0.0f), glm::vec3(50.0f, -20.0f, 50.0f), glm::vec3(0.707f,0,0.707f));
    glm::vec4 fir_Line;
    glm::vec4 sec_Line;
    glm::vec4 thr_Line;
    glm::vec4 for_Line;
    glm::mat4 matrix_transform;
    std::tie(matrix_transform, fir_Line, sec_Line, thr_Line, for_Line) = camera.Matrix(45.0f, 100, 2000);

    glm::vec4 fir_Line_4 = fir_Line;
    glm::vec4 sec_Line_4 = sec_Line;
    glm::vec4 thr_Line_4 = thr_Line;
    glm::vec4 for_Line_4 = for_Line;

    auto fir_Line_4_nrm = matrix_transform * fir_Line_4;
    fir_Line_4_nrm /= fir_Line_4_nrm.w;
    auto sec_Line_4_nrm = matrix_transform * sec_Line_4;
    sec_Line_4_nrm /= sec_Line_4_nrm.w;
    auto thr_Line_4_nrm = matrix_transform * thr_Line_4;
    thr_Line_4_nrm /= thr_Line_4_nrm.w;
    auto for_Line_4_nrm = matrix_transform * for_Line_4;
    for_Line_4_nrm /= for_Line_4_nrm.w;

    std::cout<<"End result"<<std::endl;
    std::cout<< glm::to_string(fir_Line_4_nrm)<<"\n"
             << glm::to_string(sec_Line_4_nrm)<<"\n"
             << glm::to_string(thr_Line_4_nrm)<<"\n"
             << glm::to_string(for_Line_4_nrm)<<std::endl;
    int *vertexBuffer = new int[2 * 200 * 200];
    for(int iter_i = -100 ; iter_i < 100 ; iter_i++){
        for(int iter_j = -100 ; iter_j < 100 ; iter_j++){
            vertexBuffer[2 * ((iter_i + 100) + 200 * (iter_j + 100)) + 0 ] = iter_i;
            vertexBuffer[2 * ((iter_i + 100) + 200 * (iter_j + 100)) + 1 ] = iter_j;
        }
    }
    glEnable(GL_PROGRAM_POINT_SIZE);
    VertexBuffer vb(vertexBuffer, 40000 * sizeof(int));
    VertexBufferLayout l;
    l.Push<float>(2);
    VertexArray va;
    va.AddBuffer(vb, l);
    va.Bind();
    Shader s("vertex_MarchingCubes.glsl", "fragment_MarchingCubes.glsl");
    s.SetUniformMat4f("transformation", matrix_transform);
    //heres an idea. In the vertex shades, decide the number of blocks (i.e. length along y, find)
    // std::cout<<glm::to_string(matrix_transform * glm::vec4(1,0,1,1)) << std::endl;
    // while(!glfwWindowShouldClose(window)){
    //     GLCall(glClear(GL_COLOR_BUFFER_BIT));
    
    //     // GLCall(glDrawElements(GL_TRIANGLES, nm_indices_rect, GL_UNSIGNED_INT, nullptr));
    //  	glDrawArrays(GL_POINTS, 0, 40000);

    //     glfwSwapBuffers(window);

    //     glfwPollEvents();
    // }


    glfwTerminate();
    return 0;
}