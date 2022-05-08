#include "main.h"
#include <cmath>



int main(void)
{
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

    float positions[] = {
        -0.5f,-0.5f,//0
         0.5f,-0.5f,//1
         0.5f, 0.5f,//2
        -0.5f, 0.5f,//3
    };
    const int nm_vrtx_pnts = 4 * 2;
    unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
    };
    const int nm_indices = 6;

    VertexArray va;
    VertexBuffer vb(positions, nm_vrtx_pnts * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb,layout);
    IndexBuffer ib(indices, nm_indices);
    
    Shader shader("vertex.glsl" , "fragment.glsl");

    float r = 0.0f;
    float increment = 0.05f;

    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();
    Scene s(0,10,0,10,
    [](float a,float b , float x){
        float lambda = std::min(1.0f, std::max(0.0f, (x - b )/ (a - b)));
        return lambda * lambda * ( 3 - 2 * lambda );
        },
    [](float i, float j){
        float u = 50 * i / M_PI, v = 50 * j / M_PI ;
        float interm = u * v * ( u + v );
        return 2 * (interm - (long)interm) - 1 ;
    },
     37.0f * M_PI / 180.0f, 10, 0.1f);

    while(!glfwWindowShouldClose(window)){
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3, 0.8, 1.0);
	    // GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	    // GLCall(glEnableVertexAttribArray(0));
	    // GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
        
        // GLCall(glBindVertexArray(vao));
        va.Bind();
        ib.Bind();

    
	    if (r > 1.0f)
	    	increment = -0.05f;
	    else if ( r < 0.0f)
	        increment =  0.05f;

	    r += increment;
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(window);

        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
