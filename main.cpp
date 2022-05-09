#include "main.h"



// int main(void)
// {
//     GLFWwindow* window;

//     if (!glfwInit())
//         return -1;

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    
//     window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
//     if (!window)
//     {
//         glfwTerminate();
//         std::cout<<"Could not Create window"<<std::endl;
//         return -1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSwapInterval(1);

//     if(glewInit() != GLEW_OK)
//         std::cout<<"Error"<<std::endl;  

//     std::cout<<glGetString(GL_VERSION) << std::endl;

//     float positions[] = {
//     0.651186, -1.028969, 0.837470, 0.685202,
//     0.585184, -0.148934, 0.936605, 0.766313,
//     0.279741, -0.109691, 0.606592, 0.496302,
//     };
//     const int nm_vrtx_pnts = 3 * 4;
//     unsigned int indices[] = {
// 		0, 1, 2,
// 		0, 1, 2,
//     };
//     const int nm_indices = 6;

//     VertexArray va;
//     VertexBuffer vb(positions, nm_vrtx_pnts * sizeof(float));
//     VertexBufferLayout layout;
//     layout.Push<float>(4);
//     va.AddBuffer(vb,layout);
//     IndexBuffer ib(indices, nm_indices);
    
//     Shader shader("vertex.glsl" , "fragment.glsl");

//     float r = 0.0f;
//     float increment = 0.05f;

//     va.Unbind();
//     vb.Unbind();
//     ib.Unbind();
//     shader.Unbind();

//     while(!glfwWindowShouldClose(window)){
//         GLCall(glClear(GL_COLOR_BUFFER_BIT));

//         shader.Bind();
//         shader.SetUniform4f("u_Color", r, 0.3, 0.8, 1.0);
// 	    // GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
// 	    // GLCall(glEnableVertexAttribArray(0));
// 	    // GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));
        
//         // GLCall(glBindVertexArray(vao));
//         va.Bind();
//         ib.Bind();

    
// 	    if (r > 1.0f)
// 	    	increment = -0.05f;
// 	    else if ( r < 0.0f)
// 	        increment =  0.05f;

// 	    r += increment;
//         GLCall(glClear(GL_COLOR_BUFFER_BIT));
        
//         GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

//         glfwSwapBuffers(window);

//         glfwPollEvents();
//     }


//     glfwTerminate();
//     return 0;
// }

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    int width = 640, height = 480;
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

    Scene scene(0,200,0,200,
    [](float a,float b , float x){
        float lambda = std::min(1.0f, std::max(0.0f, (x - b )/ (a - b)));
        return lambda * lambda * ( 3 - 2 * lambda );
        },
    [](float i, float j){
        float u = 50 * i / M_PI, v = 50 * j / M_PI ;
        float interm = u * v * ( u + v );
        return 2 * (interm - (long)interm) - 1 ;
    },
     37.0f * M_PI / 180.0f, 15, 
     0.50f, 0.10f, 0.50f, 
     20, 1,1);
    //  scene.block_untill_all_worker_threads_finish(100);
    scene.block_untill_all_worker_threads_finish(100);    
    // scene.print_state();
    scene.dump_obj( "firstDump.OBJ");
    // while(!glfwWindowShouldClose(window)){
    //     GLCall(glClear(GL_COLOR_BUFFER_BIT));
    //     scene.render(mvp);
    //     glfwSwapBuffers(window);
    // // GLLogCall("a","b",5);
    //     glfwPollEvents();
    // }


    glfwTerminate();
    return 0;
}
