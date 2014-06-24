#include "Named.hh"

Named::Named(){
	f_name = "NONE";
}
Named::Named(std::string& name):f_name(name){}
Named::Named(const char* name):f_name(name){}
Named::~Named(){}
const char* Named::GetName(){
	return f_name.c_str();
}







