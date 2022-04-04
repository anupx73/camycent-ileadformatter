#include "stdafx.h"
#include <exception>
#include <stdexcept>
#include "Formatter.h"
#include "..\common\Logger.h"

double dBalanceLeft = 0.0;
using namespace std;

CFormatter::CFormatter(void)
{
}

CFormatter::~CFormatter(void)
{

}


std::string CFormatter::get_CountryName()
{
  return this->country;

}
bool CFormatter::set_CountryName(std::string country)
{
  this->country=country;
  return true;
}

bool CFormatter::set_UserID(std::string uid)
{
  this->user_id=uid;
  return true;
}

std::string CFormatter::get_UserID()
{
  return this->user_id;
}

std::vector<std::string> CFormatter::extract_2_words(const std::string line)
{
  bool found_space = false;
  vector<std::string> words;
  std::string word1 , word2;
  size_t len = line.length();
  unsigned int i;
  for(i = 0 ; i<len ; i++)
  {
    if(line[i] < (char) 0)
    {
      continue;
    }
    if(isspace(line[i]))
    {
      found_space = true;
    }
    if(!found_space)
    {
      word1.push_back(line[i]);
    }
    else
    {
      word2.push_back(line[i]);
    }

  }
  words.push_back(word1);
  words.push_back(word2);
  return words;
}

string CFormatter::line_cleaner(string line)  // removes ',' from the Line
{
  string clean(line);
  char chars[] = ",";
  unsigned int i;
  for (i = 0; i <=strlen(chars); ++i)
  {
    clean.erase (std::remove(clean.begin(), clean.end(), chars[i]), clean.end());
  }
  return clean;
}


bool CFormatter::insert_db(MySqlConn *db_conn , int count , std::string username ,std::string country , std::string url)
{
  bool iSuccess = false;
  string country_id;
  string user_id = db_conn->GetUserID(db_conn , username);
  if(!db_conn->internet_failed)
  {
    this->set_CountryName(country);
    country_id = db_conn->GetCountryID(db_conn , this->get_CountryName());
    if(!db_conn->internet_failed)
    {
      if(user_id.empty() || country_id.empty())
      {
        iSuccess =  false;
        return iSuccess;
      }
      else
      {
        std::string user_id = db_conn->GetUserID(db_conn , username);
        if(!db_conn->internet_failed)
        {
          double  balance = db_conn->get_balance(db_conn, user_id , country_id);
          if(!db_conn->internet_failed)
          {
            double new_balance = balance - count;

            if(new_balance > 0)
            {
              if(db_conn->update_balance(db_conn , country_id ,  user_id , new_balance ))  // Main DB Insertion
              {
                if(db_conn->insert_lead(db_conn , country_id , url , user_id , count))
                {
                  if(!db_conn->internet_failed) // Redundant Check .. Just to be safe.
                  {
                    dBalanceLeft = new_balance;
                    iSuccess =  true;
                  }
                  else
                  {
                    iSuccess =  false; 
                  }
                }
                else
                {
                  iSuccess =  false;
                }
              }
              else
              {
                iSuccess =  false;
              }
            }
            else
            {
              dBalanceLeft = new_balance;
              iSuccess =  false; 
            }
          }
          else  // Get Balance Failed 
          {
            iSuccess =  false; 
          }
        }
        else  // Get User ID Failed 
        {
          iSuccess =  false; 
        }
      }
    }
    else  // Get Country ID
    {
      iSuccess = false;
    }
  }
  else  // Get User Failed !
  {
    iSuccess = false;
  }
  return iSuccess;
}
