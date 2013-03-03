/* Karl Stolleis
   Cianan Sims
   CS523 Spring 2013
   University of New Mexico
   All code modified from base code provided by Dr. Melanie Moses
   See Below Comments 
 */

// **********************
// Melanie Moses' Lab, Computer Science Dept, University of New Mexico,
// Author: Matthew Fricke
// Date: February 19th 2013
// Description: main is the entry point for the program. 
// This code creates a GUI object to display the antbots 
// foraging and two independent GA objects that each implement
// a genetic algorithm. A parallel loop performs the foraging
// simulation for pairs of antbot colonies (one selected from
// the the population of each GA) 
// **********************

#ifndef _MAIN
#define _MAIN
#include "GA.h" // For the Genetic Algorithm class
#include "Field.h" // For the Field class
#include <time.h> // So we can do timing 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>     // So we can catch ctrl-c
#include <sstream>      // For formating the time string
#include <iostream>
#include <iomanip>
#include <omp.h>        // For the OpenMP code so we can parallize the interactions for loop
#include <fstream>

using namespace std;

string version = "Release 1.4";

const int n_generations = 100; // How many generations should run before the program exits
const int n_interactions = 60; // Number of pairwise interactions between colonies per generation
const int n_colonies = 20; // Number of colonies in each GAs population
const int n_steps = 8000; // The number of time steps to run on the Field where each time step updates the actions of all the ants on the field.

// Create the two GAs we will use 
GA* ga1 = new GA(n_colonies);
GA* ga2 = new GA(n_colonies);

