#include "V767.hh"
#include "CrateController.hh"
#include <cstdlib>
#include <cstdio>
void readout_data(){
	/* pointers to V767 registers */
	unsigned short *status1,rdata;
	unsigned long *out_buff;
	unsigned long header,data,eob;
	/* sets V767 registers addresses */
	status1 = (unsigned short *)(TDCADDR + 0x0E);
	out_buff = (unsigned long *)(TDCADDR + 0x00);
	/* waits for DATA READY set to 1 */
	do
		rdata = *status1; /* reads status register */
	while(!(rdata & 0x01));
	/* reads the event from output buffer and prints it on the screen */
	header = *out_buff;
	printf("Event number = %d\n",header & 0xFFF);
	data = *out_buff;
	printf("Channel = %d - Time = %d\n",(data>>24) & 0x3F, data & 0xFFFFF);
	eob = *out_buff;
	printf("Number of words = %d\n",eob & 0xFFFF);
}
int main(int argc, char *argv[]){
	int wordsToRead = 1; 
	if(argc > 1)
		wordsToRead = strtol(argv[1], NULL, 0);
	CrateController* cc = CrateController::GetInstance();
	V767* tdcBoard = new V767();
	tdcBoard->ClearRegister();
	//	tdcBoard->Init();
	tdcBoard->Reset();

	tdcBoard->SetContinuousStorageMode();
	tdcBoard->DisableSubtractionOfTriggerTime();
	tdcBoard->DisableReadoutOfStartTime();
	tdcBoard->SetDataReadyNotEmpty();
	std::string acquisitionMode = tdcBoard->ReadAcquisitionMode();
	printf("Acquisition mode is %s\n",acquisitionMode.c_str());
	int windowWidth = tdcBoard->ReadWindowWidth();
	printf("Window width = 0x%x\n",windowWidth);
	int windowOffset = tdcBoard->ReadWindowOffset();
	printf("Window offset = 0x%x\n",windowOffset);
	bool testMode = tdcBoard->ReadMemoryTest();

	std::vector<bool> enMap = tdcBoard->ReadChannelEnablePattern();
	for(int i  =  0; i < 128; i++){
		if (enMap[i] == true)
			printf("Channel %d is enabled\n",i);
	}
	if(testMode == true){
		printf("The tdc board is in test mode\n");
	}
	else{
		printf("The tdc board is not in test mode\n");
	}
	//	tdcBoard->DisableOverlappingTriggers();
	tdcBoard->ReadTriggerConfiguration();
	int counter = 0;
	counter  = 0;
	while(counter < wordsToRead){

			readout_data();
			counter++;

	}
	delete tdcBoard;
	cc->Close();
	return 0;

}
