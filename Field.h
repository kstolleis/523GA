// **********************
// Melanie Moses' Lab, Computer Science Dept, University of New Mexico,
// Author: Matthew Fricke, Kenneth Letendre
// Date: February 19th 2013
// Description: Header file for the Field class. Fields are the environment
// in which colonies of antbots forage for seeds. This environment is
// represented as an nxn grid of location objects.
// A single simulation time step is performed by calling the run() method;
// **********************

#ifndef _Field
#define _Field
#include "Colony.h"
#include "Location.h"
#include "Ant.h"
#include "GUI.h"

class Field
{
 public:

  Field( int n_colonies, Colony* cols[]);
  Location** getGrid();
  int getGridWidth(){return grid_width;}
  int getGridHeight(){return grid_height;}
 
  ~Field();

  void run(bool coop);

 private:

  int total_food;
  Ant** ants;
  int n_ants;
  int n_colonies;
  Colony** colonies;
  Location** gen_grid;
  Location** grid;
  int grid_height;
  int grid_width;
};
#endif