int main(int argc, char *argv[]) {

    bool gui_enabled = false;
    bool coop_enabled = false;
    
    /* This is where the initial file for data output is created
       Cooperation or Competition creates different .csv files*/
    ofstream resultsFile;
    if (coop_enabled == true) {
        resultsFile.open("MaxMeanFitness_2_2_Coop_HiMut.csv");
    } else {
        resultsFile.open("MaxMeanFitness_2_2_HiMut.csv");
    }
    resultsFile.close();

    // Clear screen
    cout << "\033[2J\033[1;1H";

    if (argc == 3) {
        if (strcmp(argv[2], "coop") == 0) coop_enabled = true;
        else if (strcmp(argv[2], "comp") == 0) coop_enabled = false;
        else cout << "Usage: ./antbots [nogui|gui] [comp|coop]" << endl;

        if (strcmp(argv[1], "gui") == 0) gui_enabled = true;
        else if (strcmp(argv[1], "nogui") == 0) gui_enabled = false;
        else cout << "Usage: ./antbots [nogui|gui] [comp|coop]" << endl;
    } else {
        if (argc != 1)
            cout << "Usage: ./antbots [nogui|gui] [comp|coop]" << endl;
    }

    // Set the random seed
    srand((int) time(NULL));

    GUI* gui;
    if (gui_enabled) gui = new GUI();

    int* thread_current_step = NULL;

    // Generation Loop. Runs a set of interactions between colonies on the Field and then
    // uses GAs to create a new population and repeats.
    for (int current_generation = 0; current_generation < n_generations; current_generation++) {
        int total_current_interaction = 0;

        // Use openmp to parallelize the interactions loop.
#pragma omp parallel for shared(ga1, ga2, thread_current_step) schedule(dynamic,1)

        // This loop repeatedly picks a colony from each GA randomly. Puts them on a field together and
        // runs the simulation. 
        for (int current_interaction = 0; current_interaction < n_interactions; current_interaction++) {

            thread_current_step = new int[omp_get_num_threads()];
            for (int i = 0; i < omp_get_num_threads(); i++){
                thread_current_step[i] = 0;
            }


            // The number of colonies to place on the field at once
            int pop = 2;

            Colony* col1 = NULL;
            Colony* col2 = NULL;

            // To prevent different threads from choosing the same colony check
            // that the colonies are not already foraging.
            do {
                int index = rand() % ga1->getNumberOfColonies();
                col1 = ga1->getColony(index);
            } while (col1->foraging);

            // Set the colony to foraging so that it can't be selected twice for the same interaction
            col1->foraging = true;

            do {
                int index = rand() % ga2->getNumberOfColonies();
                col2 = ga2->getColony(index);
            } while (col2->foraging);

            col2->foraging = true;

            Colony * colonies[] = {col1, col2};

            // Create a field on the stack for the colonies to interact on. Will lose scope each
            // iteration of the interaction for loop
            Field field(pop, colonies);

            // Only visualize ant foraging for simulations running in thread 0.
            // The GUI takes a pointer to the 2d grid in field and displays its state.
            if (omp_get_thread_num() == 0 && gui_enabled){
                gui->setGrid(field.getGridWidth(), field.getGridHeight(), field.getGrid());
            }

            // This loop calls Field::run() repeatedly. Each call to run is a complete evaluation of
            // the ants and grid for one time step. Variable n_steps is how many time steps to run
            // before ending this interaction.
            for (int i = 0; i < n_steps; i++) {
                field.run(coop_enabled);
                if (omp_get_thread_num() == 0 && gui_enabled){
                    gui->update();
                }
                thread_current_step[omp_get_thread_num()] = i;
            }

            // If the colonies are chosen to have a fitness function that is the average
            // number of seeds collected across all the colonies on the field then execute
            // the following code to accomplish that. (ie. the coop switch was given on the command line).
            if (coop_enabled) {

                int sum_seeds = 0;
                for (int i = 0; i < pop; i++){
                    sum_seeds += colonies[i]->seeds_collected;
                }

                for (int i = 0; i < pop; i++){
                    colonies[i]->seeds_collected = sum_seeds / pop;
                }

            }

            if (omp_get_thread_num() == 0 && gui_enabled){
                gui->invalidateGrid();
            }

            colonies[0]->foraging = false;
            colonies[1]->foraging = false;
            total_current_interaction++;
        }

        /* This simply opens the file created at the top of main
         */
        if (coop_enabled == true) {
            resultsFile.open("MaxMeanFitness_2_2_Coop_HiMut.csv", ios::out | ios::app);
        } else {
            resultsFile.open("MaxMeanFitness_2_2_HiMut.csv", ios::out | ios::app);
        }

        /* The Max Fitness, Mean Fitness and Genome of Best Colony from GA1
           are written to the file here in comma delimited form
         */
        resultsFile << current_generation                               << ","; 
        resultsFile << ga1->getMaxFitness()                             << ",";
        resultsFile << ga1->getAverageFitness()                         << ",";
        resultsFile << ga1->getFittestColony()->walk_drop_rate          << ',';
        resultsFile << ga1->getFittestColony()->search_giveup_rate      << ',';
        resultsFile << ga1->getFittestColony()->dir_dev_const           << ',';
        resultsFile << ga1->getFittestColony()->dir_dev_coeff2          << ',';
        resultsFile << ga1->getFittestColony()->dir_time_pow2           << ',';
        resultsFile << ga1->getFittestColony()->trail_drop_rate         << ',';
        resultsFile << ga1->getFittestColony()->dense_thresh            << ',';
        resultsFile << ga1->getFittestColony()->dense_const             << ',';
        resultsFile << ga1->getFittestColony()->dense_thresh_patch      << ',';
        resultsFile << ga1->getFittestColony()->dense_const_patch       << ',';
        resultsFile << ga1->getFittestColony()->dense_thresh_influence  << ',';
        resultsFile << ga1->getFittestColony()->dense_const_influence   << ',';
        resultsFile << ga1->getFittestColony()->decay_rate              << ',';
        resultsFile << ga1->getFittestColony()->seeds_collected         << ',';
        
        /* The Max Fitness, Mean Fitness and Genome of Best Colony from GA2
           are written to the file here in comma delimited form
         */
        resultsFile << ga2->getMaxFitness()                             << ",";
        resultsFile << ga2->getAverageFitness()                         << ",";
        resultsFile << ga2->getFittestColony()->walk_drop_rate          << ',';
        resultsFile << ga2->getFittestColony()->search_giveup_rate      << ',';
        resultsFile << ga2->getFittestColony()->dir_dev_const           << ',';
        resultsFile << ga2->getFittestColony()->dir_dev_coeff2          << ',';
        resultsFile << ga2->getFittestColony()->dir_time_pow2           << ',';
        resultsFile << ga2->getFittestColony()->trail_drop_rate         << ',';
        resultsFile << ga2->getFittestColony()->dense_thresh            << ',';
        resultsFile << ga2->getFittestColony()->dense_const             << ',';
        resultsFile << ga2->getFittestColony()->dense_thresh_patch      << ',';
        resultsFile << ga2->getFittestColony()->dense_const_patch       << ',';
        resultsFile << ga2->getFittestColony()->dense_thresh_influence  << ',';
        resultsFile << ga2->getFittestColony()->dense_const_influence   << ',';
        resultsFile << ga2->getFittestColony()->decay_rate              << ',';
        resultsFile << ga2->getFittestColony()->seeds_collected         << '\n';

        // Close the file to complete write
        resultsFile.close();

        // Put console debug messages here
//        ga1->getFittestColony()->printGenome();
//        cout << "\n";
//        ga2->getFittestColony()->printGenome();
       
        // Tell the GAs to evaluate the fitness of their populations and generate a new population
        // of colonies (must be using crossover, mutation, tournament selection or elitism, etc)
        ga1->evolve();
        ga2->evolve();

    }

    // Done. Tell the OS there were no problems
    return 0;
}

#endif
