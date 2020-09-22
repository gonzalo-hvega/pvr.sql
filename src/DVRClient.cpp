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
#include "DVRClient.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings   *settings    ;
extern TCPClient     *client      ;
extern IPTVClient    *iptv        ;
extern void          *streamHandle;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
DVRClient::DVRClient(void)
{ 
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Creating DVR server", __FUNCTION__);
	
	// initialize addon constants
	iCurStatus        = ADDON_STATUS_OK;
	bCreated          = true;
	strBackendName    = "DVR";
	
	// clear containers
	cTimerTypes.clear();
	cTimers.clear();
	cRecordings.clear();
	
	// load sql objects
	LoadTimerTypes();
	LoadTimers();
	LoadRecordings();
	
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Created DVR server", __FUNCTION__);
}

DVRClient::~DVRClient(void)
{  
	// mark as disconnected and not created
	iCurStatus        = ADDON_STATUS_LOST_CONNECTION;
	bCreated          = false;
	
	// clear containers
	cTimerTypes.clear();
	cTimers.clear();
	cRecordings.clear();
}

/***********************************************************
 * Addon Property API Definitions
 ***********************************************************/
long DVRClient::GetStatus(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return iCurStatus;
}

bool DVRClient::IsCreated(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return bCreated;
}

string DVRClient::GetBackendName(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return strBackendName;
}

string DVRClient::GetConnectionString(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return client->IsConnected() ? "Connected" : "Disconnected";
}

time_t DVRClient::LastTimerTypesSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastTimerTypesSync;
}

time_t DVRClient::LastTimersSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastTimersSync;
}

time_t DVRClient::LastRecordingsSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastRecordingsSync;
}

/***********************************************************
 * Timer Types API Definitions
 ***********************************************************/
