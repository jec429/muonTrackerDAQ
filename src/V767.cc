#include "V767.hh"
#include "CrateController.hh"
#include <stdexcept>
#include <cstdio>
#include <iostream>

using namespace std;

V767::V767(): Named("V767"){
	controller = CrateController::GetInstance();
	if (controller->IsCrateOpen() == false)
		controller->Open();
	p_buffer = NULL;
}
V767::~V767(){

}
bool V767::NotEmpty(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister2+TDCADDR, &data16);
	if ((data16 & 0x1) == 0)
		return true;
	return false;
}
bool V767::DataReady(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister1+TDCADDR, &data16);
	if ((data16 & 0x1) == 0)
		return false;
	return true;
}

void V767::Reset(){
	u_int16_t data16 = 0x0000;
	controller->A32D16_write(singleShotResetRegister+TDCADDR, data16);
	sleep(3.5);
	printf("Single shot reset done on %s\n",f_name.c_str());

}

void V767::Init(){
	if(p_buffer == NULL){
		char str[50];
		sprintf(str, "p_buffer pointing to a null location in Init in ");
		throw std::runtime_error(str + f_name);

	}

	Reset();
	//VMEReset();
	DisableMemoryTestMode();
	//EnableAllChannels();
	DisableAllChannels();
	for(int i = 0; i < 33; i++){
		EnableChannel(i);
	}

	SetCommonStopMode();
	EnableSubtractionOfTriggerTime();
	DisableOverlappingTriggers();
	//SetWindowOffset(0xFF37);
	SetWindowOffset(0x0);
	//SetWindowWidth(0x3E8);
	SetWindowWidth(1999);
	//SetWindowWidth(20);
	SetDataReadyEventReady();
	std::vector<bool> enMap = ReadChannelEnablePattern();
	for(int i  =  0; i < 128; i++){
		if (enMap[i] == true){
			printf("Channel %d is  enabled\n",i);
		}
	}
	std::string acquisitionMode = ReadAcquisitionMode();
	printf("Acquisition mode is %s\n",acquisitionMode.c_str());
	int windowWidth = ReadWindowWidth();
	printf("Window width = 0x%x\n",windowWidth);
	int windowOffset = ReadWindowOffset();
	printf("Window offset = 0x%x\n",windowOffset);
	bool testMode = ReadMemoryTest();
	if(testMode == true){
		printf("The tdc board is in test mode\n");
	}
	else{
		printf("The tdc board is not in test mode\n");
	}

	ReadTriggerConfiguration();

	ClearRegister();
}
void V767::SetCommonStopMode(){
	u_int16_t data16 = 0x1000;
	WriteOperand(data16);
}

void V767::ReadAndDumpSingleWord(){
	u_int32_t data;
	u_int32_t header;
	while(1){

		controller->A32D32_read(outputBuffer+TDCADDR, &data);
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x1)){//header        
			header = (data & 0xFFF);
			cout << " header " << header << endl;
		}
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x0)){//measurement
			cout << "767 tdc measure entry " << header << endl;
			cout << "767 channel " <<  ((data >> 24) & 0x7F) << " time measured " << (data & 0xFFFFF)*.8 << endl;
		}

		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x0)){//eob
			cout << "EOB" << endl;
			break;
		}
		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x1)){//invalid datum
			cout << "invalid datum" << endl;
			break;
		}
		/*
		   printf("***************\n");
		   printf("Word field = 0x%x \n",wordField);
		   if (wordField != 0x2){
		   char str[70];
		   sprintf(str, "ReadSingleWord failed. WordField found equal to 0x%x while 0x2 was expected in ",wordField);
		   throw std::runtime_error(str + f_name);
		   }
		   u_int32_t eventNumber;
		   eventNumber = data32 & 0xFFF;
		   printf("event number = 0x%x \n",eventNumber);
		   wordField = 0x0;
		   int n_data = 0;
		   printf("***************\n");
		   while (wordField == 0x0){
		   controller->A32D32_read(outputBuffer+TDCADDR, &data32);
		   wordField = ((data32 & 0x600000)>>21);
		   printf("Word field = 0x%x \n",wordField);
		   if (wordField == 0x0){
		//hit
		u_int32_t channel, start, time;
		channel = (data32>>24) & 0x3F;
		printf("Channel = 0x%x \n",channel);
		start = ((data32 & 0x800000)>>23);
		printf("Start = 0x%x \n",start);
		time = data32 & 0xFFFFF;
		printf("Time = 0x%x \n",time);
		printf("***************\n");
		n_data++;
		}
		else{
		//EOB
		u_int32_t data_counter;
		data_counter = data32 & 0xFFFF;
		printf("data counter = 0x%x \n",data_counter);
		if (n_data != data_counter) printf("n_data != data counter in V767::ReadSingleWord");
		}

		printf("***************\n");
		}
		*/
	}
}
void V767::ReadAndDumpSingleWordContinuous(){
	u_int32_t data;
	int n_data = 0;
	u_int32_t header;
	while(1){

		controller->A32D32_read(outputBuffer+TDCADDR, &data);
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x1)){//header        
			header = (data & 0xFFF);
			cout << " header " << header << endl;
		}
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x0)){//measurement
			cout << "767 tdc measure entry " << header << endl;
			cout << "767 channel " <<  ((data >> 24) & 0x7F) << " time measured " << (data & 0xFFFFF)*.8 << endl;
		}

		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x0)){//eob
			cout << "EOB" << endl;
			break;
		}
		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x1)){//invalid datum
			cout << "invalid datum" << endl;
			break;
		}
	}
	/*
	   printf("**********************************************************\n");
	   while (wordField == 0x0){
	   controller->A32D32_read(outputBuffer+TDCADDR, &data32);
	   wordField = (data32>>21) & 0x3;
	   printf("Word field = 0x%x \n",wordField);
//hit
u_int32_t channel, start, time;
//channel = ((data32 & 0x7F000000)>>24);
channel = (data32>>24) & 0x3F;
//	printf("Channel = 0x%x \n",channel);
start = (data32>>23)& 0x1;
printf("Start = 0x%x \n",start);
time = data32 & 0xFFFFF;
//	printf("Time = 0x%x \n",time);
//	printf("***************\n");

n_data++;

}
*/

}
void V767::FillEmptyEvent(){
	event_t &ev = (p_buffer->data)[p_buffer->p_write];
	u_int32_t data;
	u_int32_t header;
	int n_data = 0;
	u_int32_t channel, start, time;
	u_int32_t eventNumber;
	eventNumber = 0;
	channel = 0;
	time = 0;  
	ev.tdc.times[0] = time;
	ev.tdc.channel[0] = channel;
	ev.tdc.nHits = 0;
	ev.evNum = eventNumber;
}

