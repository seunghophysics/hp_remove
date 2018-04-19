#ifndef _hp_remove_lib_
#define _hp_remove_lib_

#include "TH2.h"

using namespace std;

// This should be the name of the object (2D histogram in our case) in the ROOT files.
// This is set as "t1" by default, in the main program.
extern const char *obj_name;

// Gets histogram "obj_name" out of a file, and returns it with a new name "hist_name".
TH2S *get_hist(const char *file, const char *hist_name, const char *obj_name);

// Cuts histogram with a given "range".
TH2S *cut_hist(TH2S *hist, int range[4]);

// Cuts histogram with a given range (beg_x, end_x, beg_y, end_y).
TH2S *cut_hist(TH2S *hist, int beg_x, int end_x, int beg_y, int end_y);

// Gets threshold value of given significance, for determining hot pixels in a given dark picture.
// Setting the draw option to TRUE will draw the intensity spectrum of the dark picture.
double get_threshold(TH2S *dark, double significance, bool draw_opt);

// Returs a 2D hot pixel map taken from a given dark picture.
// Utilizes the function "get_threshold".
TH2S *hot_pix_map(TH2S *dark, double significance);

// Generates a map of all individual hot pixel maps combined (union), given a vector of hot pixel maps.
TH2S *union_map(vector<TH2S *> map_vec);

// Generates a vector of hot pixel maps out of the information written in the map card. 
// Be careful with "obj_name" if your histogram name is not "t1" as default.
vector<TH2S *> read_map(const char *map_card, double significance, const char *obj_name);

// vec and map are size-9 vectors.
// vec : contains values of a pixel (vec[4]) with its surrounding neighbors (vec[!4]).
// map : indicates whether a pixel is hot (map[4] == 1) or not (map[4] == 0), including its neighbors (map[!4]).
// Gets average value of a pixel's "normal" neighbors.
double get_average(vector<int> vec, vector<int> map);

// Determines whether a pixel (neighbor[4]) is the maximum among all the good pixels.
bool max_true(vector<int> neighbors, vector<int> neighbor_map);

// Moderation Step 1:
// Avereage out all pixels over threshold.
// Cuts the given data histogram if some range is given.
TH2S *mod1(TH2S *data, double threshold, int range[4] = NULL);

// Moderation Step 2:
// Average out pixels if they are brighter than their neighbors "and" on the given hot pixel map.
TH2S *mod2(TH2S *mod1_data, TH2S *cut_union_map);

// Moderation Step 3:
// Average out lonely bright pixels with values higher than coefficient * average of its neighbors.
TH2S *mod3(TH2S *mod2_data, double coefficient);

// Locate all pixels in data over threshold.
void locate(TH2S *data, double threshold);

#endif
