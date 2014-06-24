#ifndef CAEN1190_hh
#define CAEN1190_hh
/* This module element is for controlling the 1190 TDC*/

#include <iostream>
#include <sys/types.h>
#include <assert.h>
#include "Named.hh"
#include "data_structures.hh"
#include <stdio.h>


#define TDCADDR 0x11110000
using namespace std;

#define TIMEOUT 1000000

class CrateController;

class CAEN1190: public Named {

	public:

		CAEN1190();
		virtual ~CAEN1190();

		int DataReady();
		int WriteOperand(u_int16_t &);
		int ReadOperand(u_int16_t &);  
		void ReadSingleWord();
		void SetShm(shmbuffer_t* );
		void Init();
		void FillEmptyEvent();

	private:
		void Reset();
		u_int16_t data16;
		int return_code;
		const static u_int32_t microRegister = 0x102E;
		const static u_int32_t opcodeHandshakeRegister = 0x1030;
		const static u_int32_t moduleResetRegister = 0x1014;
		const static u_int32_t statusRegister = 0x1002;
		const static u_int32_t outputBuffer = 0x0000;
		shmbuffer_t *p_buffer;
		CrateController* controller;


};
#endif
