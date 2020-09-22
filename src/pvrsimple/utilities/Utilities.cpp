/*
 *  pvr.sql - A PVR client for Kodi using M3U, XMLTV, and FFMPEG
 *  Copyright © 2018 El_Gonz87 (Gonzalo Vega)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/***********************************************************
 * Headers
 ***********************************************************/
#include "Utilities.h"

/***********************************************************
 * C++ Utility Definitions
 ***********************************************************/
int stoh(const string& str)
{
	// get char array from string
	const char* hex = str.c_str();

	// init id prarams
	int iId = 0;
	int chr;

	// parse string and encode
	while (chr = *hex++)
		iId = ((iId << 5) + iId) + chr;

	// return encoded value
	return abs(iId);
}

bool stob(const string& str)
{
	// create container
	bool bit = false;
	
	// check if string is true
	if (str == "true" || str == "1")
		bit = true;

	// return conversion
	return bit;
}

const char* stoc(const string& str)
{
	// create container
	const char* chr = str.c_str();
	
	// return conversion
	return chr;
}

#ifdef __ANDROID__

float stof(const string& str)
{
	// create container
	stringstream ss;
	float num;

	// convert
	ss << str;
	ss >> num;

	// return conversion
	return num;
}

int stoi(const string& str)
{
	// create container
	stringstream ss;
	int num;

	// convert
	ss << str;
	ss >> num;

	// return conversion
	return num;
}

#endif

string itos(const int& num)
{
	// create container
	stringstream ss;

	// convert
	ss << num;

	// return conversion
	return ss.str();
}

string ftos(const float& num)
{
	// create container
	stringstream ss;

	// convert
	ss << num;

	// return conversion
	return ss.str();
}	

string btos(const bool& bit)
{
	// create container
	string str = "false";

	// convert
	if (bit)
		str = "true";

	// return conversion
	return str;
}

string ctos(const char* chr)
{
	// create container
	string str(chr);

	// return conversion
	return str;
}

string dtos(const time_t& dtm)
{
	// create container
	tm * ptm = localtime(&dtm);
	char chr[32];

	// convert
	strftime(chr, 32, "%Y-%m-%dT%H:%M:%S", ptm);

	// return conversion
	return string(chr);
} 

/***********************************************************
 * String Utils
 ***********************************************************/
string Join(const string& delimiter, const char *arg, ...)
{
	// initialize arguments
	va_list arguments;
    
	// initialize container
	string ret = "";

	// iterate through arguments
	for (va_start(arguments, arg); arg != NULL; arg = va_arg(arguments, const char *)) 
		if (arg != "")
			ret = (ret == "") ? arg : ret + delimiter + arg;

	// close arguments
	va_end(arguments);
    
	// return value
	return ret;
}

string Replace(const string &str, const string &oldStr, const string &newStr)
{
    // create index var
	int index = 0;
	
	// initialize container
	string ret = str;
	
	//remove illegal chars
	while ((index = ret.find(oldStr, index)) != string::npos)
	{
	    // replace with double
	    ret.replace(index, oldStr.length(), newStr);
	    
	    // adjust index
	    index += newStr.length();
	}

	// return fixed syntax
	return ret;
}

string Trim(const string &str)
{
	// initialize container
	string ret = str;
	
	// call p8 equivalent
	ret = StringUtils::Trim(ret);

	// return fixed syntax
	return ret;
}

/***********************************************************
 * OS Functions
 ***********************************************************/
#ifdef TARGET_WINDOWS
 
void sleep(const int& sec)
{
	Sleep(sec * 1000);
}

extern "C" char* strptime(const char* buf, const char* format, struct tm* tm) 
{
	// convert time
	std::istringstream input(buf);
	input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
	input >> std::get_time(tm, format);
	
	// return conversion
	return (input.fail()) ? nullptr : (char*)(buf + input.tellg());
}
 
#endif

/***********************************************************
 * Time Functions
 ***********************************************************/
unsigned int ParseWeekDay (const time_t dtm)
{
	// define constants
	const unsigned int PVR_WEEKDAY_NONE      = 0x00;
	const unsigned int PVR_WEEKDAY_MONDAY    = 0x01;
	const unsigned int PVR_WEEKDAY_TUESDAY   = 0x02;
	const unsigned int PVR_WEEKDAY_WEDNESDAY = 0x04;
	const unsigned int PVR_WEEKDAY_THURSDAY  = 0x08;
	const unsigned int PVR_WEEKDAY_FRIDAY    = 0x10;
	const unsigned int PVR_WEEKDAY_SATURDAY  = 0x20;
	const unsigned int PVR_WEEKDAY_SUNDAY    = 0x40;
	const unsigned int PVR_WEEKDAY_ALLDAYS   = PVR_WEEKDAY_MONDAY   | PVR_WEEKDAY_TUESDAY | PVR_WEEKDAY_WEDNESDAY |
	                                           PVR_WEEKDAY_THURSDAY | PVR_WEEKDAY_FRIDAY  | PVR_WEEKDAY_SATURDAY  |
	                                           PVR_WEEKDAY_SUNDAY   ;
												 
	// create output and string containers
	unsigned int ret;
	tm * ptm;
	char day[32];  
	  
	// parse day
	ptm = localtime(&dtm);
	strftime(day, 32, "%A", ptm); 
	  
	// assign value
	if      (string(day) == "Monday"   ) ret = PVR_WEEKDAY_MONDAY   ;
	else if (string(day) == "Tuesday"  ) ret = PVR_WEEKDAY_TUESDAY  ;
	else if (string(day) == "Wednesday") ret = PVR_WEEKDAY_WEDNESDAY;
	else if (string(day) == "Thursday" ) ret = PVR_WEEKDAY_THURSDAY ;
	else if (string(day) == "Friday"   ) ret = PVR_WEEKDAY_FRIDAY   ;
	else if (string(day) == "Saturday" ) ret = PVR_WEEKDAY_SATURDAY ;
	else if (string(day) == "Sunday"   ) ret = PVR_WEEKDAY_SUNDAY   ;
	else                                 ret = PVR_WEEKDAY_NONE     ;
	  
	// return value
	return ret;
}

string ParseTime (const time_t dtm)
{
  // create output and string containers
  tm * ptm;
  char tm[32];  
  
  // parse day
  ptm = localtime(&dtm);
  strftime(tm, 32, "%r", ptm); 
  
  // return value
  return string(tm);
}

 /***********************************************************
* BETA Definitions (Deprecated BETA is Over)
***********************************************************/
bool isBeta(void)
{
	const char *endDate = "20200613163512";
	struct tm dtm;
	memset(&dtm, 0, sizeof(tm));

	sscanf(endDate, "%04d%02d%02d%02d%02d%02d", &dtm.tm_year, &dtm.tm_mon, &dtm.tm_mday, &dtm.tm_hour, &dtm.tm_min, &dtm.tm_sec);

	dtm.tm_mon  -=    1;
	dtm.tm_year -= 1900;
	dtm.tm_isdst =   -1;
  
	time_t betaEnd = mktime(&dtm);
   
	return true;
}