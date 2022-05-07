#include "main.h"

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    
    
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
        100.0f, 100.0f, 0.0f, 0.0f,//0
        200.0f, 100.0f, 1.0f, 0.0f,//1
        200.0f, 200.0f, 1.0f, 1.0f,//2
        100.0f, 200.0f, 0.0f, 1.0f//3
    };
    const int nm_vrtx_pnts = 4 * 4;
    unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
    };
    const int nm_indices = 6;
    
    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 1.0f, 540.0f , -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0 ,0));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200 ,0));
    glm::mat4 mvp = proj * view * model ; 
    
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    VertexArray va;
    VertexBuffer vb(positions, nm_vrtx_pnts * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb,layout);
    IndexBuffer ib(indices, nm_indices);
    
    Shader shader("vertex.glsl" , "fragment.glsl");
    shader.Bind();
    float r = 0.0f;
    float increment = 0.05f;
    Texture texture("Apple-logo.png"); 
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);
    shader.SetUniformMat4f("u_MVP", mvp);
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();
    
    Renderer renderer;

    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glm::vec3 translation = glm::vec3(200, 200 ,0);
    while(!glfwWindowShouldClose(window)){
        renderer.Clear();
   // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glm::mat4 model = glm::translate(glm::mat4(1.0f),translation );
        glm::mat4 mvp = proj * view * model ; 
        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3, 0.8, 1.0);        
        shader.SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(va,ib,shader);
    
	    if (r > 1.0f)
	    	increment = -0.05f;
	    else if ( r < 0.0f)
	        increment =  0.05f;
        
	    r += increment;

        {
            ImGui::Begin("Hello, world!");   
            ImGui::SliderFloat3("Translation", &translation.x, 0.0f, 960.0f);
            ImGui::Text("Application average %.3f ms/frame(%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();
    return 0;
}