#include "stdafx.h"
#include "MySqlConn.h"
#include "..\common\Logger.h"

#define HOST "tcp://pools2.animusgs.co.in:3306"
#define USER "camycent"
#define PASS "XDRGBHU123"
#define DB "db_camycent"


using namespace std;


MySqlConn::~MySqlConn()
{
  //delete this->con;
  //delete this->driver;
  
}
MySqlConn::MySqlConn()
{
  this->internet_failed = false;
}

bool MySqlConn::initDBConnection()
{
  this->driver = get_driver_instance();
  try
  {
    this->con = driver->connect(HOST, USER, PASS);
    this->con->setSchema(DB);
    return true;
  }
  catch(sql::SQLException &e)
  {
    g_Logger.LogEvent("Failed TO Connect to DataBase Server" ,e.what());	
    this->internet_failed = true;
    return false;
  }
}

bool MySqlConn::CloseDBConnection()
{
  if(!this->internet_failed)
  {
    try
    {
      this->con->close();
      delete this->con;
      this->driver->threadEnd();
      return true;
    }
    catch(sql::SQLException &e)
    {
      g_Logger.LogEvent("Failed To Close Connection to DataBase Server" ,e.what());		
      this->internet_failed = true;
      return false;
    }
  }
  else
  {
    return true;
  }

}
std::string MySqlConn::GetUsername(MySqlConn *db , std::string user_id)
{
  std::string username;
  sql::Statement *stmt;
  sql::ResultSet *res;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    username = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {
      std::string query = "SELECT * from users where user_id = '";
      query.append(user_id);
      query.append("'");
      stmt = db->con->createStatement();
      res = stmt->executeQuery(query);
      if (res->next()) 
      {
        username = res->getString("username").c_str();
      }
      delete res;
      delete stmt;
    } 
    catch(sql::SQLException e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("User ID Fetch Failed",e.what());

    }
  }
  return username;
}
std::string MySqlConn::GetUrlID(MySqlConn *db , std::string url)
{
  std::string url_id;
  sql::Statement *stmt;
  sql::ResultSet *res;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    url_id = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {

      std::string query = "SELECT * from  url where url = '";
      query.append(url);
      query.append("'");
      stmt = db->con->createStatement();
      res = stmt->executeQuery(query);
      if (res->next()) 
      {
        url_id = res->getString("url_id").c_str();
      }
      //con->close();

      delete res;
      delete stmt;
    } 
    catch(sql::SQLException e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("User ID Fetch Failed",e.what());

    }
  }
  return url_id;
}
bool MySqlConn::insert_lead(MySqlConn *db , string country_id , std::string url, string user_id , double count)
{
  sql::PreparedStatement *prep_stmt;

  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    db->internet_failed = true;
    return false;
  }
  else
  {

    try
    {	
      std::string lead_id = this->GetLeadID(db);
      if(!db->internet_failed)
      {
        std::string username = this->GetUsername(db , user_id);
        if(!db->internet_failed)
        {
          std::string country = this->GetCountryName(db , country_id);
          if(db->internet_failed == false)
          {
            std::string url_id = this->GetUrlID(db , url);
            if(db->internet_failed == false)
            {
              std::string query = "insert into leads values (? , DEFAULT , ? , ? ,? ,? , ? , ? , ?)";
              prep_stmt = db->con->prepareStatement(query);
              prep_stmt->setString(1 , lead_id);
              prep_stmt->setString(2 , user_id);
              prep_stmt->setString(3 , username);
              prep_stmt->setString(4 , country_id);
              prep_stmt->setString(5 , country);
              prep_stmt->setString(6 , url_id);
              prep_stmt->setString(7 , url);
              prep_stmt->setDouble(8 , count);
              prep_stmt->execute();
              delete prep_stmt;
              //delete con;
              return true;
            }
            else // Get URL ID Failed 
            {
              return false;
            }
          }
          else //Country Name Failed 
          {
            return false;
          }
        }
        else // GET USERNAME FAILED 
        {
          return false;
        }
      }
      else //lead ID Failed 
      {
        return false;
      }
    }
    catch(sql::SQLException &e)
    {
      g_Logger.LogEvent("Failed TO Insert Count" ,e.what());	
      db->internet_failed = true;
      return false;
    }

  }

}
bool MySqlConn::update_balance(MySqlConn *db ,  string country_id, string user_id, double balance)
{
  sql::PreparedStatement *prep_stmt;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    db->internet_failed = true;
    return false;
  }
  else
  {

    try
    {		
      prep_stmt = db->con->prepareStatement("update balance set  balance = ? where user_id = ? and country_id = ?");
      prep_stmt->setDouble(1 , balance);
      prep_stmt->setString(2 , user_id);
      prep_stmt->setString(3 , country_id);
      prep_stmt->execute();
      delete prep_stmt;
      //delete con;
      return true;
    }
    catch(sql::SQLException &e)
    {
      g_Logger.LogEvent("Failed TO Insert Count" ,e.what());	
      db->internet_failed = true;
      return false;
    }
  }
}

