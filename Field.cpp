#include "Field.h"
#include <stdlib.h>
#include <math.h>
#include "Location.h"
#include "Utils.h"
#include <iostream>
//#include "Options.h"

using namespace utils;
using namespace std;

const int clumpradius = 5;
const int n_food_red = 64; // one pile
const int n_food_orange = 64; // four piles
const int n_food_green = 64; // 16 piles
const int n_food_purple = 64; // 64 piles -- not part of the AntBot setup
const int n_food_blue = 64; // random
const int num_each_clump = 5; // 667/256; // 118/32; // 128/32; 6676/32
const int n_food_background = 0; // random
int count_food1;
int count_food2;
float deposit_rate_p2 = 0.1;
float saturation_p1 = 1.0;
float saturation_p2 = 1.0;
int smell_range = 1;
float return_pheromone = 0.0;
int count_food_red = 0;
int count_food_orange = 0;
int count_food_green = 0;
int count_food_blue = 0;
int sumdx = 0;
int sumdy = 0;
const float ant_time_out_cost = 0.0000;
const float pi = 3.14159;


int search_delay = 4; // time steps between a searching ant's move.  0 is move on every time step.
int return_delay = 0; // time steps between a returning ant's move.  0 is move on every time step.


static int update_count = 0;

Field::Field(int n_cols, Colony* cols[]) {
    int total_food = 0;
    //cout << "Created a Field" << endl;
    grid_width = 300; //90  //106; //177; //200; //768;
    grid_height = 300; // 90 //106; //177; //200; //768;  //each grid cell is 10x10 cm

    n_colonies = n_cols;
    colonies = cols;

    for (int i = 0; i < n_colonies; i++) {
        colonies[i]->num_chances++;
    }

    n_ants = 24;
    ants = new Ant*[n_colonies];
    for (int i = 0; i < n_colonies; i++)
        ants[i] = new Ant[n_ants];

    // Standard grid to use across all colonies in a generation
    grid = new Location*[grid_width];
    for (int i = 0; i < grid_width; i++)
        grid[i] = new Location[grid_height];

    // Standard grid to use across all colonies in a generation
    gen_grid = new Location*[grid_width];
    for (int i = 0; i < grid_width; i++)
        gen_grid[i] = new Location[grid_height];

    //Clean up grid for new evalution
    for (int x = 0; x < grid_width; x++) {
        for (int y = 0; y < grid_width; y++) {
            gen_grid[x][y].p1 = 0;
            gen_grid[x][y].p2 = 0;
            gen_grid[x][y].ant_status = 0;
            gen_grid[x][y].carrying = 0;
            gen_grid[x][y].food = 0;
            gen_grid[x][y].nest = false;
            gen_grid[x][y].pen_down = false;
        }
    }

    // Place nest entrance randomly
    //gen_grid[nestx][nesty].nest = true;
    //for (int i = 0; i < n_cols; i++ )
    //  {
    //    colonies[i]->nestx = rand()%grid_width;
    //    colonies[i]->nesty = rand()%grid_height;
    //  }  

    // Fixed placement of two colonies
    colonies[0]->nestx = (int) grid_width * 0.25;
    colonies[0]->nesty = (int) grid_height * 0.5;

    colonies[1]->nestx = (int) grid_width * 0.75;
    colonies[1]->nesty = (int) grid_height * 0.5;
    
    // Location of colonies to maximize area and minimize interaction 
//    colonies[0]->nestx = (int) grid_width * 0.25;
//    colonies[0]->nesty = (int) grid_height * 0.25;
//
//    colonies[1]->nestx = (int) grid_width * 0.75;
//    colonies[1]->nesty = (int) grid_height * 0.75;

    int locations[4 * num_each_clump][2];
    int food_count = 0;
    int tempradius = clumpradius;
    int rad_count = 0;
    for (int clump_count = 1; clump_count <= num_each_clump; clump_count++) {
        int clumpx;
        int clumpy;
        int overlap = 1;

        //Place red food -- 1 pile.
        while (overlap == 1) {
            clumpx = rand() % (grid_width - clumpradius * 2) + clumpradius;
            clumpy = rand() % (grid_height - clumpradius * 2) + clumpradius;
            overlap = 0;
            int clumpcheck = 0;
            while (clumpcheck < (4 * clump_count - 4)) {
                if (sqrt(pow(clumpx - locations[clumpcheck][0], 2) + pow(clumpy - locations[clumpcheck][1], 2)) < clumpradius * 2)
                    overlap = 1;
                clumpcheck++;
            }
        }

        locations[4 * clump_count - 4][0] = clumpx;
        locations[4 * clump_count - 4][1] = clumpy;
        food_count = 0;

        while (food_count < n_food_red) {
            if (rad_count > n_food_red * 4) {
                tempradius++;
                rad_count = 0;
            }
            int randx = rand() % (tempradius * 2) + clumpx - tempradius;
            int randy = rand() % (tempradius * 2) + clumpy - tempradius;
            float seeddist = sqrt(pow(clumpx - randx, 2) + pow(clumpy - randy, 2));
            if (randx < 0 || randx > grid_width || randy < 0 || randy > grid_height) {
                continue;
            } else if ((gen_grid[randx][randy].food == 0) & (seeddist < tempradius)) {
                clumpy = rand() % (grid_height - clumpradius * 2) + clumpradius;
                overlap = 0;
                int clumpcheck = 0;
                while (clumpcheck < (4 * clump_count - 4)) {
                    if (sqrt(pow(clumpx - locations[clumpcheck][0], 2) + pow(clumpy - locations[clumpcheck][1], 2)) < clumpradius * 2)
                        overlap = 1;
                    clumpcheck++;
                }
            }

            locations[4 * clump_count - 4][0] = clumpx;
            locations[4 * clump_count - 4][1] = clumpy;
            total_food += food_count;
            food_count = 0;

            while (food_count < n_food_red) {
                if (rad_count > n_food_red * 4) {
                    tempradius++;
                    rad_count = 0;
                }
                int randx = rand() % (tempradius * 2) + clumpx - tempradius;
                int randy = rand() % (tempradius * 2) + clumpy - tempradius;
                float seeddist = sqrt(pow(clumpx - randx, 2) + pow(clumpy - randy, 2));
                if (randx < 0 || randx >= grid_width || randy < 0 || randy >= grid_height) {
                    continue;
                } else if ((gen_grid[randx][randy].food == 0) & (seeddist < tempradius)) {
                    gen_grid[randx][randy].food = 1;
                    food_count++;
                }
                rad_count++;
            }

            //Place orange food -- four piles
            for (int i = 0; i < 4; i++) {
                int overlap = 1;
                while (overlap == 1) {
                    clumpx = rand() % (grid_width - clumpradius * 2) + clumpradius;
                    clumpy = rand() % (grid_height - clumpradius * 2) + clumpradius;
                    overlap = 0;
                    int clumpcheck = 0;
                    while (clumpcheck < (4 * clump_count - 3)) {
                        if (sqrt(pow(clumpx - locations[clumpcheck][0], 2) + pow(clumpy - locations[clumpcheck][1], 2)) < clumpradius * 2)
                            overlap = 1;
                        clumpcheck++;
                    }
                }

                locations[4 * clump_count - 3][0] = clumpx;
                locations[4 * clump_count - 3][1] = clumpy;
                total_food += food_count;
                food_count = 0;
                tempradius = clumpradius;
                rad_count = 0;
                while (food_count < n_food_orange / 4) {
                    if (rad_count > n_food_orange * 4) {
                        tempradius++;
                        rad_count = 0;
                    }
                    int randx = rand() % (tempradius * 2) + clumpx - tempradius;
                    int randy = rand() % (tempradius * 2) + clumpy - tempradius;
                    float clumpdist = sqrt(pow(clumpx - randx, 2) + pow(clumpy - randy, 2));
                    if (randx < 0 || randx >= grid_width || randy < 0 || randy >= grid_height) {
                        continue;
                    } else if ((gen_grid[randx][randy].food == 0) & (clumpdist < tempradius)) {
                        gen_grid[randx][randy].food = 2;
                        food_count++;
                    }
                    rad_count++;
                }
            }

            //Place green food -- sixteen piles
            for (int i = 0; i < 16; i++) {
                int overlap = 1;
                while (overlap == 1) {
                    clumpx = rand() % (grid_width - clumpradius * 2) + clumpradius;
                    clumpy = rand() % (grid_height - clumpradius * 2) + clumpradius;
                    overlap = 0;
                    int clumpcheck = 0;
                    while (clumpcheck < (4 * clump_count - 2)) {
                        if (sqrt(pow(clumpx - locations[clumpcheck][0], 2) + pow(clumpy - locations[clumpcheck][1], 2)) < clumpradius * 2)
                            overlap = 1;
                        clumpcheck++;
                    }
                }
                locations[4 * clump_count - 2][0] = clumpx;
                locations[4 * clump_count - 2][1] = clumpy;
                total_food += food_count;
                food_count = 0;
                tempradius = clumpradius;
                rad_count = 0;
                while (food_count < n_food_green / 16) {
                    if (rad_count > n_food_green * 4) {
                        tempradius++;
                        rad_count = 0;
                    }
                    int randx = rand() % (tempradius * 2) + clumpx - tempradius;
                    int randy = rand() % (tempradius * 2) + clumpy - tempradius;
                    float clumpdist = sqrt(pow(clumpx - randx, 2) + pow(clumpy - randy, 2));
                    if (randx < 0 || randx > grid_width || randy < 0 || randy > grid_height) {
                        continue;
                    } else if ((gen_grid[randx][randy].food == 0) & (clumpdist < tempradius)) {
                        gen_grid[randx][randy].food = 3;
                        food_count++;
                    }
                    rad_count++;
                }
            }


            // Place purple food
            for (int i = 0; i < 64; i++) {
                int overlap = 1;
                while (overlap == 1) {
                    clumpx = rand() % (grid_width - clumpradius * 2) + clumpradius;
                    clumpy = rand() % (grid_height - clumpradius * 2) + clumpradius;
                    overlap = 0;
                    int clumpcheck = 0;
                    while (clumpcheck < (4 * clump_count - 1)) {
                        if (sqrt(pow(clumpx - locations[clumpcheck][0], 2) + pow(clumpy - locations[clumpcheck][1], 2)) < clumpradius * 2)
                            overlap = 1;
                        clumpcheck++;
                    }
                }
                locations[4 * clump_count - 1][0] = clumpx;
                locations[4 * clump_count - 1][1] = clumpy;
                total_food += food_count;
                food_count = 0;
                tempradius = clumpradius;
                rad_count = 0;
                while (food_count < n_food_purple / 64) {
                    if (rad_count > n_food_purple * 4) {
                        tempradius++;
                        rad_count = 0;
                    }
                    int randx = rand() % (tempradius * 2) + clumpx - tempradius;
                    int randy = rand() % (tempradius * 2) + clumpy - tempradius;
                    float clumpdist = sqrt(pow(clumpx - randx, 2) + pow(clumpy - randy, 2));
                    if (randx < 0 || randx > grid_width || randy < 0 || randy > grid_height) {
                        continue;
                    } else if ((gen_grid[randx][randy].food == 0) & (clumpdist < tempradius)) {
                        gen_grid[randx][randy].food = 4;
                        food_count++;
                    }
                    rad_count++;
                }
            }

            // Place blue food -- random scattering of food
            total_food += food_count;
            food_count = 0;
            while (food_count < n_food_blue) {
                int randx = rand() % grid_width;
                int randy = rand() % grid_height;
                if (gen_grid[randx][randy].food == 0) {
                    gen_grid[randx][randy].food = 5;
                    food_count++;
                }
            }
        }

        // Place background food
        //food_count = 0;
        //while(food_count <= n_food_background) {
        //  int randx = rand () % grid_width;
        //  int randy = rand () % grid_height;
        //  if(gen_grid[randx][randy].food == 0) {
        //	// cout << "Food added" << endl;
        //	gen_grid[randx][randy].food = 5;
        //	food_count++;
        //  }
        //}

        // Evaluation Loop



        // Reset grid for next colony
        return_pheromone = 0.0;
        for (int x = 0; x < grid_width; x++) {
            for (int y = 0; y < grid_width; y++) {
                grid[x][y].p1 = gen_grid[x][y].p1;
                grid[x][y].p2 = gen_grid[x][y].p2;
                grid[x][y].ant_status = gen_grid[x][y].ant_status;
                grid[x][y].carrying = gen_grid[x][y].carrying;
                grid[x][y].food = gen_grid[x][y].food;
                //grid[x][y].nest = gen_grid[x][y].nest;
                grid[x][y].pen_down = gen_grid[x][y].pen_down;
            }
        }

        // Add ants	
        for (int i = 0; i < n_colonies; i++) {
            int n_active_ants = ceil(n_ants * colonies[i]->prop_active);
            for (int j = 0; j < n_ants; j++) {
                ants[i][j].x = colonies[i]->nestx;
                ants[i][j].y = colonies[i]->nesty;
                ants[i][j].search_time = 0;
                ants[i][j].prevx = -1;
                ants[i][j].prevy = -1;
                ants[i][j].ant_status = 4;
                ants[i][j].carrying = 0;
                ants[i][j].pen_down = false;
                ants[i][j].prev_pher_scent = 0.0f;
                ants[i][j].direction = rand() % 360;
                if (i < n_active_ants) {
                    ants[i][j].ant_status = 4;
                    ants[i][j].direction = rand() % 360;
                } else {
                    ants[i][j].ant_status = 0;
                }
            }
        }
    }


    // For each colony on the field set the total number of seeds placed
    // so the colonies can normalize their fitness values.
    for (int i = 0; i < n_colonies; i++)
        colonies[i]->total_seeds_available = total_food;

    //cout << total_food << endl;
}