void V767::ReadSingleWord(){
	event_t &ev = (p_buffer->data)[p_buffer->p_write];
	u_int32_t data;
	u_int32_t header;
	int n_data = 0;
	u_int32_t channel, start, time;
	u_int32_t eventNumber;
	controller->A32D32_read(outputBuffer+TDCADDR, &data);
	while(1){

		controller->A32D32_read(outputBuffer+TDCADDR, &data);
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x1)){//header        
			//	header = (data & 0xFFF);
			eventNumber = data & 0xFFF;
			//cout << " header " << header << endl;
		}
		if( (((data >> 21) & 0x1) == 0x0) && (((data >> 22) & 0x1) == 0x0)){//measurement
			//	cout << "767 tdc measure entry " << header << endl;
			//	cout << "767 channel " <<  ((data >> 24) & 0x7F) << " time measured " << (data & 0xFFFFF)*.8 << endl;
			channel = ((data >> 24) & 0x7F);
			time = (data & 0xFFFFF);  
			ev.tdc.times[n_data] = time;
			printf("Channel = %d\n",channel);
			ev.tdc.channel[n_data] = channel;
			n_data++;

		}

		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x0)){//eob
			//	cout << "EOB" << endl;
			break;
		}
		if( (((data >> 21) & 0x1) == 0x1) && (((data >> 22) & 0x1) == 0x1)){//invalid datum
			//	cout << "invalid datum" << endl;
			break;
		}
	}
	ev.tdc.nHits = n_data;
	ev.evNum = eventNumber;
}
void V767::DisableAllChannels(){
	u_int16_t data16 = 0x2400;
	WriteOperand(data16);
}
void V767::EnableAllChannels(){
	u_int16_t data16 = 0x2300;
	WriteOperand(data16);
}

