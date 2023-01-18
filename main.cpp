


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
	const float fog_densty = parameter_json.at("fog density");
	Terrain terrain(
		terrainParam.at("noise texture file"), fog_densty,terrainParam["wave numbers active"], 
		terrainParam.at("rotation angle fractal ground"), terrainParam.at("output_increase_fctr_"), terrainParam.at("input_shrink_fctr_"), 
		terrainParam.at("lacunarity"), terrainParam.at("persistance"),terrainParam.at("write to file"), terrainParam.at("divisions"), 
		terrainParam.at("length of one side"), 
		terrainParam.at("Mountain Scale Factor"), sun_direction, glm::vec2(0.0, 0.0)
		);
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
		planeToUse.at("Rotation Along X axis"),planeToUse.at("Rotation Along Y axis"), 
		planeToUse.at("Rotation Along Z axis"),
		planeParam.at("Camera Beind Distance"), planeParam.at("Camera Up Distance"), 
		planeParam.at("Camera ViewPoint Distance"), planeToUse.at("Plane Scale"), 
		planeParam.at("Plane Speed"),
		terrain_max_height);
	float 
		FOV = parameter_json.at("camera").at("FOV"), 
		NearPlane = parameter_json.at("camera").at("Near Plane"), 
		FarPlane = parameter_json.at("camera").at("Far Plane"),
		screenRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT
		;
	const glm::mat4 projection = glm::perspective(glm::radians(FOV), screenRatio, NearPlane, FarPlane);

	const json tressParameter = parameter_json.at("tress parameters");
	Trees trees(
		tressParameter.at("tress per division"),tressParameter.at("tress scale"),
		tressParameter.at("align with normals"),
		terrain, sun_direction,fog_densty
		);
	Skybox skybox;

	const json cloudParameters = parameter_json.at("cloud parameters");
	Clouds clouds(
		cloudParameters.at("cloud per division").get<uint32_t>(),
        cloudParameters.at("scale").get<float>(),
        terrain,
        sun_direction,
        terrain_max_height + cloudParameters.at("distance above terrain").get<float>(),
		cloudParameters.at("input shrink factor"),
		cloudParameters.at("time shrink factor"),
		cloudParameters.at("velocity"),
		cloudParameters.at("rotational velocity degree")
	);
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	int32_t time = 0;
	while (!glfwWindowShouldClose(window))
	{	
		start = end;
		plane.catchInputs(window);
		clouds.flow(time++);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 VP, View;
		glm::vec3 camera_pos;
		std::tie(View, camera_pos) = plane.get_MVP_Matrix();
		VP = projection * View ;
		terrain.render(VP, camera_pos);
		
		trees.render(VP, camera_pos);

		clouds.render(VP);
		plane.render(VP);


		skybox.render(View , projection);

		glfwSwapBuffers(window);
		glfwPollEvents();
		end = std::chrono::system_clock::now();
		if(true){
			float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			printf("Frame Rate :: %.1f fps\n", (1000000.0 / elapsed));
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}
