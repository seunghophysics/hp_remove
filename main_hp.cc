#include "lib_hp.h"

#include "TH2.h"
#include "TFile.h"

#include <iostream>
#include <fstream>

using namespace std;

const char *obj_name = "t1";

int main(int argc, char *argv[])
{
	if (argc != 4 || !isdigit(*argv[3]) || *argv[3] < '0') {
		cout << "Invalid arguments. Please try again with a proper set of arguments." << endl;
		cout << "Example Usage: ./test [input] [output] [threshold]" << endl;
	return -1;
	}
	
	char *input = argv[1];
	char *output = argv[2];
	double threshold = stod(argv[3]);		
	
	TH2S *data = get_hist(input, "data", obj_name);
	TH2S *uni_map;	

	int range[4] = {100, 1500, 100, 1100};
	TH2S *cut_data = cut_hist(data, range);

	ifstream f("hp_map.root");

	if(!f.good()){
		cout << "\nThe hot pixel map does not exist. Creating a new one...\n" << endl;

		ifstream g("dark.card");
		
		if(!g.good()){
			cout << "\nNo map card found. Try again with a map card. Exiting...\n" << endl;
			exit(0);
		}

		else{
			vector<TH2S *> map_vec = read_map("dark.card", 2, obj_name);
			uni_map = union_map(map_vec);	
		}

		g.close();
	}
	
	else
		uni_map = get_hist("hp_map.root", "uni_map", "candidate_map");

	TH2S *cut_map = cut_hist(uni_map, range);

	TH2S *mod1_data = mod1(cut_data, threshold);
	TH2S *mod2_data = mod2(mod1_data, cut_map);
	TH2S *mod3_data = mod3(mod2_data, 1.5);

	TFile *h = new TFile(output, "recreate");
	mod3_data->Write();
	
	f.close();
	h->Close();

	return 0;
}
