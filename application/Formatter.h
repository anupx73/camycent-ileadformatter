#pragma once
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include "MySqlConn.h"
#include "..\common\Logger.h"


using namespace std;


class CFormatter
{
private:
	std::string user_id;
	std::string country;
public:
	CFormatter(void);
	~CFormatter(void);
	/* Pure Virtaul Function */

	virtual int format(const char * file_path , const char * save_path ) = 0;
	/*virtual void set_price(float p) = 0;
	virtual float get_price() = 0;*/
	virtual std::vector<std::string> extract_state_zip(std::string line) = 0;
	/* Getter n Setters */

	std::string get_CountryName();
	bool set_CountryName(std::string country);
	std::string get_UserID();
	bool set_UserID(std::string uid);

	/* DB Operation */

  bool insert_db(MySqlConn *db_conn , int count , std::string username , std::string country , std::string url);

	/* Format Helpers */
	std::string line_cleaner(std::string line) ;
	std::vector <std::string > extract_2_words(const std::string line);

	
	
	


};
