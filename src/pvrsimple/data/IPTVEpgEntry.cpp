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
#include "IPTVEpgEntry.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
IPTVEpgEntry::IPTVEpgEntry(string strData)
{
	// assign data to structure
	iUniqueBroadcastId  = stoi(ParseSQLValue(strData, "<iUniqueBroadcastId>" ,     0));
	iUniqueChannelId    = stoi(ParseSQLValue(strData, "<iUniqueChannelId>"   ,     0)); 
	strTitle            =      ParseSQLValue(strData, "<strTitle>"           ,    "") ;
	startTime           = stoi(ParseSQLValue(strData, "<startTime>"          ,     0));
	endTime             = stoi(ParseSQLValue(strData, "<endTime>"            ,     0));
	strPlotOutline      =      ParseSQLValue(strData, "<strPlotOutline>"     ,    "") ;
	strPlot             =      ParseSQLValue(strData, "<strPlot>"            ,    "") ;
	strOriginalTitle    =      ParseSQLValue(strData, "<strOriginalTitle>"   ,    "") ;
	strCast             =      ParseSQLValue(strData, "<strCast>"            ,    "") ;
	strDirector         =      ParseSQLValue(strData, "<strDirector>"        ,    "") ;
	strWriter           =      ParseSQLValue(strData, "<strWriter>"          ,    "") ;
	iYear               = stoi(ParseSQLValue(strData, "<iYear>"              ,     0));
	strIMDBNumber       =      ParseSQLValue(strData, "<strIMDBNumber>"      ,    "") ;
	strIconPath         =      ParseSQLValue(strData, "<strIconPath>"        ,    "") ;
	iGenreType          = stoi(ParseSQLValue(strData, "<iGenreType>"         ,     0));
	iGenreSubType       = stoi(ParseSQLValue(strData, "<iGenreSubType>"      ,     0));
	strGenreDescription =      ParseSQLValue(strData, "<strGenreDescription>",    "") ;
	firstAired          = stoi(ParseSQLValue(strData, "<firstAired>"         ,     0));
	iParentalRating     = stoi(ParseSQLValue(strData, "<iParentalRating>"    ,     0));
	iStarRating         = stoi(ParseSQLValue(strData, "<iStarRating>"        ,     0));
	bNotify             = stob(ParseSQLValue(strData, "<bNotify>"            , false));
	iSeriesNumber       = stoi(ParseSQLValue(strData, "<iSeriesNumber>"      ,     0));
	iEpisodeNumber      = stoi(ParseSQLValue(strData, "<iEpisodeNumber>"     ,     0));
	iEpisodePartNumber  = stoi(ParseSQLValue(strData, "<iEpisodePartNumber>" ,     0));
	strEpisodeName      =      ParseSQLValue(strData, "<strEpisodeName>"     ,    "") ;
	iFlags              = stoi(ParseSQLValue(strData, "<iFlags>"             ,     0));
	strSeriesLink       =      ParseSQLValue(strData, "<strSeriesLink>"      ,    "") ;
	strTvgId            =      ParseSQLValue(strData, "<strTvgId>"           ,    "") ;
}

IPTVEpgEntry::~IPTVEpgEntry(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
EPG_TAG IPTVEpgEntry::EpgTag(void)
{
	// create object
	EPG_TAG xbmcEpgEntry;
	memset(&xbmcEpgEntry, 0, sizeof(EPG_TAG));
	
	// assign data to structure
	xbmcEpgEntry.iUniqueBroadcastId  = this->GetUniqueBroadcastId();
	xbmcEpgEntry.iUniqueChannelId    = this->GetUniqueChannelId()  ;
	xbmcEpgEntry.strTitle            = this->GetTitle()            ;
	xbmcEpgEntry.startTime           = this->GetStartTime()        ;
	xbmcEpgEntry.endTime             = this->GetEndTime()          ;
	xbmcEpgEntry.strPlotOutline      = this->GetPlotOutline()      ;
	xbmcEpgEntry.strPlot             = this->GetPlot()             ;
	xbmcEpgEntry.strOriginalTitle    = this->GetOriginalTitle()    ;
	xbmcEpgEntry.strCast             = this->GetCast()             ;
	xbmcEpgEntry.strDirector         = this->GetDirector()         ;
	xbmcEpgEntry.strWriter           = this->GetWriter()           ;
	xbmcEpgEntry.iYear               = this->GetYear()             ;
	xbmcEpgEntry.strIMDBNumber       = this->GetIMDBNumber()       ;
	xbmcEpgEntry.strIconPath         = this->GetIconPath()         ;
	xbmcEpgEntry.iGenreType          = this->GetGenreType()        ;
	xbmcEpgEntry.iGenreSubType       = this->GetGenreSubType()     ;
	xbmcEpgEntry.strGenreDescription = this->GetGenreDescription() ;
	xbmcEpgEntry.firstAired          = this->GetFirstAired()       ;
	xbmcEpgEntry.iParentalRating     = this->GetParentalRating()   ;
	xbmcEpgEntry.iStarRating         = this->GetStarRating()       ;
	xbmcEpgEntry.bNotify             = this->GetNotify()           ;
	xbmcEpgEntry.iSeriesNumber       = this->GetSeriesNumber()     ;
	xbmcEpgEntry.iEpisodeNumber      = this->GetEpisodeNumber()    ;
	xbmcEpgEntry.iEpisodePartNumber  = this->GetEpisodePartNumber();
	xbmcEpgEntry.strEpisodeName      = this->GetEpisodeName()      ;
	xbmcEpgEntry.iFlags              = this->GetFlags()            ;
	xbmcEpgEntry.strSeriesLink       = this->GetSeriesLink()       ;
		   
	// return object
	return xbmcEpgEntry;
}