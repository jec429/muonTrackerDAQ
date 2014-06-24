#include "V767.hh"
#include "CrateController.hh"
#include <cstdlib>
#include <cstdio>

int main(int argc, char *argv[]){
	int wordsToRead = 1; 
	if(argc > 1)
		wordsToRead = strtol(argv[1], NULL, 0);
	CrateController* cc = CrateController::GetInstance();
	V767* tdcBoard = new V767();
	tdcBoard->ClearRegister();
	tdcBoard->VMEReset();
	tdcBoard->Reset();
	tdcBoard->DisableMemoryTestMode();
	tdcBoard->EnableAllChannels();
	tdcBoard->SetCommonStopMode();
	tdcBoard->EnableSubtractionOfTriggerTime();
	tdcBoard->SetWindowOffset(0xFF37);
	tdcBoard->SetWindowWidth(0x3E8);
	tdcBoard->SetDataReadyEventReady();
	std::vector<bool> enMap = tdcBoard->ReadChannelEnablePattern();
	for(int i  =  0; i < 128; i++){
		if (enMap[i] == true)
			printf("Channel %d is enabled\n",i);
	}
	std::string acquisitionMode = tdcBoard->ReadAcquisitionMode();
	printf("Acquisition mode is %s\n",acquisitionMode.c_str());
	int windowWidth = tdcBoard->ReadWindowWidth();
	printf("Window width = 0x%x\n",windowWidth);
	int windowOffset = tdcBoard->ReadWindowOffset();
	printf("Window offset = 0x%x\n",windowOffset);
	bool testMode = tdcBoard->ReadMemoryTest();
	if(testMode == true){
		printf("The tdc board is in test mode\n");
	}
	else{
		printf("The tdc board is not in test mode\n");
	}

	tdcBoard->ReadTriggerConfiguration();
	int counter = 0;
	counter  = 0;

	tdcBoard->ClearRegister();
	while(counter < wordsToRead){

		if ((tdcBoard->DataReady()) == true){
			if ((tdcBoard->GlobalTDCError()) == true) {
				printf("Global TDC error\n");
				u_int16_t TDC = tdcBoard->WhichTDCError();
				tdcBoard->ReadErrorCodeOfTDC(TDC);
				tdcBoard->ClearRegister();
			}
			else{
				tdcBoard->ReadAndDumpSingleWord();
				counter++;
			}
		}
		else{
		//	printf("Data not ready\n");
		}

		//	if ((tdcBoard->IsBusy()) == true) printf("Board Busy\n");
		//	if ((tdcBoard->BufferFull()) == true) printf("Buffer Full\n");
		if ((tdcBoard->GlobalTDCError()) == true) {
			printf("Global TDC error\n");
			u_int16_t TDC = tdcBoard->WhichTDCError();
			tdcBoard->ReadErrorCodeOfTDC(TDC);
		}
		//		if ((tdcBoard->BufferEmpty()) == true) printf("Buffer empty\n");



	}
	delete tdcBoard;
	cc->Close();
	return 0;

}
