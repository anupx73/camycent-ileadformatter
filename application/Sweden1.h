#pragma once
#include "Sweden.h"
/*
Accepts Format Copied from - http://www.118100.se/
*/
class Sweden1 :	public Sweden
{
private:
	bool set_comp;
	int set_found;
	bool valid_state;
	bool valid_zip ;
	bool nPh;
  int invalid;
public:
	Sweden1(void);
	~Sweden1(void);
	/* OverLoaded Funtions */
	int format(const char * file_path , const char * save_path);
	/* member Function */
	bool bracket_line(std::string line);
	bool end_of_set(std::string line);

};
