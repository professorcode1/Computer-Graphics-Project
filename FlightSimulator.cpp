#include <GL/glut.h>
#include <emscripten.h>
#include <stdio.h>
#include "IncGLM.hpp"
#include "OpenGL/Skybox.hpp"
#include "Source/plane.h"
#include "FOSS_Code/json.hpp"
#include "Source/Grid.h"
#include <cstring>
using json = nlohmann::json;

constexpr int KEY_VALUE_OUTSIDE_GLUT_RANGE = 1000;
char const * const RenderingParameterJSON = R"(
{
    "screen width" : 500,
    "screen height" : 500,
    "sky color RGB" : [135,206,235],
    "fog density" : 0.00025,
    "terrain parameters":{
        "divisions" : 10, 
        "wave numbers active" : [0,1,2,3,4,5,6,7,8,9,10],
        "rotation angle fractal ground" : 36.87,
        "output_increase_fctr_" : 3.0,
        "input_shrink_fctr_": 3.0,
        "length of one side":20.0,
        "lacunarity" : 2.0,
        "persistance" : 3.1,
        "write normals" : false,
        "write to file" : false,
        "noise texture file" : "assets/Ground Textures/soil.jpg",
        "Mountain Scale Factor" : 50
    },
    "plane parameters":{
        "plane":"two",
        "Camera Beind Distance" : 20.0,
        "Camera Up Distance" : 3.50,
        "Camera ViewPoint Distance" : 1.0,
        "Plane Speed" : 5      
    },
    "cloud parameters":{
        "distance above terrain" : 350,
        "cloud per division" : 50,
        "scale" : 0.4,
        "input shrink factor" : 500.0,
        "time shrink factor" : 600.0,
        "velocity" : 0.15,
        "rotational velocity degree" : 0.10
    },
    "tress parameters":{
        "tress per division" : 4,
        "tress scale" : 100.0
    },
    "sun parameters":{
        "direction vector":[ -0.624695 , 0.468521 , -0.624695 ]
    },
    "camera":{
        "FOV": 45.0,
        "Near Plane" : 0.1,
        "Far Plane" : 10000.0
    },
    "planes":{
        "one":{
            "Plane OBJ file" : "assets/Planes/Aeroplane/Plane.obj",
            "Plane Texuture file" : "assets/Planes/Aeroplane/Color plane map.png",
            "Plane Scale" : 1.0,
            "rotation angles":[ 0.0 , 0.0 , 0.0 ],
            "rotation axises":[ "y" , "z" , "x" ]            
        },
        "two":{
            "Plane OBJ file" : "assets/Planes/Airplane/PUSHILIN_Plane.obj",
            "Plane Texuture file" : "assets/Planes/Airplane/PUSHILIN_PLANE.png",
            "Plane Scale" : 1.0,
            "rotation angles":[ -90.0 , -10.0 , 0.0 ],
            "rotation axises":[ "y" , "z" , "x" ]
        },
        "three":{
            "Plane OBJ file" : "assets/Planes/Airplane (1)/Airplane.obj",
            "Plane Texuture file" : "assets/Planes/Airplane (1)/Airplane Texture.png",
            "Plane Scale" : 0.0025,
            "rotation angles":[ 0.0 , 0.0 ,0.0 ],
            "rotation axises":[ "x" , "y" , "z" ]
        }

    }

}   
)";

class FlightSimulator{
private:
	Skybox skybox;
  Plane* plane;
  Grid* TerrainGrid;
  float fog_densty;
  glm::vec3 sun_direction;
  json terrainParam;
public:
  FlightSimulator(){
	  json parameter_json = json::parse(RenderingParameterJSON);
    
    glutInitWindowSize(
      parameter_json.at("screen width").get<int>(), 
      parameter_json.at("screen height").get<int>()
    ); 

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
  void forward_input_to_plane(int key){
    this->plane->catchInputs(key);
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

// extern "C" 
// void EMSCRIPTEN_KEEPALIVE rewrite_render_parameters(
//   const std::string &new_rendering_parameter_json
// ) {   
//     delete flight_simulator;
//     flight_simulator = new FlightSimulator();
//     glutPostRedisplay();
// }


void key(int key, int x, int y) {
  if(flight_simulator == nullptr) return ;
  flight_simulator->forward_input_to_plane(key);  

 
}; 

/* Mainfunction: GLUT runs as a console application starting at main() */
int main(int argc, char ** argv) {
  glutInit( & argc, argv); // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH);

  glutInitWindowSize(500, 500); // Set the window's initial width & height - non-square
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