PVR_ERROR DVRClient::GetTimerTypes(PVR_TIMER_TYPE types[], int *size)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
		
	// initialize size of array
	*size = 0;
		
	// iterate through timer types and pass back to kodi
	for (vector<DVRTimerType>::iterator cTimerType = cTimerTypes.begin(); cTimerType != cTimerTypes.end(); cTimerType++)
	{	
		// create object
		PVR_TIMER_TYPE xbmcTimerType;
		memset(&xbmcTimerType, 0, sizeof(PVR_TIMER_TYPE));
		
		       xbmcTimerType.iId                              =                                      cTimerType->GetId()                             ;
		       xbmcTimerType.iAttributes                      =                                      cTimerType->GetAttributes()                     ;
		strcpy(xbmcTimerType.strDescription                   ,                                      cTimerType->GetDescription())                   ;
		       xbmcTimerType.iPrioritiesSize                  =                                      cTimerType->GetPrioritiesSize()                 ;/*
		       xbmcTimerType.priorities                       = (PVR_TIMER_TYPE_ATTRIBUTE_INT_VALUE) cTimerType->GetPriorities()                     ;
		       xbmcTimerType.iPrioritiesDefault               =                                      cTimerType->GetPrioritiesDefault()              ;*/
		       xbmcTimerType.iLifetimesSize                   =                                      cTimerType->GetLifetimesSize()                  ;/*
		       xbmcTimerType.lifetimes                        = (PVR_TIMER_TYPE_ATTRIBUTE_INT_VALUE) cTimerType->GetLifetimes()                      ;
		       xbmcTimerType.iLifetimesDefault                =                                      cTimerType->GetLifetimesDefault()               ;
		       xbmcTimerType.iPreventDuplicateEpisodesSize    =                                      cTimerType->GetPreventDuplicateEpisodesSize()   ;
		       xbmcTimerType.preventDuplicateEpisodes         = (PVR_TIMER_TYPE_ATTRIBUTE_INT_VALUE) cTimerType->GetPreventDuplicateEpisodes()       ;
		       xbmcTimerType.iPreventDuplicateEpisodesDefault =                                      cTimerType->GetPreventDuplicateEpisodesDefault();
		       xbmcTimerType.iRecordingGroupSize              =                                      cTimerType->GetRecordingGroupSize()             ;
		       xbmcTimerType.recordingGroup                   = (PVR_TIMER_TYPE_ATTRIBUTE_INT_VALUE) cTimerType->GetRecordingGroup()                 ;
		       xbmcTimerType.iRecordingGroupDefault           =                                      cTimerType->GetRecordingGroupDefault()          ;
		       xbmcTimerType.iMaxRecordingsSize               =                                      cTimerType->GetMaxRecordingsSize()              ;
		       xbmcTimerType.maxRecordings                    = (PVR_TIMER_TYPE_ATTRIBUTE_INT_VALUE) cTimerType->GetMaxRecordings()                  ;
		       xbmcTimerType.iMaxRecordingsDefault            =                                      cTimerType->GetMaxRecordingsDefault()           ;*/

		// pass back to kodi
		PVR_TransferTimerTypeEntry(types, size, &xbmcTimerType);
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

int DVRClient::GetTimerTypesAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cTimerTypes.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

/***********************************************************
 * Timers API Definitions
 ***********************************************************/
PVR_ERROR DVRClient::GetTimers(ADDON_HANDLE handle)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
  		
	// iterate through timers and pass back to kodi
	for (vector<DVRTimer>::iterator cTimer = cTimers.begin(); cTimer != cTimers.end(); cTimer++)
	{
		// pass back appropriate entries
		if (cTimer->GetState() == PVR_TIMER_STATE_SCHEDULED || cTimer->GetState() == PVR_TIMER_STATE_RECORDING || cTimer->GetState() == PVR_TIMER_STATE_ERROR)
		{
			// create object
			PVR_TIMER xbmcTimer;
			memset(&xbmcTimer, 0, sizeof(PVR_TIMER));			
	
			       xbmcTimer.iClientIndex              =                   cTimer->GetClientIndex()             ;
			       xbmcTimer.iParentClientIndex        =                   cTimer->GetParentClientIndex()       ;
			       xbmcTimer.iClientChannelUid         =                   cTimer->GetClientChannelUid()        ;
			       xbmcTimer.startTime                 =                   cTimer->GetStartTime()               ;
			       xbmcTimer.endTime                   =                   cTimer->GetEndTime()                 ;
			       xbmcTimer.bStartAnyTime             =                   cTimer->GetStartAnyTime()            ;
			       xbmcTimer.bEndAnyTime               =                   cTimer->GetEndAnyTime()              ;
			       xbmcTimer.state                     = (PVR_TIMER_STATE) cTimer->GetState()                   ;
			       xbmcTimer.iTimerType                =                   cTimer->GetTimerType()               ;
			strcpy(xbmcTimer.strTitle                  ,                   cTimer->GetTitle())                  ;
			strcpy(xbmcTimer.strEpgSearchString        ,                   cTimer->GetEpgSearchString())        ;
			       xbmcTimer.bFullTextEpgSearch        =                   cTimer->GetFullTextEpgSearch()       ;
			strcpy(xbmcTimer.strDirectory              ,                   cTimer->GetDirectory())              ;
			strcpy(xbmcTimer.strSummary                ,                   cTimer->GetSummary())                ;
			       xbmcTimer.iPriority                 =                   cTimer->GetPriority()                ;
			       xbmcTimer.iLifetime                 =                   cTimer->GetLifetime()                ;
			       xbmcTimer.iMaxRecordings            =                   cTimer->GetMaxRecordings()           ;
			       xbmcTimer.iRecordingGroup           =                   cTimer->GetRecordingGroup()          ;
			       xbmcTimer.firstDay                  =                   cTimer->GetFirstDay()                ;
			       xbmcTimer.iWeekdays                 =                   cTimer->GetWeekdays()                ;
			       xbmcTimer.iPreventDuplicateEpisodes =                   cTimer->GetPreventDuplicateEpisodes();
			       xbmcTimer.iEpgUid                   =                   cTimer->GetEpgUid()                  ;
			       xbmcTimer.iMarginStart              =                   cTimer->GetMarginStart()             ;
			       xbmcTimer.iMarginEnd                =                   cTimer->GetMarginEnd()               ;
			       xbmcTimer.iGenreType                =                   cTimer->GetGenreType()               ;
			       xbmcTimer.iGenreSubType             =                   cTimer->GetGenreSubType()            ;
			strcpy(xbmcTimer.strSeriesLink             ,                   cTimer->GetSeriesLink())             ;

			// pass back to kodi
			PVR->TransferTimerEntry(handle, &xbmcTimer);
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVRClient::AddTimer(const PVR_TIMER &timer, int iSchedAdj /* = 0 */)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// check that SQL connection is active
	if (!client->IsConnected())
	{
		XBMC->Log(LOG_ERROR, "C+: %s - Disconnected from SQL database, please check above in the log for further details", __FUNCTION__);
		return PVR_ERROR_FAILED;
	}

	// create client index as channel + start time if no index passed in
	int iClientIndex = timer.iClientIndex;

	if (iClientIndex == PVR_TIMER_NO_CLIENT_INDEX)
		iClientIndex = time(NULL) + iSchedAdj;

	// create sql object
	string strTimer = string("(iClientIndex             , iParentClientIndex   , iClientChannelUid, startTime , endTime      ,") + 
	                  string(" bStartAnyTime            , bEndAnyTime          , state            , iTimerType, strTitle     ,") +
					  string(" strEpgSearchString       , bFullTextEpgSearch   , strDirectory     , strSummary, iPriority    ,") +
					  string(" iLifetime                , iMaxRecordings       , iRecordingGroup  , firstDay  , iWeekdays    ,") +
					  string(" iPreventDuplicateEpisodes, iEpgUid, iMarginStart, iMarginEnd       , iGenreType, iGenreSubType,") +
					  string(" strSeriesLink                                                                                 )") + 
					  string(" VALUES ") + 
					  string("( ") + StringUtils_Replace(itos(      iClientIndex             ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iParentClientIndex       ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iClientChannelUid        ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.startTime                ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.endTime                  ),"'", "''") + string(" , ") +
					  string(" '") + StringUtils_Replace(btos(timer.bStartAnyTime            ),"'", "''") + string("', ") +
					  string(" '") + StringUtils_Replace(btos(timer.bEndAnyTime              ),"'", "''") + string("', ") +
					  string("  ") + StringUtils_Replace(itos(timer.state                    ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iTimerType               ),"'", "''") + string(" , ") +
					  string(" '") + StringUtils_Replace(    (timer.strTitle                 ),"'", "''") + string("', ") +
					  string(" '") + StringUtils_Replace(    (timer.strEpgSearchString       ),"'", "''") + string("', ") +
					  string(" '") + StringUtils_Replace(btos(timer.bFullTextEpgSearch       ),"'", "''") + string("', ") +
					  string(" '") + StringUtils_Replace(    (timer.strDirectory             ),"'", "''") + string("', ") +
					  string(" '") + StringUtils_Replace(    (timer.strSummary               ),"'", "''") + string("', ") +
					  string("  ") + StringUtils_Replace(itos(timer.iPriority                ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iLifetime                ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iMaxRecordings           ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iRecordingGroup          ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.firstDay                 ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iWeekdays                ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iPreventDuplicateEpisodes),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iEpgUid                  ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iMarginStart             ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iMarginEnd               ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iGenreType               ),"'", "''") + string(" , ") +
					  string("  ") + StringUtils_Replace(itos(timer.iGenreSubType            ),"'", "''") + string(" , ") +
					  string(" '") + StringUtils_Replace(    (timer.strSeriesLink            ),"'", "''") + string("');") ;
					  
	// add select statement to read back timer
	strTimer += " SELECT * FROM Timers WHERE iClientIndex = " + itos(iClientIndex) + ";";
	
	// create return object to update our local instance
	SQLRecord sqlTimer;	
					  				  
	// send to database and return our updated record
	client->AddRecord("Timers", strTimer, &sqlTimer);
	
	// convert record back to timer
	DVRTimer cTimer(sqlTimer.GetRecord());
	
	// push back to timer types container
	cTimers.push_back(cTimer);	

	// log addition of entry
	XBMC->Log(LOG_NOTICE, "C+: %s - Created %s timer (%i)", __FUNCTION__, timer.strTitle, iClientIndex);
	
	// unlock threads
	SetUnlock();
	
	// trigger update
	PVR->TriggerTimerUpdate();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVRClient::UpdateTimer(const PVR_TIMER &timer)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
	
	// check that SQL connection is active
	if (!client->IsConnected())
	{
		XBMC->Log(LOG_ERROR, "C+: %s - Disconnected from SQL database, please check above in the log for further details", __FUNCTION__);
		return PVR_ERROR_FAILED;
	}
	  
	// iterate through timers and update entry
	for (vector<DVRTimer>::iterator cTimer = cTimers.begin(); cTimer != cTimers.end(); cTimer++)
	{
		// if matches entry id then update
		if (cTimer->GetClientIndex() == timer.iClientIndex)
		{
			// create update
			string strTimer = " SET ";
			
			// look for changes
			if (       cTimer->GetClientChannelUid()         != timer.iClientChannelUid              ) strTimer += "iClientChannelUid = "         + StringUtils_Replace(itos(timer.iClientChannelUid        ),"'", "''") + " , ";
			if (       cTimer->GetStartTime()                != timer.startTime                      ) strTimer += "startTime = '"                + StringUtils_Replace(itos(timer.startTime                ),"'", "''") + "', ";
			if (       cTimer->GetEndTime()                  != timer.endTime                        ) strTimer += "endTime = '"                  + StringUtils_Replace(itos(timer.endTime                  ),"'", "''") + "', ";
			if (       cTimer->GetStartAnyTime()             != timer.bStartAnyTime                  ) strTimer += "bStartAnyTime = '"            + StringUtils_Replace(btos(timer.bStartAnyTime            ),"'", "''") + "', ";
			if (       cTimer->GetEndAnyTime()               != timer.bEndAnyTime                    ) strTimer += "bEndAnyTime = '"              + StringUtils_Replace(btos(timer.bEndAnyTime              ),"'", "''") + "', ";
			if (       cTimer->GetState()                    != timer.state                          ) strTimer += "state = "                     + StringUtils_Replace(itos(timer.state                    ),"'", "''") + " , ";
			if (       cTimer->GetTimerType()                != timer.iTimerType                     ) strTimer += "iTimerType = "                + StringUtils_Replace(itos(timer.iTimerType               ),"'", "''") + " , ";
			if (strcmp(cTimer->GetTitle()                    ,  timer.strTitle                 ) == 0) strTimer += "strTitle = '"                 + StringUtils_Replace(ctos(timer.strTitle                 ),"'", "''") + "', ";
			if (strcmp(cTimer->GetEpgSearchString()          ,  timer.strEpgSearchString       ) == 0) strTimer += "strEpgSearchString = '"       + StringUtils_Replace(ctos(timer.strEpgSearchString       ),"'", "''") + "', ";
			if (       cTimer->GetFullTextEpgSearch()        != timer.bFullTextEpgSearch             ) strTimer += "bFullTextEpgSearch = '"       + StringUtils_Replace(btos(timer.bFullTextEpgSearch       ),"'", "''") + "', ";
			if (strcmp(cTimer->GetDirectory()                ,  timer.strDirectory             ) == 0) strTimer += "strDirectory = '"             + StringUtils_Replace(ctos(timer.strDirectory             ),"'", "''") + "', ";
			if (strcmp(cTimer->GetSummary()                  ,  timer.strSummary               ) == 0) strTimer += "strSummary = '"               + StringUtils_Replace(ctos(timer.strSummary               ),"'", "''") + "', ";
			if (       cTimer->GetPriority()                 != timer.iPriority                      ) strTimer += "iPriority = "                 + StringUtils_Replace(itos(timer.iPriority                ),"'", "''") + " , ";
			if (       cTimer->GetLifetime()                 != timer.iLifetime                      ) strTimer += "iLifetime = "                 + StringUtils_Replace(itos(timer.iLifetime                ),"'", "''") + " , ";
			if (       cTimer->GetMaxRecordings()            != timer.iMaxRecordings                 ) strTimer += "iMaxRecordings = "            + StringUtils_Replace(itos(timer.iMaxRecordings           ),"'", "''") + " , ";
			if (       cTimer->GetRecordingGroup()           != timer.iRecordingGroup                ) strTimer += "iRecordingGroup = "           + StringUtils_Replace(itos(timer.iRecordingGroup          ),"'", "''") + " , ";
			if (       cTimer->GetFirstDay()                 != timer.firstDay                       ) strTimer += "firstDay = '"                 + StringUtils_Replace(itos(timer.firstDay                 ),"'", "''") + "', ";
			if (       cTimer->GetWeekdays()                 != timer.iWeekdays                      ) strTimer += "iWeekdays = "                 + StringUtils_Replace(itos(timer.iWeekdays                ),"'", "''") + " , ";
			if (       cTimer->GetPreventDuplicateEpisodes() != timer.iPreventDuplicateEpisodes      ) strTimer += "iPreventDuplicateEpisodes = " + StringUtils_Replace(itos(timer.iPreventDuplicateEpisodes),"'", "''") + " , ";
			if (       cTimer->GetEpgUid()                   != timer.iEpgUid                        ) strTimer += "iEpgUid = "                   + StringUtils_Replace(itos(timer.iEpgUid                  ),"'", "''") + " , ";
			if (       cTimer->GetMarginStart()              != timer.iMarginStart                   ) strTimer += "iMarginStart = "              + StringUtils_Replace(itos(timer.iMarginStart             ),"'", "''") + " , ";
			if (       cTimer->GetMarginEnd()                != timer.iMarginEnd                     ) strTimer += "iMarginEnd = "                + StringUtils_Replace(itos(timer.iMarginEnd               ),"'", "''") + " , ";
			if (       cTimer->GetGenreType()                != timer.iGenreType                     ) strTimer += "iGenreType = "                + StringUtils_Replace(itos(timer.iGenreType               ),"'", "''") + " , ";
			if (       cTimer->GetGenreSubType()             != timer.iGenreSubType                  ) strTimer += "iGenreSubType = "             + StringUtils_Replace(itos(timer.iGenreSubType            ),"'", "''") + " , ";
			if (strcmp(cTimer->GetSeriesLink()               ,  timer.strSeriesLink            ) == 0) strTimer += "strSeriesLink = '"            + StringUtils_Replace(ctos(timer.strSeriesLink            ),"'", "''") + "', ";
			
			// remove last char should be a comma
			strTimer = strTimer.substr(0, strTimer.length()-2);
			
			// add client index
			strTimer += " WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";"
			            " SELECT * FROM Timers WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";";
			
			// create return object to update our local instance
			SQLRecord sqlTimer;
			
			// send to database and return our updated record
			client->UpdateRecord("Timers", strTimer, &sqlTimer);
			
			// convert record to timer
			DVRTimer xTimer(sqlTimer.GetRecord());

			// assign all values
			*cTimer = xTimer;

			// log update of entry
			XBMC->Log(LOG_NOTICE, "C+: %s - Updated %s timer (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);
			
			// break loop on non-repeating timers
			if (timer.iTimerType == TIMER_ONCE_MANUAL || timer.iTimerType == TIMER_ONCE_EPG) break;
		}
		
		// if matches parent then delete, let scheduler re-do it
		else if (cTimer->GetParentClientIndex() == timer.iClientIndex)
		{
			// if recording set to aborted, otherwise set to canceled
			string strTimer = " SET state = " + itos(cTimer->GetState() == PVR_TIMER_STATE_RECORDING ? PVR_TIMER_STATE_ABORTED : PVR_TIMER_STATE_CANCELLED) + " WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";"
			                  " SELECT * FROM Timers WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";";
			
			// create return object to update our local instance
			SQLRecord sqlTimer;
			
			// send to database and return our updated record
			client->UpdateRecord("Timers", strTimer, &sqlTimer);

			// convert record to timer
			DVRTimer xTimer(sqlTimer.GetRecord());

			// assign all values
			*cTimer = xTimer;

			// log deletion of entry
			XBMC->Log(LOG_NOTICE, "C+: %s - Deleted %s timer (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);
			
			// break loop on non-repeating timers
			if (timer.iTimerType == TIMER_ONCE_MANUAL || timer.iTimerType == TIMER_ONCE_EPG) break;
		}
	}
	
	// unlock threads
	SetUnlock();

	// trigger update
	PVR->TriggerTimerUpdate();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVRClient::DeleteTimer(const PVR_TIMER &timer, bool bForceDelete)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
	
	// check that SQL connection is active
	if (!client->IsConnected())
	{
		XBMC->Log(LOG_ERROR, "C+: %s - Disconnected from SQL database, please check above in the log for further details", __FUNCTION__);
		return PVR_ERROR_FAILED;
	}
	  
	// check that it is not a timer rule for scheduler calls
	if (bForceDelete)
	{
		if (timer.iTimerType == TIMER_REPEATING_MANUAL || timer.iTimerType == TIMER_REPEATING_EPG || timer.iTimerType == TIMER_REPEATING_SERIESLINK)
		{
			XBMC->Log(LOG_DEBUG, "C+: %s - Timer rule detected, passing back to scheduler", __FUNCTION__);	
			return PVR_ERROR_NO_ERROR;			
		}
	}

	// iterate through timers and update entry
	for (vector<DVRTimer>::iterator cTimer = cTimers.begin(); cTimer != cTimers.end(); cTimer++)
	{
		// if matches entry id or parent id then update
		if (cTimer->GetClientIndex() == timer.iClientIndex || cTimer->GetParentClientIndex() == timer.iClientIndex)
		{
			// if recording set to aborted, otherwise set to canceled
			string strTimer = " SET state = " + itos(cTimer->GetState() == PVR_TIMER_STATE_RECORDING ? PVR_TIMER_STATE_ABORTED : PVR_TIMER_STATE_CANCELLED) + " WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";"
			                  " SELECT * FROM Timers WHERE iClientIndex = " + itos(cTimer->GetClientIndex()) + ";";
			
			// create return object to update our local instance
			SQLRecord sqlTimer;
			
			// send to database and return our updated record
			client->UpdateRecord("Timers", strTimer, &sqlTimer);
			
			// convert record to timer
			DVRTimer xTimer(sqlTimer.GetRecord());

			// assign all values
			*cTimer = xTimer;

			// log deletion of entry
			XBMC->Log(LOG_NOTICE, "C+: %s - Deleted %s timer (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);

			// break loop on non-repeating timers
			if (timer.iTimerType == TIMER_ONCE_MANUAL || timer.iTimerType == TIMER_ONCE_EPG) break;
		}
	}
	
	// unlock threads
	SetUnlock();

	// trigger update
	PVR->TriggerTimerUpdate();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

int DVRClient::GetTimersAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cTimers.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

/***********************************************************
 * Recordings API Definitions
 ***********************************************************/
PVR_ERROR DVRClient::GetRecordings(ADDON_HANDLE handle, bool deleted)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
  		
	// iterate through recordings and pass back to kodi
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// pass back appropriate entries
		if (cRecording->GetIsDeleted() == deleted)
		{	
			// create object
			PVR_RECORDING xbmcRecording;
			memset(&xbmcRecording, 0, sizeof(PVR_RECORDING));
	  
			strcpy(xbmcRecording.strRecordingId      ,                              cRecording->GetRecordingId())      ;
			strcpy(xbmcRecording.strTitle            ,                              cRecording->GetTitle())            ;
			strcpy(xbmcRecording.strEpisodeName      ,                              cRecording->GetEpisodeName())      ;
			       xbmcRecording.iSeriesNumber       =                              cRecording->GetSeriesNumber()      ;
			       xbmcRecording.iEpisodeNumber      =                              cRecording->GetEpisodeNumber()     ;
			       xbmcRecording.iYear               =                              cRecording->GetYear()              ;
			strcpy(xbmcRecording.strDirectory        ,                              cRecording->GetDirectory())        ;
			strcpy(xbmcRecording.strPlotOutline      ,                              cRecording->GetPlotOutline())      ;
			strcpy(xbmcRecording.strPlot             ,                              cRecording->GetPlot())             ;
			strcpy(xbmcRecording.strGenreDescription ,                              cRecording->GetGenreDescription()) ;
			strcpy(xbmcRecording.strChannelName      ,                              cRecording->GetChannelName())      ;
			strcpy(xbmcRecording.strIconPath         ,                              cRecording->GetIconPath())         ;
			strcpy(xbmcRecording.strThumbnailPath    ,                              cRecording->GetThumbnailPath())    ;
			strcpy(xbmcRecording.strFanartPath       ,                              cRecording->GetFanartPath())       ;
			       xbmcRecording.recordingTime       =                              cRecording->GetRecordingTime()     ;
			       xbmcRecording.iDuration           =                              cRecording->GetDuration()          ;
			       xbmcRecording.iPriority           =                              cRecording->GetPriority()          ;
			       xbmcRecording.iLifetime           =                              cRecording->GetLifetime()          ;
			       xbmcRecording.iGenreType          =                              cRecording->GetGenreType()         ;
			       xbmcRecording.iGenreSubType       =                              cRecording->GetGenreSubType()      ;
			       xbmcRecording.iPlayCount          =                              cRecording->GetPlayCount()         ;
			       xbmcRecording.iLastPlayedPosition =                              cRecording->GetLastPlayedPosition();
			       xbmcRecording.bIsDeleted          =                              cRecording->GetIsDeleted()         ;
			       xbmcRecording.iEpgEventId         =                              cRecording->GetEpgEventId()        ;
			       xbmcRecording.iChannelUid         =                              cRecording->GetChannelUid()        ;
			       xbmcRecording.channelType         = (PVR_RECORDING_CHANNEL_TYPE) cRecording->GetChannelType()       ;

			// pass back to kodi
			PVR->TransferRecordingEntry(handle, &xbmcRecording);
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVRClient::DeleteRecording(const PVR_RECORDING &recording)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// check that SQL connection is active
	if (!client->IsConnected())
	{
		XBMC->Log(LOG_ERROR, "C+: %s - Disconnected from SQL database, please check above in the log for further details", __FUNCTION__);
		return PVR_ERROR_FAILED;
	}
	
	// skip delete if path not set
	if (settings->GetDVRPath() == "")
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No DVR path to store files has been set, cannot delete recording", __FUNCTION__);
		return PVR_ERROR_FAILED;
	}

	// skip delete if path does not exist
	if (!XBMC->DirectoryExists(settings->GetDVRPath().c_str()))
	{
		XBMC->Log(LOG_ERROR, "C+: %s - Directory does not exits (check connection to network drives), cannot delete recording", __FUNCTION__);
		return PVR_ERROR_SERVER_ERROR; 
	}

	// iterate through recordings and delete file
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// if matches id exit loop with found
		if (strcmp(cRecording->GetRecordingId(), recording.strRecordingId) == 0)
		{		
			// derive separator
			string SEPARATOR = ParseFolderSeparator(settings->GetDVRPath()).c_str();
		
			// create folder & file path based on current directory
			string strFolderPath = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), cRecording->GetDirectory()                                   );
			string strFilePath   = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), cRecording->GetDirectory(), cRecording->GetFileName().c_str());
			
			// if recording set to aborted, otherwise set to canceled
			string strRecording = " SET bIsDeleted = '" + btos(true) + "' WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';"
			                      " SELECT * FROM Recordings WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';";
			
			// create return object to update our local instance
			SQLRecord sqlRecording;
			
			// send to database and return our updated record
			client->UpdateRecord("Recordings", strRecording, &sqlRecording);

			// convert record to timer
			DVRRecording xRecording(sqlRecording.GetRecord() + "<strFilePath>" + cRecording->GetFilePath() + "</strFilePath>");

			// assign all values
			*cRecording = xRecording;

			// log found
			XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to delete %s recording (%i)", __FUNCTION__, recording.strTitle, recording.strRecordingId);

			// delete file (log deletion), if not in a sub dir just cleanup file
			if (string(cRecording->GetDirectory()) == "")
			{
				if (XBMC->DeleteFile(strFilePath.c_str()))
					XBMC->Log(LOG_NOTICE, "C+: %s - Deleted [%s] recording file", __FUNCTION__, cRecording->GetFileName());
			}
			else
			{
				if (XBMC_DeleteFileAndFolder(strFilePath.c_str(), strFolderPath.c_str()))
					XBMC->Log(LOG_NOTICE, "C+: %s - Deleted [%s] recording file and directory", __FUNCTION__, cRecording->GetFileName());
			}

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();

	// trigger update
	PVR->TriggerRecordingUpdate();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR DVRClient::GetRecordingStreamProperties(const PVR_RECORDING* recording, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CPPLog(); 

	// lock object
	SetLock();

	// iterate through recordings to find in database
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// look for recording id
		if (strcmp(cRecording->GetRecordingId(), (*recording).strRecordingId) == 0)
		{
			// initialize array size
			*iPropertiesCount = 0;

			// create property var
			string strProperty;

			// assign stream property
			strProperty = PVR_STREAM_PROPERTY_STREAMURL;

			memcpy(properties[*iPropertiesCount].strName ,             strProperty.c_str()  ,             strProperty.size()  );
			memcpy(properties[*iPropertiesCount].strValue, cRecording->GetFilePath().c_str(), cRecording->GetFilePath().size());
			(*iPropertiesCount)++;

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

bool DVRClient::OpenRecordedStream(const PVR_RECORDING &recording)
{
	// log function call
	CPPLog();
	
	// lock object
	SetLock();

	// assume will not be found
	bool ret = false;

	// iterate through recordings and pass back entry
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// if matches id pass open
		if (strcmp(cRecording->GetRecordingId(), recording.strRecordingId) == 0)
		{
			// assign handle
			streamHandle = XBMC->OpenFile(cRecording->GetFilePath().c_str(), 0);

			// return opened
			if (streamHandle)
			{
				XBMC->Log(LOG_NOTICE, "C+: %s - Opened stream natively", __FUNCTION__);
				ret = true;
			}

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();

	// return value
	return ret; 
}

int DVRClient::ReadRecordedStream(unsigned char *pBuffer, unsigned int iBufferSize)
{
	// log function call
	CPPLog();

	// return value
	if (streamHandle)
		return XBMC->ReadFile(streamHandle, pBuffer, iBufferSize);

	// return error if stream handle not set
	return 0; 
}

long long DVRClient::SeekRecordedStream(long long iPosition, int iWhence /* = SEEK_SET */)
{
	// log function call
	CPPLog();

	// return value
	if (streamHandle)
		return XBMC->SeekFile(streamHandle, iPosition, iWhence);

	// return error if stream handle not set
	return 0; 
}

long long DVRClient::PositionRecordedStream(void)
{
	// log function call
	CPPLog();

	// return value
	if (streamHandle)
		return XBMC->GetFilePosition(streamHandle);

	// return error if stream handle not set
	return 0; 
}

long long DVRClient::LengthRecordedStream(void)
{
	// log function call
	CPPLog();

	// return value
	if (streamHandle)
		return XBMC->GetFileLength(streamHandle);

	// return error if stream handle not set
	return 0; 
}

void DVRClient::CloseRecordedStream(void)
{
	// log function call
	CPPLog();

	// close value
	if (streamHandle)
		XBMC->CloseFile(streamHandle);

	// set back to null
	streamHandle = NULL;
}

PVR_ERROR DVRClient::SetRecordingPlayCount(const PVR_RECORDING &recording, int count)
{
	// log function call
	CPPLog();
	
	// lock object
	SetLock();

	// assume will not be found
	PVR_ERROR ret = PVR_ERROR_SERVER_ERROR;

	// iterate through recordings and pass back entry
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// if matches id pass set play count
		if (strcmp(cRecording->GetRecordingId(), recording.strRecordingId) == 0)
		{
			// if recording set to aborted, otherwise set to canceled
			string strRecording = " SET iPlayCount = '" + itos(count) + "' WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';"
			                      " SELECT * FROM Recordings WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';";
			
			// create return object to update our local instance
			SQLRecord sqlRecording;
			
			// send to database and return our updated record
			client->UpdateRecord("Recordings", strRecording, &sqlRecording);

			// convert record to timer
			DVRRecording xRecording(sqlRecording.GetRecord() + "<strFilePath>" + cRecording->GetFilePath() + "</strFilePath>");

			// assign all values
			*cRecording = xRecording;

			// put play count in log
			XBMC->Log(LOG_NOTICE, "C+: %s - Play count set successfully %s to %i plays", __FUNCTION__, cRecording->GetTitle(), count);

			// return no issue
			ret = PVR_ERROR_NO_ERROR;

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();
	
	// trigger update
	PVR->TriggerRecordingUpdate();

	// return value
	return ret;   
}

PVR_ERROR DVRClient::SetRecordingLastPlayedPosition(const PVR_RECORDING &recording, int lastplayedposition)
{
	// log function call
	CPPLog();

	// lock object
	SetLock();

	// assume will not be found
	PVR_ERROR ret = PVR_ERROR_SERVER_ERROR;

	// iterate through recordings and pass back entry
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// if matches id pass set play position
		if (strcmp(cRecording->GetRecordingId(), recording.strRecordingId) == 0)
		{
			// if recording set to aborted, otherwise set to canceled
			string strRecording = " SET iLastPlayedPosition = '" + itos(lastplayedposition) + "' WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';"
			                      " SELECT * FROM Recordings WHERE strRecordingId = '" + cRecording->GetRecordingId() + "';";
			
			// create return object to update our local instance
			SQLRecord sqlRecording;
			
			// send to database and return our updated record
			client->UpdateRecord("Recordings", strRecording, &sqlRecording);

			// convert record to timer
			DVRRecording xRecording(sqlRecording.GetRecord() + "<strFilePath>" + cRecording->GetFilePath() + "</strFilePath>");

			// assign all values
			*cRecording = xRecording;

			// put scrobble in queue and log
			XBMC->QueueNotification(QUEUE_INFO, "Scrobbled successfully");

			// put scrobble in log
			XBMC->Log(LOG_NOTICE, "C+: %s - Playback srobbled successfully %s to %i secs", __FUNCTION__, cRecording->GetTitle(), lastplayedposition);

			// return no issue
			ret = PVR_ERROR_NO_ERROR;

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();
	
	// trigger update
	PVR->TriggerRecordingUpdate();

	// return value
	return ret; 
}

int DVRClient::GetRecordingLastPlayedPosition(const PVR_RECORDING &recording)
{ 
	// log function call
	CPPLog();

	// lock object
	SetLock();
	
	// assume will not be found
	int ret = -1;

	// iterate through recordings and pass back entry
	for (vector<DVRRecording>::iterator cRecording = cRecordings.begin(); cRecording != cRecordings.end(); cRecording++)
	{
		// if matches id pass get play position
		if (strcmp(cRecording->GetRecordingId(), recording.strRecordingId) == 0)
		{
			// return last played pos
			ret = cRecording->GetLastPlayedPosition();

			// break loop
			break;
		}
	}

	// unlock object
	SetUnlock();

	// return value
	return ret; 
}

int DVRClient::GetRecordingsAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cRecordings.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

/***********************************************************
 * Fetch Copy of Data API Definitions
 ***********************************************************/
vector<DVRTimerType> DVRClient::GetTimerTypes(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<DVRTimerType> ret = cTimerTypes;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<DVRTimer> DVRClient::GetTimers(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<DVRTimer> ret = cTimers;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<DVRRecording> DVRClient::GetRecordings(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<DVRRecording> ret = cRecordings;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

/***********************************************************
 * Trigger Reload API Definitions
 ***********************************************************/
void DVRClient::TriggerTimersUpdate(time_t tSync)
{
	// log function call
	CPPLog();	
	
	// lock threads
	SetLock();
	
	// reload sql objects
	LoadTimers(tSync, false);

	// unlock threads
	SetUnlock();
	
	// notify user of reload
	XBMC->Log(LOG_NOTICE, "C+: %s - %i timer(s) re-loaded", __FUNCTION__, GetTimersAmount());
	
	// trigger client update
	PVR->TriggerTimerUpdate();
}

void DVRClient::TriggerRecordingsUpdate(time_t tSync)
{
	// log function call
	CPPLog();	
	
	// lock threads
	SetLock();
	
	// reload sql objects
	LoadRecordings(tSync, false);

	// unlock threads
	SetUnlock();
	
	// notify user of reload
	XBMC->Log(LOG_NOTICE, "C+: %s - %i recording(s) re-loaded", __FUNCTION__, GetRecordingsAmount());
		
	// trigger client update
	PVR->TriggerRecordingUpdate();
}

/***********************************************************
 * Load Data Definitions
 ***********************************************************/
void DVRClient::LoadTimerTypes(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cTimerTypes.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlTimerTypes = client->GetRecords("TimerTypes");
	
	// iterate through timer types and add to dvr cache
	for (vector<SQLRecord>::iterator sqlTimerType = sqlTimerTypes.begin(); sqlTimerType != sqlTimerTypes.end(); sqlTimerType++)
	{	
		// initialize record
		DVRTimerType cTimerType(sqlTimerType->GetRecord());
		
		// push back to timer types container
		cTimerTypes.push_back(cTimerType);	
	}
	
	// notify user of timer types loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i timer type(s) loaded", GetTimerTypesAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i timer type(s) loaded", __FUNCTION__, GetTimerTypesAmount());
	}
	
	// set last sync to now
	tLastTimerTypesSync = tSync;
}

void DVRClient::LoadTimers(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cTimers.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlTimers = client->GetRecords("Timers");
	
	// iterate through timers and add to dvr cache
	for (vector<SQLRecord>::iterator sqlTimer = sqlTimers.begin(); sqlTimer != sqlTimers.end(); sqlTimer++)
	{
		// initialize record
		DVRTimer cTimer(sqlTimer->GetRecord());
		
		// push back to timer types container
		cTimers.push_back(cTimer);	
	}
	
	// notify user of timers loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i timer(s) loaded", GetTimersAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i timer(s) loaded", __FUNCTION__, GetTimersAmount());
	}
	
	// set last sync to now
	tLastTimersSync = tSync;
}

void DVRClient::LoadRecordings(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cRecordings.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlRecordings = client->GetRecords("Recordings");
	
	// iterate through recordings and add to dvr cache
	for (vector<SQLRecord>::iterator sqlRecording = sqlRecordings.begin(); sqlRecording != sqlRecordings.end(); sqlRecording++)
	{
		// derive separator
		string SEPARATOR = ParseFolderSeparator(settings->GetDVRPath()).c_str();
	
		// create folder & file path based on current directory
		string strFolderPath = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), ParseSQLValue(sqlRecording->GetRecord(), "<strDirectory>", "").c_str()                                                                       );
		string strFilePath   = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), ParseSQLValue(sqlRecording->GetRecord(), "<strDirectory>", "").c_str(), ParseSQLValue(sqlRecording->GetRecord(), "<strFileName>", "").c_str());
		
		// initialize record
		DVRRecording cRecording(sqlRecording->GetRecord() + "<strFilePath>" + strFilePath + "</strFilePath>"); 
		
		// push back to recording types container
		cRecordings.push_back(cRecording);	
	}
	
	// notify user of recordings loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i recording(s) loaded", GetRecordingsAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i recording(s) loaded", __FUNCTION__, GetRecordingsAmount());
	}
	
	// set last sync to now
	tLastRecordingsSync = tSync;
}