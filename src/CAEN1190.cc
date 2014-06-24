/* This module element is for controlling the 1190 TDC*/
#include "CAEN1190.hh"
#include "CrateController.hh"
#include <string>

//constructor
CAEN1190::CAEN1190(): Named("CAEN1190"){
	cout << "CONSTRUCT AND 1190 " << endl;
controller = CrateController::GetInstance();
	if (controller->IsCrateOpen() == false)
		controller->Open();
	p_buffer = NULL;

}

void CAEN1190::Reset(){
	//Perform reset of TDC
	cout << "Resetting TDC, sleep(3)" << endl;
	data16=0x00;
	controller->A32D16_write(TDCADDR+moduleResetRegister, data16);
	sleep(2.5);


}
//deconstructor
CAEN1190::~CAEN1190(){
	cout << "DECONSTRUCT 1190 MODULE" << endl;
}


//function
void CAEN1190::Init(){
	Reset();
	cout << "CONFIGURE 1190 " << endl;
	cout << "setting default config" << endl;
	u_int16_t tmp_val=0x0500;
	WriteOperand(tmp_val);
	tmp_val=0x0000;
	cout << "Setting trigger matching mode 1190" << endl;
	WriteOperand(tmp_val);
	cout << "Reading type of acquisition mode" << endl;
	tmp_val=0x0200;
	WriteOperand(tmp_val);
	ReadOperand(tmp_val);
	cout << "acquisition mode is " << tmp_val << " : " << endl; 
	if( (tmp_val & 0x1) == 0 ) cout << "continuous storage mode" << endl;
	if( (tmp_val & 0x1) == 1 ) cout << "trigger matching mode enabled" << endl;

	cout << "Setting window width" << endl;
	tmp_val=0x1000;
	WriteOperand(tmp_val);
	//0x20 = 800 ns
	tmp_val = 0x20;
	WriteOperand(tmp_val);
	cout << "Setting offset" << endl;
	tmp_val=0x1100;
	WriteOperand(tmp_val);
	tmp_val = 0x0;
	WriteOperand(tmp_val);
	cout << "Setting relative timing" << endl;
	tmp_val=0x1400;
	WriteOperand(tmp_val);
	cout<<"Enabling all channels"<<endl;
	tmp_val = 0x4200;
	WriteOperand(tmp_val);
	cout<<"Set leading edge config"<<endl;
	tmp_val = 0x2200;
	WriteOperand(tmp_val);
	tmp_val = 0x2;
	WriteOperand(tmp_val);
	cout<<"Set LSB"<<endl;
	tmp_val = 0x2400;
	WriteOperand(tmp_val);
	// 100 ps LSB
	tmp_val = 0x2;
	WriteOperand(tmp_val);
	cout << "Reading type of trigger configuration" << endl;
	tmp_val=0x1600;
	WriteOperand(tmp_val);
	ReadOperand(tmp_val);
	cout << "match window width is " << tmp_val << " : " << endl; 
	ReadOperand(tmp_val);
	cout << "window offset is " << tmp_val << " : " << endl; 
	ReadOperand(tmp_val);
	cout << "extra search window width is : " << tmp_val << endl; 
	ReadOperand(tmp_val);
	cout << "reject margin is : " << tmp_val << endl; 
	ReadOperand(tmp_val);
	cout << "LSB of subtraction time is : " << tmp_val  << endl; 

	if( (tmp_val & 0x1) == 0 ) cout << "trigger time subtraction disabled" << endl;
	if( (tmp_val & 0x1) == 1 ) cout << "trigger time subtraction enabled" << endl;


	cout << "Reading type of edge mode" << endl;
	tmp_val=0x2300;
	WriteOperand(tmp_val);
	ReadOperand(tmp_val);
	cout << "type of edge " << tmp_val << endl;

	if( (tmp_val & 0x3) == 0 ) cout << "pair mode" << endl;
	if( (tmp_val & 0x3) == 1 ) cout << "trailing" << endl;
	if( (tmp_val & 0x3) == 2 ) cout << "leading" << endl;
	if( (tmp_val & 0x3) == 3 ) cout << "trailing and leading" << endl;

	cout << "Setting deadtime between hits" << endl;
	tmp_val=0x2800;
	WriteOperand(tmp_val);
	//setting dead time to 100 ns
	tmp_val=0x3;
	WriteOperand(tmp_val);
	cout << "Read channel dead time" << endl;
	tmp_val= 0x2900;
	WriteOperand(tmp_val);
	ReadOperand(tmp_val);
	cout << "Channel dead time is " << tmp_val<<endl;

	if( (tmp_val & 0x3) == 0x3 ) cout << " 100 ns" << endl;


	cout << "Reading type of header/trailer" << endl;
	tmp_val=0x3200;
	WriteOperand(tmp_val);
	ReadOperand(tmp_val);
	cout << "header trailer " << tmp_val << endl;



}

