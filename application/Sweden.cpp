#include "StdAfx.h"
#include "Sweden.h"

Sweden::Sweden(void)
{
  this->valid_state = false;
  this->valid_zip = false;
  this->valid_count = 0;
  this->invalid_no = 0;
}

Sweden::~Sweden(void)
{
}

std::string Sweden::clean_phno(std::string phno)
{
  /* Clean Invalid Charecters from Phone Number */


  std::string clean;
  size_t len = phno.length();
  int digits = 0;
  unsigned int i;
  for( i=0 ; i <= len ; i++)
  {
    int CharVal = (int)phno[i];
    if(CharVal < 0)
    {
      continue;
    }
    if(isdigit(phno[i]))
    {
		digits++;
		if(digits == 1 && phno[i] == '0')
		{
			continue;
		}

      clean.push_back(phno[i]);
    }
    else
    {
      continue;
    }
  }
  return clean;
}

int Sweden::format(const char *file_path, const char *save_path)
{
  /* Sweden Formater */
 

  int count=0 , i=1;
  ifstream rd;
  ofstream wr; 
  rd.open(file_path);
  //this->set_price(0.05f);
  if(!rd)
  {
    g_Logger.LogEvent("File Not Found" , "Sweden");
    return -6;
  }
  wr.open(save_path);
  //SetFileAttributesA(save_path , FILE_ATTRIBUTE_HIDDEN);
  std::string line;
  while(std::getline(rd , line))
  {
    size_t len = line.length();
    if(len < 4 )
    {
      continue;
    }
    else
    {
      line = this->line_cleaner(line);
      if(i == 1)  // First line is the Name
      {
        if(this->valid_alphaLine(line))
        {
          std::size_t found = line.find("www.");
          std::size_t found_1 = line.find("snart!");
          if(found == std::string::npos && found_1 == std::string::npos){  // String Not Found
            wr<<line<<" , "; // Nothing to be done , insert in the comma seperated CSV
          }
          else
          {
            continue;
          }
        }
        else
        {
          wr<<"Name Not Found"<<",";
          ++i; // It is a Phno
        }
      }
      if(i == 2)   // Second line is the ph no
      {
        std::string clean_no = this->clean_phno(line);
        if(clean_no.empty() || clean_no.length() < 5)
        {
          this->invalid_no++;
          wr<<"0000000000"<<" ,";
        }
        else
        {
          wr<<clean_no<<" ,"; // Clean Invalid charecters
          
        }
      }
      if( i == 3)   // Address Noting to be Done
      {
        if(this->valid_alphaLine(line) == true) // Makes sure its an address not a no Ph 
        {
          wr<<line<<" ,";
        }
        else
        {
          continue;   // Some people can hove two nos so we discard one to maintain uniformity
        }
      }
      if( i == 4 )   // Zip Code and Country
      {
        // Seperate's Zip Code and Country

        vector<std::string> zip_state = this->extract_state_zip(line); 
        /*
        No Zip in the last line means that 
        one of the line was missing in the set of 4
        */
        if(this->valid_zip)  
        {
          /*
          Zip Valid writing both of them
          and insert new line
          Set counter to 1
          */
          wr<<zip_state.at(0)<<" ,";
          wr<<zip_state.at(1)<<" ,";
          wr<<"\n";
          i = 1 ;
          this->valid_count++;
        }
        else
        {
          /* Zip Invalid 
          Filling two Cells with Gargabe
          Inserting two Line

          Now This means the Line in curson 
          is the name for the next Set
          Inserting that after a new line
          and seting the set counter 'i' to 2
          Since the Next Line will be Ph Number

          */
          wr<<"000000"<<" ,";
          wr<<"NA"<<" ,";
          wr<<"\n";
          wr<<line<<" ,";
          i = 2 ;
        }

        this->reset_validty();

        // Incrementing the Lead Count
        count++;

        // Ignoring the File Counter Intrement
        continue;
      }
      i++;

    }

  }

  ///*int ves = count - this->valid_count; 
  ///* total No of invalid Zips */
  //int invalid_zip_set = count  - ves;
  ///* total no of sets without a zip */
  //int invalid_no_set = count - this->invalid_no;
  ///* Tottal no of sets without a valid no */
  //float invalidity = invalid_zip_set + invalid_no_set;
  ///* total no of invalid sets */
  //float allowed_percent =  10 /100 * count;
  //if(invalidity > allowed_percent )
  //{
  //	/* if it is more than 25% Discard */
  //	count = -1;
  //} 
  /* 
  Not Full proof 
  Contribute if u can do anything better .. !
  */
  int lead_count = count - this->invalid_no;
  return lead_count;
}

bool Sweden::valid_alphaLine(std::string line)
{
  bool valid = false;
  bool found = false;
  size_t len = line.length();
  unsigned int i ;
  for( i = 0 ; i <= len ; i++)
  {
    int CharVal = (int)line[i];
    if(CharVal < 0)
    {
      continue;
    }
    if(isalpha(line[i]))
    {
      found = true;
      break;
    }
  }
  if(found)
  {
    valid = true;
  }
  else
  {
    valid=false;
  }
  return valid;
}
vector<std::string> Sweden::extract_state_zip(std::string line)
{
  /* Extract Zip Code and State */

  vector<std::string> state_zip;
  std::string zip;
  std::string state;
  size_t len = line.length();
  unsigned int i;
  bool alphaFound = false;


  for( i=0 ; i <= len ; i++)
  {
    int CharVal = (int)line[i];
    if(CharVal < 0)
    {
      continue;
    }
    if(isalpha(line[i]))
    {
      alphaFound = true; // zip String Ends
    }
    if(alphaFound == false)
    {
      if(isdigit(line[i]))
      {
        if(!(isspace(line[i])))
        {
          zip.push_back(line[i]);
        }
      }
    }
    if(alphaFound == true)
    {
      state.push_back(line[i]);
    }

  }
  if(zip.empty() || zip.length() <1)
  {
    this->valid_zip = false;
  }
  else
  {
    this->valid_zip = true;
  }
  if(state.empty() || state.length() < 1)
  {
    this->valid_state = false;
  }
  else
  {
    this->valid_state = true;
  }
  state_zip.push_back(zip);
  state_zip.push_back(state);

  return state_zip;
}

void Sweden::reset_validty()
{
  this->valid_state = false;
  this->valid_zip = false;
}