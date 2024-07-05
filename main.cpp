#include "main.h"

class FlightSimulator{
private:
	Skybox skybox;
public:
  Plane* plane;
  Grid* TerrainGrid;
  float fog_densty;
  glm::vec3 sun_direction;
  json terrainParam;
  FlightSimulator(){
	  // json parameter_json = json::parse(RenderingParameterJSON);


    const json planeParam = parameter_json.at("plane parameters");
	  const json planeToUse = parameter_json.at("planes").at(planeParam.at("plane"));
  	const json terrainParam = parameter_json.at("terrain parameters");
  	const float terrain_max_height = terrainParam.at("output_increase_fctr_").get<float>() * terrainParam.at("Mountain Scale Factor").get<float>();

	  this->plane = new Plane( 
	  	planeToUse.at("Plane OBJ file"), planeToUse.at("Plane Texuture file"), 
	  	planeParam.at("Camera Beind Distance"), planeParam.at("Camera Up Distance"), 
	  	planeParam.at("Camera ViewPoint Distance"), planeToUse.at("Plane Scale"), 
	  	planeParam.at("Plane Speed"),
	  	terrain_max_height,
	  	planeToUse.at("rotation angles").get<std::vector<float> >(),
	  	planeToUse.at("rotation axises").get<std::vector<std::string> >()
	  );

	  const json sunParameters = parameter_json.at("sun parameters");
	  const std::vector<float> sun_dir_vec = sunParameters.at("direction vector").get<std::vector<float>>();
	  const glm::vec3 sun_direction = glm::vec3(sun_dir_vec.at(0),sun_dir_vec.at(1), sun_dir_vec.at(2)); 
	  const float fog_densty = parameter_json.at("fog density").get<float>();
	  const json tressParameter = parameter_json.at("tress parameters");
	  const json cloudParameters = parameter_json.at("cloud parameters");
    this->fog_densty=fog_densty;
    this->sun_direction=sun_direction;
    this->terrainParam=terrainParam;
    std::cout<<"calling grid constructor from flight simulator constructor"<<std::endl;
    this->TerrainGrid = new Grid(fog_densty, sun_direction, terrainParam, tressParameter, cloudParameters);
  };
  void render(){
  	static uint32_t time = 0;

		glm::mat4 VP, View;
		glm::vec3 camera_pos;

		std::tie(View, camera_pos) = plane->get_MVP_Matrix();


    const glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), 
      float(1920)/1080, 
      0.1f, 
      10000.0f);

    this->skybox.render(View , projection);
		VP = projection * View ;
  	this->plane->render(VP);
    this->plane->catchInputs(KEY_VALUE_OUTSIDE_GLUT_RANGE);

    this->TerrainGrid->update(time++, plane->get_position(), fog_densty, sun_direction, terrainParam);
		this->TerrainGrid->render(VP, camera_pos);

  }

};
FlightSimulator* flight_simulator = nullptr;

void initGL() {

  glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  flight_simulator = new FlightSimulator();
}
void idle() {
  glutPostRedisplay();
}
void display() {
  flight_simulator->render();
}


void reshape(GLsizei width, GLsizei height) { 
  // if (height == 0) height = 1; 
  // GLfloat aspect = (GLfloat) width / (GLfloat) height;

  // glViewport(0, 0, width, height);

  // glMatrixMode(GL_PROJECTION); 
  // glLoadIdentity();
  // if (width >= height) { 
  //   gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
  // } else {
  //   gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
  // }
}

extern "C" 
void EMSCRIPTEN_KEEPALIVE change_plane(
  // const std::string &new_rendering_parameter_json
  const int new_plane
) {
  const json planeToUse = parameter_json.at("planes").at(std::to_string(new_plane));


  flight_simulator->plane->setup_plane(
    planeToUse.at("Plane OBJ file"),
    planeToUse.at("Plane Texuture file"),
    planeToUse.at("Plane Scale"),
  	planeToUse.at("rotation angles").get<std::vector<float> >(),
	  planeToUse.at("rotation axises").get<std::vector<std::string> >()
  );
}


extern "C"
void EMSCRIPTEN_KEEPALIVE change_terrain_parameters(
  const int divisions, 
  const int wave_numbers_active,
  const float rotation_angle_fractal_ground,
  const float output_increase_fctr_,
  const float input_shrink_fctr_,
  const float lacunarity,
  const float persistance
){
  	const json sunParameters = parameter_json.at("sun parameters");
	  const std::vector<float> sun_dir_vec = sunParameters.at("direction vector").get<std::vector<float>>();
	  const glm::vec3 sun_direction = glm::vec3(sun_dir_vec.at(0),sun_dir_vec.at(1), sun_dir_vec.at(2)); 
	  const float fog_densty = parameter_json.at("fog density").get<float>();
    std::vector<int> wave_numbers_active_arr;
    for(int wave_number = 0 ; wave_number < 32 ; wave_number++){
      if((wave_numbers_active & (1 << wave_number)) != 0){
        wave_numbers_active_arr.push_back(wave_number);
      }
    }
    json terrainParams = {
        {"divisions" , divisions}, 
        {"wave numbers active" , wave_numbers_active_arr},
        {"rotation angle fractal ground" , rotation_angle_fractal_ground},
        {"output_increase_fctr_" , output_increase_fctr_},
        {"input_shrink_fctr_", input_shrink_fctr_},
        {"length of one side", 20.0},
        {"lacunarity" , lacunarity},
        {"persistance" , persistance},
        {"Mountain Scale Factor" , 50.0},
    };
    parameter_json["terrain parameters"] = terrainParams;
	  const json tressParameter = parameter_json.at("tress parameters");
    delete flight_simulator->TerrainGrid;
    flight_simulator->TerrainGrid = new Grid(
      fog_densty, 
      sun_direction, 
      terrainParams, 
      tressParameter, 
      json({})
    );
    
}

void key(int key, int x, int y) {
  // if(flight_simulator == nullptr) return ;
  flight_simulator->plane->catchInputs(key);  

 
}; 

/* Mainfunction: GLUT runs as a console application starting at main() */
int main(int argc, char ** argv) {
  glutInit( & argc, argv); // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH);

  glutInitWindowSize(700, 394); // Set the window's initial width & height - non-square
  glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
  glutCreateWindow("Namma Metro 1SK16CS017"); // Create window with the given title
  glutDisplayFunc(display); // Register callback handlerfor window re-paint event
  glutReshapeFunc(reshape); // Register callback handlerfor window re-size event
  glutIdleFunc(idle); // Register callback handler if no other event
  initGL();
  glutSpecialFunc(key); // Our own OpenGL initialization
  glutMainLoop(); // Enter the infinite event-processing loop
  return 0;
} 