int CAEN1190::DataReady(){
	//cout << "Checking if 1190 data is available" << endl;
	u_int16_t tmp_val=0x0;
	controller->A32D16_read( statusRegister+TDCADDR, &tmp_val) ;

	return (tmp_val & 0x1);
}

int CAEN1190::WriteOperand (u_int16_t& data)
{
	//  unsigned short *opc_hs, *opc_reg;
	u_int16_t rdata=0x0000;
	int time=0;
	int return_code;

	// checks the Write OK bit 
	do
	{
		controller->A32D16_read( opcodeHandshakeRegister+TDCADDR, &rdata ) ;
		//cout << "rdata " << rdata << endl;
		time++;
	}
	while( (rdata != 0x01) && (time < TIMEOUT) );
	if(time == TIMEOUT) 
		// a time out error is occurred
	{
		printf("Timeout error in %s\n",f_name.c_str());
		return(-1);
	}
	usleep(10000);
	controller->A32D16_write( microRegister+TDCADDR, data ) ;

	return(0);

}


int CAEN1190::ReadOperand (u_int16_t& data)
{
	u_int16_t rdata=0x0000;
	int time=0;
	int return_code;

	// waits until a new read/write opcode operation is ready
	do
	{      

		controller->A32D16_read( opcodeHandshakeRegister+TDCADDR, &rdata ) ;
		time++;
	}
	while( (rdata != 0x02) && (time < TIMEOUT) );
	if(time == TIMEOUT) 
		// a time out error is occurred
	{
		printf("Timeout error in %s\n", f_name.c_str());
		return(-1);
	}
	usleep(10000);
	controller->A32D16_read( microRegister+TDCADDR, &data ) ;
	return(0);
}

void CAEN1190::ReadSingleWord(){

	event_t &ev = (p_buffer->data)[p_buffer->p_write];
	/* pointers to V1190 registers */
	u_int16_t rdata;
	u_int32_t data;
	int event_id, return_code;
	/* waits for DATA READY set to 1 */
	u_int32_t channel, time;
	u_int32_t eventNumber;
	int n_data = 0;


	while(1){
		controller->A32D32_read(outputBuffer+TDCADDR, &data);
		//	cout << "data " << data << endl;

		ev.tdc.error = 0;
		if((data >> 27) == 0x8){//global header
			//	cout << "global header" << endl;
			eventNumber = ((data >> 5) & 0x3FFFFF); 
			//	cout << "event count " << ((data >> 5) & 0x3FFFFF) << endl;
		}
		if((data >> 27) == 0x1){//tdc header
			//	cout << "tdc header" << endl;
			//	cout << "bunch id " << (data & 0xFFF) << endl;
			//	cout << "event id " << ((data >> 12) & 0xFFF) << endl;
			event_id = ((data >> 12) & 0xFFF);
		}
		if((data >> 27) == 0x0){//tdc measure
			//	cout << "1190 tdc measure entry " << endl;
			cout << "channel " << ((data >> 19) & 0x7F) << endl;
			//	cout << "1190 channel " <<  ((data >> 19) & 0x7F) << " time measured " << (data & 0x7FFFF)*.1 << endl;
			//	cout << "measurement type " << ((data >> 26) & 0x1) << endl;
			channel = ((data >> 19) & 0x7F); 
			time = (data & 0x7FFFF);
			ev.tdc.times[n_data] = time;
			ev.tdc.channel[n_data] = channel;
			n_data++;
		}
		if((data >> 27) == 0x4){//tdc error
			cout << "tdc error" << endl;
			ev.tdc.error = (data  & 0x7FFF); 
			//	err_counter++;
		}
		if((data >> 27) == 0x3){//tdc trailer
			//	 cout << "tdc_trailer" << endl;
		}
		if((data >> 27) == 0x11){//global trigger time tag
			//	cout << "global trigger time tag" << endl;
		}
		if((data >> 27) == 0x10){//global trailer
			//	cout << "global trailer" << endl;
			break;
		}
	}
	ev.tdc.nHits = n_data;
	//call root function to map hit
}
void CAEN1190::SetShm(shmbuffer_t* buf){
	p_buffer = buf;
}

void CAEN1190::FillEmptyEvent(){
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
	ev.tdc.error = 0;
	ev.evNum = eventNumber;
}


