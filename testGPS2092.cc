#include "GPS2092.hh"
#include "CrateController.hh"
#include <cstdio>

int main(int argc, char *argv[]){
	CrateController* cc = CrateController::GetInstance();
	GPS2092* gps = new GPS2092();
	gps->Reset();
	u_int16_t idVector = gps->ReadIDVector();
	printf("The ID vector is 0x%x\n",idVector);
	u_int16_t modelCode = gps->ReadModelCode();
	printf("The ID vector is %d\n",modelCode);
	for(int i = 0; i < 3; i++){
		bool ready = gps->IsReady();
		if(ready== true) printf("GPS board is ready\n");
		else printf("GPS board in not ready\n");
		bool data = gps->AreThereData();
		if(data == true) printf("GPS board has data\n");
		else printf("GPS board has not data\n");

	}
	gps->ReadTimeStamp();

}

