#pragma once

#include "mysql_connection.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>
#include <vector>
#include <string>
#include "..\common\Logger.h"


//#pragma comment(lib, "libmysql.lib")
//#pragma comment(lib, "mysqlcppconn.lib")

#define NUMOFFSET 100
#define COLNAME 200

using namespace std;
class MySqlConn
{
private:


public:
	sql::Driver *driver;
	sql::Connection *con;
	bool internet_failed;


	//sql::Connection::Connection CMYSQLConnector();
	~MySqlConn();
	MySqlConn();
	bool initDBConnection();
	bool CloseDBConnection();


	std::string GetLeadID(MySqlConn *db);
	std::string GetUserID(MySqlConn *db , std::string username);
	std::string GetUsername(MySqlConn *db , std::string user_id);
	std::string GetCountryID(MySqlConn *db , std::string country);
	int auth( MySqlConn *db , std::string username , std::string password);
	std::string increment_id(std::string id);
	vector<std::string> allowed_countries(MySqlConn *db  , std::string username);
	std::string GetCountryName(MySqlConn *db , std::string country_id);
	vector <std::string> GetURL(MySqlConn *db , std::string country_id);
	std::string GetUrlID(MySqlConn *db , std::string url);

	bool update_balance(MySqlConn *db , string country_id , string user_id , double balance);
	double get_balance( MySqlConn *db , std::string user_id , std::string country_id);
	std::vector<std::string> FetchAllBalance(MySqlConn *db , std::string username);
	std::vector<std::string> FetchAllCountryURL(MySqlConn *db , std::string username);
	bool insert_lead(MySqlConn *db , string country_id , std::string url, string user_id , double count);



};
