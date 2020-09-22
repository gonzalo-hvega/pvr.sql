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
#include "DVRTimerType.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
DVRTimerType::DVRTimerType(string strData)
{
	// assign data to structure
	iId              = stoi(ParseSQLValue(strData, "<iId>"            ,  0));
	iAttributes      = stoi(ParseSQLValue(strData, "<iAttributes>"    ,  0));
	strDescription   =      ParseSQLValue(strData, "<strDescription>" , "") ;
	iPrioritiesSize  = stoi(ParseSQLValue(strData, "<iPrioritiesSize>",  0));
	iLifetimesSize   = stoi(ParseSQLValue(strData, "<iLifetimesSize>" ,  0));
}

DVRTimerType::~DVRTimerType(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_TIMER_TYPE DVRTimerType::TimerType(void)
{
	// create object
	PVR_TIMER_TYPE xbmcTimerType;
	memset(&xbmcTimerType, 0, sizeof(PVR_TIMER_TYPE));
	
	// assign data to structure
		   xbmcTimerType.iId                              = this->GetId()                             ;
		   xbmcTimerType.iAttributes                      = this->GetAttributes()                     ;
	strcpy(xbmcTimerType.strDescription                   , this->GetDescription())                   ;
		   xbmcTimerType.iPrioritiesSize                  = this->GetPrioritiesSize()                 ;/*
		   xbmcTimerType.priorities                       = this->GetPriorities()                     ;
		   xbmcTimerType.iPrioritiesDefault               = this->GetPrioritiesDefault()              ;*/
		   xbmcTimerType.iLifetimesSize                   = this->GetLifetimesSize()                  ;/*
		   xbmcTimerType.lifetimes                        = this->GetLifetimes()                      ;
		   xbmcTimerType.iLifetimesDefault                = this->GetLifetimesDefault()               ;
		   xbmcTimerType.iPreventDuplicateEpisodesSize    = this->GetPreventDuplicateEpisodesSize()   ;
		   xbmcTimerType.preventDuplicateEpisodes         = this->GetPreventDuplicateEpisodes()       ;
		   xbmcTimerType.iPreventDuplicateEpisodesDefault = this->GetPreventDuplicateEpisodesDefault();
		   xbmcTimerType.iRecordingGroupSize              = this->GetRecordingGroupSize()             ; 
		   xbmcTimerType.recordingGroup                   = this->GetRecordingGroup()                 ;
		   xbmcTimerType.iRecordingGroupDefault           = this->GetRecordingGroupDefault()          ;
		   xbmcTimerType.iMaxRecordingsSize               = this->GetMaxRecordingsSize()              ;
		   xbmcTimerType.maxRecordings                    = this->GetMaxRecordings()                  ;
		   xbmcTimerType.iMaxRecordingsDefault            = this->GetMaxRecordingsDefault()           ;*/
		   
	// return object
	return xbmcTimerType;
}