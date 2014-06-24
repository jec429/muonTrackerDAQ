#include "CrateController.hh"
#include <stdexcept>
#include <cstdio>
#include <fcntl.h>
#include "/root/drivers/sis1100-2.13-9/sis3100_calls/sis3100_vme_calls.h"
#include "/root/drivers/sis1100-2.13-9/dev/pci/sis1100_var.h"

CrateController* CrateController:: instance = NULL;

CrateController* CrateController::GetInstance(){
	if(!instance)
		instance = new CrateController();
return instance;
}
CrateController::CrateController(): Named("CrateController"), f_crate(0), f_isopen(false){
}

CrateController::~CrateController(){
//this is never called, I must call the Close() function myself
	if (IsCrateOpen() == true){
		Close();
	}
	delete instance;
}

void CrateController::A32D32_read(u_int32_t addr, u_int32_t* data){
	int ret = vme_A32D32_read(f_crate,addr,data);

	if (ret != 0){
		char str[50];
		sprintf(str, "vme_A32D32read failed on crate %d, addr 0x%x in ",f_crate, addr);
		throw std::runtime_error(str + f_name);
	}
}

void CrateController::A32D16_read(u_int32_t addr, u_int16_t* data){
	int ret = vme_A32D16_read(f_crate,addr,data);

	if (ret != 0){
		char str[50];
		sprintf(str, "vme_A32D16read failed on crate %d, addr 0x%x in ",f_crate, addr);
		throw std::runtime_error(str + f_name);
	}
}

void CrateController::A32D16_write(u_int32_t addr, u_int16_t data){
	int ret = vme_A32D16_write(f_crate,addr,data);

	if (ret != 0){
		char str[50];
		sprintf(str, "vme_A32D16write failed on crate %d, addr 0x%x word0x %x in ",f_crate, addr, data);
		throw std::runtime_error(str + f_name);
	}
}
void CrateController::A16D16_write(u_int16_t addr, u_int16_t data){
	int ret = vme_A16D16_write(f_crate,addr,data);

	if (ret != 0){
		char str[50];
		sprintf(str, "vme_A16D16write failed on crate %d, addr 0x%x word0x%x in ",f_crate, addr, data);
		throw std::runtime_error(str + f_name);
	}
}

void CrateController::Open(){
	if ((f_crate = open("/dev/sis1100_00remote", O_RDWR, 0))<0) {
		char str[50];
		sprintf(str, " could not open crate in ");
		throw std::runtime_error(str + f_name);
	}
	printf("Crate opened successfully\n");
	f_isopen = true;
}
void CrateController::Close(){
	if (close(f_crate)!= 0){
		char str[50];
		sprintf(str, "could not close crate in ");
		throw std::runtime_error(str + f_name);
	}
	f_isopen = false;
}
bool CrateController::IsCrateOpen(){
	return f_isopen;
}
void CrateController::A16D16_read(u_int16_t addr, u_int16_t* data){
	int ret = vme_A16D16_read(f_crate,addr,data);

	if (ret != 0){
		char str[50];
		sprintf(str, "vme_A16D16read failed on crate %d, addr 0x%x in ",f_crate, addr);
		throw std::runtime_error(str + f_name);
	}
}


