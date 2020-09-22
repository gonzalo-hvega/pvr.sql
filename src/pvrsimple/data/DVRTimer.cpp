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
#include "DVRTimer.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
DVRTimer::DVRTimer(string strData)
{
	// assign data to structure	
	iClientIndex              = stoi(ParseSQLValue(strData, "<iClientIndex>"             ,     0));
	iParentClientIndex        = stoi(ParseSQLValue(strData, "<iParentClientIndex>"       ,     0));
	iClientChannelUid         = stoi(ParseSQLValue(strData, "<iClientChannelUid>"        ,     0));
	startTime                 = stoi(ParseSQLValue(strData, "<startTime>"                ,     0));
	endTime                   = stoi(ParseSQLValue(strData, "<endTime>"                  ,     0));
	bStartAnyTime             = stob(ParseSQLValue(strData, "<bStartAnyTime>"            , false));
	bEndAnyTime               = stob(ParseSQLValue(strData, "<bEndAnyTime>"              , false));
	state                     = stoi(ParseSQLValue(strData, "<state>"                    ,     0));
	iTimerType                = stoi(ParseSQLValue(strData, "<iTimerType>"               ,     0));
	strTitle                  =      ParseSQLValue(strData, "<strTitle>"                 ,    "") ;
	strEpgSearchString        =      ParseSQLValue(strData, "<strEpgSearchString>"       ,    "") ;
	bFullTextEpgSearch        = stob(ParseSQLValue(strData, "<bFullTextEpgSearch>"       , false));
	strDirectory              =      ParseSQLValue(strData, "<strDirectory>"             ,    "") ;
	strSummary                =      ParseSQLValue(strData, "<strSummary>"               ,    "") ;
	iPriority                 = stoi(ParseSQLValue(strData, "<iPriority>"                ,     0));
	iLifetime                 = stoi(ParseSQLValue(strData, "<iLifetime>"                ,     0));
	iMaxRecordings            = stoi(ParseSQLValue(strData, "<iMaxRecordings>"           ,     0));
	iRecordingGroup           = stoi(ParseSQLValue(strData, "<iRecordingGroup>"          ,     0));
	firstDay                  = stoi(ParseSQLValue(strData, "<firstDay>"                 ,     0));
	iWeekdays                 = stoi(ParseSQLValue(strData, "<iWeekdays>"                ,     0));
	iPreventDuplicateEpisodes = stoi(ParseSQLValue(strData, "<iPreventDuplicateEpisodes>",     0));
	iEpgUid                   = stoi(ParseSQLValue(strData, "<iEpgUid>"                  ,     0));
	iMarginStart              = stoi(ParseSQLValue(strData, "<iMarginStart>"             ,     0));
	iMarginEnd                = stoi(ParseSQLValue(strData, "<iMarginEnd>"               ,     0));
	iGenreType                = stoi(ParseSQLValue(strData, "<iGenreType>"               ,     0));
	iGenreSubType             = stoi(ParseSQLValue(strData, "<iGenreSubType>"            ,     0));
	strSeriesLink             =      ParseSQLValue(strData, "<strSeriesLink>"            ,    "") ;
}

DVRTimer::DVRTimer(void)
{
	// no need to construct stubb for assignment operator
}

DVRTimer::~DVRTimer(void)
{
	// no need to destruct
}

/***********************************************************
 * Operator Definitions
 ***********************************************************/
