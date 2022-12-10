


#include "main.h"



const unsigned int SCREEN_WIDTH = 1920 ;
const unsigned int SCREEN_HEIGHT = 1024;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = true;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Compute Shaders", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);
	// GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    if(glewInit() != GLEW_OK){
        std::cout<<"Error"<<std::endl;  
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	std::ifstream parameter_file("parameters.json");
	json parameter_json;
	parameter_file >> parameter_json;
	json terrainParam = parameter_json.at("terrain parameters");
	json planeParam = parameter_json.at("plane parameters");

	VertexBufferLayout vertex_layout;
	CREATE_VERTEX_LAYOUT(vertex_layout);
	Terrain terain("shader_compute.glsl", "shader_vertex.glsl", "shader_fragment.glsl",vertex_layout, terrainParam.at("noise texture file"), terrainParam.at("atmosphere light damping constant"),
		terrainParam.at("atmosphere red"), terrainParam.at("atmosphere green"), terrainParam.at("atmosphere blue"),terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), terrainParam.at("lacunarity"), 
		terrainParam.at("persistance"),terrainParam.at("write to file"), terrainParam.at("divisions"), terrainParam.at("min_x"), terrainParam.at("max_x"), 
		terrainParam.at("min_z"), terrainParam.at("max_z"), terrainParam.at("Mountain Scale Factor")
		);

	glfwSwapInterval(1);
	
	glm::vec3 position (0,0.1716,0.5228);
	glm::vec3 focal_point (5,-0.02399,5);
	glm::vec3 view_up (-0.29207,0.897259,-0.331091);
	glm::vec3 plane_Position(3,3,3);




	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(185.0f / 225.0f, 235.0f / 225.0f, 255.0f / 225.0f, 1.0f);
	// glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	Plane plane("shader_collition.glsl", "shader_vertex_plane.glsl", "shader_fragment_plane.glsl", planeParam.at("Plane OBJ file"),
	planeParam.at("Plane Texuture file"), 1, planeParam.at("Camera Beind Distance"),
	 planeParam.at("Camera Up Distance"), planeParam.at("Camera ViewPoint Distance"),
	  planeParam.at("Plane Scale"), planeParam.at("Plane Speed"),vertex_layout);
	
	float 
		FOV = parameter_json.at("FOV"), 
		NearPlane = parameter_json.at("Near Plane"), 
		FarPlane = parameter_json.at("Far Plane"),
		screenRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT
		;

	while (!glfwWindowShouldClose(window))
	{	

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 VP, MVP_plane;
		glm::vec3 camera_pos;
		std::tie(VP, camera_pos) = plane.get_MVP_Matrix( FOV, NearPlane, FarPlane, screenRatio );
		
		terain.render(VP, camera_pos);

		plane.catchInputs(window);
		plane.render(VP);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();
}