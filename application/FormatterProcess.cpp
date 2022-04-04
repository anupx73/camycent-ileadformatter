#include "StdAfx.h"
#include "FormatterProcess.h"
#include "MySqlConn.h"
#include "..\common\Logger.h"

CFormatterProcess::CFormatterProcess(void)
{
	this->format_status = false;
	this->insertion_status = false;
}

CFormatterProcess::~CFormatterProcess(void)
{
}
int CFormatterProcess::build_status(MySqlConn *db_conn)
{
	int status;
	if(format_status == true && insertion_status == true && db_conn->internet_failed == false)
	{
		status = 0;  //  File and DB opration Successful , internet not disconnected
	}
	else
	{

		if( format_status  == false)
		{
			status = 1;   // File Format Failed 
		}
		else
		{
			if(insertion_status == false)
			{
				status = 2;   //  DB Insert Failed
			}
			else
			{
				if(db_conn->internet_failed)
				{
					status = 3;
				}
			}
		}
	}

	return status;
}
int CFormatterProcess::ProcessCSV(MySqlConn *db_conn , std::string country , std::string url ,  std::string username ,  std::string FilePath , std::string SavePath , std::vector<std::string> Filters)
{
	if(country == "SWEDEN")
	{
		if(url == "http://www.hitta.se/")
		{
			Sweden swd;
			this->initProc(db_conn , swd , "SWEDEN" , "http://www.hitta.se/" , username , FilePath , SavePath , Filters);
		}
		else if(url == "http://www.118100.se/")
		{
			Sweden1 swd1;
			this->initProc(db_conn , swd1 , "SWEDEN" , "http://www.118100.se/" , username , FilePath , SavePath , Filters);
		}
	}

	int status = build_status(db_conn);
	return status;
}
template <typename T> void CFormatterProcess::initProc(MySqlConn *db_conn , T obj ,std::string country , std::string url , std::string username , std::string FilePath , std::string SavePath , std::vector<std::string> Filters  )
{
	bool inserted;
	int count;
	count = obj.format((FilePath).c_str() , (SavePath).c_str());
	if(count > 0)
	{
		format_status = true;
		if(db_conn->internet_failed != true)
		{
			/* Apply Filter */
			int filter = this->FilterOutput(SavePath , Filters);	
			count = count - filter;
			inserted = obj.insert_db(db_conn , count , username , country , url);
			if(inserted == true)
			{
				insertion_status = true;
				SetFileAttributesA( (SavePath).c_str() , FILE_ATTRIBUTE_NORMAL);
			}
			else
			{
				insertion_status = false;
				g_Logger.LogEvent("DB Insertion Failed for "+country+" Username - "+username , count);
			}
		}
		else
		{
			g_Logger.LogEvent("Internet Disconencted " , country);
			format_status = false;
			insertion_status = false;
		}
	}
	else
	{
		g_Logger.LogEvent("Invalid File Format " , country);
		format_status = false;
		insertion_status = false;
	}
}

std::vector < std::string > CFormatterProcess::GetFilterVector(std::string FilterStr)
{
	
	std::vector <std::string> Filters;
	size_t len = FilterStr.length();
	for(size_t i = 0 ; i<= len ; i++)
	{
		if(FilterStr[i] == '1')
		{
			stringstream st;
			st << i;
			std::string FilterNo = st.str();
			Filters.push_back(FilterNo);
		}
	}
	return Filters;

}


std::string CFormatterProcess::Extract_Number(std::string line)
{
	bool digit_found = false;
	std::vector <char> number;
	for(size_t i = 0 ; i<=line.length(); i++)
	{
		if(line[i] < (char) 0)
		{
			continue;
		}
		if(isdigit(line[i]))
		{
			digit_found = true;
			number.push_back(line[i]);
		}
		if(digit_found && isalpha(line[i]) )
		{
			break;
		}
	}
	std::string numbers(number.begin() , number.end());
	return numbers;
}
int CFormatterProcess::FilterOutput(std::string output_file, std::vector<std::string> Filters)
{
	int filtered = 0 ;
	std::string read_file = output_file;
	read_file.append("_unfiltered");
	// Renaming the converted File
	rename(output_file.c_str() , read_file.c_str());
	ofstream wr;
	// Opening the converted file in write mode
	wr.open(output_file.c_str());
	ifstream rd;
	// Opening the renamed file in read mode
	rd.open(read_file.c_str());
	if(!rd)
	{
		g_Logger.LogEvent("File Not Found" , "Sweden");
		return -6;
	}
	std::string line;
	bool skip = false;
	std::string zero = "0";
	Filters.push_back(zero);
	while(std::getline(rd , line))
	{
		skip = false;
		std::string number = this->Extract_Number(line);
		std::vector<std::string>::iterator i;

		for(i = Filters.begin(); i != Filters.end(); i++)
		{
			std::string ft_no(*i);
			std::string frst_digit = number.substr(0 , 1);
			if(ft_no == frst_digit)
			{
				skip = true;
			}
		}
		if(skip)
		{
			filtered++;
			continue;
		}
		else
		{
			wr<<line<<endl;
		}

	}
	 remove(read_file.c_str());
	 return filtered;
	

}