#include <stdexcept>
#include <signal.h>
#include "GPS2092.hh" 
#include "V767.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAXEVENTSPERFILE 100
FILE* outFile = NULL;
shmbuffer_t *p_buffer;
int eventnumber = 0;
void term(int);
int WriteEvent();
void term(int signum)
{
	if (outFile){
		fclose(outFile);
	}
	printf("Event Writer received SIGTERM, exiting...\n");
	exit(0);

}
int main(int argc, char *argv[]){
	try{
		struct sigaction action;
		memset(&action, 0, sizeof(struct sigaction));
		action.sa_handler = term;
		sigaction(SIGINT, &action, NULL);

		int shmid = 5000;
		int shmhandle = ::shmget( shmid, 0, 0777 );
		if (shmhandle == -1)
		{
			char str[50];
			sprintf(str, "Event writer: cannot create shared memory. %s \n", strerror(errno));
			throw std::runtime_error(str);

		}
		void* shmat_return = ::shmat(shmhandle,0,0);
		if((long int)shmat_return == -1)
		{
			char str[50];
			sprintf(str, "Error attaching to muon shared memory. %s \n", strerror(errno));
			throw std::runtime_error(str);

		}
		else
		{	
			p_buffer = (shmbuffer_t*)shmat_return;
			printf("\n Event writer: attached to shared memory. handle = %d\n",shmhandle);
		}
		char filename[150];
		int filenumber = 0;
		eventnumber = 0;
		sprintf(filename,"./outFile_%d.dat",filenumber);
		while(outFile = fopen(filename,"r")){
			filenumber++;
			fclose(outFile);
			sprintf(filename,"./outFile_%d.dat",filenumber);
		}
		outFile = fopen(filename,"w");

		while(1){
			WriteEvent();
			if(eventnumber >= MAXEVENTSPERFILE){
				filenumber++;
				fclose(outFile);
				sprintf(filename,"./outFile_%d.dat",filenumber);
				eventnumber = 0;
				outFile = fopen(filename,"w");
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

int WriteEvent(){
	int nWords = p_buffer->p_write - p_buffer->p_read;
	if (nWords == 0) return 0;

	if (nWords < 0){
		nWords = NEVENTSSHM - nWords;
	}
	struct event_t datum = (p_buffer->data)[p_buffer->p_read];
	size_t ret = fwrite(&datum,sizeof(datum),1,outFile);
	if(ret != 1){
		char str[50];
		sprintf(str, "Written %d objects instead of 1 !\n",(int)ret);
		throw std::runtime_error(str);

	}

	p_buffer->p_read +=1;
	if(p_buffer->p_read >= NEVENTSSHM) p_buffer->p_read -= NEVENTSSHM;
	eventnumber++;
	return 1;

} 

