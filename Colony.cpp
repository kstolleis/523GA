#include "Colony.h"

#include <iomanip>
#include <iostream>

using namespace std;

void Colony::printGenome(){
  cout << setprecision (10) 
       << "Genome: " << endl
       << "Decay Rate: " << decay_rate << endl
       << "walk_drop_rate: " << walk_drop_rate << endl
       << "search_giveup_rate: " << search_giveup_rate << endl
       << "trail_drop_rate: " << trail_drop_rate << endl
       << "dir_dev_const: " << dir_dev_const << endl
       << "dir_dev_coeff1: " << dir_dev_coeff1 << endl
       << "dir_time_pow1: " << dir_time_pow1 << endl
       << "dir_dev_coeff2: " << dir_dev_coeff2 << endl
       << "dir_time_pow2: " << dir_time_pow2 << endl
       << "dense_sens: " << dense_sens << endl
       << "dense_thresh: " << dense_thresh << endl
       << "dense_const: " << dense_const << endl
       << "dense_thresh_patch: " << dense_thresh_patch << endl
       << "dense_const_patch: " << dense_const_patch << endl
       << "dense_thresh_influence: " << dense_thresh_influence << endl
       << "dense_const_influence: " << dense_const_influence << endl
       << "prop_active: " << prop_active << endl
       << "activate_sensitivity: " << activate_sensitivity << endl
       << "decay_rate_return: " <<  decay_rate_return << endl         
       << "dir_dev_const: " << dir_dev_const << endl
       << "seeds_collected: " << seeds_collected/num_chances << endl;
}

Colony::~Colony(){
}
