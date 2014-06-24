#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include <sys/types.h>
#include "TH1I.h"
#include <iostream>
#define NWIRES 75

void MakePlots(char* fileName){
	TFile* file = new TFile(fileName,"read");
	TTree* tree = (TTree*)file->Get("mu_tree");
	unsigned int evNum;
	unsigned short nHits;
	unsigned int chan[NWIRES];
	unsigned int time[NWIRES];
	TH1I* hChannels = new TH1I("h_channels","",128, 0,127);
	TH1I* hTimes = new TH1I("h_times","",1000, 0,10000);
	tree->SetBranchAddress("event_number",&evNum);
	tree->SetBranchAddress("tdc_nHits",&nHits); 
	tree->SetBranchAddress("tdc_times",&time);
	tree->SetBranchAddress("tdc_channel",&chan);
	int nentries = tree->GetEntries();
	std::cout<<" nentries = "<<nentries<<std::endl;
	for (int i = 0; i < nentries; i++){
		tree->GetEntry(i);
		for (int q = 0; q <  nHits; q++){
			hChannels->Fill(chan[q]);
			hTimes->Fill(time[q]);
		}
	}
	hChannels->Draw();
	TCanvas* c = new TCanvas();
	hTimes->Draw();

}
