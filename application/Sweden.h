#pragma once
#include "formatter.h"

/*
Accepts Format Copied from - http://www.hitta.se/
*/

class Sweden :	public CFormatter
{
private:
	int valid_count , invalid_no;
public:
  bool valid_state;
	bool valid_zip ;
	Sweden(void);
	~Sweden(void);
	/* Virtual Functions */
	int format(const char * file_path , const char * save_path);
	vector<std::string> extract_state_zip(std::string line) ;
	/* member Functions */
	void reset_validty();
	std::string clean_phno(std::string phno); 
	bool valid_alphaLine(std::string line);

};
