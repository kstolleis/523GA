// **********************
// Melanie Moses' Lab, Computer Science Dept, University of New Mexico,
// Author: Matthew Fricke
// Date: February 19th 2013
// Description: Header file for the GUI class. The GUI displays the current
// state of a Field and the colonies on that field.
// GLUT is a C-style library so some hacks are needed to make it object-oriented.
// There are multiple call back functions that are defined in GUI.cpp but are not in the
// GUI classes' scope. Regard those functions are boiler plate.
// **********************

#ifndef _GUI
#define _GUI

#ifdef DARWIN
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

#if LINUX
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif

#include <pthread.h>
#include "Location.h"

class GUI
{

public:
  GUI();
  void draw(); // Iterate over the Field grid and display each location with some color
  void idle(); // Called by glut when not updating the display. Checks the update_needed flag. 
  void resize(); // not implemented yet
  void update(); // Used to set the update_needed flag. Causes glut to post a redisplay
  void mainReshape(); // not implemented yet
  void setGrid(int gw, int gh, Location** g); //Sets the grid that the GUI will display. Releases the draw_mutex lock.
  void invalidateGrid(); // Tells the GUI that it's grid is about to be destroyed. Locks the draw mutex. Does not return to the caller until the current draw() function has completed.
  ~GUI();

 private:
  void setupCallbacks(); // Associate glut functions with class methods in GUI
  bool grid_valid; // unused at the moment - superceeded by mutexes.
  bool update_needed; // Flag checked periodically by idle() to see if a glutPortRedisplay() is needed
  Location** grid; // Pointer to the grid to be displayed. The gui does not own the grid. The Field class does so GUI never changed the grid and does not free its memory. Those tasks are left to the Field objects only.
  int main_window; 
  int grid_width; // Width of the grid to display
  int grid_height; // Height of the grid to display
  long frame_count; // Unused at the moment but could be just to skip display frames
  pthread_t gui_thread; // The child thread in which the glut code runs. We do this to the model can be updated in parallel. Huge performance increase.
};

#endif
