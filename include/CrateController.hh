#include <sys/types.h>
#include "Named.hh"
#ifndef crate_controller_hh
#define crate_controller_hh

// singleton class //
class CrateController: public Named{

public:
static CrateController* GetInstance();
void A32D32_read(u_int32_t, u_int32_t*);
void A32D16_read(u_int32_t, u_int16_t*);
void A32D16_write(u_int32_t, u_int16_t);
void A16D16_write(u_int16_t, u_int16_t);
void A16D16_read(u_int16_t, u_int16_t*);
void Open();
void Close();
bool IsCrateOpen();

private:
CrateController();
virtual ~CrateController();
static CrateController* instance;
int f_crate;
bool f_isopen;


};
#endif
