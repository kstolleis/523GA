// **********************
// Melanie Moses' Lab, Computer Science Dept, University of New Mexico,
// Author: Matthew Fricke, Kenneth Letendre
// Date: February 19th 2013
// Description: Header file for the GA class. The GA class implements a
// genetic algorithm and contains a population of colonies to evolve over
// time. The evolve() function applies the genetic algorithm to the population,
// so each call to evolve() represents a generation.
// **********************

#ifndef _GA
#define _GA

#include "Colony.h"
#include <Algorithm>

class GA
{
    public:

        GA(int pop);
        void evolve();                  // The function which evaluates the current population and creates a new one
        int getNumberOfColonies();
        Colony* getColony(int index);
        Colony* getFittestColony();
        float getAverageFitness();
        float getMaxFitness();
        ~GA();

    private:
        Colony* colonies;               // Array to hold the population of colonies
        float mutation_range;           // range of a gaussian distribution centered on 0 from which mutations are chosen
        float crossover_rate;	        // Percent chance for crossover.  50 is complete shuffling.  0 or 100 is no crossover

        int n_colonies;


};
#endif
