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
#include "XMLHelpers.h"

/***********************************************************
 * Node Function Definitions
 ***********************************************************/
string GetXMLAttributeValue(const xml_node<> *pNode, const char* strAttributeName, const char* strDefault)
{
	// create node pointer
	xml_attribute<> *pAttribute;
	
	// execute if not null
	if (pNode)
	{
		// look for attrib
		pAttribute = pNode->first_attribute(strAttributeName);
	}

	// assign attrib value to string parameter
	return (pAttribute == NULL) ? string(strDefault) : pAttribute->value();
}

string GetXMLNodeValue(const xml_node<> *pRootNode, const char* strTag, const char* strSeparator, const char* strDefault)
{
	// create node pointer
	xml_node<> *pChildNode;
	
	// create container
	string strNodeValue = "";
	
	// execute if not null
	if (pRootNode)
	{
		// look for parent node
		pChildNode = pRootNode->first_node(strTag);

		// append each node
		if (pChildNode)
			for (xml_node<> *pIterator = pRootNode->first_node(strTag); pIterator; pIterator = pIterator->next_sibling(strTag))
				strNodeValue.append(pIterator->value() + string(strSeparator));

		// substring actor to remove extra comma
		strNodeValue = strNodeValue.substr(0, strNodeValue.length()-string(strSeparator).length());
	}

	// assign node value to string parameter
	return (pChildNode == NULL) ? string(strDefault) : strNodeValue;
}

string GetXMLChildAttributeValue(const xml_node<> *pNode, const char* strTag, const char* strAttributeName, const char* strDefault)
{
	// create node pointer
	xml_node<> *pChildNode;
	
	// create container
	string strChildren = "";
	
	// execute if not null
	if (pNode)
	{
		// look for child node
		pChildNode = pNode->first_node(strTag);
	
		// look for attrib
		if (pChildNode)
			strChildren = GetXMLAttributeValue(pChildNode, strAttributeName, "");
	}

	// assign node value to string parameter
	return (pChildNode == NULL) ? string(strDefault) : strChildren;
}

string GetXMLChildNodeValue(const xml_node<> *pRootNode, const char* strParent, const char* strTag, const char* strSeparator, const char* strDefault)
{
	// create node pointer
	xml_node<> *pChildNode;
	
	// create container
	string strChildren = "";
	
	// execute if not null
	if (pRootNode)
	{
		// look for child node
		pChildNode = pRootNode->first_node(strParent);
	  
		// append each child value
		if (pChildNode)
			for (xml_node<> *pIterator = pChildNode->first_node(strTag); pIterator; pIterator = pIterator->next_sibling(strTag))
				strChildren.append(pIterator->value() + string(strSeparator));

		// substring to remove extra comma
		strChildren = strChildren.substr(0, strChildren.length()-string(strSeparator).length());
	}

	// assign node value to string parameter
	return (pChildNode == NULL) ? string(strDefault) : strChildren;
}

string GetXMLNodeValueAttr(const xml_node<> *pRootNode, const char* strTag, const char* strAttributeName, const char* strAttributeValue, const char* strSeparator, const char* strDefault)
{
	// create node pointer
	xml_node<> *pChildNode;
	
	// create container
	string strNodeValue = "";
	
	// execute if not null
	if (pRootNode)
	{
		// look for child node
		pChildNode = pRootNode->first_node(strTag);
	
		// append each matching attrib
		if (pChildNode != NULL)
			for (xml_node<> *pIterator = pRootNode->first_node(strTag); pIterator; pIterator = pIterator->next_sibling(strTag))
				if (GetXMLAttributeValue(pIterator, strAttributeName, "NULL") == string(strAttributeValue))
					strNodeValue.append(pIterator->value() + string(strSeparator));
				
		// substring to remove extra comma
		strNodeValue = strNodeValue.substr(0, strNodeValue.length()-string(strSeparator).length());
	}

	// assign node value to string parameter
	return (pChildNode == NULL) ? string(strDefault) : strNodeValue;
}

/***********************************************************
 * Parse Function Definitions
 ***********************************************************/
int ParseXMLYear(const string& strDate)
{
	// create date container
	string date = strDate;
	
	// parse out year expected at beginning
	if (date.length() > 4)
		date = date.substr(0, 4);
	
	// return date;
	return stoi(date);
}

time_t ParseXMLDate(const string& strDateTm)
{
	// create container
	time_t date = 0;
	  
	// check appropriate length
	if (strDateTm.length() == 8)
	{
		// create time container
		struct tm time;
		memset(&time, 0, sizeof(tm));
			
		// create time zone adjustors
		char tz_sign = '+';
		int  tz_hour =  0 ;
		int  tz_min  =  0 ;
			
		// parse time string
		sscanf(strDateTm.c_str(), "%04d%02d%02d", &time.tm_year, &time.tm_mon, &time.tm_mday);
			
		// adjust read time
		time.tm_mon  -=    1;
		time.tm_year -= 1900;
		time.tm_isdst =   -1;
			
		// set date
		date = mktime(&time);
	}
	  
	// return container
	return date;
}

