#include "TH2.h"
#include "TFile.h"
#include "TF1.h"
#include "TROOT.h"

#include <iostream>
#include <fstream>

using namespace std;

TH2S *get_hist(const char *file, const char *hist_name, const char *obj_name = "t1")
{
  TFile *f = new TFile(file);

  TH2S *hist = 0;
	f->GetObject(obj_name, hist);
	hist->SetName(hist_name);
	hist->SetDirectory(0);

	f->Close();

  return hist;
}


TH2S *cut_hist(TH2S *hist, int range[4])
{  
	double x_width = hist->GetXaxis()->GetBinWidth(1);
	double y_width = hist->GetYaxis()->GetBinWidth(1);

	double x_min = hist->GetXaxis()->GetXmin();
	double x_max = hist->GetXaxis()->GetXmax();
	double y_min = hist->GetYaxis()->GetXmin();
	double y_max = hist->GetYaxis()->GetXmax();

	int beg_x = range[0];
	int end_x = range[1];
	int beg_y = range[2];
	int end_y = range[3];
 
	if (beg_x < x_min || end_x > x_max || beg_y < y_min || end_y > y_max) {
		cout << "Improper range for cut_hist. Please try again." << endl;
		exit(0);
	}
	
  TH2S *cut_hist = new TH2S(Form("%s_%s", hist->GetName(), "cut"), "Cut Histogram; x coordinate; y coordinate;", (end_x - beg_x) / x_width, beg_x, end_x, (end_y - beg_y) / y_width, beg_y, end_y);

  for (int i = 1; i <= cut_hist->GetNbinsX(); i++)
    for (int j = 1; j <= cut_hist->GetNbinsY(); j++)
      cut_hist->SetBinContent(i, j, hist->GetBinContent((beg_x - x_min) / x_width + i, (beg_y - y_min) / y_width + j));

  return cut_hist;
}


TH2S *cut_hist(TH2S *hist, int beg_x, int end_x, int beg_y, int end_y)
{  
	double x_width = hist->GetXaxis()->GetBinWidth(1);
	double y_width = hist->GetYaxis()->GetBinWidth(1);

	double x_min = hist->GetXaxis()->GetXmin();
	double x_max = hist->GetXaxis()->GetXmax();
	double y_min = hist->GetYaxis()->GetXmin();
	double y_max = hist->GetYaxis()->GetXmax();
 
	if (beg_x < x_min || end_x > x_max || beg_y < y_min || end_y > y_max) {
		cout << "Improper range for cut_hist. Please try again." << endl;
		exit(0);
	}
	
  TH2S *cut_hist = new TH2S(Form("%s_%s", hist->GetName(), "cut"), "Cut Histogram; x coordinate; y coordinate;", (end_x - beg_x) / x_width, beg_x, end_x, (end_y - beg_y) / y_width, beg_y, end_y);

  for (int i = 1; i <= cut_hist->GetNbinsX(); i++)
    for (int j = 1; j <= cut_hist->GetNbinsY(); j++)
      cut_hist->SetBinContent(i, j, hist->GetBinContent((beg_x - x_min) / x_width + i, (beg_y - y_min) / y_width + j));

  return cut_hist;
}


double get_threshold(TH2S *dark, double significance, bool draw_opt)
{
  int max_content = (int)dark->GetMaximum();
  
	if (gROOT->FindObject("amp_spec") != NULL)
		gROOT->FindObject("amp_spec")->Delete();
   
  TH1D *amp_spec = new TH1D("amp_spec", "Intensity Spectrum; Intensity (ADC counts); Frequency;", max_content, 0, max_content);
  
  for(int i = 1; i <= dark->GetNbinsX(); i++)
    for(int j = 1; j <= dark->GetNbinsY(); j++)
      amp_spec->Fill(dark->GetBinContent(i, j));
  
  TF1 *gf = new TF1("gf", "gaus", 0, max_content);
  amp_spec->Fit(gf, "Q0");
  
  double mean = gf->GetParameter(1);
  double sigma = gf->GetParameter(2);
  
  if(draw_opt) 
    amp_spec->Draw();

  return mean + significance * sigma;
}