std::string MySqlConn::increment_id(std::string id)
{
  /* 
  Increments lead_id from 
  lead_A_001 - lead_0_9999999
  lead_B_001 - lead_1_9999999
  ....
  lead_Z_001 - lead_Z_9999999
  ...
  lead_AA_001 - lead_AA_999999
  ..
  lead_AZ_001 - lead_ZZ_999999
  ..
  lead_AB_001 - lead_AB_999999
  ..
  lead_AC_001 - lead_AC_999999
  ...
  ...
  lead_AZ_001 - lead_AZ_999999
  ..
  ..
  lead_BA_001 - lead_BA_999999
  ..
  lead_BZ_001 - lead_BZ_999999
  ..
  lead_ZZ_001 - lead_ZZ_999999


  */
  try
  {
    int len= id.length();

    if(id[len-1] == 57 && id[len-2] ==57 && id[len-3] == 57 && id[len-4] == 57 && id[len-5] == 57 && id[len-6] == 57 && id[len-8]==90 )
    {
      // lead_AZ_999999 - lead_BA_999999
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3] = 48 ;
      id[len-4] = 48 ;
      id[len-5] = 48 ;
      id[len-6] = 48 ;
      id[len-8] = 65;
      id[len-9] ++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] ==57 && id[len-3] == 57 && id[len-4] == 57 && id[len-5] == 57 && id[len-6] == 57)
    {
      // lead_AA_999999 - lead_AZ_999999
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3] = 48 ;
      id[len-4] = 48 ;
      id[len-5] = 48 ;
      id[len-6] = 48 ;
      id[len-8] ++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] ==57 && id[len-3] == 57 && id[len-4] == 57 && id[len-5] == 57)
    {
      //Lakhs  Place +1
      // lead_AA_100000 .. lead_AA_900000
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3] = 48 ;
      id[len-4] = 48 ;
      id[len-5] = 48 ;
      id[len-6] ++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] ==57 && id[len-3] == 57 && id[len-4] == 57)
    {
      //Ten Thousands Place +1
      // lead_AAA_010000 .. lead_AA_090000
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3] = 48 ;
      id[len-4] = 48 ;
      id[len-5] ++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] ==57 && id[len-3] == 57)
    {
      //Thousands Place +1
      // lead_AA_001000 .. lead_AA_009000
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3] = 48 ;
      id[len-4] ++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] ==57)
    {
      //Hundreds Place +1
      // lead_AA_000100 .. lead_AA_000900
      id[len-1] = 48 ;
      id[len-2] = 48 ;
      id[len-3]++;
      return id;
    }
    else if(id[len-1] == 57 && id[len-2] !=57 )
    {
      // Tens Place +1
      //lead_AA_000010 - lead_AA_000090
      id[len-1] = 48 ;
      id[len-2]++;
      return id;
    }
    if(id[len-1] != 57)  
    {
      // Normal increment last digit +1
      id[len-1]++; 		
      return id;
    }

    return id;		

  }
  catch(std::exception &e)
  {
 //   db->internet_failed = true;
    g_Logger.LogEvent("ID Incrementation Failed",e.what());
  }

  return id;
}

string MySqlConn::GetLeadID(MySqlConn *db)
{
  string lead_id;
  /*sql::Driver *driver;
  sql::Connection *con;
  */
  sql::Statement *stmt;
  sql::ResultSet *res;
  //driver = get_driver_instance();
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    lead_id = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {
      //con = driver->connect(HOST, USER, PASS);
      /* Connect to the MySQL database */
      //con->setSchema(DB);
      stmt = db->con->createStatement();
      res = stmt->executeQuery("SELECT lead_id from leads order by lead_id desc");
      if(res->next())
      {
        lead_id= res->getString(1).c_str();
        lead_id= increment_id(lead_id);
      }
      else
      {
        lead_id= "lead_AA_000001";
      }
      //con->close();
      delete res;
      delete stmt;
      //delete con;
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Lead ID Generation Failed",e.what());
    }
  }


  return lead_id;


}



