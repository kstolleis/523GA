/* Karl Stolleis
   Cianan Sims
   CS523 Spring 2013
   University of New Mexico
   All code modified from base code provided by Dr. Melanie Moses
   See Below Comments 
 */

// The Genetic Algorithm Class implementation file
#include "GA.h"
#include "Utils.h"      // Some utility functions, ie. normalRand() is defined in Utils.h
#include <ctime>

using namespace std;
using namespace utils;  // So we can access the namespace in Utils.h

// Flags to set whether the ants use site fidelity and pheromone recruitment
bool recruit = true;
bool patch = true;

// The constructor takes a parameter which is the size of the population.

GA::GA(int pop) {
    crossover_rate = 10; // Set the crossover rate % - so 10 means 0.1 chance of crossover at each gene
    mutation_range = 0.05; // This parameter is the range that a mutated value can fall in. The distribution is a Gaussian centered on the old value in the genome.

    n_colonies = pop;

    colonies = new Colony[n_colonies];
    
    srand((unsigned int) time(NULL));

    // Set some initial random values into the first generation so we have initial diversity to work with.
    for (int col_count = 0; col_count < n_colonies; col_count++) {

        colonies[col_count].decay_rate = rand() % 20000 / 1000000.0f;
        colonies[col_count].walk_drop_rate = rand() % 20000 / 1000000.0f;
        colonies[col_count].search_giveup_rate = rand() % 10000 / 1000000.0f;
        colonies[col_count].trail_drop_rate = rand() % 20000 / 1000000.0f;
        colonies[col_count].dir_dev_const = rand() % 314 / 100.0f;
        colonies[col_count].dir_dev_coeff2 = rand() % 314 / 100.0f;
        colonies[col_count].dir_time_pow2 = rand() % 200 / 1000.0f;

        if (recruit) {
            colonies[col_count].dense_thresh = rand() % 80 / 10.0f;
            colonies[col_count].dense_const = rand() % 200 / 100.0f - 1;
        } else {
            colonies[col_count].dense_thresh = 1.0;
            colonies[col_count].dense_const = -9.0;
        }
        if (patch) {
            colonies[col_count].dense_thresh_patch = rand() % 80 / 10.0f;
            colonies[col_count].dense_const_patch = rand() % 200 / 100.0f - 1;
        } else {
            colonies[col_count].dense_thresh_patch = 1.0;
            colonies[col_count].dense_const_patch = -9.0;
        }
        colonies[col_count].dense_thresh_influence = rand() % 80 / 10.0f;
        colonies[col_count].dense_const_influence = rand() % 200 / 100.0f - 1;
        colonies[col_count].activate_sensitivity = rand() % 100 / 100.0f;
        colonies[col_count].decay_rate_return = rand() % 500 / 1000.0f;
        colonies[col_count].prop_active = 1.0;
    }
}

int GA::getNumberOfColonies() {
    return n_colonies;
}

Colony* GA::getColony(int index) {
    return colonies + index;
}


