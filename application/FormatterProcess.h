#pragma once
#include <vector>
#include <sstream>
#include "USA.h"
#include "AUS.h"
#include "Sweden.h"
#include "Sweden1.h"
#include "..\common\Logger.h"
#include "MySqlConn.h"



class CFormatterProcess
{
private:
	bool insertion_status ;
	bool format_status ;
  template <typename T> void initProc( MySqlConn *db_conn , T obj , std::string country , std::string url ,  std::string username , std::string FilePath , std::string Savepath  , std::vector<std::string> Filters);
	int build_status(MySqlConn *db_conn);
	int FilterOutput(std::string output_file , std::vector< std::string > Filters);
	std::string Extract_Number(std::string line);
  
public:
	CFormatterProcess(void);
	~CFormatterProcess(void);
	int ProcessCSV(MySqlConn *db_conn , std::string country , std::string url ,  std::string username ,  std::string FilePath , std::string SavePath , std::vector<std::string> Filters);
   std::vector < std::string > GetFilterVector(std::string FilterStr);
	
};