std::string MySqlConn::GetUserID(MySqlConn *db , std::string username)
{
  string user_id;
  sql::Statement *stmt;
  sql::ResultSet *res;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    user_id = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {
      std::string query = "SELECT * from users where username = '";
      query.append(username);
      query.append("'");
      stmt = db->con->createStatement();
      res = stmt->executeQuery(query);
      if (res->next()) 
      {
        user_id = res->getString("user_id").c_str();
      }
      delete res;
      delete stmt;
    } 
    catch(sql::SQLException e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("User ID Fetch Failed",e.what());

    }
  }

  return user_id;


}

string MySqlConn::GetCountryID(MySqlConn *db ,  std::string country)
{
  string country_id;
  sql::Statement *stmt;
  sql::ResultSet *res;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    country_id = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {
      stmt = db->con->createStatement();
      string query = "SELECT * from country where country_name = '";
      query.append(country);
      query.append("'");
      res = stmt->executeQuery(query);
      if(res->next())
      {
        country_id = res->getString(1).c_str();
        //		con->close();
        delete res;
        delete stmt;
        //	delete con;
        return country_id;
      }
      else
      {
        //con->close();
        delete res;
        delete stmt;
        //delete con;
        return ""; 
      }
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Country ID Fetch Failed",e.what());
      return "";
    }
  }


  return country_id;
}

int MySqlConn::auth( MySqlConn *db , std::string username , std::string password)
{
  //	sql::Driver *driver;
  //sql::Connection *con;
  sql::Statement *stmt;
  sql::ResultSet *res;
  std::string status;
  //driver = get_driver_instance();
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    return 4;
  }
  else
  {
    try
    {
      //con = driver->connect(HOST, USER, PASS);
      /* Connect to the MySQL database */
      //con->setSchema(DB);
      std::string query = "select * from users where username = '";
      query.append(username);
      query.append("' and password = '");
      query.append(password);
      query.append("'");
      stmt = db->con->createStatement();
      res = stmt->executeQuery(query);
      size_t count = res->rowsCount();
      //con->close();
      //delete con;
      if(count  <= 0 )
      {
        g_Logger.LogEvent("Invalid Authentication" , username);
        return 1; // Auth Failed. Username Pass Mismatched.
      }
      else
      {
        if(res->next())
        {
          status = res->getString("status").c_str();
          delete res;
          delete stmt;
          if(status == "active")
          {	
            g_Logger.LogEvent("Authenticated ! " , username);
            return 0;  // Auth Successful
          }
          else if(status == "inactive")
          {
            g_Logger.LogEvent("Account Inactive " , username);
            return 2;  // Account Suspended
          }
          else if(status == "deactivated")
          {
            g_Logger.LogEvent("Account Deactivated " , username);
            return 3;  // Account Deactivated
          }
        }
      }
      return 1;
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Connection Problem - Auth" , e.what());
      return 4; // Internet Connection not Found.
    }
  }


}
std::string MySqlConn::GetCountryName(MySqlConn *db , std::string country_id)
{
  std::string country_name;
  sql::Statement *stmt;
  sql::ResultSet *res;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    country_name = "NULL";
    db->internet_failed = true;
  }
  else
  {
    try
    {
      stmt = db->con->createStatement();
      string query = "SELECT * from country where country_id = '";
      query.append(country_id);
      query.append("'");
      res = stmt->executeQuery(query);
      if(res->next())
      {
        country_id = res->getString("country_name").c_str();
        delete res;
        delete stmt;
        return country_id;
      }
      else
      {
        delete res;
        delete stmt;
        return ""; 
      }
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Country Name Fetch Failed",e.what());
      return "";
    }
  }


  return country_name;
}
vector <std::string> MySqlConn::GetURL(MySqlConn *db , std::string country_id)
{
  sql::Statement *stmt;
  sql::ResultSet *res;
  vector<std::string> url_list;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    url_list.push_back("NULL");
    db->internet_failed = true;
  }
  else
  {
    try
    {
      stmt = db->con->createStatement();
      // Fetching all URL of a Country
      string query = "SELECT * from url where country_id = '";
      query.append(country_id);
      query.append("'");
      res = stmt->executeQuery(query);
      while(res->next())
      {
        std::string url = res->getString("url").c_str();
        url_list.push_back(url);
      }
      delete res;
  delete stmt;
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Connection Problem - Get URL" , e.what());
    }
  }
  
  return url_list;

}
vector<std::string> MySqlConn::allowed_countries(MySqlConn *db , std::string username)
{
  sql::Statement *stmt;
  sql::ResultSet *res;
  vector<std::string> allowed_id;
  std::string output;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    allowed_id.push_back("NULL");
    db->internet_failed = true;
  }
  else
  {
    try
    {
      // Converting sername to User ID
      std::string user_id = this->GetUserID(db , username);
      if(!db->internet_failed)
      {
        stmt = db->con->createStatement();
        // Fetching all the Allowed Country ID 
        string query = "SELECT * from allowed_countries where user_id = '";
        query.append(user_id);
        query.append("'");
        res = stmt->executeQuery(query);
        while(res->next())
        {
          std::string id = res->getString("country_id").c_str();
          allowed_id.push_back(id);
        }
        delete res;
        delete stmt;
      }
      else // Get UserID failed 
      {
        allowed_id.push_back("NULL");

      }
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Connection Problem - fill_country" , e.what());
    }

  }
  return allowed_id;
}