void Field::run(bool coop_enabled) {

    //scout << "Running simulation" << endl;

    for (int col_count = 0; col_count < n_colonies; col_count++) {

        update_count++;

        //decay return_pheromone
        return_pheromone *= (1 - colonies[col_count]->decay_rate_return);

        // Update ants
        for (int ant_count = 0; ant_count < n_ants; ant_count++) {
            if (ants[col_count][ant_count].ant_status != 0)
                colonies[col_count]->ant_time_out++;

            // Update pheromones
            if (ants[col_count][ant_count].pen_down == true) {
                grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].p2 =
                        grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].p2 + deposit_rate_p2;
                grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].p2_time_updated =
                        update_count;
            }
            // Searching ants pick up food
            if (ants[col_count][ant_count].rfidx >= 0 && ants[col_count][ant_count].rfidx < grid_width
                    && ants[col_count][ant_count].rfidy >= 0
                    && ants[col_count][ant_count].rfidy < grid_height) {
                if ((ants[col_count][ant_count].ant_status == 3)
                        && grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].food > 0) {
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 2;
                    ants[col_count][ant_count].prevx = ants[col_count][ant_count].prevy =
                            ants[col_count][ant_count].search_time = -1;
                    ants[col_count][ant_count].since_move = 0;
                    ants[col_count][ant_count].carrying =
                            grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].food;
                    grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].food = 0;
                    grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].ant_status =
                            0;
                    ants[col_count][ant_count].rfidx = ants[col_count][ant_count].rfidy = -1;

                    int density_count = 0;
                    //  Scan for seeds in the neighborhood
                    for (int k = -smell_range; k <= smell_range; k++) {
                        if (ants[col_count][ant_count].x + k < 0
                                || ants[col_count][ant_count].x + k >= grid_width)
                            continue;
                        for (int l = -smell_range; l <= smell_range; l++) {
                            if (ants[col_count][ant_count].y + l < 0
                                    || ants[col_count][ant_count].y + l >= grid_height) {
                                continue;
                            }
                            if (grid[ants[col_count][ant_count].x + k][ants[col_count][ant_count].y + l].
                                    food > 0) {
                                density_count++;
                            }
                        }
                    }

                    if (rand() % 100 / 100.0f <=
                            (density_count / colonies[col_count]->dense_thresh +
                            colonies[col_count]->dense_const)) //Should I lay a pheromone?
                    { // Lay a trail
                        ants[col_count][ant_count].pen_down = true;
                        grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].p1 += 20;
                    }
                    if (rand() % 100 / 100.0f >=
                            (density_count /
                            colonies[col_count]->dense_thresh_influence +
                            colonies[col_count]->dense_const_influence)) //Should I follow a pheromone?
                    {
                        ants[col_count][ant_count].influenceable = true;
                    }
                    if (rand() % 100 / 100.0f <=
                            (density_count / colonies[col_count]->dense_thresh_patch +
                            colonies[col_count]->dense_const_patch)) //Should I use site fidelity?
                    {
                        ants[col_count][ant_count].return_x = ants[col_count][ant_count].x;
                        ants[col_count][ant_count].return_y = ants[col_count][ant_count].y;
                    } else { // Just go back to nest and pick a random direction to walk if not following a trail
                        ants[col_count][ant_count].return_x = ants[col_count][ant_count].return_y = -1;
                    }

                    /* SIMPLIFIED DECISION TREE -- non-GA-selected
                       if ((recruit == true) && (density_count >= 2))
                       {  // Lay a trail
                       ants[ant_count].pen_down = true;
                       grid[ants[ant_count].x][ants[ant_count].y].p1 += 20;
                       }
                       if ((recruit == true) && (density_count < 2))
                       {  // Will follow pheromone trails from the nest if any exist
                       ants[ant_count].influenceable = true;
                       }
                       if ((patch == true) && (density_count >= 0)) // bots always return to patch if not following a trail!
                       {  // Return to the patch
                       ants[ant_count].return_x = ants[ant_count].x;
                       ants[ant_count].return_y = ants[ant_count].y;
                       }
                       else { // Just go back to nest and pick a random direction to walk, or follow a trail if any exist
                       ants[ant_count].return_x = ants[ant_count].return_y = -1;
                       }
                     */

                }
            }
            // Arrive at nest
            if (ants[col_count][ant_count].ant_status == 2
                    && ants[col_count][ant_count].x == colonies[col_count]->nestx
                    && ants[col_count][ant_count].y == colonies[col_count]->nesty) {
                ants[col_count][ant_count].prevx = ants[col_count][ant_count].prevy = -1;
                if (ants[col_count][ant_count].carrying > 0) {
                    colonies[col_count]->seeds_collected++;

                    // Activate ants in the nest proportional to return_pheromone at nest entrance
                    // Pheromone trails present around nest?
                    float sum_pheromone = 0.0f;
                    for (int k = -1; k <= 1; k++) {
                        int i_k = ants[col_count][ant_count].x + k;
                        if (i_k < 0 || i_k >= grid_width)
                            continue;
                        for (int l = -1; l <= 1; l++) {
                            // Skip ourselves
                            if (l == 0 && k == 0)
                                continue;
                            int j_l = ants[col_count][ant_count].y + l;
                            if (j_l < 0 || j_l >= grid_height)
                                continue;
                            if (sqrt
                                    (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                    pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                    2)) - sqrt(pow(i_k - colonies[col_count]->nestx,
                                    2) + pow(j_l - colonies[col_count]->nesty,
                                    2)) <= 0) {
                                // Sum pheromones within 1 square of nest -- adjust pheromone strength as the square of distance from current location
                                sum_pheromone +=
                                        grid[i_k][j_l].p2 /
                                        pow(sqrt
                                        (pow(ants[col_count][ant_count].x - i_k, 2) +
                                        pow(ants[col_count][ant_count].y - j_l, 2)), 2);
                            }
                        }
                    }
                    // but only if arriving ant is recruiting
                    if (ants[col_count][ant_count].pen_down == true) {
                        return_pheromone += 1;
                        for (int n = 0; n < n_ants; n++) {
                            if ((ants[col_count][n].ant_status == 0)
                                    && (rand() % (1000) / (1000.0f) <
                                    (return_pheromone *
                                    colonies[col_count]->activate_sensitivity) /
                                    (n_ants))) {
                                if (sum_pheromone > 0.0) {
                                    grid[ants[col_count][n].x][ants[col_count][n].y].ant_status = ants[col_count][n].ant_status = 1; // Ant in nest sets out to follow a pheromone trail
                                } else {
                                    grid[ants[col_count][n].x][ants[col_count][n].y].ant_status =
                                            ants[col_count][n].ant_status = 4;
                                    ants[col_count][n].direction = rand() % 360;
                                }
                            }
                        }
                    }

                }
                if (ants[col_count][ant_count].carrying > 0) {
                    switch (ants[col_count][ant_count].carrying) {
                        case 1:
                            count_food_red++;
                            break;
                        case 2:
                            count_food_orange++;
                            break;
                        case 3:
                            count_food_green++;
                            break;
                        case 4:
                            count_food_blue++;
                            break;
                    }

                }

                // Pheromone trails present around nest?
                float sum_pheromone = 0.0f;
                for (int k = -1; k <= 1; k++) {
                    int i_k = ants[col_count][ant_count].x + k;
                    if (i_k < 0 || i_k >= grid_width)
                        continue;
                    for (int l = -1; l <= 1; l++) {
                        // Skip ourselves
                        if (l == 0 && k == 0)
                            continue;
                        int j_l = ants[col_count][ant_count].y + l;
                        if (j_l < 0 || j_l >= grid_height)
                            continue;
                        if (sqrt
                                (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                2)) - sqrt(pow(i_k - colonies[col_count]->nestx,
                                2) + pow(j_l - colonies[col_count]->nesty, 2)) <= 0) {
                            // Sum pheromones within 1 square of nest -- adjust pheromone strength as the square of distance from current location
                            sum_pheromone +=
                                    grid[i_k][j_l].p2 /
                                    pow(sqrt
                                    (pow(ants[col_count][ant_count].x - i_k, 2) +
                                    pow(ants[col_count][ant_count].y - j_l, 2)), 2);
                        }
                    }
                }

                grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                        ants[col_count][ant_count].ant_status = 1;

                if ((sum_pheromone > 0) & (ants[col_count][ant_count].influenceable == true)) {
                    // Ant will follow a trail leading from nest
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 1;
                } else if ((ants[col_count][ant_count].return_x != -1) &
                        (ants[col_count][ant_count].return_y != -1)) {
                    // Ant will return to patch
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 5;
                } else {
                    // Ants choose a direction to start walking
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 4;
                    ants[col_count][ant_count].direction = rand() % 360;
                }

                ants[col_count][ant_count].pen_down = false;
                ants[col_count][ant_count].influenceable = false;
                ants[col_count][ant_count].carrying = 0;
            }
            // Move ants
            // Ants following a trail
            if (ants[col_count][ant_count].ant_status == 1) {
                bool move_accepted = false;
                int reason = 0;

                // Follow trail if one exists
                // Find the out-bound cell with greatest pheromone and sum of pheromone weight on all such cells
                float back_pheromone = 0.0f;
                float sum_pheromone = 0.0f;
                float most_pheromone = 0.0f;
                for (int k = -1; k <= 1; k++) {
                    int i_k = ants[col_count][ant_count].x + k;
                    if (i_k < 0 || i_k >= grid_width)
                        continue;

                    for (int l = -1; l <= 1; l++) {
                        // Skip ourselves
                        if (l == 0 && k == 0)
                            continue;
                        int j_l = ants[col_count][ant_count].y + l;
                        if (j_l < 0 || j_l >= grid_height)
                            continue;

                        if (sqrt
                                (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                2)) - sqrt(pow(i_k - colonies[col_count]->nestx,
                                2) + pow(j_l - colonies[col_count]->nesty, 2)) <= 0) {

                            if (grid[i_k][j_l].p2 > 0) {
                                grid[i_k][j_l].p2 *=
                                        pow((1 - colonies[col_count]->decay_rate),
                                        (update_count -
                                        grid[i_k][j_l].p2_time_updated));
                                grid[i_k][j_l].p2_time_updated = update_count;

                                if (grid[i_k][j_l].p2 < 0.001f) {
                                    grid[i_k][j_l].p2 = 0;
                                }
                            }

                            // Sum pheromones within smell_range squares away from nest -- adjust pheromone strength as the square of distance from current location
                            sum_pheromone += grid[i_k][j_l].p2;
                            // Get highest pheromone on any adjacent out-bound square
                            if (grid[i_k][j_l].p2 > most_pheromone
                                    && grid[i_k][j_l].p2 > 0.0f) {
                                most_pheromone = grid[i_k][j_l].p2;
                            }
                        } else {
                            // Sum pheromones within smell_range squares in direction of nest -- adjust pheromone strength as the square of distance from current location
                            back_pheromone +=
                                    grid[i_k][j_l].p2 /
                                    pow(sqrt
                                    (pow(ants[col_count][ant_count].x - i_k, 2) +
                                    pow(ants[col_count][ant_count].y - j_l, 2)), 2);
                        }

                    }
                }


                // Drop off trail probabilistically in proportion to the degree of trail weakening in outbound moves as compared to backward moves
                // ...but only if the ant isn't in the nest or within smelling range of it!
                // ...and only if the ant is in a spot where a trail ends (i.e. p1 > 0.0)
                if ((rand() % 1000 / 1000.0f >
                        sum_pheromone /
                        back_pheromone) & (sqrt(pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                        pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                        2)) > sqrt(2 * pow(smell_range,
                        2))) &
                        (grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].p1 > 0.0)) {
                    // Traveling ant drops off the pheromone trail and begins searching
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].search_time = 0;
                    reason = 1;
                }
                    // Drop off the trail if no more pheromone on out-bound cells (trail has evaporated)
                else if (sum_pheromone <= 0.0f) {
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].search_time = -1;
                    reason = 2;
                }
                    // ants have a small probability of dropping off the trail each time step
                else if (rand() % 10000 / 10000.0f <
                        colonies[col_count]->trail_drop_rate) {
                    // Traveling ant drops off the pheromone trail and begins searching
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                            ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].prevx = ants[col_count][ant_count].prevy = -1;
                    ants[col_count][ant_count].search_time = -1;
                    reason = 3;
                }
                else {
                    // Random, accept a move with probability proportional to the ratio of the
                    // pheromone on the square to be moved to and the adjacent square with the
                    // highest amount of pheromone
                    int new_x = -1, new_y = -1;
                    while (!move_accepted) {

                        new_x = ants[col_count][ant_count].x + rand() % 3 - 1;
                        new_y = ants[col_count][ant_count].y + rand() % 3 - 1;
                        if ((new_x < 0) || (new_x >= grid_width) || (new_y < 0)
                                || (new_y >= grid_height))
                            continue;
                        // Disregard possible moves that take out-bound ant closer to nest
                        if (sqrt
                                (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                2)) - sqrt(pow(new_x - colonies[col_count]->nestx,
                                2) + pow(new_y - colonies[col_count]->nesty, 2)) > 0)
                            continue;
                        if ((most_pheromone <= 0.0f)
                                || (rand() % 100 / 100.0f <
                                grid[new_x][new_y].p2 / most_pheromone)) {
                            move_accepted = true;
                        }
                    }

                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status = 0;

                    // Guard against illegal positions
                    if (new_x < 0 || new_x >= grid_width ||
                            new_y < 0 || new_y >= grid_height) {
                        cout << "Debug: Illegal position " << "(" << new_x << ", " << new_y << ")" << endl;
                    } else {
                        ants[col_count][ant_count].x = new_x;
                        ants[col_count][ant_count].y = new_y;
                    }

                    grid[new_x][new_y].ant_status = ants[col_count][ant_count].ant_status;
                    ants[col_count][ant_count].prev_pher_scent = sum_pheromone;
                }
                reason = reason; // To avoid unused varable compile error
            }

            // Ants returning to last successful patch
            if (ants[col_count][ant_count].ant_status == 5) {
                if ((ants[col_count][ant_count].return_x == -1
                        && ants[col_count][ant_count].return_y == -1)
                        || (ants[col_count][ant_count].x == ants[col_count][ant_count].return_x
                        && ants[col_count][ant_count].y == ants[col_count][ant_count].return_y)) {
                    ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].search_time = 0;
                    ants[col_count][ant_count].search_direction = rand() % 360 - 180;
                    int newdx = round(cos(ants[col_count][ant_count].search_direction));
                    int newdy = round(sin(ants[col_count][ant_count].search_direction));
                    if (ants[col_count][ant_count].x + newdx >= 0
                            && ants[col_count][ant_count].x + newdx < grid_width
                            && ants[col_count][ant_count].y + newdy >= 0
                            && ants[col_count][ant_count].y + newdy < grid_height) {
                        ants[col_count][ant_count].rfidx = ants[col_count][ant_count].x + newdx; // place rfid/qr reader in front of ant
                        ants[col_count][ant_count].rfidy = ants[col_count][ant_count].y + newdy;
                        grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                ant_status = 9;
                    }
                } else {

                    // Find the adjacent square that decreases euclidean distance to patch the most
                    float most_distance = 0.0f;
                    for (int k = -1; k < 2; k++) {
                        int i_k = ants[col_count][ant_count].x + k;
                        if (i_k < 0 || i_k >= grid_width)
                            continue;

                        for (int l = -1; l < 2; l++) {
                            // Skip ourselves
                            if (l == 0 && k == 0)
                                continue;
                            int j_l = ants[col_count][ant_count].y + l;
                            if (j_l < 0 || j_l >= grid_height)
                                continue;

                            // Distance
                            if (sqrt
                                    (pow
                                    (ants[col_count][ant_count].x - ants[col_count][ant_count].return_x,
                                    2) + pow(ants[col_count][ant_count].y -
                                    ants[col_count][ant_count].return_y,
                                    2)) - sqrt(pow(i_k -
                                    ants[col_count][ant_count].
                                    return_x,
                                    2) + pow(j_l -
                                    ants[col_count]
                                    [ant_count].
                                    return_y,
                                    2)) >
                                    most_distance) {
                                most_distance =
                                        sqrt(pow
                                        (ants[col_count][ant_count].x -
                                        ants[col_count][ant_count].return_x,
                                        2) + pow(ants[col_count][ant_count].y -
                                        ants[col_count][ant_count].return_y,
                                        2)) - sqrt(pow(i_k -
                                        ants[col_count][ant_count].
                                        return_x,
                                        2) + pow(j_l -
                                        ants[col_count]
                                        [ant_count].
                                        return_y,
                                        2));
                                //
                            }
                        }
                    }
                    // Random, accept a move with probability proportional to the ratio of the
                    // distance of the square to be moved to and the adjacent square with the
                    // greatest decrease in distance from the patch
                    int new_x = -1, new_y = -1;
                    bool move_accepted = false;
                    while (!move_accepted) {
                        new_x = ants[col_count][ant_count].x + rand() % 3 - 1;
                        new_y = ants[col_count][ant_count].y + rand() % 3 - 1;
                        if ((most_distance <= 0.0f)
                                || rand() % 100 / 100.0f <
                                (sqrt
                                (pow(ants[col_count][ant_count].x - ants[col_count][ant_count].return_x, 2)
                                + pow(ants[col_count][ant_count].y - ants[col_count][ant_count].return_y,
                                2)) - sqrt(pow(new_x -
                                ants[col_count][ant_count].return_x,
                                2) + pow(new_y -
                                ants[col_count][ant_count].
                                return_y,
                                2))) /
                                most_distance) {
                            move_accepted = true;
                        }
                    }
                    if (new_x < 0 || new_x >= grid_width || new_y < 0
                            || new_y >= grid_height)
                        continue;
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status = 0;
                    grid[new_x][new_y].ant_status = ants[col_count][ant_count].ant_status;

                    // Guard against illegal positions
                    if (new_x < 0 || new_x >= grid_width ||
                            new_y < 0 || new_y >= grid_height) {
                        cout << "Debug: Illegal position " << "(" << new_x << ", " << new_y << ")" << endl;
                    } else {
                        ants[col_count][ant_count].x = new_x;
                        ants[col_count][ant_count].y = new_y;
                    }
                }

            }

            // Traveling ants, out-bound from nest but not following a trail
            if (ants[col_count][ant_count].ant_status == 4) {
                if ((rand() % 10000 / 10000.0f <
                        colonies[col_count]->walk_drop_rate)) {
                    ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].search_time = -1;
                    ants[col_count][ant_count].search_direction = 360.0;
                    int newdx = round(cos(ants[col_count][ant_count].search_direction));
                    int newdy = round(sin(ants[col_count][ant_count].search_direction));
                    if (ants[col_count][ant_count].x + newdx >= 0
                            && ants[col_count][ant_count].x + newdx < grid_width
                            && ants[col_count][ant_count].y + newdy >= 0
                            && ants[col_count][ant_count].y + newdy < grid_height) {
                        ants[col_count][ant_count].rfidx = ants[col_count][ant_count].x + newdx; // place rfid reader behind ant
                        ants[col_count][ant_count].rfidy = ants[col_count][ant_count].y + newdy;
                        grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                ant_status = 9;
                    }
                    continue;
                }
                float idealx; // Optimal X and Y move given the ant's chosen direction
                float idealy; // Move may not be possible (i.e., not on the grid) but the ant will try to get as close as possible
                if (fabs(sin(ants[col_count][ant_count].direction * pi / 180)) >
                        fabs(cos(ants[col_count][ant_count].direction * pi / 180))) {
                    idealx =
                            ants[col_count][ant_count].x +
                            50 * (cos(ants[col_count][ant_count].direction * pi / 180) /
                            fabs(sin(ants[col_count][ant_count].direction * pi / 180)));
                    idealy =
                            ants[col_count][ant_count].y +
                            50 * (sin(ants[col_count][ant_count].direction * pi / 180) /
                            fabs(sin(ants[col_count][ant_count].direction * pi / 180)));
                } else {
                    idealx =
                            ants[col_count][ant_count].x +
                            50 * (cos(ants[col_count][ant_count].direction * pi / 180) /
                            fabs(cos(ants[col_count][ant_count].direction * pi / 180)));
                    idealy =
                            ants[col_count][ant_count].y +
                            50 * (sin(ants[col_count][ant_count].direction * pi / 180) /
                            fabs(cos(ants[col_count][ant_count].direction * pi / 180)));
                }


                // Find the move that would decrease distance to the ideal move the most
                float most_distance = 0.0f;
                for (int k = -1; k < 2; k++) {
                    int i_k = ants[col_count][ant_count].x + k;
                    if (i_k < 0 || i_k >= grid_width)
                        continue;

                    for (int l = -1; l < 2; l++) {
                        // Skip ourselves
                        if (l == 0 && k == 0)
                            continue;
                        int j_l = ants[col_count][ant_count].y + l;
                        if (j_l < 0 || j_l >= grid_height)
                            continue;

                        // Distance
                        if (sqrt
                                (pow(ants[col_count][ant_count].x - idealx, 2) +
                                pow(ants[col_count][ant_count].y - idealy,
                                2)) - sqrt(pow(i_k - idealx,
                                2) + pow(j_l - idealy,
                                2)) > most_distance) {
                            most_distance =
                                    sqrt(pow(ants[col_count][ant_count].x - idealx, 2) +
                                    pow(ants[col_count][ant_count].y - idealy,
                                    2)) - sqrt(pow(i_k - idealx,
                                    2) + pow(j_l - idealy,
                                    2));

                        }
                    }
                }
                // Random, accept a move with probability proportional to the ratio of the
                // distance of the square to be moved to and the adjacent square with the
                // greatest decrease in distance from the ideal move
                int new_x = -1, new_y = -1;
                bool move_accepted = false;
                while (!move_accepted) {
                    new_x = ants[col_count][ant_count].x + rand() % 3 - 1;
                    new_y = ants[col_count][ant_count].y + rand() % 3 - 1;
                    //Skip ourselves
                    if ((new_x == ants[col_count][ant_count].x) & (new_y == ants[col_count][ant_count].y))
                        continue;
                    if ((most_distance <= 0.0f)
                            || rand() % 100 / 100.0f <
                            (sqrt
                            (pow(ants[col_count][ant_count].x - idealx, 2) +
                            pow(ants[col_count][ant_count].y - idealy,
                            2)) - sqrt(pow(new_x - idealx,
                            2) + pow(new_y - idealy,
                            2))) / most_distance) {
                        move_accepted = true;
                    }
                }

                if (new_x < 0 || new_x >= grid_width || new_y < 0
                        || new_y >= grid_height) {
                    ants[col_count][ant_count].ant_status = 3;
                    ants[col_count][ant_count].search_time = -1;
                    continue;
                }


                if (ants[col_count][ant_count].x < 0 || ants[col_count][ant_count].x >= grid_width
                        || ants[col_count][ant_count].y < 0 || ants[col_count][ant_count].y >= grid_height
                        || new_x < 0 || new_x >= grid_width || new_y < 0 || new_y >= grid_height) {
                    cout << "Debug: Illegal position " << "(" << ants[col_count][ant_count].x << ", " << ants[col_count][ant_count].y << ")" << endl;
                } else {
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status = 0;
                    grid[new_x][new_y].ant_status = ants[col_count][ant_count].ant_status;
                }

                // Guard against illegal positions
                if (new_x < 0 || new_x >= grid_width ||
                        new_y < 0 || new_y >= grid_height) {
                    cout << "Debug: Illegal position " << "(" << new_x << ", " << new_y << ")" << endl;
                } else {
                    ants[col_count][ant_count].x = new_x;
                    ants[col_count][ant_count].y = new_y;
                }
            }

            // Searching ants
            if (ants[col_count][ant_count].ant_status == 3) {
                if (ants[col_count][ant_count].since_move < search_delay)
                    ants[col_count][ant_count].since_move++;
                else {
                    // Searching ants smell food in adjacent squares.  If an ant smells food in an adjacent square
                    // where no other ant is present (the food is available for pickup) the ant will move to one of
                    // these squares; otherwise selects a square at random.

                    // CODE FOR ALLOWING SEARCHING ANTS TO SMELL AND MOVE TO FOOD IN ADJACENT SQUARES -- off for the AntBots
                    int food_count = 0;
                    /*		      for ( int k = -1; k < 2; k++ )
                                            {
                                            for ( int l = -1; l < 2; l++ )
                                            {
                                            if ( ants[col_count][ant_count].x+k < grid_width && ants[col_count][ant_count].x+k >= 0 && ants[col_count][ant_count].y+l < grid_height && ants[col_count][ant_count].y+l >= 0 )
                                            {
                                            if ( grid[ants[col_count][ant_count].x+k][ants[col_count][ant_count].y+l].food > 0 & grid[ants[col_count][ant_count].x+k][ants[col_count][ant_count].y+l].ant_status == 0 ) food_count++;
                                            }  
                                            }
                                            } */
                    int new_x = -1, new_y = -1;
                    int search_loop = 0;
                    bool found_a_seed = false;
                    bool move_accepted = false;
                    float new_direction;
                    while (!move_accepted && ants[col_count][ant_count].ant_status == 3) {
                        search_loop++;
                        if (((ants[col_count][ant_count].search_direction == 360.0))
                                || (ants[col_count][ant_count].x == 0) || (ants[col_count][ant_count].y == 0)
                                || (ants[col_count][ant_count].x == grid_width - 1)
                                || (ants[col_count][ant_count].y == grid_height - 1)
                                || (food_count > 0)) {
                            ants[col_count][ant_count].search_direction =
                                    (rand() % 360) - 180;
                        }

                        float d_theta;
                        if (ants[col_count][ant_count].search_time >= 0.0) {
                            d_theta =
                                    randomNormal(0,
                                    ((colonies[col_count]->dir_dev_coeff1 *
                                    pow(ants[col_count][ant_count].search_time,
                                    colonies[col_count]->
                                    dir_time_pow1)) +
                                    (colonies[col_count]->dir_dev_coeff2 /
                                    pow(ants[col_count][ant_count].search_time,
                                    colonies[col_count]->
                                    dir_time_pow2)) +
                                    colonies[col_count]->dir_dev_const));
                        } else
                            d_theta =
                                randomNormal(0, colonies[col_count]->dir_dev_const);
                        new_direction = ants[col_count][ant_count].search_direction + d_theta;
                        if (update_count % 3 == 0) { // ants pick a new direction only every 30 cm, like the antbots.
                            new_direction = pmod(ants[col_count][ant_count].search_direction + (d_theta * 180.f / M_PI), 360);
                            if (ants[col_count][ant_count].search_time >= 0.0)
                                ants[col_count][ant_count].search_time++;
                        } else {
                            new_direction = ants[col_count][ant_count].search_direction;
                        }
                        int newdx = round(cos(new_direction));
                        int newdy = round(sin(new_direction));
                        new_x = ants[col_count][ant_count].x + newdx;
                        new_y = ants[col_count][ant_count].y + newdy;

                        if ((new_x < 0) || (new_x >= grid_width) || (new_y < 0)
                                || (new_y >= grid_height))
                            continue;
                        if ((new_x == ants[col_count][ant_count].x) & (new_y ==
                                ants[col_count][ant_count].y))
                            continue;
                        // CODE FOR MOVING TO FOOD IN ADJACENT SQUARE
                        if (food_count > 0) {

                            if (grid[new_x][new_y].food > 0
                                    && grid[new_x][new_y].ant_status == 0) {
                                move_accepted = true;
                            } else
                                continue;
                        } else
                            if (rand() % 10000 / 10000.0f <
                                colonies[col_count]->search_giveup_rate) {
                            // Ants that do not smell food probabilistically give up searching and begin to return to the nest
                            ants[col_count][ant_count].prevx = ants[col_count][ant_count].prevy = -1;
                            ants[col_count][ant_count].search_direction = 360.0;
                            ants[col_count][ant_count].ant_status = 2;
                            if (ants[col_count][ant_count].rfidx >= 0
                                    && ants[col_count][ant_count].rfidx < grid_width
                                    && ants[col_count][ant_count].rfidy >= 0
                                    && ants[col_count][ant_count].rfidy < grid_height) {
                                grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                        ant_status = 0;
                            }
                            ants[col_count][ant_count].rfidx = ants[col_count][ant_count].rfidy = -1;
                            // Will pick a random spot to search if no pheromone trails at nest.
                            ants[col_count][ant_count].ant_status = 4;
                            // Will follow pheromone trails from the nest if any exist
                            ants[col_count][ant_count].influenceable = true;
                            continue;
                        } else {
                            move_accepted = true;
                        }

                    }
                    if (ants[col_count][ant_count].ant_status == 3) {
                        // ant turns in the direction it intends to travel, RFID/QR reader rotates in front of it
                        float rfid_direction = ants[col_count][ant_count].search_direction;
                        int newdx = round(cos(rfid_direction));
                        int newdy = round(sin(rfid_direction));
                        if (ants[col_count][ant_count].x + newdx >= 0
                                && ants[col_count][ant_count].x + newdx < grid_width
                                && ants[col_count][ant_count].y + newdy >= 0
                                && ants[col_count][ant_count].y + newdy < grid_height) {
                            if (ants[col_count][ant_count].rfidx >= 0
                                    && ants[col_count][ant_count].rfidx < grid_width
                                    && ants[col_count][ant_count].rfidy >= 0
                                    && ants[col_count][ant_count].rfidy < grid_height) {
                                grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                        ant_status = 0;
                            }
                            ants[col_count][ant_count].rfidx = ants[col_count][ant_count].x + newdx; // place rfid reader behind ant
                            ants[col_count][ant_count].rfidy = ants[col_count][ant_count].y + newdy;
                            if (grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                    food > 0) {
                                found_a_seed = true;
                            }
                        }

                        // if the RFID reader has not passed over a seed then move the ant to its new location.
                        if (found_a_seed == false) {
                            sumdx += new_x - ants[col_count][ant_count].x;
                            sumdy += new_y - ants[col_count][ant_count].y;
                            grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status =
                                    0;
                            if (ants[col_count][ant_count].rfidx >= 0
                                    && ants[col_count][ant_count].rfidx < grid_width
                                    && ants[col_count][ant_count].rfidy >= 0
                                    && ants[col_count][ant_count].rfidy < grid_height) {
                                grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                        ant_status = 0;
                            }
                            ants[col_count][ant_count].x = new_x;
                            ants[col_count][ant_count].y = new_y;
                            ants[col_count][ant_count].search_direction = new_direction;

                            ants[col_count][ant_count].rfidx = ants[col_count][ant_count].x + newdx; // place rfid reader in front of ant again
                            ants[col_count][ant_count].rfidy = ants[col_count][ant_count].y + newdy;
                            if (ants[col_count][ant_count].rfidx >= 0
                                    && ants[col_count][ant_count].rfidx < grid_width
                                    && ants[col_count][ant_count].rfidy >= 0
                                    && ants[col_count][ant_count].rfidy < grid_height) {
                                grid[ants[col_count][ant_count].rfidx][ants[col_count][ant_count].rfidy].
                                        ant_status = 9;
                            }
                            grid[new_x][new_y].ant_status =
                                    ants[col_count][ant_count].ant_status;

                            ants[col_count][ant_count].since_move = 0;
                        }
                    }
                }
            }                // Return to the nest
            else if (ants[col_count][ant_count].ant_status == 2) {
                // Find the adjacent square that decreases euclidean distance to nest the most
                if ((ants[col_count][ant_count].carrying != 0)
                        && (ants[col_count][ant_count].since_move < return_delay)) {
                    ants[col_count][ant_count].since_move++;
                } else {

                    float most_distance = 0.0f;
                    for (int k = -1; k < 2; k++) {
                        int i_k = ants[col_count][ant_count].x + k;
                        if (i_k < 0 || i_k >= grid_width)
                            continue;

                        for (int l = -1; l < 2; l++) {
                            // Skip ourselves
                            if (l == 0 && k == 0)
                                continue;
                            int j_l = ants[col_count][ant_count].y + l;
                            if (j_l < 0 || j_l >= grid_height)
                                continue;

                            // Distance
                            if (sqrt
                                    (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                    pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                    2)) - sqrt(pow(i_k - colonies[col_count]->nestx,
                                    2) + pow(j_l - colonies[col_count]->nesty,
                                    2)) >
                                    most_distance) {
                                most_distance =
                                        sqrt(pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                        pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                        2)) - sqrt(pow(i_k - colonies[col_count]->nestx,
                                        2) + pow(j_l - colonies[col_count]->nesty,
                                        2));
                                //
                            }
                        }
                    }
                    // Random, accept a move with probability proportional to the ratio of the
                    // distance of the square to be moved to and the adjacent square with the
                    // greatest decrease in distance from the nest
                    int new_x = -1, new_y = -1;
                    bool move_accepted = false;
                    while (!move_accepted) {
                        //                      cout << "...moving?" << endl;
                        new_x = ants[col_count][ant_count].x + rand() % 3 - 1;
                        new_y = ants[col_count][ant_count].y + rand() % 3 - 1;
                        if (new_x < 0 || new_x >= grid_width || new_y < 0
                                || new_y >= grid_height)
                            continue;
                        if ((most_distance <= 0.0f)
                                || rand() % 100 / 100.0f <
                                (sqrt
                                (pow(ants[col_count][ant_count].x - colonies[col_count]->nestx, 2) +
                                pow(ants[col_count][ant_count].y - colonies[col_count]->nesty,
                                2)) - sqrt(pow(new_x - colonies[col_count]->nestx,
                                2) + pow(new_y - colonies[col_count]->nesty,
                                2))) / most_distance) {
                            move_accepted = true;
                        }
                    }
                    grid[ants[col_count][ant_count].x][ants[col_count][ant_count].y].ant_status = 0;
                    grid[new_x][new_y].ant_status = ants[col_count][ant_count].ant_status;


                    // Guard against illegal positions
                    if (new_x < 0 || new_x >= grid_width ||
                            new_y < 0 || new_y >= grid_height) {
                        cout << "Debug: Illegal position " << "(" << new_x << ", " << new_y << ")" << endl;
                    } else {
                        ants[col_count][ant_count].x = new_x;
                        ants[col_count][ant_count].y = new_y;
                    }
                }

                ants[col_count][ant_count].since_move = 0;
            }
        }



    }


}

Location** Field::getGrid() {
    return grid;
}

Field::~Field() {

    for (int i = 0; i < n_colonies; i++) {
        delete [] ants[i];
    }
    delete [] ants;

    for (int i = 0; i < grid_width; i++) {
        delete [] gen_grid[i];
    }
    delete [] gen_grid;

    for (int i = 0; i < grid_width; i++) {
        delete [] grid[i];
    }
    delete [] grid;

}
