#pragma once
#include "formatter.h"

class CAus :	public CFormatter
{
private:
	float price;
public:
	CAus(void);
	~CAus(void);

	/* Virtaul Functions Implementations */

	int format(const char * file_path , const char * save_path);
	/*void set_price(float p) ;
	float get_price() ;*/
	vector<std::string> extract_state_zip(std::string line) ;

};
