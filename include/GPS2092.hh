#ifndef gps2092_hh
#define gps2092_hh
#include "Named.hh"
#include <sys/types.h>
#include "data_structures.hh"


#define GPSADDR 0x1000
#define OUTPUTBUFFERADDR GPSADDR + 0x800

class CrateController;

class GPS2092: public Named {
	public:

		GPS2092();
		virtual ~GPS2092();
		u_int16_t ReadIDVector();
		u_int16_t ReadModelCode();
		void Reset();
		bool IsReady();
		void ReadTimeStamp();
		void FillEmptyTime();
		bool AreThereData();
		void ClearIFlag();
		void SetShm(shmbuffer_t*);


	private:
		const static u_int16_t controlStatusRegister = 0x04;
		const static u_int16_t idVectorRegister = 0x00;
		const static u_int16_t modelCodeRegister = 0x02;
		const static u_int16_t time1Register = 0x00;
		const static u_int16_t time2Register = 0x02;
		const static u_int16_t time3Register = 0x04;
		const static u_int16_t time4Register = 0x06;
		CrateController* controller;
		shmbuffer_t *p_buffer;



};
#endif

