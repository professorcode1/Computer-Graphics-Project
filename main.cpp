#include "main.h"
#include "Cherno_OpenGL_Library/Renderer.h"

int main(){
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    float x_start{0}, x_end{100}, z_start{0}, z_start{100};
    while(!glfwWindowShouldClose(window)){
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
    
        // GLCall(glDrawElements(GL_TRIANGLES, nm_indices_rect, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}