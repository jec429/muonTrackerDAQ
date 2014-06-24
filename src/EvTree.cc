#include "EvTree.hh"
#include "TTree.h"
#include "data_structures.hh"

EvTree::EvTree(): Named("EvTree"){
	f_tree = NULL;
	printf("You are creating a NULL tree, the program will chash!\n");
}
EvTree::EvTree(const char* name, const char* title, event_t &ev, int splitlevel): Named("EvTree"){
	f_tree = new TTree(name, title, splitlevel);
	f_tree->Branch("event_number",&ev.evNum,"event_number/i");
	f_tree->Branch("gps_nsIntoSec",&ev.gps.nsIntoSec,"gps_nsIntoSec/i");
	f_tree->Branch("gps_secIntoDay",&ev.gps.secIntoDay,"gps_secIntoDay/i");
	f_tree->Branch("gps_daysIntoYear",&ev.gps.daysIntoYear,"gps_daysIntoYear/s");
	f_tree->Branch("gps_Year",&ev.gps.year,"gps_Year/s");
	f_tree->Branch("gps_ctrlFlag",&ev.gps.controlFlags,"gps_ctrlFlag/s");
	f_tree->Branch("tdc_nHits",&ev.tdc.nHits,"tdc_nHits/s");
	f_tree->Branch("tdc_times",&ev.tdc.times,"tdc_times[tdc_nHits]/i");
	f_tree->Branch("tdc_channel",&ev.tdc.channel,"tdc_channel[tdc_nHits]/i");
	f_tree->Branch("tdc_error",&ev.tdc.error,"tdc_error/s");
}

EvTree::~EvTree() {
	if(f_tree) delete f_tree;
}
void EvTree::Fill(){
	f_tree->Fill();
}
void EvTree::Write(){
	f_tree->Write();
}

