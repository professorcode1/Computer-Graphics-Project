#include <GL/glut.h>
#include <emscripten.h>
#include <stdio.h>
#include "IncGLM.hpp"
#include "OpenGL/Skybox.hpp"

class FlightSimulator{
private:
	Skybox skybox;
public:
  FlightSimulator(){};
  void render(){
    glm::vec3(4,3,-3);
    const glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), 
      float(1920)/1080, 
      0.1f, 
      10000.0f);

    glm::mat4 View = glm::lookAt(glm::vec3(4,3,-3), glm::vec3(0,0,0),glm::vec3(0,1,0));
    this->skybox.render(View , projection);
  }
};
EMSCRIPTEN_KEEPALIVE FlightSimulator* flight_simulator = nullptr;

void initGL() {
  glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
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

extern "C" void EMSCRIPTEN_KEEPALIVE move_train_forward() { 
    // angle -= 0.01f;
    glutPostRedisplay();
 }

 extern "C" void EMSCRIPTEN_KEEPALIVE move_train_backward() { 
    // angle += 0.01f;
    glutPostRedisplay();
 }


void key(int key, int x, int y) {
  // switch (key) {
  // case GLUT_KEY_RIGHT:
  //   angle += 0.01f;
  //   glutPostRedisplay();
  //   break;
  // case GLUT_KEY_LEFT:
  //   angle -= 0.01f;
  //   glutPostRedisplay();
  //   break;
  // }
 
}; 

/* Mainfunction: GLUT runs as a console application starting at main() */
int main(int argc, char ** argv) {
  glutInit( & argc, argv); // Initialize GLUT
  glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
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
