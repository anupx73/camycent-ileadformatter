#include "stdafx.h"
#include "USA.h"
#include "..\common\Logger.h"

using namespace std;

//void USA::set_price(float p)
//{
//	this->price = p;
//}
//
//float USA::get_price()
//{
//	return this->price;
//}

vector<std::string> USA::extract_state_zip(std::string line)
{
	bool comma_found = false;
	bool space_found = false;
	bool digit_found = false;
	std::string city; 
	std::string state;
	std::string zip;
	vector<std::string> words;
	size_t len = line.length();
	unsigned int i;
	for(i = 0 ; i<len ; i++)
	{
		if(ispunct(line[i]))
		{
			comma_found=true;
		}
		else
		{
			if(isdigit(line[i]))
			{
				digit_found=true;
			}
		}
		if(!comma_found)
		{
			city.push_back(line[i]);
		}
		if(comma_found && !(digit_found))
		{
			state.push_back(line[i]);
		}
		if(digit_found)
		{
			zip.push_back(line[i]);
		}

	}
	words.push_back(city);
	words.push_back(state);
	words.push_back(zip);
	if(words[0].empty() || !(comma_found))
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



int USA::format(const char * file_path , const char * save_path)
{
	int lead_seperator = 0;
	int count=0 , i=1;
	ifstream rd;
	ofstream wr; 
	rd.open(file_path);
	//this->set_price(0.05f);
	if(!rd)
	{
		g_Logger.LogEvent("File Not Found" , "USA");
		return -6;
	}
	wr.open(save_path);
	SetFileAttributesA(save_path , FILE_ATTRIBUTE_HIDDEN);
	std::string line;
	while(std::getline(rd , line))
	{
		if((count - lead_seperator > 4))
		{
			g_Logger.LogEvent("Invalid File Format" , "USA");
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

		if(i<=8)
		{
			if(i==1)
			{
				line=line_cleaner(line);
				vector<std::string> words = extract_2_words(line);
				std::vector<std::string>::iterator itr;
				for( itr = words.begin(); itr < words.end(); itr++)
				{
					wr<<*itr<<",";
				}
			}
			else
			{
				if(i==3)
				{
					vector<std::string> words_3 = extract_state_zip(line);
					std::vector<std::string>::iterator itr;
					for( itr = words_3.begin(); itr < words_3.end(); itr++)
					{
						wr<<*itr<<",";
					}
				}
				else
				{
					line=line_cleaner(line);
					if(i==8)
					{
						if(line.length() > 5)
						{
							std::string sep = line.substr(0 , 5);
							if(sep == "Email")
							{
								lead_seperator++;
							}
						}
						wr<<"\n";
						count++;
					}
					else
					{
						if(i>4 && i<=7)
						{
							++i;
							continue;
						}
						else
						{
							if(i==4)
							{
								if(!ispunct(line[0]))
								{
									wr<<" ,";
									++i;
								}
								else
								{
									wr<<line<<",";
								}
							}
							else
							{
								if(i==2)
								{
									if(!isdigit(line[0]))
									{
										wr<<"  ,";
										++i;
									}
									else
									{
										wr<<line<<",";
									}

								}
							}
						}
					}
				}
			}
		}
		else
		{
			i=0;
			++i;
			continue;
		}
		++i;

	}
	
	return count;
}




