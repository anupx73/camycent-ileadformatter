#include "StdAfx.h"
#include "Sweden1.h"

Sweden1::Sweden1(void)
{
	this->set_comp = false;
	this->set_found = 0;
	this->nPh = false;
  this->invalid = 0;
}

Sweden1::~Sweden1(void)
{
}
bool Sweden1::bracket_line(std::string line)
{
	if(line[0] == '(')
	{
		return true;
	}
	else
	{
		return false;
	}

}



int Sweden1::format(const char *file_path, const char *save_path)
{
	//this->GetFilters();
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
	std::string clean;
	while(std::getline(rd , line))
	{
		if(this->set_comp)
		{
			this->set_comp = false;
		}
		size_t len = line.length();
		if(len < 4 || line.empty())
		{
			continue;
		}
		else
		{
			line = this->line_cleaner(line); // Removes ','
			if(line.compare("Mer information") == 0)
			{
				count++;
				wr<<" \n";
				i = 1;
				this->set_comp = true;
				this->set_found++;
				continue;
			}
			else
			{

				if(i == 1) // Name
				{
					std::size_t found = line.find("Nästa");
					if(found == std::string::npos)
					{
						if(this->valid_alphaLine(line))  // has alpha chars
						{
							wr<<line<<" , ";
						}
						else
						{
							wr<<"Name Not Found "<<" , ";
							++i;
							if(this->bracket_line(line)) 
							{
								continue;
							}
							else
							{
								clean = this->clean_phno(line);
								if(clean.length() > 5)
								{
									this->nPh = false;
									wr<<clean<<" , ";
								}
								else
								{
									if(this->nPh == false)
									{
										this->nPh = true;
										continue;
									}
									else
									{
										this->nPh = false;
                    this->invalid++;
										wr<<"00000000"<<" ,";
										wr<<"NA" <<", " ;
										++i;
									}
								}

							}
						}
					} 
					else
					{
						continue;
					}
				}  // Line 1 Ends
				if(i == 2)
				{
					if(this->bracket_line(line)) 
					{
						continue;
					}
					else
					{
						clean = this->clean_phno(line);
						if(clean.length() > 6)
						{
							wr<<clean<<" , ";
						}
						else
						{
							if(!this->nPh)
							{
								this->nPh = true;
								continue;
							}
							else
							{
								this->nPh = false;
                this->invalid++;
								wr<<"00000000"<<" ,";
								wr<<"NA" <<", " ;
								i = i+2;
							}
						}
					}
				}
				if( i == 3)
				{
					if(this->valid_alphaLine(line))
					{
						wr<<line<<" , ";
					}
					else
					{
						wr<<"Address Not Found"<<" ,";
					}
				}
				if( i == 4)
				{
					vector<std::string> zip_state = this->extract_state_zip(line); 
					if(this->valid_zip)  
					{
						wr<<zip_state.at(0)<<" ,";
						wr<<zip_state.at(1)<<" ,";
					}
					else
					{
						wr<<"000000"<<" ,";
						wr<<"NA"<<" ,";
					}
				}
				if(i == 5)
				{
					if(this->set_comp == false)
					{
						wr<<"\n";
						count++;
						i = 1;
						if(this->valid_alphaLine(line))  // has alpha chars
						{
							wr<<line<<" , ";
						}
						else
						{
							wr<<"Name Not Found "<<" , ";
							++i;
							if(this->bracket_line(line)) 
							{
								continue;
							}
							else
							{
								line = this->clean_phno(line);
								if(line.length() > 6)
								{
									wr<<line<<" , ";
								}
								else
								{
									wr<<"00000000"<<" ,";
								}

							}
						}
					}
				}


				i++;
			} // End of Last Line Else
		} // End of Else
	}// End of while

	int no_set = count - this->set_found;
	int half = count /2;
	if(no_set > half)
	{
		count = -1;
	}
  int lead_count;
  lead_count = count - this->invalid;
	return lead_count;
}
