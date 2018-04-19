#ifndef _hp_remove_lib_
#define _hp_remove_lib_

#include "TH2.h"

using namespace std;

extern const char *obj_name;

TH2S *get_hist(const char *file, const char *hist_name, const char *obj_name);

TH2S *cut_hist(TH2S *hist, int range[4]);

TH2S *cut_hist(TH2S *hist, int beg_x, int end_x, int beg_y, int end_y);

double get_threshold(TH2S *dark, double significance, bool draw_opt);

TH2S *hot_pix_map(TH2S *dark, double significance);

TH2S *union_map(vector<TH2S *> map_vec);

vector<TH2S *> read_map(const char *map_card, double significance, const char *obj_name);

double get_average(vector<int> vec, vector<int> map);

bool max_true(vector<int> neighbors, vector<int> neighbor_map);

TH2S *mod1(TH2S *data, double threshold, int range[4] = NULL);

TH2S *mod2(TH2S *mod1_data, TH2S *cut_union_map);

TH2S *mod3(TH2S *mod2_data, double coefficient);

void locate(TH2S *data, double threshold);

#endif