std::vector<std::string> MySqlConn::FetchAllCountryURL(MySqlConn *db , std::string username)
{
  vector <std::string> country_url;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    country_url.push_back("NULL");
    db->internet_failed = true;
  }
  else
  {
    vector <std::string> allowed_id = this->allowed_countries(db , username);
    if(allowed_id.at(0) == "NULL")
    {
      country_url.push_back("NULL");
    }
    else
    {

      std::string country , output;
      std::vector<std::string>::iterator itr;
      vector <std::string>::iterator url_list;
      for( itr = allowed_id.begin(); itr < allowed_id.end(); itr++)
      {

        // Get Country Name From Country IDs

        country = (this->GetCountryName(db , *itr));
        if(!db->internet_failed)
        {
          std::size_t found = output.find(country);
          if(found == std::string::npos)
          {
            output = country;
            output.append("|");
          }
          // Get all URL for a Country

          vector <std::string>  url;
          url = (this->GetURL(db , *itr));
          for(url_list = url.begin(); url_list < url.end(); url_list++)
          {

            output.append(*url_list);
            output.append("|");

          }
          country_url.push_back(output);

        }
        else // Get country Name Failed !
        {
          country_url.push_back("NULL");
          break;
        }
      }
    }
  }
  return country_url;
}

double MySqlConn::get_balance(MySqlConn *db, std::string user_id, std::string country_id)
{
  sql::Statement *stmt;
  sql::ResultSet *res;
  double balance =0;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    db->internet_failed = true;
  }
  else
  {
    try
    {
      string query = "SELECT * from balance where user_id = '";
      query.append(user_id);
      query.append("' and country_id ='");
      query.append(country_id);
      query.append("'");
      stmt = db->con->createStatement();
      res = stmt->executeQuery(query);
      while(res->next())
      {
        balance = res->getInt("balance");
      }
      delete res;
  delete stmt;
    }
    catch(sql::SQLException &e)
    {
      db->internet_failed = true;
      g_Logger.LogEvent("Connection Problem - fill_country" , e.what());
    }
  }
  
  return balance;
}

std::vector <std::string> MySqlConn::FetchAllBalance(MySqlConn *db, std::string username)
{
  std::vector < std::string > user_bal;
  bool db_disconnected = db->con->isClosed();
  if(db_disconnected)
  {
    db->internet_failed = true;
    user_bal.push_back("NULL");
  }
  else
  {
    std::string output;
    std::vector<std::string> allowed_id = this->allowed_countries(db , username);
    if(allowed_id.at(0) == "NULL")
    {
      user_bal.push_back("NULL");
    }
    else
    {
      std::vector<std::string>::iterator itr;

      std::string user_id = this->GetUserID(db , username);
      if(!db->internet_failed)
      {
        for( itr = allowed_id.begin(); itr < allowed_id.end(); itr++)
        {
          std::string country = (this->GetCountryName(db , *itr));
          if(!db->internet_failed)
          {
            output = country;
            output.append("|");
            double balance = this->get_balance(db , user_id , *itr);
            if(!db->internet_failed)
            {
              // Convert Double to CString 
              CString cstrValue;
              cstrValue.Format(_T("%f"), balance);
              // Convert CSTRING TO DOUBLE
              CT2CA bal_val(cstrValue);
              std::string balval(bal_val);
              output.append(bal_val);
              user_bal.push_back(output);
              output.clear();
            }
            else  // Get Balance Failed ! 
            {
              user_bal.push_back("NULL");
              // Exit Loop
              break;
            }
          }
          else //GetCountryname Failed
          {
            user_bal.push_back("NULL");
            // Exit Loop
            break;
          }
        }
      }
      else // Get UserID Failed 
      {
        user_bal.push_back("NULL");
      }
    }
  }
  return user_bal;
}







