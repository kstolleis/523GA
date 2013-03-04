// **********************
// Melanie Moses' Lab, Computer Science Dept, University of New Mexico,
// Author: Matthew Fricke, Kenneth Letendre
// Date: February 19th 2013
// Description: Header file for the Colony structure. This is mostly a compound type definition that
// hold the genome for a particular colony. There are some convenience functions such as printGenome() and getFitness().
// **********************

#ifndef _Colony
#define _Colony

#include <iostream>
using namespace std;

struct Colony
{
    Colony()
    {
        foraging = false;
        decay_rate = 0.0;
        trail_drop_rate = 0.0;

        walk_drop_rate = 0.0;

        search_giveup_rate = 0.0;

        dir_dev_const = 1.0;
        dir_dev_coeff1 = 0.0;
        dir_time_pow1 = 0.0;
        dir_dev_coeff2 = 0.0;
        dir_time_pow2 = 0.0;

        dense_thresh = 0.0;
        dense_sens = 0.0;
        dense_const = 0.0;

        dense_thresh_patch = 0.0;
        dense_const_patch = 0.0;

        dense_thresh_influence = 0.0;
        dense_const_influence = 0.0;

        prop_active = 0.0;
        decay_rate_return = 0.0;
        activate_sensitivity = 0.0;

        seeds_collected = 0.0;
        ant_time_out = 0.0;

        num_chances = 0;
        total_seeds_available = 0;
    }

    void printGenome();

    float getFitness()
    {
        // If this colony has never interacted with another colony set its fitness to 0
        if (num_chances == 0) return 0;

        // Calculate the fitness for this colony based on the number of seeds available
        // to collect per interaction and the numner of interactions.
        if (total_seeds_available == 0) return 0; // Avoid div by zero
        return ((seeds_collected/(total_seeds_available*1.0f))/num_chances); 
    }

    ~Colony();

    int total_seeds_available;  // The total number of seeds placed on the field
    // used in normalizing the fitness value. Set by the Field object when this
    // colony is added to it.

    bool foraging;              // Flag to keep threads from choosing to work on the same Colony at the same time.
    int num_chances;            // Number of interations this colony has been involved with.

    // THE GENOME. These are the evolvable parameters. The GA will try to find optimal values for these parameters that maximize seed intake.
    float decay_rate;           // evolvable param: pheromone decay_rate, grid[i][j].p2 *= (1-decay_rate);
    float trail_drop_rate;      // evolvable param: probability that an ant following a trail will stop walking and start searching, if rand()%1000/1000.0f < drop_rate

    float search_giveup_rate;   // evolvable param: probability each time step that a searching ant will give up and return to nest.

    float walk_drop_rate;       // evolvable param: probability that a traveling ant will stop traveling and start searching, if rand()%1000/1000.0f < drop_rate

    float dir_dev_const;        // evolvable param: weakness of correlation in correlated random walk search

    // UNUSED
    // evolvable parameter: coefficient for change in directional deviation with search time.
    // high values, e.g. 1.0, indicate more turning early in search.  low values, e.g., -1.0, indicate less turning early in search.
    float dir_dev_coeff1;   

    // UNUSED
    // evolvable parameter: float d_theta = sto.Normal(0,(colonies[col_count].dir_dev_coeff1*
    //                      pow(ants[ant_count].search_time,colonies[col_count].dir_time_pow1))+
    //                      (colonies[col_count].dir_dev_coeff2/pow(ants[ant_count].search_time,colonies[col_count].dir_time_pow2))+
    //                      colonies[col_count].dir_dev_const);
    float dir_time_pow1;    

    // evolvable parameter: coefficient for change in directional deviation with search time.
    // high values, e.g. 1.0, indicate more turning early in search.  low values, e.g., -1.0, indicate less turning early in search.
    float dir_dev_coeff2;   

    // evolvable parameter: float d_theta = sto.Normal(0,(colonies[col_count].dir_dev_coeff1*
    //                      pow(ants[ant_count].search_time,colonies[col_count].dir_time_pow1))+
    //                      (colonies[col_count].dir_dev_coeff2/pow(ants[ant_count].search_time,colonies[col_count].dir_time_pow2))+
    //                      colonies[col_count].dir_dev_const);
    float dir_time_pow2;    

    float dense_thresh;             // evolvable param: density_count/dense_thresh is the threshold for deciding to lay a trail

    float dense_sens;               // UNUSED. evolvable param: sensitivity of the decision to lay a trail.  rand()%dense_sens/((float) dense_sens)
    float dense_const;              // evolvable param: sensitivity of the decision to lay a trail.  rand()%dense_sens/((float) dense_sens)

    float dense_thresh_patch;       // evolvable param: density_count/dense_thresh is the threshold for deciding to return to patch
    float dense_const_patch;        // evolvable param: sensitivity of the decision to return to patch.  rand()%dense_sens/((float) dense_sens)

    float dense_thresh_influence;   // evolvable param: density_count/dense_thresh is the threshold for deciding to be influenced by pheromones at nest if any present
    float dense_const_influence;    // evolvable param: sensitivity of the decision to be influenced by pheromones at nest if any present.  rand()%dense_sens/((float) dense_sens)

    float prop_active;              // TEMP UNUSED. evolvable param: proportion of foragers active at model init
    float decay_rate_return;        // TEMP UNUSED. evolvable param: decay rate of return "pheromone"
    float activate_sensitivity;     // TEMP UNUSED. responsiveness of idle foragers to return of incoming, recruiting foragers.  0.0 is no response.

    float seeds_collected;          // selection heuristic: count of all seeds returned to nest
    float ant_time_out;             // selection heuristic: count of time steps ants have spent outside nest

    int nestx;                      // Location of this colonies nest in the grid
    int nesty;
    int pherminx;
    int phermaxx;
    int pherminy;
    int phermaxy;

};
#endif