void V767::EnableChannel(u_int16_t ch){
	u_int16_t data16 = 0x2000 + ch;
	WriteOperand(data16);
}
std::string V767::ReadAcquisitionMode(){
	u_int16_t data16 = 0x1400;
	WriteOperand(data16);
	u_int16_t mode;
	ReadOperand(&mode);
	if ((mode & 0x3) == 0)
		return std::string("Stop Trigger Matching Mode");
	if((mode & 0x3) == 1)
		return std::string("Start Trigger Matching Mode");
	if((mode & 0x3) == 2)
		return std::string("Start Gating Mode");
	if((mode & 0x3) == 3)
		return std::string("Continuous Storage Mode");
	char str[50];
	sprintf(str, "ReadAcquisitionMode: unrecognized acq. mode code: %x in ",mode);
	throw std::runtime_error(str + f_name);

}
void V767::SetWindowWidth(u_int16_t width){
	u_int16_t data16 = 0x3000;
	WriteOperand(data16);
	WriteOperand(width);
	//width in clock cycles (40 MHz/25ns internal clock or external). From 1 to 0x84D8
	//0x84D8 corresponds to 850 us
	//0x7CF corresponds to 49.975 us
	//	u_int16_t width = 0x7CF;
}
int V767::ReadWindowWidth(){
	u_int16_t data16 = 0x3100;
	u_int16_t width;
	WriteOperand(data16);
	ReadOperand(&width);
	return width;

}
void V767::SetWindowOffset(u_int16_t offset){
	u_int16_t data16 = 0x3200;
	WriteOperand(data16);
	WriteOperand(offset);
	//width in clock cycles (40 MHz/25ns internal clock or external). From -0x8300 to 2000 - width
	//mist be offset + width <2000
	//u_int16_t offset = 0x0;
	printf("Window offset set to 0x%x\n",offset);
}
int V767::ReadWindowOffset(){
	u_int16_t data16 = 0x3300;
	u_int16_t offset;
	WriteOperand(data16);
	ReadOperand(&offset);
	return offset;
}

std::vector<bool> V767::ReadChannelEnablePattern(){
	u_int16_t data16 = 0x2600;
	u_int16_t enabled[8];
	std::vector<bool> chMap;
	WriteOperand(data16);
	for(int i = 0; i < 8; i++){
		ReadOperand(&(enabled[i]));
	}
	for(int i = 0; i < 8; i++){
		for (int j = 0; j < 16; j++){
			chMap.push_back(((enabled[i] & (0x1<<j))>>j));
		}
	}
	return chMap;
}
void V767::InitForTest(){
	Init();
	EnableMemoryTestMode();

}

void V767::EnableMemoryTestMode(){
	u_int16_t data16 = 0x0100;
	WriteOperand(data16);
	printf("Enable memory test mode enabled\n");

}
void V767::DisableMemoryTestMode(){
	u_int16_t data16 = 0x0200;
	WriteOperand(data16);
	printf("Enable memory test mode disabled on %s\n",f_name.c_str());
}

void V767::ClearRegister(){
	u_int16_t data16 = 0x0000;
	controller->A32D16_write(clearRegister+TDCADDR, data16);
	printf("Clear issued on %s\n",f_name.c_str());
}
void V767::GenerateTestTrigger(){
	u_int16_t data16 = 0x0000;
	controller->A32D16_write(softwareTriggerRegister+TDCADDR, data16);
	printf("Software trigger register generated\n");
}

void V767::SetWordForMemoryTest(u_int32_t word){

	u_int16_t lowBits = word & 0xFFFF;
	u_int16_t highBits = word >> 16;
	highBits = highBits & 0xFFFF;
	controller->A32D16_write(testWordLowRegister+TDCADDR, lowBits);
	controller->A32D16_write(testWordHighRegister+TDCADDR, highBits);
	printf("Test word set to 0x%x\n",word);

}

bool V767::ReadMemoryTest(){
	u_int16_t data16 = 0x0300;
	u_int16_t testMode;
	WriteOperand(data16);
	ReadOperand(&testMode);
	bool isTestMode = testMode & 0x1;
	return isTestMode;
}

void V767::TestReadSingleWord(){
	u_int32_t data32;
	controller->A32D32_read(outputBuffer+TDCADDR, &data32);
	printf("Word: 0x%x\n",data32);
}

void V767::VMEReset(){
	u_int16_t data16 = 0x1;
	data16 = data16 << 7;
	controller->A32D16_write(bitSetRegister+TDCADDR, data16);
	sleep(2);
	controller->A32D16_write(bitClearRegister+TDCADDR, data16);
	sleep(2);
	printf("VME reset performed on %s\n",f_name.c_str());
}

void V767::SetDataReadyEventReady(){
	u_int16_t data16 = 0x7000;
	WriteOperand(data16);
	printf("Set Data Ready = Event Ready on %s \n",f_name.c_str());	
}

void V767::SetShm(shmbuffer_t* buf){
	p_buffer = buf;
}

int V767::ReadTriggerLatency(){
	u_int16_t data16 = 0x3500;
	u_int16_t latency;
	WriteOperand(data16);
	ReadOperand(&latency);
	return latency;

}

void V767::EnableSubtractionOfTriggerTime(){
	u_int16_t data16 = 0x3600;
	WriteOperand(data16);
}
void V767::DisableSubtractionOfTriggerTime(){
	u_int16_t data16 = 0x3700;
	WriteOperand(data16);
}
void V767::ReadTriggerConfiguration(){
	u_int16_t data16 = 0x3A00;
	u_int16_t trgConf;
	WriteOperand(data16);
	ReadOperand(&trgConf);
	if ((trgConf & 1) == 0) printf("Trigger time subtraction disabled in %s\n",f_name.c_str());
	else printf("Trigger time subtraction enabled in %s\n",f_name.c_str());
	if ((trgConf & 0x2) == 0) printf("Overlapping triggers disabled in %s\n",f_name.c_str());
	else printf("Overlapping triggers enabled in %s\n",f_name.c_str());
}

