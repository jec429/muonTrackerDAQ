//methods to read the V767 board
#ifndef v767_hh
#define v767_hh

#include <sys/types.h>
#include "Named.hh"
#include "data_structures.hh"
#include <vector>
//#define TDCADDR 0x00000000
#define TDCADDR 0x20000000
//#define TDCADDR 0x11110000
#define TIMEOUT 1000000

class CrateController;

class V767: public Named {
	public:

		V767();
		virtual ~V767();
		void Reset();
		//initialize to stop trigger matching
		void Init();
		void InitForTest();
		bool DataReady();
		void ReadSingleWord();
		void FillEmptyEvent();
		std::string ReadAcquisitionMode();
		int ReadWindowWidth();
		int ReadWindowOffset();
		std::vector<bool> ReadChannelEnablePattern();
		void ClearRegister();
		void GenerateTestTrigger();
		bool ReadMemoryTest();
		void TestReadSingleWord();
		void VMEReset();
		void SetWordForMemoryTest(u_int32_t);
		void ReadAndDumpSingleWord();
		void ReadAndDumpSingleWordContinuous();
		void SetShm(shmbuffer_t* );
		int ReadTriggerLatency();
		void EnableSubtractionOfTriggerTime();
		void DisableSubtractionOfTriggerTime();
		void EnableOverlappingTriggers();
		void DisableOverlappingTriggers();
		void ReadTriggerConfiguration();
		void DisableReadoutOfStartTime();
		void SetContinuousStorageMode();
		void SetDataReadyNotEmpty();
		void SetCommonStopMode();
		void SetWindowWidth(u_int16_t);
		void SetWindowOffset(u_int16_t);
		void SetDataReadyEventReady();
		void EnableAllChannels();
		void DisableMemoryTestMode();
		bool IsBusy();
		void EnableMemoryTestMode();
		bool BufferEmpty();
		bool BufferFull();
		bool GlobalTDCError();
		u_int16_t WhichTDCError();
		void SetInterruptLevel(u_int16_t);
		u_int16_t ReadErrorCodeOfTDC(u_int16_t);
		void EnableChannel(u_int16_t);
		void DisableAllChannels();

	private:

		bool NotEmpty();
		int ReadOperand( u_int16_t *data);
		int WriteOperand( u_int16_t data);

		const static u_int32_t statusRegister1 = 0x000E;
		const static u_int32_t singleShotResetRegister = 0x0018;
		const static u_int32_t statusRegister2 = 0x0048;
		const static u_int32_t bitSetRegister = 0x0006;
		const static u_int32_t bitClearRegister = 0x0008;
		const static u_int32_t controlRegister2 = 0x004A;
		const static u_int32_t opcodeHandshakeRegister = 0x0050;
		const static u_int32_t opcodeRegister = 0x0052;
		const static u_int32_t outputBuffer = 0x0000;
		const static u_int32_t clearRegister = 0x0054;
		const static u_int32_t softwareTriggerRegister = 0x005A;
		const static u_int32_t testWordHighRegister = 0x0056;
		const static u_int32_t testWordLowRegister = 0x0058;
		const static u_int32_t interruptVectorRegister = 0x000C;
		CrateController* controller;
		shmbuffer_t *p_buffer;


};
#endif