DVRTimer& DVRTimer::operator= (const DVRTimer &rhs)
{
	// assign new values if new
	if (this != &rhs)
	{
		iClientIndex              = rhs.iClientIndex             ;
		iParentClientIndex        = rhs.iParentClientIndex       ;
		iClientChannelUid         = rhs.iClientChannelUid        ;
		startTime                 = rhs.startTime                ;
		endTime                   = rhs.endTime                  ;
		bStartAnyTime             = rhs.bStartAnyTime            ;
		bEndAnyTime               = rhs.bEndAnyTime              ;
		state                     = rhs.state                    ;
		iTimerType                = rhs.iTimerType               ;
		strTitle                  = rhs.strTitle                 ;
		strEpgSearchString        = rhs.strEpgSearchString       ;
		bFullTextEpgSearch        = rhs.bFullTextEpgSearch       ;
		strDirectory              = rhs.strDirectory             ;
		strSummary                = rhs.strSummary               ;
		iPriority                 = rhs.iPriority                ;
		iLifetime                 = rhs.iLifetime                ;
		iMaxRecordings            = rhs.iMaxRecordings           ;
		iRecordingGroup           = rhs.iRecordingGroup          ;
		firstDay                  = rhs.firstDay                 ;
		iWeekdays                 = rhs.iWeekdays                ;
		iPreventDuplicateEpisodes = rhs.iPreventDuplicateEpisodes;
		iEpgUid                   = rhs.iEpgUid                  ;
		iMarginStart              = rhs.iMarginStart             ;
		iMarginEnd                = rhs.iMarginEnd               ;
		iGenreType                = rhs.iGenreType               ;
		iGenreSubType             = rhs.iGenreSubType            ;
		strSeriesLink             = rhs.strSeriesLink            ;
	}
	
	// return itself
	return *this;
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_TIMER DVRTimer::Timer(void)
{
	// create object
	PVR_TIMER xbmcTimer;
	memset(&xbmcTimer, 0, sizeof(PVR_TIMER));
	
	// assign data to structure
		   xbmcTimer.iClientIndex              =                   this->GetClientIndex()             ;
		   xbmcTimer.iParentClientIndex        =                   this->GetParentClientIndex()       ;
		   xbmcTimer.iClientChannelUid         =                   this->GetClientChannelUid()        ;
		   xbmcTimer.startTime                 =                   this->GetStartTime()               ;
		   xbmcTimer.endTime                   =                   this->GetEndTime()                 ;
		   xbmcTimer.bStartAnyTime             =                   this->GetStartAnyTime()            ;
		   xbmcTimer.bEndAnyTime               =                   this->GetEndAnyTime()              ;
		   xbmcTimer.state                     = (PVR_TIMER_STATE) this->GetState()                   ;
		   xbmcTimer.iTimerType                =                   this->GetTimerType()               ;
	strcpy(xbmcTimer.strTitle                  ,                   this->GetTitle())                  ;
	strcpy(xbmcTimer.strEpgSearchString        ,                   this->GetEpgSearchString())        ;
		   xbmcTimer.bFullTextEpgSearch        =                   this->GetFullTextEpgSearch()       ;
	strcpy(xbmcTimer.strDirectory              ,                   this->GetDirectory())              ;
	strcpy(xbmcTimer.strSummary                ,                   this->GetSummary())                ;
		   xbmcTimer.iPriority                 =                   this->GetPriority()                ;
		   xbmcTimer.iLifetime                 =                   this->GetLifetime()                ;
		   xbmcTimer.iMaxRecordings            =                   this->GetMaxRecordings()           ;
		   xbmcTimer.iRecordingGroup           =                   this->GetRecordingGroup()          ;
		   xbmcTimer.firstDay                  =                   this->GetFirstDay()                ;
		   xbmcTimer.iWeekdays                 =                   this->GetWeekdays()                ;
		   xbmcTimer.iPreventDuplicateEpisodes =                   this->GetPreventDuplicateEpisodes();
		   xbmcTimer.iEpgUid                   =                   this->GetEpgUid()                  ;
		   xbmcTimer.iMarginStart              =                   this->GetMarginStart()             ;
		   xbmcTimer.iMarginEnd                =                   this->GetMarginEnd()               ;
		   xbmcTimer.iGenreType                =                   this->GetGenreType()               ;
		   xbmcTimer.iGenreSubType             =                   this->GetGenreSubType()            ;
	strcpy(xbmcTimer.strSeriesLink             ,                   this->GetSeriesLink())             ;
		   
	// return object
	return xbmcTimer;
}