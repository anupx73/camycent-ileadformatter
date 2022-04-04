#include "StdAfx.h"
#include "Aus.h"
#include "..\common\Logger.h"

CAus::CAus(void)
{
}

CAus::~CAus(void)
{
}
//void CAus::set_price(float p)
//{
//	this->price = p;
//}
//
//float CAus::get_price()
//{
//	return this->price;
//}

vector<std::string> CAus::extract_state_zip(std::string line)
{
	bool space1_found = false;
	bool space2_found = false;
	bool digit_found = false;
	size_t init_zip =0 , init_state = 0 ,city_end = 0;
	std::string city; 
	std::string state;
	std::string zip;
	vector<std::string> words;
	size_t len = line.length();
	unsigned int i;
	for(i = (len-1) ; i>0 ; i--)
	{
		if(!(space1_found) && isspace(line[i]))
		{
			space1_found=true;
			if(digit_found)
			{
				init_zip = i;
			}
			else
			{
				init_state=i;
			}
		}
		else
		{
			if(isspace(line[i]) && space1_found && !(space2_found))
			{
				space2_found=true;
				if(digit_found)
				{
					init_state = i;
				}
				city_end = i;
			}
			else
			{
				if(isdigit(line[i]))
				{
					digit_found=true;
				}
			}
		}

	}

	int state_len = init_zip - init_state;

	if(digit_found)
	{
		words.push_back(line.substr(0 , city_end));
		words.push_back(line.substr(init_state , state_len));
		words.push_back(line.substr(init_zip));
	}
	else
	{
		words.push_back(line.substr(0 , line.find(" ")));
		words.push_back(line.substr(init_state));
		words.push_back(line.substr(init_zip));
	}



	if(words[0].empty() || !(space1_found))
	{
		words[0] = "  , ";
	}
	if(words[1].empty())
	{
		words[1] = "  ";
	}
	if(words[2].empty() || !(digit_found))
	{
		words[2] = "   ";
	}
	return words;
}

int CAus::format(const char * file_path , const char * save_path )
{
	int view_map = 0;
	int commit=0;
	int count = 1 , i=1;
	ifstream rd;
	ofstream wr; 
	//this->set_price(0.05f);
	rd.open(file_path);
	if(!rd)
	{
		g_Logger.LogEvent("File Not Found" , "AUS");
		return -6;
	}
	wr.open(save_path);
	SetFileAttributesA(save_path , FILE_ATTRIBUTE_HIDDEN);
	std::string line;
	while(std::getline(rd , line ,'\n'))
	{
		if((count-view_map) > 4 )
		{
			g_Logger.LogEvent("Invalid File Format" , "AUSTRALIA");
			return -1;
		}
		if(line.empty())
		{
			continue;
		}
		else
		{
			if(line.length()<=3)
			{
				continue;
			}
		}
		if(i<=5)
		{
			if(i==1 && line!="View map")
			{
				line=line_cleaner(line);
				vector<std::string> words = extract_2_words(line);
				std::vector<std::string>::iterator itr;
				for( itr = words.begin(); itr < words.end(); itr++)
				{
					wr<<*itr<<",";
				}
				++i;
				continue;
			}
			else
			{
				if(i==2 && line!="View map")
				{
					if(isdigit(line[0]))
					{
						wr<<line<<",";
						i++;
						continue;
					}
					else
					{
						wr<<" ,";
						++i;
						continue;
					}
				}
				else
				{
					if(i==3 && line!="View map")
					{
						vector<std::string> words_3 = extract_state_zip(line);
						std::vector<std::string>::iterator itr;
						for( itr = words_3.begin(); itr < words_3.end(); itr++)
						{
							wr<<*itr<<",";
						}
						++i;
						continue;
					}
				}
				if(i==4 && line!="View map")
				{
					if(ispunct(line[0]) || isdigit(line[0]))
					{
						wr<<line<<",";
						i++;
						continue;
					}
					else
					{
						wr<<",";
						i++;
						continue;
					}
				}
				else
				{
					if(line=="View map" || i==5)
					{
						if(line=="View map")
						{
							view_map++;
						}
						wr<<"\n";
						i=1;
						count++;
						continue;
					}
				}
			}
		}
	}
	
	return count;
}