void V767::EnableOverlappingTriggers(){
	u_int16_t data16 = 0x3800;
	WriteOperand(data16);
	printf("Overlapping triggers enabled in %s\n",f_name.c_str());
}

void V767::DisableOverlappingTriggers(){
	u_int16_t data16 = 0x3900;
	WriteOperand(data16);
	printf("Overlapping triggers disabled in %s\n",f_name.c_str());
}

void V767::SetContinuousStorageMode(){
	u_int16_t data16 = 0x1300;
	WriteOperand(data16);
	printf("Continuous storage mode set in %s\n",f_name.c_str());
}

void V767::DisableReadoutOfStartTime(){
	u_int16_t data16 = 0x4200;
	WriteOperand(data16);
	printf("Disabled readout of Start Time in %s\n",f_name.c_str());
}

void V767::SetDataReadyNotEmpty(){
	u_int16_t data16 = 0x7200;
	WriteOperand(data16);
	printf("Set data ready = not empty %s\n",f_name.c_str());
}

bool V767::IsBusy(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister1+TDCADDR, &data16);
	if (((data16>>2) & 0x1) == 0)
		return false;
	return true;

}
bool V767::BufferEmpty(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister2+TDCADDR, &data16);
	if ((data16 & 0x1) == 0)
		return false;
	return true;
}

bool V767::BufferFull(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister2+TDCADDR, &data16);
	if (((data16 >>1 )& 0x1) == 0)
		return false;
	return true;
}

bool V767::GlobalTDCError(){
	u_int16_t data16;
	controller->A32D16_read(statusRegister2+TDCADDR, &data16);
	if (((data16 >>3 )& 0x1) == 0)
		return false;
	return true;
}

void V767::SetInterruptLevel(u_int16_t data16){
	controller->A32D16_write(interruptVectorRegister+TDCADDR, data16);
}

u_int16_t V767::WhichTDCError(){
	u_int16_t data16;
	u_int16_t return_val = 10;
	controller->A32D16_read(statusRegister2+TDCADDR, &data16);
	if (((data16 >>12 )& 0x1) == 1){
		return_val = 0;
		printf("TDC 0 has an error in %s\n",f_name.c_str());
	}
	if (((data16 >>13 )& 0x1) == 1){
		return_val = 1;
		printf("TDC 1 has an error in %s\n",f_name.c_str());
	}
	if (((data16 >>14 )& 0x1) == 1){
		return_val = 2;
		printf("TDC 2 has an error in %s\n",f_name.c_str());
	}
	if (((data16 >>15 )& 0x1) == 1){
		return_val = 3;
		printf("TDC 3 has an error in %s\n",f_name.c_str());
	}

	return return_val;

}

u_int16_t V767::ReadErrorCodeOfTDC(u_int16_t TDC){
	u_int16_t data16 = 0x8000;
	data16 += TDC;
	u_int16_t error;
	WriteOperand(data16);
	ReadOperand(&error);
	if(error & 0x1 == 1) printf("TDC %d not locked\n", TDC);
	if((error>>1) & 0x1 == 1) printf("TDC %d: Hit error\n",TDC);
	if((error>>2) & 0x1 == 1) printf("TDC %d: Event buffer Overflow\n",TDC);
	if((error>>3) & 0x1 == 1) printf("TDC %d: Trigger buffer Overflow\n",TDC);

	return error;

}
int V767::ReadOperand( u_int16_t *data){
	u_int16_t rdata;
	int time = 0;
	do{
		controller->A32D16_read(opcodeHandshakeRegister+TDCADDR, &rdata);
		time++;
	}
	while((rdata != 0x01)&&(time < TIMEOUT));
	if (time == TIMEOUT)
	{
		char str[50];
		sprintf(str, "timeout error in %s",f_name.c_str());
		throw std::runtime_error(str + f_name);

	}
	usleep(10);
	controller->A32D16_read(opcodeRegister+TDCADDR, data);
	return(0);
}
int V767::WriteOperand( u_int16_t data){
	u_int16_t rdata;
	int time = 0;
	do{
		controller->A32D16_read(opcodeHandshakeRegister+TDCADDR, &rdata);
		time++;
	}
	while((rdata != 0x02)&&(time < TIMEOUT));
	if (time == TIMEOUT)
	{
		char str[50];
		sprintf(str, "timeout error in %s",f_name.c_str());
		throw std::runtime_error(str + f_name);


	}
	usleep(10);
	controller->A32D16_write(opcodeRegister+TDCADDR, data);
	return(0);
}


