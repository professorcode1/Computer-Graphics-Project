


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

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raghav's Relaxing Flight Simulator", NULL, NULL);
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
	const json terrainParam = parameter_json.at("terrain parameters");
	const json planeParam = parameter_json.at("plane parameters");
	const json planeToUse = parameter_json.at("planes").at(planeParam.at("plane"));
	const json tressParameter = parameter_json.at("tress parameters");
	const json sunParameters = parameter_json.at("sun parameters");
	const std::vector<float> sun_dir_vec = sunParameters.at("direction vector").get<std::vector<float>>();
	const glm::vec3 sun_direction = glm::vec3(sun_dir_vec.at(0),sun_dir_vec.at(1), sun_dir_vec.at(2)); 
	const float fog_densty = parameter_json.at("fog density");
	Terrain terain(
		terrainParam.at("noise texture file"), fog_densty,terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
		terrainParam.at("lacunarity"), terrainParam.at("persistance"),terrainParam.at("write to file"), terrainParam.at("divisions"), 
		terrainParam.at("min_x"), terrainParam.at("max_x"), terrainParam.at("min_z"), terrainParam.at("max_z"), 
		terrainParam.at("Mountain Scale Factor"), sun_direction
		);

	glfwSwapInterval(1);


	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	const int skyColorR = parameter_json.at("sky color RGB")[0];
	const int skyColorG = parameter_json.at("sky color RGB")[1];
	const int skyColorB = parameter_json.at("sky color RGB")[2];
	glClearColor( skyColorR / 225.0f, skyColorG / 225.0f, skyColorB / 225.0f, 1.0f );
	// glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	Plane plane( 
		planeToUse.at("Plane OBJ file"), planeToUse.at("Plane Texuture file"), 
		planeToUse.at("Rotation Along X axis"),planeToUse.at("Rotation Along Y axis"), 
		planeToUse.at("Rotation Along Z axis"),
		planeParam.at("Camera Beind Distance"), planeParam.at("Camera Up Distance"), 
		planeParam.at("Camera ViewPoint Distance"), planeParam.at("Plane Scale"), 
		planeParam.at("Plane Speed"));
	float 
		FOV = parameter_json.at("camera").at("FOV"), 
		NearPlane = parameter_json.at("camera").at("Near Plane"), 
		FarPlane = parameter_json.at("camera").at("Far Plane"),
		screenRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT
		;


	Trees trees(
		tressParameter.at("tress per division"),tressParameter.at("tress scale"),
		terain, sun_direction,fog_densty
		);
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window))
	{	
		start = end;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 VP;
		glm::vec3 camera_pos;
		std::tie(VP, camera_pos) = plane.get_MVP_Matrix( FOV, NearPlane, FarPlane, screenRatio );
		
		terain.render(VP, camera_pos);

		trees.render(VP, camera_pos);

		plane.catchInputs(window);
		plane.render(VP);



		glfwSwapBuffers(window);
		glfwPollEvents();
		end = std::chrono::system_clock::now();
		float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		printf("Frame Rate :: %.1f fps\n", (1000000.0 / elapsed));
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}