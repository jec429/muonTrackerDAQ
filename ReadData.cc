//#include "V767.hh"
#include "GPS2092.hh" 
#include "CrateController.hh"
#include <cstdlib>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include <signal.h>
#include "CAEN1190.hh"

void term(int signum)
{
	printf("Event Reader received SIGTERM, exiting...\n");
	exit(0);
}

int main(int argc, char *argv[]){

	try{
		// set-up signal handler
		struct sigaction action;
		memset(&action, 0, sizeof(struct sigaction));
		action.sa_handler = term;
		sigaction(SIGINT, &action, NULL);


		CrateController* cc = CrateController::GetInstance();
		int shmid = 5000;
		int tot_size = 2 * sizeof(unsigned long long)+sizeof(struct event_t)*NEVENTSSHM;

		int shmhandle = ::shmget( shmid, tot_size, IPC_CREAT | IPC_EXCL | 0777 );
		if ( shmhandle == -1)
		{
			char str[50];
			sprintf(str, "Error creating shared memory in ReadData: %s \n", strerror(errno));
			throw std::runtime_error(str);

		}
		else
		{
			printf("created shared memory. handle = %d\n",shmhandle);
		}
		shmbuffer_t *p_buffer = (shmbuffer_t*)::shmat(shmhandle,0,0);
		p_buffer->p_read = 0;
		p_buffer->p_write = 0;
		//V767* tdcBoard = new V767();
		CAEN1190* tdcBoard = new CAEN1190();
		//Note: the buffer should be passed to the board before Init() is called 
		tdcBoard->SetShm(p_buffer);
		tdcBoard->Init();		
		int eventCounter = 0;
			GPS2092* gpsBoard = new GPS2092();
			gpsBoard->SetShm(p_buffer);
			gpsBoard->Reset();
			int sleepcounter = 0;
			/*	int idgps = gpsBoard->ReadIDVector();
			int modelgps = gpsBoard->ReadModelCode();
			printf("The ID vector of the gps board is %d\n",idgps);
			printf("The model code of the gps board is %d\n",modelgps);
			*/		
			while(1){
				if (tdcBoard->DataReady() == true || gpsBoard->AreThereData() == true){ 
					if (tdcBoard->DataReady() == true && gpsBoard->AreThereData() == true){ 
						//read datum here
						gpsBoard->ReadTimeStamp();
						tdcBoard->ReadSingleWord();
						p_buffer->p_write += 1;
						if((p_buffer->p_write) >= NEVENTSSHM) 
							p_buffer->p_write -= NEVENTSSHM;
						eventCounter++;
						if(eventCounter%10 == 0) printf("Acquired %d events\n",eventCounter);
						printf("Read full Event\n");
						sleepcounter = 0;
					}
					else if (sleepcounter == 1000){
						if(tdcBoard->DataReady() == true){
							tdcBoard->ReadSingleWord();
							gpsBoard->FillEmptyTime();
							p_buffer->p_write += 1;
							if((p_buffer->p_write) >= NEVENTSSHM) 
								p_buffer->p_write -= NEVENTSSHM;
							eventCounter++;
							if(eventCounter%10 == 0) printf("Acquired %d events\n",eventCounter);
							printf("Read TDC Board\n");
							sleepcounter = 0;
						}
						else if (gpsBoard->AreThereData() == true){
							gpsBoard->ReadTimeStamp();
							tdcBoard->FillEmptyEvent();
							p_buffer->p_write += 1;
							if((p_buffer->p_write) >= NEVENTSSHM) 
								p_buffer->p_write -= NEVENTSSHM;
							eventCounter++;
						//	if(eventCounter%10 == 0) printf("Acquired %d events\n",eventCounter);
							printf("Read GPS Board\n");
							sleepcounter = 0;

						}
					}
					else{
						usleep(1);
						sleepcounter++;
						if(sleepcounter%100 == 0) printf("Waiting...\n");
					}
				}
			}
	}
	catch (std::exception &ex) 
	{
		printf(" >>>> Exception caught in ReadData: standard exception\n");
		printf(" >>>> %s \n",  ex.what());
	}
	catch( ... ) 
	{
		printf(" >>>> Exception caught in ReadData:\n");
		printf(" >>>> Unknown exception type\n ");
	}
}
