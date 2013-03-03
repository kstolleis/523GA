
#include "GUI.h"
#include <iostream>
using namespace std;

pthread_mutex_t draw_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t draw_cond = PTHREAD_COND_INITIALIZER;

GUI* g_CurrentInstance;

extern "C"
void drawCallback() {
    g_CurrentInstance->draw();
}

extern "C"
void idleCallback() {
    g_CurrentInstance->idle();
}

extern "C"
void mainReshape() {
    g_CurrentInstance->resize();
}

void setupCallbacks() {
    glutDisplayFunc(::drawCallback);
    glutIdleFunc(::idleCallback);
    //::glutReshapeFunc(::mainReshape);
}

void* runGUI(void* a) {
    int window_height = 600;
    int window_width = 600;

    char *fakeargv[] = {(char*) "fakeParam", NULL};
    int fakeargc = 1;
    glutInit(&fakeargc, fakeargv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(window_height, window_width);
    glutInitWindowPosition(300, 200);
    glutCreateWindow("AntBot Evolver");
    setupCallbacks();
    glutMainLoop();
    return NULL;
}

GUI::GUI() {
    ::g_CurrentInstance = this;
    frame_count = 0;


    //glutReshapeFunc(main_handleResize);


    update_needed = false;

    //setupCallbacks();
    pthread_create(&gui_thread, NULL, runGUI, NULL);
}

void GUI::resize() {

}

void GUI::idle() {
    if (update_needed) {
        glutPostRedisplay();
        update_needed = false;
    }
}

void GUI::setGrid(int gw, int gh, Location** g) {
    grid_height = gh;
    grid_width = gw;
    grid = g;
    pthread_mutex_unlock(&draw_lock);
}

/*
void GUI::draw()
 {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 500.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(2, 2, 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glScalef(.005,.005,.005);
  glRotatef(20, 0, 1, 0);
  glRotatef(30, 0, 0, 1);
  glRotatef(5, 1, 0, 0);
  glTranslatef(-300, 0, 0);
    
  glColor3f(1,1,1);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'H');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'l');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'l');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'o');
  
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'W');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'o');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'r');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'l');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'd');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, '!');
        
  glutSwapBuffers();
 }
 */


void GUI::draw() {
    pthread_mutex_lock(&draw_lock);

    glEnable(GL_DEPTH_TEST); // To make GL draw items behind others when necessary
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-1.0, -1.0, 0.0);

    for (int i = 0; i < grid_width; i++) {
        glTranslatef(2.0f / grid_width, 0.0, 0.0);
        for (int j = 0; j < grid_height; j++) {
            glTranslatef(0.0, 2.0f / grid_height, 0.0);

            if (grid[i][j].nest == true) {
                glColor4f(0.0, 1.0, 0.0, 1.0);
            } else if (grid[i][j].food > 0)
                switch (grid[i][j].food) {
                    case 1: glColor4f(1.0, 0.0, 0.0, 1.0);
                        break;
                    case 2: glColor4f(1.0, 0.65, 0.0, 1.0);
                        break;
                    case 3: glColor4f(0.0, 1.0, 0.0, 1.0);
                        break;
                    case 4: glColor4f(1.0, 0.0, 1.0, 1.0);
                        break;
                    case 5: glColor4f(0.0, 0.0, 1.0, 1.0);
                        break;
                    case 6: glColor4f(0.0, 0.0, 1.0, 1.0);
                        break;
                } else {

                switch (grid[i][j].ant_status) {
                    case 0:
                        if (grid[i][j].p2 != 0.0f)
                            glColor4f(0.0, grid[i][j].p2, 0.0, 0.5);
                        else continue;
                        break;
                    case 1: glColor4f(1.0, 1.0, 1.0, 1.0);
                        break;
                    case 2: glColor4f(1.0, 0.0, 1.0, 1.0);
                        break;
                    case 3: glColor4f(0.0, 1.0, 1.0, 1.0);
                        break;
                    case 4: glColor4f(0.0, 1.0, 0.0, 1.0);
                        break;
                }
            }

            glBegin(GL_QUADS);
            glVertex3f(-1.0f / grid_width, 1.0f / grid_height, 0.0f); // Top Left
            glVertex3f(1.0f / grid_width, 1.0f / grid_height, 0.0f); // Top Right
            glVertex3f(1.0f / grid_width, -1.0f / grid_height, 0.0f); // Bottom Right
            glVertex3f(-1.0f / grid_width, -1.0f / grid_height, 0.0f); // Bottom Left
            glEnd();


        }
        glTranslatef(0.0, -2.0f, 0.0);
    }

    // Make sure changes appear onscreen
    glutSwapBuffers();
    //glFlush();

    //pthread_cond_signal(&draw_cond);
    pthread_mutex_unlock(&draw_lock);
}

void GUI::invalidateGrid() {
    //pthread_cond_wait(&draw_cond, &draw_lock);
    pthread_mutex_lock(&draw_lock);
}

void GUI::update() {
    update_needed = true;

}

GUI::~GUI() {

}