unsigned int ParseXMLWeekDay (const time_t inputTime)
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
	                                           PVR_WEEKDAY_SUNDAY                                                 ;

	// create output and string containers
	unsigned int ret;
	tm * ptm;
	char day[32];  
	  
	// parse day
	ptm = localtime(&inputTime);
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

string ParseXMLTime (const time_t inputTime)
{
	// create output and string containers
	tm * ptm;
	char tm[32];  
	  
	// parse day
	ptm = localtime(&inputTime);
	strftime(tm, 32, "%r", ptm); 
	  
	// return value
	return string(tm);
}

time_t ParseXMLDateTime(const string& strDateTm)
{
	// create container
	time_t datetm = time(NULL);
	  
	// check appropriate length
	if (strDateTm.length() == 20)
	{
		// create time container
		struct tm time;
		memset(&time, 0, sizeof(tm));

		// create time zone adjustors
		char tz_sign = '+';
		int  tz_hour =  0 ;
		int  tz_min  =  0 ;

		// parse time string
		sscanf(strDateTm.c_str(), "%04d%02d%02d%02d%02d%02d %c%02d%02d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour, &time.tm_min, &time.tm_sec, &tz_sign, &tz_hour, &tz_min);

		// adjust read time
		time.tm_mon  -=    1;
		time.tm_year -= 1900;
		time.tm_isdst =   -1;

		// create local offset times
		time_t     current_time ;
		std::time(&current_time);
		long       lt_offset    ;

		// get machin local time
		#ifndef TARGET_WINDOWS
			lt_offset = -localtime(&current_time)->tm_gmtoff;
		#else
			TIME_ZONE_INFORMATION tz;
			DWORD rc = ::GetTimeZoneInformation(&tz);
			lt_offset = (tz.Bias + (TIME_ZONE_ID_DAYLIGHT != rc ?  tz.StandardBias : tz.DaylightBias))*60;
		#endif // TARGET_WINDOWS

		// get time zone offset from epg
		long tz_offset = (tz_sign == '-' ? -1 : 1)*((tz_hour * 60 * 60) + (tz_min * 60));

		// set time
		datetm = mktime(&time) - tz_offset - lt_offset;
	}
	  
	// return container
	return datetm;
}

int ParseXMLStarRating(const string& strText)
{
	// create containers
    float iRating, iScale;
    
    // look for rating
    if (sscanf(strText.c_str(), "%f/%f", &iRating, &iScale) != 2)
        iRating = 0;
        
    // convert to kodi scale
    if (iRating)
    {
        iRating /= iScale;
        iRating *= int(STAR_RATING_SCALE);
    }
    
	// return part
	return (round(iRating) < int(STAR_RATING_SCALE)) ?  round(iRating) : int(STAR_RATING_SCALE); 
}

int ParseXMLSeason(const string& strText)
{
	// create containers
	int iSeason, iEpisode, iPartNumber, iPartTotal;
		
	// look for season, episode, part number
    if(sscanf(strText.c_str(), "%i.%i.%i/%i", &iSeason, &iEpisode, &iPartNumber, &iPartTotal) != 4)
        iSeason = -2;
			
	// return season
	return iSeason+1;
}

int ParseXMLEpisode(const string& strText)
{
	// create containers
	int iSeason, iEpisode, iPartNumber, iPartTotal;
		
	// look for season, episode, part number
    if(sscanf(strText.c_str(), "%i.%i.%i/%i", &iSeason, &iEpisode, &iPartNumber, &iPartTotal) != 4)
        iEpisode = -2;
			
	// return episode
	return iEpisode+1;
}

int ParseXMLEpPart(const string& strText)
{
	// create containers
	int iSeason, iEpisode, iPartNumber, iPartTotal;
		
	// look for season, episode, part number
    if(sscanf(strText.c_str(), "%i.%i.%i/%i", &iSeason, &iEpisode, &iPartNumber, &iPartTotal) != 4)
        iPartNumber = -2;
			
	// return part
	return (iPartTotal > 1) ? iPartNumber+1 : -1;
}

string ParseXMLSeries(const string& strText)
{
    // create containers
    string strLink, strTVdb;
    
    // assume no series
    strTVdb = strText;
    
    // some nodes have episode data (remove)
	if (strText.find(",") != string::npos)
		strTVdb = strText.substr(0, strText.find(","));
        
	// return series id
	return string(strTVdb);
}

/***********************************************************
 * Date Function Definitions
 ***********************************************************/
string GetDate(const time_t inputTime)
{
	// convert to string
	string strDate = dtos(inputTime);
	
	// substring to date
	if (strDate.size() >= 10)
		strDate = strDate.substr(0, 10);
	
	// return value
	return strDate;
}