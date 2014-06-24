#ifndef data_structures_hh
#define data_structures_hh
#include <sys/types.h>
#define NEVENTSSHM 1000 
#define NWIRES 75

struct GPS_t{
	u_int32_t nsIntoSec;
	u_int32_t secIntoDay;
	u_int16_t daysIntoYear;
	u_int16_t year;
	u_int16_t controlFlags;
};
struct TDC_t{
	u_int16_t nHits;
	u_int32_t times[NWIRES];
	u_int32_t channel[NWIRES];
	u_int16_t error;
};
struct event_t{
	u_int32_t evNum;
	TDC_t tdc;
	GPS_t gps;
};
struct shmbuffer_t{
	unsigned long long p_read;
	unsigned long long p_write;
	event_t data[NEVENTSSHM];
};
#endif
