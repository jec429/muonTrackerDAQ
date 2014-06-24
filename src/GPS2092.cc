#include "GPS2092.hh"
#include "CrateController.hh"
#include <cstdio>
#include <stdexcept>


GPS2092::GPS2092(): Named("GPS2092"){
	controller = CrateController::GetInstance();
	if (controller->IsCrateOpen() == false)
		controller->Open();
	p_buffer = NULL;


}
GPS2092::~GPS2092(){

}

u_int16_t GPS2092::ReadIDVector(){
	u_int16_t data16;
	controller->A16D16_read(idVectorRegister+GPSADDR, &data16);
	return data16;

}
u_int16_t GPS2092::ReadModelCode(){
	u_int16_t data16;
	controller->A16D16_read(modelCodeRegister+GPSADDR, &data16);
	return data16;


}
void GPS2092::Reset(){
	if(p_buffer == NULL){
		char str[50];
		sprintf(str, "p_buffer pointing to a null location in Reset in ");
		throw std::runtime_error(str + f_name);
	}

	u_int16_t data16;
	controller->A16D16_read(controlStatusRegister+GPSADDR, &data16);
	data16 = 0x1;
	controller->A16D16_write(controlStatusRegister+GPSADDR, data16);
	printf("Reset performed on %s\n",f_name.c_str());

}

bool GPS2092::IsReady(){
	u_int16_t data16;
	controller->A16D16_read(controlStatusRegister+GPSADDR, &data16);
	data16 = (data16 >> 9) & 0x1;
	return data16;

}
void GPS2092::FillEmptyTime(){
	event_t &ev = (p_buffer->data)[p_buffer->p_write];
	u_int32_t timeNs = 0;
	ev.gps.nsIntoSec = 0;
	ev.gps.secIntoDay = 0;
	ev.gps.daysIntoYear = 0;
	ev.gps.year = 0;
	ev.gps.controlFlags = 0;

}


void GPS2092::ReadTimeStamp(){
	event_t &ev = (p_buffer->data)[p_buffer->p_write];
	//FIXME: rivedere
	u_int16_t low;
	controller->A16D16_read(time1Register+OUTPUTBUFFERADDR, &low);
	u_int16_t high;
	controller->A16D16_read(time2Register+OUTPUTBUFFERADDR, &high);
	u_int32_t timeNs = high & 0x7FF;
	timeNs = timeNs << 16;
	timeNs +=low;
	ev.gps.nsIntoSec = timeNs;
	u_int32_t lowSecondsInDay = (high & 0xF800) >> 11;
	u_int16_t data16;
	controller->A16D16_read(time3Register+OUTPUTBUFFERADDR, &data16);
	u_int32_t secondsInDay = (data16 & 0xFFF) <<5;
	secondsInDay += lowSecondsInDay;
	ev.gps.secIntoDay = secondsInDay;
	u_int16_t dayIntoYearLow = (data16 >> 12) & 0xF;
	controller->A16D16_read(time4Register+OUTPUTBUFFERADDR, &data16);
	u_int16_t dayIntoYear = (data16 & 0x1F)<<4;
	dayIntoYear += dayIntoYearLow;
	ev.gps.daysIntoYear = dayIntoYear;
	u_int16_t year = (data16 >> 5) & 0x1F;
	ev.gps.year = year;
	u_int16_t controlFlags = (data16 >> 6) & 0x3F;
	ev.gps.controlFlags = controlFlags;
	ClearIFlag();

}

bool GPS2092::AreThereData(){
	u_int16_t data16;
	controller->A16D16_read(controlStatusRegister+GPSADDR, &data16);
	data16 = (data16 >> 9) & 0x1;
	return data16;

}

void GPS2092::ClearIFlag(){
	u_int16_t data16;
	controller->A16D16_read(controlStatusRegister+GPSADDR, &data16);
	data16 = data16 | 0x200; //Set bit 9 to 1
	data16 = data16 & 0xFFFE; //Set bit 1 to 0
	controller->A16D16_write(controlStatusRegister+GPSADDR, data16);
	//printf("Interrupt flag cleared on %s\n",f_name.c_str());

}
void GPS2092::SetShm(shmbuffer_t* buf){
	p_buffer = buf;
}