TH2S *hot_pix_map(TH2S *dark, double significance)
{
	double threshold = get_threshold(dark, significance, 0);
	cout << "Threshold : " << threshold << endl;

	double x_min = dark->GetXaxis()->GetXmin();
	double x_max = dark->GetXaxis()->GetXmax();
	double y_min = dark->GetYaxis()->GetXmin();
	double y_max = dark->GetYaxis()->GetXmax();
	 
	if (gROOT->FindObject(Form("%s_%s", dark->GetName(), "hp_map")) != NULL)
		gROOT->FindObject(Form("%s_%s", dark->GetName(), "hp_map"))->Delete();
   
  TH2S *map = new TH2S(Form("%s_%s", dark->GetName(), "hp_map"), "Hot Pixel Coordinates; x coordinate; y coordinate", dark->GetNbinsX(), x_min, x_max, dark->GetNbinsY(), y_min, y_max);
  
  for(int i = 1; i <= dark->GetNbinsX(); i++){
    for(int j = 1; j <= dark->GetNbinsY(); j++){
      if(dark->GetBinContent(i, j) < threshold)
        map->SetBinContent(i, j, 0);
      else
        map->SetBinContent(i, j, 1);
    }
  }      
  
  return map;
}


TH2S *union_map(vector<TH2S *> map_vec)
{
	if (map_vec.empty()) {
		cout << "Empty map vector. Please try again." << endl;
		exit(0);
	}
	
	cout << "\nGenerating hot pixel candidate map from the dark pictures...\n" << endl;

	TH2S *union_map = (TH2S*)map_vec[0]->Clone("union_map");
	union_map->Reset();

	int x_bins =  map_vec[0]->GetNbinsX();
	int y_bins =  map_vec[0]->GetNbinsY();

	for (int i = 1; i <= map_vec[0]->GetNbinsX(); i++) {
		for (int j = 1; j <= map_vec[0]->GetNbinsY(); j++) {
			for (int k = 0; k < map_vec.size(); k++) {
				if (map_vec[k]->GetBinContent(i, j) == 1) {
					union_map->SetBinContent(i, j, 1);
					continue;
				}
			}
		}
	}

	return union_map;
}


vector<TH2S *> read_map(const char *map_card, double significance, const char *obj_name = "t1")
{
	ifstream in_file(map_card);
	string line;
	
	vector<TH2S *> map_vec;

	int i = 1;
	
	while (!in_file.eof()) {
		getline(in_file, line);
		
		while (line.empty()) {
			getline(in_file, line);
			if (in_file.eof())
				break;
		}
			
		if(in_file.eof())
			break;

		cout << Form("\nReading dark #%d from ", i) << line << "..." << endl;

		map_vec.push_back(hot_pix_map(get_hist(line.c_str(), Form("dark_%d", i), obj_name), significance));
	
		i++;
	}

	in_file.close();

	return map_vec;
}


double get_average(vector<int> vec, vector<int> map)
{
  double sum = 0.0;
  double denominator = 0.0;

  for(int i = 0; i <= 8; i++){
    if(!map[i]){
      sum += vec[i];
      denominator++;
    }
  }
  
  denominator += 1e-5;

  return sum / denominator;
}


TH2S* mod1(TH2S *data, double threshold, int range[4] = NULL)
{
	TH2S *mod1_data;
	TH2S *cut_data;

	if (range == NULL) {
		cut_data = (TH2S*)data->Clone("cut_data");
		mod1_data = (TH2S*)data->Clone("mod1_data");
	}

	else { 
		cut_data = cut_hist(data, range);	
		mod1_data = (TH2S*)cut_data->Clone("mod1_data");
	}

	vector<int> neighbors;
	vector<int> neighbor_map;
	
	int average;

	cout << "\nReplacing a value (> threshold) with the average of its good neighbors...\n" << endl;	
	cout << "Marking X on the right, in case average > threshold...\n" << endl;
	cout << "Location\t" << "Value\t\t" << "Average" << endl;
	cout << "--------\t" << "-----\t\t" << "-------" << endl;

	for(int i = 1; i <= cut_data->GetNbinsX(); i++){
		for(int j = 1; j <= cut_data->GetNbinsY(); j++){

			neighbors.clear();
			neighbor_map.clear();		

			if(cut_data->GetBinContent(i ,j) > threshold){
				
				for(int k = -1; k <= 1; k++){
					for(int l = -1; l <= 1; l++){
						neighbors.push_back(cut_data->GetBinContent(i + k, j + l));
						if(cut_data->GetBinContent(i + k, j + l) > threshold) neighbor_map.push_back(1);
						else neighbor_map.push_back(0);
					}
				}

				average = (int)(get_average(neighbors, neighbor_map) + 0.5);
				mod1_data->SetBinContent(i, j, average);

				cout << Form("(%d, %d) \t", i, j) << cut_data->GetBinContent(i, j) << "\t\t" << average;
				if (average > threshold) cout << " \t  X" << endl;
				else cout << endl;
			}
	
		}
	}

	return mod1_data;
}