// Apply the genetic algorithm to the current population and generate a new population. 
// Corresponds to the creation of a new generation.
void GA::evolve() {

    // Allocate a place to put the new population
    Colony* new_colonies = new Colony[n_colonies];
//    const int count = n_colonies;
    
    // Include constant parameters for crossover and mutation functions
    int crossoverPercentage = 100;
    int mutationPercentage = 10;      // 10 is 10% chance of mutation, 5 is 20%, 20 is 5% -- and so on

    // This is the cumulative sum of fitness values for roulette selection
//    float fitnessSum[count];
//    for (int j = 0; j < count; j++) {
//        cout << "fitness " << getColony(j)->getFitness() << endl;
//        if(j == 0) {
//            fitnessSum[j] = getColony(j)->getFitness();
//        } else {
//            fitnessSum[j] = fitnessSum[j-1] + getColony(j)->getFitness();
//        }
//    }
//    // This normalizes the fitness array for roulette selection
//    for (int k = 0; k < count; k++){
//        float temp = fitnessSum[k] / fitnessSum[count -1];
//        fitnessSum[k] = temp;
//    }

    // Parents of the next population are selected by tournament. Two members of the population are chosen randomly. The fitter of the two is chosen to be a parent. This is repeated so two parents are selected. The parents genomes are mutated and crossed over to produce offspring to fill the next population array. 
    for (int i = 0; i < n_colonies; i++) {
        int parent1;
        int parent2;
        int candidate1;
        int candidate2;
        
        
        // This is the stuff  that actually executes the roulette selection
//        float rouletteSpin1 = rand() % 100001 / 100000.0;
//        float rouletteSpin2 = rand() % 100001 / 100000.0;
//        cout << "roulette val " << rouletteSpin1 << endl;
//        cout << "roulette val " << rouletteSpin2 << endl;
//
//        int mom = 0;
//        while (fitnessSum[mom] < rouletteSpin1) {
//            mom++;
//        }
//        parent1 = mom;
//        cout << parent1 << " " << colonies[parent1].getFitness() << endl;
//        int dad = 0;
//        while (fitnessSum[dad] < rouletteSpin2) {
//            dad++;
//        }
//        parent2 = dad;
//        cout << parent2 << " " << colonies[parent2].getFitness() << endl;
//        cout << "\n";
        
        // 1st parent candidates
        candidate1 = rand() % n_colonies;
        candidate2 = rand() % n_colonies;
        while (candidate1 == candidate2){
            candidate2 = rand() % n_colonies;
        }

        if (colonies[candidate1].getFitness() > colonies[candidate2].getFitness()) {
            parent1 = candidate1;
        } else {
            parent1 = candidate2;
        }

        // 2nd parent candidates
        candidate1 = rand() % n_colonies;
        candidate2 = rand() % n_colonies;
        while (candidate1 == candidate2){
            candidate2 = rand() % n_colonies;
        }

        if (colonies[candidate1].getFitness() > colonies[candidate2].getFitness()) {
            parent2 = candidate1;
        } else {
            parent2 = candidate2;
        }

        // Begin crossover of the parental genomes. (This version of crossover is not standard)

        // Independent assortment of decay_rate parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].decay_rate = colonies[parent1].decay_rate;
        } else {
            new_colonies[i].decay_rate = colonies[parent2].decay_rate;
        }

        // Independent assortment of walk_drop_rate parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].walk_drop_rate = colonies[parent1].walk_drop_rate;
        } else {
            new_colonies[i].walk_drop_rate = colonies[parent2].walk_drop_rate;
        }

        // Independent assortment of search_giveup_rate parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].search_giveup_rate = colonies[parent1].search_giveup_rate;
        } else {
            new_colonies[i].search_giveup_rate = colonies[parent2].search_giveup_rate;
        }

        // Independent assortment of trail_drop_rate parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].trail_drop_rate = colonies[parent1].trail_drop_rate;
        } else {
            new_colonies[i].trail_drop_rate = colonies[parent2].trail_drop_rate;
        }

        // Independent assortment of dir_dev_const parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dir_dev_const = colonies[parent1].dir_dev_const;
        } else {
            new_colonies[i].dir_dev_const = colonies[parent2].dir_dev_const;
        }

        // Independent assortment of dir_dev_coeff1 parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dir_dev_coeff1 = colonies[parent1].dir_dev_coeff1;
        } else {
            new_colonies[i].dir_dev_coeff1 = colonies[parent2].dir_dev_coeff1;
        }

        // Independent assortment of dir_time_pow1 parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dir_time_pow1 = colonies[parent1].dir_time_pow1;
        } else {
            new_colonies[i].dir_time_pow1 = colonies[parent2].dir_time_pow1;
        }

        // Independent assortment of dir_dev_coeff2 parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dir_dev_coeff2 = colonies[parent1].dir_dev_coeff2;
        } else {
            new_colonies[i].dir_dev_coeff2 = colonies[parent2].dir_dev_coeff2;
        }

        // Independent assortment of dir_time_pow2 parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dir_time_pow2 = colonies[parent1].dir_time_pow2;
        } else {
            new_colonies[i].dir_time_pow2 = colonies[parent2].dir_time_pow2;
        }

        // Independent assortment of dense_sens parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_sens = colonies[parent1].dense_sens;
        } else {
            new_colonies[i].dense_sens = colonies[parent2].dense_sens;
        }
        // Independent assortment of dense_thresh parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_thresh = colonies[parent1].dense_thresh;
        } else {
            new_colonies[i].dense_thresh = colonies[parent2].dense_thresh;
        }
        // Independent assortment of dense_const parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_const = colonies[parent1].dense_const;
        } else {
            new_colonies[i].dense_const = colonies[parent2].dense_const;
        }
        // Independent assortment of dense_const_patch parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_const_patch = colonies[parent1].dense_const_patch;
        } else {
            new_colonies[i].dense_const_patch = colonies[parent2].dense_const_patch;
        }
        // Independent assortment of dense_thresh_patch parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_thresh_patch = colonies[parent1].dense_thresh_patch;
        } else {
            new_colonies[i].dense_thresh_patch = colonies[parent2].dense_thresh_patch;
        }

        // Independent assortment of dense_const_influence parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_const_influence = colonies[parent1].dense_const_influence;
        } else {
            new_colonies[i].dense_const_influence = colonies[parent2].dense_const_influence;
        }
        // Independent assortment of dense_thresh_influence parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].dense_thresh_influence = colonies[parent1].dense_thresh_influence;
        } else {
            new_colonies[i].dense_thresh_influence = colonies[parent2].dense_thresh_influence;
        }

        // Independent assortment of prop_active parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].prop_active = colonies[parent1].prop_active;
        } else {
            new_colonies[i].prop_active = colonies[parent2].prop_active;
        }
        // Independent assortment of activate_sensitivity parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].activate_sensitivity = colonies[parent1].activate_sensitivity;
        } else {
            new_colonies[i].activate_sensitivity = colonies[parent2].activate_sensitivity;
        }
        // Independent assortment of decay_rate_return parameter
        if (rand() % crossoverPercentage < crossover_rate) {
            new_colonies[i].decay_rate_return = colonies[parent1].decay_rate_return;
        } else {
            new_colonies[i].decay_rate_return = colonies[parent2].decay_rate_return;
        }

        // Begin random mutation of the offsprings genome. Every location in the genome has a 1/10 probability of being mutated.

        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].decay_rate += randomNormal(0, new_colonies[i].decay_rate * mutation_range);
            if (new_colonies[i].decay_rate < 0.0f)
                new_colonies[i].decay_rate = 0;
            if (new_colonies[i].decay_rate > 1.0f)
                new_colonies[i].decay_rate = 1.0f;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].walk_drop_rate += randomNormal(0, new_colonies[i].walk_drop_rate * mutation_range);
            if (new_colonies[i].walk_drop_rate < 0.0f)
                new_colonies[i].walk_drop_rate = 0;
            if (new_colonies[i].walk_drop_rate > 1.0f)
                new_colonies[i].walk_drop_rate = 1.0f;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].search_giveup_rate += randomNormal(0, new_colonies[i].search_giveup_rate * mutation_range);
            if (new_colonies[i].search_giveup_rate < 0.0f)
                new_colonies[i].search_giveup_rate = 0;
            if (new_colonies[i].search_giveup_rate > 1.0f)
                new_colonies[i].search_giveup_rate = 1.0f;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].trail_drop_rate += randomNormal(0, new_colonies[i].trail_drop_rate * mutation_range);
            if (new_colonies[i].trail_drop_rate < 0.0f)
                new_colonies[i].trail_drop_rate = 0;
            if (new_colonies[i].trail_drop_rate > 1.0f)
                new_colonies[i].trail_drop_rate = 1.0f;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].dir_dev_const += randomNormal(0, 0.001 + fabs(new_colonies[i].dir_dev_const * mutation_range));
            if (new_colonies[i].dir_dev_const < 0.0f)
                new_colonies[i].dir_dev_const = 0;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].dir_dev_coeff2 += randomNormal(0, 0.001 + fabs(new_colonies[i].dir_dev_coeff2 * mutation_range));
            if (new_colonies[i].dir_dev_coeff2 < 0.0f)
                new_colonies[i].dir_dev_coeff2 = 0;
        }
        // Random mutation
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].dir_time_pow2 += randomNormal(0, 0.001 + fabs(new_colonies[i].dir_time_pow2 * mutation_range));
            if (new_colonies[i].dir_time_pow2 < 0.0f)
                new_colonies[i].dir_time_pow2 = 0;
        }
        // Random mutation
        if (recruit) {
            if (rand() % mutationPercentage == 0) {
                new_colonies[i].dense_thresh += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_thresh * mutation_range));
            }
            if (rand() % mutationPercentage == 0) {
                new_colonies[i].dense_const += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_const * mutation_range));
            }
        }
        if (patch) // The patch flag indicates whether the ants empty patch fidelity (ie they return to the last place they got a seed).
        {
            if (rand() % mutationPercentage == 0) {
                new_colonies[i].dense_const_patch += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_const_patch * mutation_range));
            }
            if (rand() % mutationPercentage == 0) {
                new_colonies[i].dense_thresh_patch += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_thresh_patch * mutation_range));
            }
        }
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].dense_const_influence += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_const_influence * mutation_range));
        }
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].dense_thresh_influence += randomNormal(0, 0.001 + fabs(new_colonies[i].dense_thresh_influence * mutation_range));
        }
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].activate_sensitivity += randomNormal(0, 0.001 + fabs(new_colonies[i].activate_sensitivity * mutation_range));
            if (new_colonies[i].activate_sensitivity < 0.0f)
                new_colonies[i].activate_sensitivity = 0.0;
        }
        if (rand() % mutationPercentage == 0) {
            new_colonies[i].decay_rate_return += randomNormal(0, 0.001 + fabs(new_colonies[i].decay_rate_return * mutation_range));
            if (new_colonies[i].decay_rate_return < 0.0f)
                new_colonies[i].decay_rate_return = 0.0;
            if (new_colonies[i].decay_rate_return > 1.0f)
                new_colonies[i].decay_rate_return = 1.0;
        }

    }

    // Set next generation of colonies, and obtain means for previous generation
    float sum_seeds = 0;
    int best_seeds = 0;
    float sum_decay = 0;
    float sum_walk_drop = 0;
    float sum_search_giveup = 0;
    float sum_trail_drop = 0;
    float sum_dir_dev_const = 0;
    float sum_dir_dev_coeff1 = 0;
    float sum_dir_time_pow1 = 0;
    float sum_dir_dev_coeff2 = 0;
    float sum_dir_time_pow2 = 0;
    float sum_dense_sens = 0;
    float sum_dense_thresh = 0;
    float sum_dense_const = 0;
    float sum_dense_thresh_patch = 0;
    float sum_dense_const_patch = 0;
    float sum_dense_thresh_influence = 0;
    float sum_dense_const_influence = 0;
    float sum_prop_active = 0;
    float sum_activate_sensitivity = 0;
    float sum_decay_rate_return = 0;


    // Keep running statistics
    for (int i = 0; i < n_colonies; i++) {
        sum_seeds += colonies[i].seeds_collected;
        if (colonies[i].seeds_collected > best_seeds)
            best_seeds = colonies[i].seeds_collected;
        sum_decay += colonies[i].decay_rate;
        sum_search_giveup += colonies[i].search_giveup_rate;
        sum_dir_dev_const += colonies[i].dir_dev_const;
        sum_dir_dev_coeff1 += colonies[i].dir_dev_coeff1;
        sum_dir_time_pow1 += colonies[i].dir_time_pow1;
        sum_dir_dev_coeff2 += colonies[i].dir_dev_coeff2;
        sum_dir_time_pow2 += colonies[i].dir_time_pow2;
        sum_dense_sens += colonies[i].dense_sens;
        sum_dense_thresh += colonies[i].dense_thresh;
        sum_dense_const += colonies[i].dense_const;
        sum_dense_thresh_patch += colonies[i].dense_thresh_patch;
        sum_dense_const_patch += colonies[i].dense_const_patch;
        sum_dense_thresh_influence += colonies[i].dense_thresh_influence;
        sum_dense_const_influence += colonies[i].dense_const_influence;
        sum_prop_active += colonies[i].prop_active;
        sum_activate_sensitivity += colonies[i].activate_sensitivity;
        sum_decay_rate_return += colonies[i].decay_rate_return;


        sum_walk_drop += colonies[i].walk_drop_rate;
        sum_trail_drop += colonies[i].trail_drop_rate;


        // Copy the new populations genome back into the population array

        colonies[i].decay_rate = new_colonies[i].decay_rate;

        colonies[i].walk_drop_rate = new_colonies[i].walk_drop_rate;
        colonies[i].search_giveup_rate = new_colonies[i].search_giveup_rate;
        colonies[i].trail_drop_rate = new_colonies[i].trail_drop_rate;
        colonies[i].dir_dev_const = new_colonies[i].dir_dev_const;
        colonies[i].dir_dev_coeff1 = new_colonies[i].dir_dev_coeff1;
        colonies[i].dir_time_pow1 = new_colonies[i].dir_time_pow1;
        colonies[i].dir_dev_coeff2 = new_colonies[i].dir_dev_coeff2;
        colonies[i].dir_time_pow2 = new_colonies[i].dir_time_pow2;
        colonies[i].dense_sens = new_colonies[i].dense_sens;
        colonies[i].dense_thresh = new_colonies[i].dense_thresh;
        colonies[i].dense_const = new_colonies[i].dense_const;
        colonies[i].dense_thresh_patch = new_colonies[i].dense_thresh_patch;
        colonies[i].dense_const_patch = new_colonies[i].dense_const_patch;
        colonies[i].dense_thresh_influence = new_colonies[i].dense_thresh_influence;
        colonies[i].dense_const_influence = new_colonies[i].dense_const_influence;
        colonies[i].prop_active = new_colonies[i].prop_active;
        colonies[i].activate_sensitivity = new_colonies[i].activate_sensitivity;
        colonies[i].decay_rate_return = new_colonies[i].decay_rate_return;

        colonies[i].seeds_collected = 0;
        colonies[i].num_chances = 0;


    }

    // THIS MIGHT BE REPETITION OF THE STEP ABOVE SINCE WE ARE JUST REPLACING COLONIES WITH NEW COLONIES IN ONE GO.
    // Clean up memory
    delete [] colonies;

    colonies = new_colonies;
}

Colony* GA::getFittestColony() {
    int max = 0;
    for (int i = 0; i < n_colonies; i++)
        if (colonies[i].getFitness() > colonies[max].getFitness()) max = i;

    return colonies + max;
}

float GA::getAverageFitness() {
    float sum = 0;
    for (int i = 0; i < n_colonies; i++) {
        sum += colonies[i].getFitness();
    }
    return sum / n_colonies;
}

float GA::getMaxFitness() {
    float max = 0;
    for (int i = 0; i < n_colonies; i++) {
        if (colonies[i].getFitness() > max)
            max = colonies[i].getFitness();
    }
    return max;
}

GA::~GA() {
    delete [] colonies;
}
