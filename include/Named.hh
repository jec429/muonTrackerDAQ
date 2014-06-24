#ifndef named_hh
#define named_hh

#include <string>

class Named{
	public:
		Named();
		Named(std::string&);
		Named(const char*);
		virtual ~Named();
		const char* GetName();

	protected:
		std::string f_name;

};
#endif