bool max_true(vector<int> neighbors, vector<int> neighbor_map)
{
	vector<int> good_neighbors;

	for(int i = 0; i < 9; i++) {
		if(neighbor_map[i] == 0)
			good_neighbors.push_back(neighbors[i]);
	}
	
	if(good_neighbors.empty())
		return false;

	else if(neighbors[4] > *max_element(good_neighbors.begin(), good_neighbors.end()))
		return true;

	else
		return false; 	
}


TH2S *mod2(TH2S *mod1_data, TH2S *cut_union_map)
{	
	TH2S *mod2_data = (TH2S*)mod1_data->Clone("mod2_data");	

	vector<int> neighbors;
	vector<int> neighbor_map;
	
	int average;

	cout << "\nReplacing a value (> good neighbors) with the average of its good neighbors...\n" << endl;
	cout << "Marking X on the right, in case value > average...\n" << endl;
	cout << "Location\t" << "Value\t\t" << "Average" << endl;
	cout << "--------\t" << "-----\t\t" << "-------" << endl;

	for(int i = 2; i < mod1_data->GetNbinsX(); i++){
		for(int j = 2; j < mod1_data->GetNbinsY(); j++){

			neighbors.clear();
			neighbor_map.clear();		

			for(int k = -1; k <= 1; k++){
				for(int l = -1; l <= 1; l++){
					neighbors.push_back(mod1_data->GetBinContent(i + k, j + l));
					if(cut_union_map->GetBinContent(i + k, j + l) == 1) neighbor_map.push_back(1);
					else neighbor_map.push_back(0);
				}
			}

			if(neighbor_map[4] == 1 && max_true(neighbors, neighbor_map)) {

				average = (int)(get_average(neighbors, neighbor_map) + 0.5);
				mod2_data->SetBinContent(i, j, average);

				cout << Form("(%d, %d) \t", i, j) << mod1_data->GetBinContent(i, j) << "\t\t" << average;
				if(average > mod1_data->GetBinContent(i, j))
					cout << "\tX" << endl;
				else
					cout << endl;
			}
		}
	}
	
	return mod2_data;
}


TH2S *mod3(TH2S *mod2_data, double coefficient)
{	
	TH2S *mod3_data = (TH2S*)mod2_data->Clone("mod3_data");	

	vector<int> neighbors;
	vector<int> neighbor_map = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	
	int average;

	cout << "\nReplacing a value (> coefficient * neighbors) with the average of its neighbors...\n" << endl;
	cout << "Location\t" << "Value\t\t" << "Average" << endl;
	cout << "--------\t" << "-----\t\t" << "-------" << endl;

	for(int i = 2; i < mod2_data->GetNbinsX(); i++){
		for(int j = 2; j < mod2_data->GetNbinsY(); j++){

			neighbors.clear();

			for(int k = -1; k <= 1; k++)
				for(int l = -1; l <= 1; l++)
					neighbors.push_back(mod2_data->GetBinContent(i + k, j + l));

			average = (int)(get_average(neighbors, neighbor_map) + 0.5);
			if(neighbors[4] == *max_element(neighbors.begin(), neighbors.end()) && neighbors[4] > coefficient * average) {

				mod3_data->SetBinContent(i, j, average);

				cout << Form("(%d, %d) \t", i, j) << mod2_data->GetBinContent(i, j) << "\t\t" << average << endl;
			}
		}
	}
	
	return mod3_data;
}


void locate(TH2S *data, double threshold)
{
	int total = 0;

	for (int i = 1; i <= data->GetNbinsX(); i++) {
		for (int j = 1; j <= data->GetNbinsY(); j++) {
			if (data->GetBinContent(i, j) > threshold) {
				total++;
				cout << "Points with over-threshold values:" << endl;
				cout << "Location: (" << i << ", " << j << ")\tValue: " << data->GetBinContent(i, j) << endl;
			}
		}
	}

	cout << "\nTotal Number: " << total << endl;

}
