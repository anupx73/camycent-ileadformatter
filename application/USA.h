#pragma once
#include "formatter.h"

class USA :public CFormatter
{
private:
	float price;
	
public:
		/* Virtaul Functions Implementations */

  int format(const char * file_path , const char * save_path);
	vector<std::string> extract_state_zip(std::string line);
	/*void set_price(float p);
	float get_price();*/
	
};
