


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
	const json sunParameters = parameter_json.at("sun parameters");
	const std::vector<float> sun_dir_vec = sunParameters.at("direction vector").get<std::vector<float>>();
	const glm::vec3 sun_direction = glm::vec3(sun_dir_vec.at(0),sun_dir_vec.at(1), sun_dir_vec.at(2)); 
	const float fog_densty = parameter_json.at("fog density").get<float>();
	
	const json tressParameter = parameter_json.at("tress parameters");
	const json cloudParameters = parameter_json.at("cloud parameters");

	Grid terrain(fog_densty, sun_direction, terrainParam, tressParameter, cloudParameters);
	const float terrain_max_height = terrainParam.at("output_increase_fctr_").get<float>() * terrainParam.at("Mountain Scale Factor").get<float>();
	glfwSwapInterval(1);


	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	const int skyColorR = parameter_json.at("sky color RGB")[0];
	const int skyColorG = parameter_json.at("sky color RGB")[1];
	const int skyColorB = parameter_json.at("sky color RGB")[2];
	glClearColor( skyColorR / 225.0f, skyColorG / 225.0f, skyColorB / 225.0f, 1.0f );
	// glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


	const json planeParam = parameter_json.at("plane parameters");
	const json planeToUse = parameter_json.at("planes").at(planeParam.at("plane"));

	Plane plane( 
		planeToUse.at("Plane OBJ file"), planeToUse.at("Plane Texuture file"), 
		planeParam.at("Camera Beind Distance"), planeParam.at("Camera Up Distance"), 
		planeParam.at("Camera ViewPoint Distance"), planeToUse.at("Plane Scale"), 
		planeParam.at("Plane Speed"),
		terrain_max_height,
		planeToUse.at("rotation angles").get<std::vector<float> >(),
		planeToUse.at("rotation axises").get<std::vector<std::string> >()
	);
	float 
		FOV = parameter_json.at("camera").at("FOV"), 
		NearPlane = parameter_json.at("camera").at("Near Plane"), 
		FarPlane = parameter_json.at("camera").at("Far Plane"),
		screenRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT
		;
	const glm::mat4 projection = glm::perspective(glm::radians(FOV), screenRatio, NearPlane, FarPlane);

	Skybox skybox;


	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	uint32_t time = 0;
	while (!glfwWindowShouldClose(window))
	{	
		start = end;
		plane.catchInputs(window);
		terrain.update(time++, plane.get_position(), fog_densty, sun_direction, terrainParam);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 VP, View;
		glm::vec3 camera_pos;
		std::tie(View, camera_pos) = plane.get_MVP_Matrix();
		VP = projection * View ;
		terrain.render(VP, camera_pos);
		

		plane.render(VP);


		skybox.render(View , projection);

		glfwSwapBuffers(window);
		glfwPollEvents();
		end = std::chrono::system_clock::now();
		if(false){
			float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			printf("Frame Rate :: %.1f fps\n", (1000000.0 / elapsed));
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
