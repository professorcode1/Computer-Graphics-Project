#include <GL/glut.h>
#include <emscripten.h>
#include <stdio.h>


// class FlightSimulator{
// private:
// 	Skybox skybox;
// public:
//   FlightSimulator(){};
//   render(){
    
//   }
// };
EMSCRIPTEN_KEEPALIVE float angle = 0;

void initGL() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
void idle() {
  glutPostRedisplay();
}
void display() {
  int x = 0;
  glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer with current clearing color
  glMatrixMode(GL_MODELVIEW); // To operate on Model-View matrix
  glLoadIdentity(); // Reset the model-view matrix
  glPushMatrix(); // Save model-view matrix setting
  glBegin(GL_QUADS); // Each set of 4 verticesform a quad
  //SUNSET BACKGROUND
  glColor3f(1.000f, 0.549f, 0.000f);
  glVertex2f(-1.0f, -1.0f);
  glColor3f(1.000f, 0.549f, 0.000f); // Dark grey
  glVertex2f(1.0f, -1.0f); //  so that the normal (front-face) isfacing you
  glColor3f(1.000f, 0.271f, 0.000f); // Dark grey      			
  glVertex2f(1.0f, 1.0f);
  glColor3f(1.000f, 0.271f, 0.000f); // Dark grey
  glVertex2f(-1.0f, +1.0f);
  //BUILDING
  glColor3f(0.000, 0.502, 0.502); // Dark grey
  glVertex2f(-0.8f, -0.8f);
  glColor3f(0.125, 0.698, 0.667); // Dark grey
  glVertex2f(-0.2f, -0.8f); //  so that the normal (front-face) isfacing you
  glColor3f(0.000, 0.502, 0.502); // Dark grey      			
  glVertex2f(-0.8f, 0.5f);
  glColor3f(0.125, 0.698, 0.667); // Dark grey
  glVertex2f(-0.2f, 0.5f);
  //TRACKS
  glColor3f(0.0f, 0.0f, 0.0f); // Dark grey
  glVertex2f(-1.0f, -1.0f);
  glColor3f(0.0f, 0.0f, 0.0f); // Dark grey
  glVertex2f(1.0f, -1.0f); //  so that the normal (front-face) isfacing you
  glColor3f(0.4f, 0.4f, 0.4f); // Dark grey      			
  glVertex2f(1.0f, -0.8f);
  glColor3f(0.4f, 0.4f, 0.4f); // Dark grey
  glVertex2f(-1.0f, -0.8f);
  glEnd();
  glPopMatrix(); // Restore the model-view matrix
  glPushMatrix(); // Save model-view matrix setting
  glTranslatef(angle, 0.0f, 0.0f); // Translate
  glBegin(GL_QUADS);
  //BODY OF TRAIN
  glColor3f(1.0f, 0.0f, 1.0f); // WHITE
  glVertex2f(0.0f, -0.8f);
  glColor3f(1.0f, 0.0f, 1.0f);
  glVertex2f(1.0f, -0.8f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2f(1.0f, -0.4f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2f(0.0f, -0.4f);
  //WINDOW PANEL
  glColor3f(0.0f, 0.0f, 1.0f); // WHITE
  glVertex2f(0.05f, -0.6f);
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex2f(0.95f, -0.6f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(0.95f, -0.45f);
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex2f(0.05f, -0.45f);

  glEnd();

  glEnd();
  glPopMatrix(); // Restore the model-view matrix
  glPushMatrix();
  glTranslatef(angle, 0.0f, 0.0f); // Translate

  glPopMatrix();
  glutSwapBuffers(); // Double buffered - swap thefront and back buffers
  // Change the rotational angle after each display()

}


void reshape(GLsizei width, GLsizei height) { 
  if (height == 0) height = 1; 
  GLfloat aspect = (GLfloat) width / (GLfloat) height;

  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  if (width >= height) {
    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
  } else {
    gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
  }
}

extern "C" void EMSCRIPTEN_KEEPALIVE move_train_forward() { 
    angle -= 0.01f;
    glutPostRedisplay();
 }

 extern "C" void EMSCRIPTEN_KEEPALIVE move_train_backward() { 
    angle += 0.01f;
    glutPostRedisplay();
 }


void key(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_RIGHT:
    angle += 0.01f;
    glutPostRedisplay();
    break;
  case GLUT_KEY_LEFT:
    angle -= 0.01f;
    glutPostRedisplay();
    break;
  }

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
