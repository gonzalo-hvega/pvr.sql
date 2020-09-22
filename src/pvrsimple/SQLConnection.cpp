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
#include "SQLConnection.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings *settings;
extern TCPClient   *server  ;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
SQLConnection::SQLConnection()
{
	// only server mode
	if (settings->GetDVRMode() == SERVER_MODE)
	{
		// log attempt to create object
		XBMC->Log(LOG_NOTICE, "C+: %s - Creating SQL connection", __FUNCTION__);
		
		// no establish connection at start
		bIsConnected = false;
		bIsWorking   = false;
		bStop        = false;
		
		// clear inervals
		tLastM3URead = 0;
		tLastEPGRead = 0;
		iNumEPGDays  = 0;
		
		// add folder name to directory
		strDBPath += settings->GetUserPath() + DATABASE_FOLDER + ParseFolderSeparator(settings->GetUserPath());
		
		// create directory if they don't exist  
		if (!XBMC->DirectoryExists(strDBPath.c_str()))
			XBMC->CreateDirectory(strDBPath.c_str());
		
		// add file name to directory
		strDBPath += DATABASE_FILE;
		
		// clear callback buffer, logs, and tasks
		sqlCallback.clear();
		sqlLog.clear();
		sqlTasks.clear();
		
		// create change log
		SQLMsg sqlMsg;
		
		sqlMsg.strTable = "Channels"           ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "ChannelGroups"      ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "ChannelGroupMembers"; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "EpgChannels"        ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "EpgEntries"         ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "TimerTypes"         ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "Timers"             ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
		sqlMsg.strTable = "Recordings"         ; sqlMsg.iModTime = 0; sqlLog.push_back(sqlMsg);
			
		// call connection to database
		Connect();
		
		// if empty add tables
		if (XBMC_FileSize(strDBPath.c_str()) == 0)
		{		
			// attempt to create channels, channel groups, channel group members, guide, timer types, timers, and recordings table
			if (!CreateChannels() || !CreateChannelGroups() || !CreateChannelGroupMembers() || !CreateGuideChannels() || !CreateGuideEntries() || !CreateTimerTypes() || !CreateTimers() || !CreateRecordings())
				bStop = true;
		}
		
		// call clear/clean functions
		ClearChannels();
		ClearChannelGroups();
		ClearChannelGroupMembers();
		ClearGuideChannels();
		ClearGuideEntries();
		ClearTimerTypes();
		CleanTimers();
		CleanRecordings();
		
		// import external files
		ImportM3U();
		ImportXMLTV();
		ImportTimerTypes();
		
		// apply filters
		FilterChannelsEPG();
		
		// log creation of object
		XBMC->Log(LOG_NOTICE, "C+: %s - Created SQL connection", __FUNCTION__);
		
		// create thread
		if (IsConnected()) CreateThread();
	}
}

SQLConnection::~SQLConnection(void)
{
	// only server mode
	if (settings->GetDVRMode() == SERVER_MODE)
	{
		// mark as stopped
		bStop = true;
			
		// clear callback buffer, logs, and tasks
		sqlCallback.clear();
		sqlLog.clear();
		sqlTasks.clear();
		
		// call clear/clean functions
		CleanTimers();
		CleanRecordings();
		 
		// attempt to disconnect
		Disconnect();	
	}
}

/***********************************************************
 * Get Staus/Local Variable API Definitions
 ***********************************************************/
bool SQLConnection::IsConnected(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsConnected;
}

bool SQLConnection::IsWorking(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsWorking;
}

time_t SQLConnection::LastM3URead(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastM3URead;
}

time_t SQLConnection::LastEPGRead(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastEPGRead;
}

int SQLConnection::GetEPGDays(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return iNumEPGDays ? iNumEPGDays : 1;
}

string SQLConnection::GetDBLog(void)
{
	// log function call
	CPPLog(); 
	
	// create return string
	string strLog;
	
	// iterate through log
	for (vector<SQLMsg>::iterator sqlMsg = sqlLog.begin(); sqlMsg != sqlLog.end(); sqlMsg++)
		strLog += "<strTable>"+sqlMsg->strTable+"</strTable>"+"<iModTime>"+itos(sqlMsg->iModTime)+"</iModTime>"+"\n";
	
	// delete last 2 characters (extra line break)
	strLog = strLog.substr(1, strLog.length()-2);
	
	// return log
	return strLog;
}

/***********************************************************
 * Records API Definitions
 ***********************************************************/
void SQLConnection::AddRecord(const char* strTable, const string strRecord, SQLRecord* sqlRecord /* = NULL */)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// create return vector to push back to kodi
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create query and sql text
	string sqlAddRecord = string("INSERT INTO ") + string(strTable) + string(strRecord);
	
	// call query and assign head of vector to return
	if (SendQuery(sqlAddRecord.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlRecord)
			if (sqlReturn.size())
				*sqlRecord = sqlReturn.front();
			
	// update change log
	for (vector<SQLMsg>::iterator sqlMsg = sqlLog.begin(); sqlMsg != sqlLog.end(); sqlMsg++)
		if (string(strTable) == sqlMsg->strTable)
			sqlMsg->iModTime = time(NULL);
			
	// unlock threads
	SetUnlock();
}

void SQLConnection::UpdateRecord(const char* strTable, const string strRecord, SQLRecord* sqlRecord)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// create return vector to push back to kodi
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create query and sql text
	string sqlUpdateRecord = string("UPDATE ") + string(strTable) + string(strRecord);
	
	// call query and assign head of vector to return
	if (SendQuery(sqlUpdateRecord.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlRecord)
			if (sqlReturn.size())
				*sqlRecord = sqlReturn.front();
				
	// update change log
	for (vector<SQLMsg>::iterator sqlMsg = sqlLog.begin(); sqlMsg != sqlLog.end(); sqlMsg++)
		if (string(strTable) == sqlMsg->strTable)
			sqlMsg->iModTime = time(NULL);
			
	// unlock threads
	SetUnlock();
}

void SQLConnection::DeleteRecord(const char* strTable, const string strRecord)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// create query and sql text
	string sqlDeleteRecord = string("DELETE FROM ") + string(strTable) + string(strRecord);
	
	// call query
	SendQuery(sqlDeleteRecord.c_str(), NULL);
	
	// update change log
	for (vector<SQLMsg>::iterator sqlMsg = sqlLog.begin(); sqlMsg != sqlLog.end(); sqlMsg++)
		if (string(strTable) == sqlMsg->strTable)
			sqlMsg->iModTime = time(NULL);
	
	// unlock threads
	SetUnlock();
}

bool SQLConnection::FindRecord(const char* strTable, const char* iId, const int iValue, SQLRecord& sqlRecord)
{
	// log function call
	CPPLog();

	// lock threads
	SetLock();	
		
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create query and sql text
	string sqlFindRecordBy = string("SELECT * FROM ") + string(strTable) + string(" WHERE ") + string(iId) + string(" = ") + StringUtils_Replace(itos(iValue), "'", "''") + string("");
	
	// call query
	if (SendQuery(sqlFindRecordBy.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlReturn.size())
			sqlRecord = sqlReturn.front();

	// unlock threads
	SetUnlock();
	
	// return record
	return (sqlReturn.size() > 0);	
}

bool SQLConnection::FindRecord(const char* strTable, const char* strId, const char* strValue, SQLRecord& sqlRecord)
{
	// log function call
	CPPLog();

	// lock threads
	SetLock();	
	
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create query and sql text
	string sqlFindRecordBy = string("SELECT * FROM ") + string(strTable) + string(" WHERE ") + string(strId) + string(" = '") + StringUtils_Replace(string(strValue), "'", "''") + string("'");
	
	// call query
	if (SendQuery(sqlFindRecordBy.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlReturn.size())
			sqlRecord = sqlReturn.front();
		
	// unlock threads
	SetUnlock();
	
	// return record
	return (sqlReturn.size() > 0);	
}

bool SQLConnection::FindRecord(const char* strTable, const string strWhere, SQLRecord& sqlRecord)
{
	// log function call
	CPPLog();

	// lock threads
	SetLock();	
	
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create query and sql text
	string sqlFindRecordBy = string("SELECT * FROM ") + string(strTable) + string(" WHERE ") + string(strWhere) + string("");
	
	// call query
	if (SendQuery(sqlFindRecordBy.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlReturn.size())
			sqlRecord = sqlReturn.front();
		
	// unlock threads
	SetUnlock();
	
	// return record
	return (sqlReturn.size() > 0);	
}

int SQLConnection::GetTableSize(const char* strTable)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
	
	// create return value
	int iReturn =  -1;
	
	// create query and sql text
	string sqlGetSize  = string("SELECT COUNT(*) FROM ") + string(strTable);
	
	// call query
	if (SendQuery(sqlGetSize.c_str(), &sqlReturn) == SQLITE_OK)
		if (sqlReturn.size())
			iReturn = stoi(ParseSQLValue(sqlReturn.front().GetRecord(), "<COUNT(*)>", ""));
		
	// unlock threads
	SetUnlock();
	
	// return size
	return iReturn;	
}

/***********************************************************
 * Tables API Definitions
 ***********************************************************/
vector<SQLRecord> SQLConnection::GetRecords(const char* strTable)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();
	
	// create return vector
	vector<SQLRecord> sqlReturn;
	
	// clear vector
	sqlReturn.clear();
		
	// create query and sql text
	string sqlGetRecords = string("SELECT * FROM ") + string(strTable);
	
	// call query
	if (SendQuery(sqlGetRecords.c_str(), &sqlReturn) != SQLITE_OK)
		sqlReturn.clear();
	
	// unlock threads
	SetUnlock();
	
	// return records
	return sqlReturn;	
}

/***********************************************************
 * Connect/Disconnect Definitions
 ***********************************************************/
void SQLConnection::Connect(void)
{
	// log function call
	CPPLog(); 

	// attempt to establish a connection
	bIsConnected = !sqlite3_open(strDBPath.c_str(), &sqlDatabase);
	
	// log failure to connect
	if (!bIsConnected)
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to open SQL database [%s] with error [%s]", __FUNCTION__, strDBPath.c_str(), sqlite3_errmsg(sqlDatabase));
}

void SQLConnection::Disconnect(bool bWait /* = true */)
{
	// log function call
	CPPLog(); 
	
	// log waiting for thread to close
	while (bIsWorking)
	{
		// sleep to allow thread time to close
		sleep(1);
	}
	
	// close established connection
	sqlite3_close(sqlDatabase);

	// force close thread
	StopThread(bWait);
}

/***********************************************************
 * Query Definitions
 ***********************************************************/
static int cCallback(void *ObjPtr, int Vars, char** ColVal, char** ColName)
{
	// log function call
	CPPLog(); 
	
	// create pointer to class
	SQLConnection *SQL = reinterpret_cast<SQLConnection*>(ObjPtr);
	
	// return value from class callback
	return SQL->Callback(Vars, ColVal, ColName);	
}

int SQLConnection::Callback(int Vars, char** ColVal, char** ColName) 
{
	// log function call
	CPPLog(); 
	
	// create container for string
	string strRecord;
	
	// parse call back data to buffer
	for(int i = 0; i < Vars; i++)
		strRecord += string("<") + string(ColName[i]) + string(">") + string(ColVal[i] ? ColVal[i] : "") + string("</") + string(ColName[i]) + string(">");
	
	// create record object
	SQLRecord sqlRecord(strRecord);
	
	// push back to callback
	sqlCallback.push_back(sqlRecord);

	// return to query
	return 0;
}

int SQLConnection::SendQuery(const char* strSyntax, void* sqlResponse)
{
	// log function call
	CPPLog(); 
	
	// create local containers
	char* strErrMsg =          0;
	int   iResponse =  SQLITE_OK;
	
	// clear callback buffer
	sqlCallback.clear();
	
	// attempt to call function
	iResponse = sqlite3_exec(sqlDatabase, strSyntax, cCallback, this, &strErrMsg);

	// log failure
	if (iResponse != SQLITE_OK)
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to query SQL database [%s] with error [%s]", __FUNCTION__, strSyntax, strErrMsg);
	
	// assign data if not null
	if (sqlResponse)
	{
		// create pointer to vector
		vector<SQLRecord> *sqlReturn = reinterpret_cast<vector<SQLRecord>*>(sqlResponse);
		
		// assign callback buffer to response
		*sqlReturn = sqlCallback;
	}

	// return message
	return iResponse;
}

/***********************************************************
 * SQL Server Process Definitions
 ***********************************************************/
void *SQLConnection::Process(void)
{
	// log creation of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Started SQL connection", __FUNCTION__);
	
	// set thread working
	bIsWorking = true;
	
	// create vector to hold current timers
	vector<SQLRecord> sqlTimers;	
	
	// create last checked interval
	time_t lastCheck = time(NULL) - settings->GetSchedPoll();
	
	// execute scheduler
	while (!bStop)
	{
		// sleep thread so machine doesn't idle at 100% cpu
		sleep(1);
		
		// check connection
		if (XBMC->FileExists(strDBPath.c_str(), false))
		{
			// log if reconnect
			if (!IsConnected())
			{
				// notify on screen
				PVR->ConnectionStateChange("Connected", PVR_CONNECTION_STATE_CONNECTED, "DVR reconnected");
				
				// send message to log
				XBMC->Log(LOG_NOTICE, "C+: %s - The SQL connection regained access to database [%s]", __FUNCTION__, strDBPath.c_str());
			}
			
			// set connection
			bIsConnected = true;
			
			// if polling interval passed to check timers    
			if (lastCheck + settings->GetSchedPoll() <= time(NULL))
			{		
				// clear container
				sqlTimers.clear();
	
				// read current timers
				sqlTimers = GetRecords("Timers");
				
				// iterate through timers and schedule timers, start rec, stop rec
				for (vector<SQLRecord>::iterator sqlTimer = sqlTimers.begin(); sqlTimer != sqlTimers.end(); sqlTimer++)
				{	
					// convert to pvr timer
					DVRTimer cTimer(sqlTimer->GetRecord());
					
					// scheduled events
					if (cTimer.GetState() == PVR_TIMER_STATE_SCHEDULED)
					{
						// start & stop recording for scheduled timers
						if (cTimer.GetTimerType() == TIMER_ONCE_MANUAL || cTimer.GetTimerType() == TIMER_ONCE_EPG)
						{
							// call delete timer if missed
							if (cTimer.GetEndTime()   + cTimer.GetMarginEnd()*0    < time(NULL) - settings->GetSchedPoll())
								this->DeleteTimer(cTimer.Timer());

							// call start recording if start time is now
							if (cTimer.GetStartTime() - cTimer.GetMarginStart()*60 < time(NULL) - settings->GetSchedPoll())
								this->StartTimer(cTimer.Timer());
						}
						
						// schedule timer rules
						if (cTimer.GetTimerType() == TIMER_REPEATING_MANUAL || cTimer.GetTimerType() == TIMER_REPEATING_EPG || cTimer.GetTimerType() == TIMER_REPEATING_SERIESLINK)
						{
							// call scheduling to explode past timer rule
							if (cTimer.GetEndTime()   + cTimer.GetMarginEnd()*0    <= time(NULL) - settings->GetSchedPoll())
								this->ScheduleTimer(cTimer.Timer());				

							// call scheduling to explode future timer rule
							if (cTimer.GetEndTime()   + cTimer.GetMarginEnd()*0    >= time(NULL) - settings->GetSchedPoll())
								this->ScheduleTimer(cTimer.Timer());									
						}
					}
					
					// completed timers
					if (cTimer.GetState() == PVR_TIMER_STATE_ERROR    )
					{
						// start & stop recording for scheduled timers
						if (cTimer.GetTimerType() == TIMER_ONCE_MANUAL || cTimer.GetTimerType() == TIMER_ONCE_EPG)
						{
							// call delete timer if error and missed
							if (cTimer.GetEndTime()   + cTimer.GetMarginEnd()*0    < time(NULL) - settings->GetSchedPoll())
								this->DeleteTimer(cTimer.Timer());
							
							// call start recording to retry start time is now
							if (cTimer.GetStartTime() - cTimer.GetMarginStart()*60 < time(NULL) - settings->GetSchedPoll())
								this->StartTimer(cTimer.Timer());	
						}
					}
					  
					// recording timers
					if (cTimer.GetState() == PVR_TIMER_STATE_RECORDING)
					{
						// start & stop recording for scheduled timers
						if (cTimer.GetTimerType() == TIMER_ONCE_MANUAL || cTimer.GetTimerType() == TIMER_ONCE_EPG)
						{
							// call to delete timer if recording over
							if (cTimer.GetEndTime()   + cTimer.GetMarginEnd()*60   < time(NULL) + settings->GetSchedPoll())
								this->StopTimer(cTimer.Timer());
						}
					}
				}
				
				// reload M3U if enabled
				if (settings->GetM3URefresh())
				{
					if (LastM3URead() + ((SECONDS_IN_DAY)*((settings->GetM3URefresh()) == 2 ? 7 : 1)) < time(NULL))
					{
						// log reload
						XBMC->Log(LOG_NOTICE, "C+: %s - The M3U refresh interval has passed, proceed to import", __FUNCTION__);
						  
						// clear data
						SetLock();
						ClearChannels();
						ClearChannelGroups();
						ClearChannelGroupMembers();
						SetUnlock();
						
						// reload playlist						
						ImportM3U();
					}
				}

				// reload EPG if enabled
				if (settings->GetEPGRefresh())
				{
					if (LastEPGRead() + ((SECONDS_IN_DAY)*((settings->GetEPGRefresh()) == 2 ? 7 : 1)) < time(NULL))
					{
						// log reload
						XBMC->Log(LOG_NOTICE, "C+: %s - The EPG refresh interval has passed, proceed to import", __FUNCTION__);
						  
						// clear data
						SetLock();
						ClearGuideChannels();
						ClearGuideEntries();
						SetUnlock();
						
						// reload programming guide
						ImportXMLTV();
					}
				}
			}
		}
		else
		{
			// log if disconnect
			if (IsConnected())
			{
				// notify on screen
				PVR->ConnectionStateChange("Disconnected", PVR_CONNECTION_STATE_DISCONNECTED, "DVR disconnected");
				
				// send message to log
				XBMC->Log(LOG_ERROR, "C+: %s - The SQL connection lost access to database [%s]", __FUNCTION__, strDBPath.c_str());
			}
			
			// set connection
			bIsConnected = false;
		}
	}

	// iterate through timers and stop all recordings
	sqlTimers.clear();

	// read current timers
	sqlTimers = GetRecords("Timers");
	
	// iterate through timers and stop all rec
	for (vector<SQLRecord>::iterator sqlTimer = sqlTimers.begin(); sqlTimer != sqlTimers.end(); sqlTimer++)
	{	
		// convert to pvr timer
		DVRTimer cTimer(sqlTimer->GetRecord());
					
		// recording timers
		if (cTimer.GetState() == PVR_TIMER_STATE_RECORDING)
		{
			// stop recording for scheduled timers
			if (cTimer.GetTimerType() == TIMER_ONCE_MANUAL || cTimer.GetTimerType() == TIMER_ONCE_EPG)
			{
				// call to delete timer if recording (program exit)
				this->StopTimer(cTimer.Timer());
			}
		}
	}	

	// clear local containers
	sqlTimers.clear();

	// end thread work
	bIsWorking = false;

	// log termination of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Closed SQL connection", __FUNCTION__);

	// return completion
	return NULL;
}

/***********************************************************
 * Create Tables Definitions
 ***********************************************************/
bool SQLConnection::CreateChannels(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create channels table syntax
	strQuery = string("CREATE TABLE Channels(                                                                                  ") +
			   string("iUniqueId         INT                                                              PRIMARY KEY NOT NULL,") + 
			   string("bIsRadio          CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH        ) + string(")                     ,") + 
			   string("iChannelNumber    INT                                                                                  ,") + 
			   string("iSubChannelNumber INT                                                                                  ,") + 
			   string("strChannelName    CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                     ,") + 
			   string("strInputFormat    CHAR(") + itos(PVR_ADDON_INPUT_FORMAT_STRING_LENGTH) + string(")                     ,") + 
			   string("iEncryptionSystem INT                                                                                  ,") + 
			   string("strIconPath       CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH         ) + string(")                     ,") + 
			   string("bIsHidden         CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH        ) + string(")                     ,") + 
			   string("strGroupName      CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                     ,") + 
			   string("strStreamURL      CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH         ) + string(")                     ,") + 
			   string("strTvgId          CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                     ,") + 
			   string("strTvgName        CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                     ,") + 
			   string("strTvgLogo        CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH         ) + string(")                     ,") + 
			   string("iTvgShift         FLOAT                                                                                )") ;

	// send query to create channels table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateChannelGroups(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create channel groups table syntax
	strQuery = string("CREATE TABLE ChannelGroups(                                                                ") +
			   string("strGroupName CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH) + string(") PRIMARY KEY NOT NULL,") + 
			   string("bIsRadio     CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                     ,") + 
			   string("iPosition    INT                                                                          )") ; 
			   
	// send query to create channel groups table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateChannelGroupMembers(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create channel group members table syntax
	strQuery = string("CREATE TABLE ChannelGroupMembers(                                                                                                           ") +
			   string("           strGroupName      CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                                              NOT NULL,") + 
			   string("           iChannelUniqueId  INT                                                                                                   NOT NULL,") + 
			   string("           iChannelNumber    INT                                                                                                           ,") + 
			   string("           iSubChannelNumber INT                                                                                                           ,") + 
			   string("           bIsRadio          CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                                                      ,") +  
			   string("CONSTRAINT sqlMemberId                                                                PRIMARY KEY (strGroupName, iChannelUniqueId)         )") ;
			   
	// send query to create channel group members table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateGuideChannels(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create guide channels table syntax
	strQuery = string("CREATE TABLE EpgChannels(                                                                                                        ") +
			   string("           strTvgId          CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                           NOT NULL,") + 
			   string("           strTvgName        CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH        ) + string(")                           NOT NULL,") + 
			   string("           strTvgLogo        CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH         ) + string(")                           NOT NULL,") + 
			   string("CONSTRAINT sqlChannelId                                                                       PRIMARY KEY (strTvgId, strTvgName))") ;			   
			   
	// send query to create guide channels table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateGuideEntries(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create guide table syntax
	strQuery = string("CREATE TABLE EpgEntries(                                                                  ") +
			   string("           iUniqueBroadcastId  INT                                               NOT NULL,") + 
			   string("           iUniqueChannelId    INT                                               NOT NULL,") + 
			   string("           strTitle            VARCHAR                                                   ,") + 
			   string("           startTime           VARCHAR                                                   ,") + 
			   string("           endTime             VARCHAR                                                   ,") + 
			   string("           strPlotOutline      VARCHAR                                                   ,") + 
			   string("           strPlot             VARCHAR                                                   ,") + 
			   string("           strOriginalTitle    VARCHAR                                                   ,") + 
			   string("           strCast             VARCHAR                                                   ,") + 
			   string("           strDirector         VARCHAR                                                   ,") + 
			   string("           strWriter           VARCHAR                                                   ,") + 
			   string("           iYear               INT                                                       ,") + 
			   string("           strIMDBNumber       VARCHAR                                                   ,") + 
			   string("           strIconPath         VARCHAR                                                   ,") + 
			   string("           iGenreType          INT                                                       ,") + 
			   string("           iGenreSubType       INT                                                       ,") + 
			   string("           strGenreDescription VARCHAR                                                   ,") + 
			   string("           firstAired          VARCHAR                                                   ,") + 
			   string("           iParentalRating     INT                                                       ,") + 
			   string("           iStarRating         INT                                                       ,") + 
			   string("           bNotify             VARCHAR                                                   ,") + 
			   string("           iSeriesNumber       INT                                                       ,") + 
			   string("           iEpisodeNumber      INT                                                       ,") + 
			   string("           iEpisodePartNumber  INT                                                       ,") + 
			   string("           strEpisodeName      VARCHAR                                                   ,") + 
			   string("           iFlags              INT                                                       ,") + 
			   string("           strSeriesLink       VARCHAR                                                   ,") + 
			   string("           strTvgId            VARCHAR                                                   ,") + 
			   string("CONSTRAINT sqlGuideId                  PRIMARY KEY (iUniqueBroadcastId, iUniqueChannelId))") ;			   
			   
	// send query to create guide table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateTimerTypes(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create timer types table syntax
	strQuery = string("CREATE TABLE TimerTypes(                                                                                                  ") +
			   string("iId                              INT                                                                 PRIMARY KEY NOT NULL,") + 
			   string("iAttributes                      INT                                                                                     ,") + 
			   string("strDescription                   CHAR(") + itos(PVR_ADDON_TIMERTYPE_STRING_LENGTH      ) + string(")                     ,") + 
			   string("iPrioritiesSize                  INT                                                                                     ,") + 
			   string("priorities                       CHAR(") + itos(PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH) + string(")                     ,") + 
			   string("iPrioritiesDefault               INT                                                                                     ,") + 
			   string("iLifetimesSize                   INT                                                                                     ,") + 
			   string("lifetimes                        CHAR(") + itos(PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH) + string(")                     ,") + 
			   string("iLifetimesDefault                INT                                                                                     ,") + 
			   string("iPreventDuplicateEpisodesSize    INT                                                                                     ,") + 
			   string("preventDuplicateEpisodes         CHAR(") + itos(PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH) + string(")                     ,") + 
			   string("iPreventDuplicateEpisodesDefault INT                                                                                     ,") + 
			   string("iRecordingGroupSize              INT                                                                                     ,") + 
			   string("recordingGroup                   CHAR(") + itos(PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH) + string(")                     ,") + 
			   string("iRecordingGroupDefault           INT                                                                                     ,") + 
			   string("iMaxRecordingsSize               INT                                                                                     ,") + 
			   string("maxRecordings                    CHAR(") + itos(PVR_ADDON_TIMERTYPE_VALUES_ARRAY_LENGTH) + string(")                     ,") + 
			   string("iMaxRecordingsDefault            INT                                                                                     )") ; 

	// send query to create timer types table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateTimers(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create timers table syntax
	strQuery = string("CREATE TABLE Timers(                                                                                   ") +
			   string("iClientIndex              INT                                                     PRIMARY KEY NOT NULL,") + 
			   string("iParentClientIndex        INT                                                                         ,") + 
			   string("iClientChannelUid         INT                                                                         ,") + 
			   string("startTime                 VARCHAR                                                                     ,") + 
			   string("endTime                   VARCHAR                                                                     ,") + 
			   string("bStartAnyTime             CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                    ,") + 
			   string("bEndAnyTime               CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                    ,") + 
			   string("state                     INT                                                                         ,") + 
			   string("iTimerType                INT                                                                         ,") + 
			   string("strTitle                  CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                    ,") + 
			   string("strEpgSearchString        CHAR(") + itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                    ,") + 
			   string("bFullTextEpgSearch        CHAR(") + itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                    ,") + 
			   string("strDirectory              CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                    ,") + 
			   string("strSummary                CHAR(") + itos(PVR_ADDON_DESC_STRING_LENGTH) + string(")                    ,") + 
			   string("iPriority                 INT                                                                         ,") + 
			   string("iLifetime                 INT                                                                         ,") + 
			   string("iMaxRecordings            INT                                                                         ,") + 
			   string("iRecordingGroup           INT                                                                         ,") + 
			   string("firstDay                  VARCHAR                                                                     ,") + 
			   string("iWeekdays                 INT                                                                         ,") + 
			   string("iPreventDuplicateEpisodes INT                                                                         ,") + 
			   string("iEpgUid                   INT                                                                         ,") + 
			   string("iMarginStart              INT                                                                         ,") + 
			   string("iMarginEnd                INT                                                                         ,") + 
			   string("iGenreType                INT                                                                         ,") + 
			   string("iGenreSubType             INT                                                                         ,") + 
			   string("strSeriesLink             CHAR(") + itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                    )") ;
			
	// send query to create timers table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

bool SQLConnection::CreateRecordings(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	int    iResponse;
	
	// create recordings table syntax
	strQuery = string("CREATE TABLE Recordings(                                                                         ") +
			   string("strRecordingId      CHAR(")+ itos(PVR_ADDON_NAME_STRING_LENGTH) + string(") PRIMARY KEY NOT NULL,") +
			   string("strTitle            CHAR(")+ itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                     ,") +
			   string("strEpisodeName      CHAR(")+ itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                     ,") +
			   string("iSeriesNumber       INT                                                                         ,") +
			   string("iEpisodeNumber      INT                                                                         ,") +
			   string("iYear               INT                                                                         ,") +
			   string("strDirectory        CHAR(")+ itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                     ,") +
			   string("strPlotOutline      CHAR(")+ itos(PVR_ADDON_DESC_STRING_LENGTH) + string(")                     ,") +
			   string("strPlot             CHAR(")+ itos(PVR_ADDON_DESC_STRING_LENGTH) + string(")                     ,") +
			   string("strGenreDescription CHAR(")+ itos(PVR_ADDON_DESC_STRING_LENGTH) + string(")                     ,") +
			   string("strChannelName      CHAR(")+ itos(PVR_ADDON_NAME_STRING_LENGTH) + string(")                     ,") +
			   string("strIconPath         CHAR(")+ itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                     ,") +
			   string("strThumbnailPath    CHAR(")+ itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                     ,") +
			   string("strFanartPath       CHAR(")+ itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                     ,") +
			   string("recordingTime       VARCHAR                                                                     ,") +
			   string("iDuration           INT                                                                         ,") +
			   string("iPriority           INT                                                                         ,") +
			   string("iLifetime           INT                                                                         ,") +
			   string("iGenreType          INT                                                                         ,") +
			   string("iGenreSubType       INT                                                                         ,") +
			   string("iPlayCount          INT                                                                         ,") +
			   string("iLastPlayedPosition INT                                                                         ,") +
			   string("bIsDeleted          CHAR(")+ itos(PVR_ADDON_BOOL_STRING_LENGTH) + string(")                     ,") +
			   string("iEpgEventId         INT                                                                         ,") +
			   string("iChannelUid         INT                                                                         ,") +
			   string("channelType         INT                                                                         ,") +
			   string("strFileName         CHAR(")+ itos(PVR_ADDON_URL_STRING_LENGTH ) + string(")                     )") ;
					
	// send query to create recordings table
	iResponse = SendQuery(strQuery.c_str(), NULL);
	
	// return value
	return (iResponse == SQLITE_OK);
}

/***********************************************************
 * Clear & Clear Tables Definitions
 ***********************************************************/
void SQLConnection::ClearChannels(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create channels clear syntax
	strQuery = string("DELETE FROM Channels") ;

	// send query to clear channels table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::ClearChannelGroups(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create channel groups clear syntax
	strQuery = string("DELETE FROM ChannelGroups") ;

	// send query to clear channel groups table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::ClearChannelGroupMembers(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create channel group members clear syntax
	strQuery = string("DELETE FROM ChannelGroupMembers") ;

	// send query to clear channel group members table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::ClearGuideChannels(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create guide channels clear syntax
	strQuery = string("DELETE FROM EpgChannels") ;

	// send query to clear guide channels table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::ClearGuideEntries(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create guide clear syntax
	strQuery = string("DELETE FROM EpgEntries") ;

	// send query to clear guide table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::ClearTimerTypes(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create timer types clear syntax
	strQuery = string("DELETE FROM TimerTypes") ;

	// send query to clear timer types table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::CleanTimers(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create timer types clear syntax
	strQuery = string("DELETE FROM Timers WHERE state = ") + itos((int) PVR_TIMER_STATE_ABORTED) + " OR state = " + itos((int) PVR_TIMER_STATE_CANCELLED) + " OR state = " + itos((int) PVR_TIMER_STATE_COMPLETED);

	// send query to clear timer types table
	SendQuery(strQuery.c_str(), NULL);
}

void SQLConnection::CleanRecordings(void)
{
	// log function call
	CPPLog(); 
	
	// create query container
	string strQuery;
	
	// create recording types clear syntax
	strQuery = string("DELETE FROM Recordings WHERE bIsDeleted = '") + btos(true) + "'";

	// send query to clear recording types table
	SendQuery(strQuery.c_str(), NULL);
}

/***********************************************************
 * Load External Files Definitions
 ***********************************************************/
void SQLConnection::ImportM3U(void)
{
	// log function call
	CPPLog();

	// open the M3U file via kodi
	void *fileHandle = XBMC->OpenFile(settings->GetM3UPath().c_str(), 0);

	// check if file could be open otherwise return error
	if (!fileHandle)
	{
		// log issue
		XBMC->Log(LOG_ERROR, "C+: %s - No M3U file found in path, skipping import", __FUNCTION__);

		// return error
		return;
	}

	// read in file to memory
	char   buffer[1024];
	string content     ;

	while (int bytes = XBMC->ReadFile(fileHandle, buffer, 1024))
	{
		if (bytes) content.append(buffer, bytes);
	}

	// close file
	XBMC->CloseFile(fileHandle);

	// remove bad data from start of file if present
	if (StringUtils::Left(content, 3) == "\xEF\xBB\xBF")
		content.erase(0, 3);

	// split into lines
	vector<string> pLines = StringUtils::Split(content, "\n");

	// check if valid M3U file
	if (!StringUtils::StartsWith(pLines[0], M3U_START_MARKER))
	{
		// log issue
		XBMC->Log(LOG_ERROR, "C+: %s - Not a valid M3U file, skipping import", __FUNCTION__);

		// return error
		return;
	}

	// create string list for groups
	string channel_group_list;

	// create containers for channels parsed text
	int    iUniqueId         = PVR_CHANNEL_INVALID_UID     ;
	bool   bIsRadio          = false                       ;
	int    iChannelNumber    = settings->GetM3UStartNum()-1;
	int    iTvgChannelNo     = 0                           ;
	int    iSubChannelNumber = 0                           ;
	string strChannelName    = ""                          ;
	string strInputFormat    = ""                          ;
	int    iEncryptionSystem = 0                           ;
	string strIconPath       = ""                          ;
	bool   bIsHidden         = false                       ;
	string strGroupName      = ""                          ;
	string strStreamURL      = ""                          ;
	string strTvgId          = ""                          ;
	string strTvgName        = ""                          ;
	string strTvgLogo        = ""                          ;
	float  iTvgShift         = 0.0                         ;
	
	// create containers for channel groups parsed text
	int iPosition = 0;
	
	// start transaction
	sqlite3_exec(sqlDatabase, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	
	// go through the split lines
	for (vector<string>::iterator pLine = pLines.begin()+1; pLine != pLines.end(); pLine++)
	{
		// parse the M3U file
		if (pLine->size() > 1)
		{
			// point to line
			string pLineNode = *pLine;

			// read marker values
			if (StringUtils::StartsWith(pLineNode, M3U_INFO_MARKER))
			{
				// get attributes from line
				iUniqueId      =                           stoh(GetDisplayName    (pLineNode                                 ));
				bIsRadio       =                           stob(ReadM3UMarkerValue(pLineNode, M3U_RADIO_MARKER     ,  "false"));
				iChannelNumber++                                                                                               ;
				iTvgChannelNo  =                           stoi(ReadM3UMarkerValue(pLineNode, TVG_INFO_CHNO_MARKER ,      "0"));
				strChannelName =                                GetDisplayName    (pLineNode                                 ) ;
				strIconPath    = settings->GetLogoPath() +      ReadM3UMarkerValue(pLineNode, TVG_INFO_LOGO_MARKER ,      "" ) ;
				bIsHidden      =                           stob(ReadM3UMarkerValue(pLineNode, M3U_HIDDEN_MARKER    ,  "false"));
				strGroupName   =                                ReadM3UMarkerValue(pLineNode, M3U_GROUP_NAME_MARKER,      "" ) ;
				strTvgId       =                                ReadM3UMarkerValue(pLineNode, TVG_INFO_ID_MARKER   ,      "" ) ;
				strTvgName     =                                ReadM3UMarkerValue(pLineNode, TVG_INFO_NAME_MARKER ,      "" ) ;
				strTvgLogo     =                                ReadM3UMarkerValue(pLineNode, TVG_INFO_LOGO_MARKER ,      "" ) ;
				iTvgShift      =                           stof(ReadM3UMarkerValue(pLineNode, TVG_INFO_SHIFT_MARKER,   "0.0" ));
				
				// add channel group
				if ((int)channel_group_list.find(((bIsRadio ? string("Radio:") : string("TV:")) + strGroupName).c_str()) < 0)
				{
					// create sql ontainer
					string sqlChannelGroup = string("(strGroupName, bIsRadio, iPosition   )") +
					                         string(" VALUES ") +  
					                         string("('") + StringUtils_Replace(    (strGroupName),"'", "''") + string("',") +
											 string(" '") + StringUtils_Replace(btos(bIsRadio    ),"'", "''") + string("',") +
											 string("  ") + StringUtils_Replace(itos(iPosition++ ),"'", "''") + string(" )") ;										
					
					// push to database
					AddRecord("ChannelGroups", sqlChannelGroup);
	
					// log addition
					XBMC->Log(LOG_DEBUG, "C+: %s - Added channel group %s (%s)", __FUNCTION__, strGroupName.c_str(), (bIsRadio ? string("Radio") : string("TV")).c_str());

					// add to list of groups
					channel_group_list.append((string("{") + (bIsRadio ? string("Radio:") : string("TV:")) + strGroupName + string("}")).c_str());
				}
				
				// add channel to group if present
				if (strGroupName != "")
				{				
					// create sql ontainer
					string sqlChannelGroupMember = string("(strGroupName, iChannelUniqueId, iChannelNumber, iSubChannelNumber, bIsRadio)") +
					                               string(" VALUES ") +  
					                               string("('") + StringUtils_Replace(    (strGroupName     ),"'", "''") + string("', ") +
					                               string("  ") + StringUtils_Replace(itos(iUniqueId        ),"'", "''") + string(" , ") +
					                               string("  ") + StringUtils_Replace(itos(iChannelNumber   ),"'", "''") + string(" , ") +
					                               string("  ") + StringUtils_Replace(itos(iSubChannelNumber),"'", "''") + string(" , ") +
					                               string(" '") + StringUtils_Replace(btos(bIsRadio         ),"'", "''") + string("') ") ;

					// push to database
					AddRecord("ChannelGroupMembers", sqlChannelGroupMember);
	
					// log addition
					XBMC->Log(LOG_DEBUG, "C+: %s - Added to channel group %s (Ch#%i)", __FUNCTION__, strGroupName.c_str(), iChannelNumber);
				}
			}
			else if (!StringUtils::StartsWith(pLineNode, M3U_START_MARKER))
			{
				// get url from line
				strStreamURL = StringUtils::Trim(pLineNode);
				
				// add if url present
				if (strStreamURL.substr(0, 1) != "" && strStreamURL.substr(0, 1) != "#")
				{
					// create sql ontainer
					string sqlChannel  = string("(iUniqueId     , bIsRadio      , iChannelNumber   , iSubChannelNumber   ,") +
					                     string(" strChannelName, strInputFormat, iEncryptionSystem, strIconPath         ,") +
					                     string(" bIsHidden     , strGroupName  , strStreamURL     , strTvgId            ,") +
					                     string(" strTvgName    , strTvgLogo    , iTvgShift                              )") +
					                     string(" VALUES ") +  
					                     string("( ") + StringUtils_Replace(itos(iUniqueId                                     ),"'", "''") + string(" , ") +
					                     string(" '") + StringUtils_Replace(btos(bIsRadio                                      ),"'", "''") + string("', ") +
					                     string("  ") + StringUtils_Replace(itos(iTvgChannelNo ? iTvgChannelNo : iChannelNumber),"'", "''") + string(" , ") +
					                     string("  ") + StringUtils_Replace(itos(iSubChannelNumber                             ),"'", "''") + string(" , ") +
					                     string(" '") + StringUtils_Replace(    (strChannelName                                ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strInputFormat                                ),"'", "''") + string("', ") +
					                     string("  ") + StringUtils_Replace(itos(iEncryptionSystem                             ),"'", "''") + string(" , ") +
					                     string(" '") + StringUtils_Replace(    (strIconPath                                   ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(btos(bIsHidden                                     ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strGroupName                                  ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strStreamURL                                  ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strTvgId                                      ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strTvgName                                    ),"'", "''") + string("', ") +
					                     string(" '") + StringUtils_Replace(    (strTvgLogo                                    ),"'", "''") + string("', ") +
					                     string("  ") + StringUtils_Replace(ftos(iTvgShift                                     ),"'", "''") + string(" ) ") ;

					// push to database
					AddRecord("Channels", sqlChannel);
	
					// log addition
					XBMC->Log(LOG_DEBUG, "C+: %s - Added channel #%i (%s)", __FUNCTION__, iTvgChannelNo ? iTvgChannelNo : iChannelNumber, strChannelName.c_str());
				}
			}
		}
	}
	
	// end transaction
	sqlite3_exec(sqlDatabase, "END TRANSACTION;", NULL, NULL, NULL);

	// log channels imported
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channels imported", __FUNCTION__, GetTableSize("Channels"));

	// log channel groups imported
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channel groups imported", __FUNCTION__, GetTableSize("ChannelGroups"));
	
	// log channel group members imported
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channel groups members imported", __FUNCTION__, GetTableSize("ChannelGroupMembers"));
	
	// get read time of file
	tLastM3URead = time(NULL);
	
	// return no issue
	return;	
}

void SQLConnection::ImportXMLTV(void)
{
	// log function call
	CPPLog(); 

	// open the EPG file via kodi
	void *fileHandle = XBMC->OpenFile(settings->GetEPGPath().c_str(), 0);

	// check if file could be open otherwise return error
	if (!fileHandle)
	{
		// log issue
		XBMC->Log(LOG_ERROR, "C+: %s - No XMLTV file found in EPG path, skipping import", __FUNCTION__);

		// return error
		return;
	}

	// read in file to memory
	char   buffer[1024];
	string content     ;

	while (int bytes = XBMC->ReadFile(fileHandle, buffer, 1024))
	{
		if (bytes) content.append(buffer, bytes);
	}

	// close file
	XBMC->CloseFile(fileHandle);
	
	// check if a compressed archive
	if (StringUtils::Left(content, 3) == "\x1F\x8B\x08")
	{
		// create container cor decompression
		string decompressed;
		
		// attempt to decompress using zlib
		if (!GzipInflate(content, decompressed))
		{
			XBMC->Log(LOG_ERROR, "C+: %s - Invalid EPG file unable to decompress file (%s)", __FUNCTION__, settings->GetEPGPath().c_str());
			return;
		}
		
		// assign decompressed data back to content
		content = decompressed;
	}

	// convert content to charr array
	char* pLines = new char [content.length() + 1];
	strcpy(pLines, content.c_str());

	// create xml doc
	xml_document<> xml;

	// parse xml doc
	try
	{
		xml.parse<0>(pLines);
	}
	catch (parse_error err)
	{
		// log issue
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to parse XMLTV file, %s", __FUNCTION__, err.what());

		// return error
		return;
	}

	// get xml root
	xml_node<> *pRootNode = xml.first_node(XMLTV_START_MARKER);

	// check if valid EPG file
	if (!pRootNode)
	{
		// log issue
		XBMC->Log(LOG_ERROR, "C+: %s - Not a valid XMLTV file, skipping import", __FUNCTION__);

		// return error
		return;
	}
	
	// create containers for parsed text for epg channels
	string strTvgId   = "";
	string strTvgName = "";
	string strTvgLogo = "";	
	
	// create container to look for channel
	SQLRecord sqlEpgChannel;
	
	// start transaction
	sqlite3_exec(sqlDatabase, "BEGIN TRANSACTION;", NULL, NULL, NULL);

	// read in epg channels
	for (xml_node<> *pChannelNode = pRootNode->first_node(XMLTV_CHANNEL_MARKER); pChannelNode; pChannelNode = pChannelNode->next_sibling(XMLTV_CHANNEL_MARKER))
	{	
		// get attributes from xml node (ignore display name, pure tvg id)
		strTvgId   = GetXMLAttributeValue     (pChannelNode, XMLTV_CHANNEL_ID_MARKER ,                         "");
		strTvgName =                                                                   EPG_CHANNEL_NO_NAME        ;
		strTvgLogo = GetXMLChildAttributeValue(pChannelNode, XMLTV_ICON_MARKER       , XMLTV_SOURCE_MARKER,    ""); 
		
		// add unique
		if (!FindRecord("EpgChannels", "strTvgId", strTvgId.c_str(), sqlEpgChannel))
		{
			// create sql ontainer
			string sqlEpgChannel  = string("(strTvgId, strTvgName, strTvgLogo)") +
								    string(" VALUES ") +  
								    string("('") + StringUtils_Replace((strTvgId  ),"'", "''") + string("', ") +
								    string(" '") + StringUtils_Replace((strTvgName),"'", "''") + string("', ") +
								    string(" '") + StringUtils_Replace((strTvgLogo),"'", "''") + string("') ") ;
										 		
			// push to database
			AddRecord("EpgChannels", sqlEpgChannel);

			// log addition
			XBMC->Log(LOG_DEBUG, "C+: %s - Added guide channel %s (%s)", __FUNCTION__, strTvgId.c_str(), strTvgName.c_str());
		}
		
		// go through display names and add
		for (xml_node<> *pDisplayNameNode = pChannelNode->first_node(XMLTV_CHANNEL_NAME_MARKER); pDisplayNameNode; pDisplayNameNode = pDisplayNameNode->next_sibling(XMLTV_CHANNEL_NAME_MARKER))
		{
			// get attributes from xml node (ignore display name, pure tvg id)
			strTvgName = pDisplayNameNode->value();
			
			// add unique
			if (!FindRecord("EpgChannels", "strTvgName", strTvgName.c_str(), sqlEpgChannel))
			{
				// create sql ontainer
				string sqlEpgChannel  = string("(strTvgId, strTvgName, strTvgLogo)") +
									    string(" VALUES ") +  
									    string("('") + StringUtils_Replace((strTvgId  ),"'", "''") + string("', ") +
									    string(" '") + StringUtils_Replace((strTvgName),"'", "''") + string("', ") +
									    string(" '") + StringUtils_Replace((strTvgLogo),"'", "''") + string("') ") ;
													
				// push to database
				AddRecord("EpgChannels", sqlEpgChannel);

				// log addition
				XBMC->Log(LOG_DEBUG, "C+: %s - Added guide channel %s (%s)", __FUNCTION__, strTvgId.c_str(), strTvgName.c_str());
			}			
		}
	}
	
	// end transaction
	sqlite3_exec(sqlDatabase, "END TRANSACTION;", NULL, NULL, NULL);
	
	// create containers for parsed text for epg entries
	int    iUniqueBroadcastId  = EPG_TAG_INVALID_UID           ;
	int    iUniqueChannelId    = PVR_CHANNEL_INVALID_UID       ;
	string strTitle            = ""                            ;
	time_t startTime           = 0                             ;
	time_t endTime             = 0                             ;
	string strPlotOutline      = ""                            ;
	string strPlot             = ""                            ;
	string strOriginalTitle    = ""                            ;
	string strCast             = ""                            ;
	string strDirector         = ""                            ;
	string strWriter           = ""                            ;
	int    iYear               = 0                             ;
	string strIMDBNumber       = ""                            ;
	string strIconPath         = ""                            ;
	int    iGenreType          = EPG_GENRE_USE_STRING          ;
	int    iGenreSubType       = 0                             ;
	string strGenreDescription = ""                            ;
	time_t firstAired          = 0                             ;
	int    iParentalRating     = 0                             ;
	int    iStarRating         = 0                             ;
	bool   bNotify             = false                         ;
	int    iSeriesNumber       = EPG_TAG_INVALID_SERIES_EPISODE;
	int    iEpisodeNumber      = EPG_TAG_INVALID_SERIES_EPISODE;
	int    iEpisodePartNumber  = EPG_TAG_INVALID_SERIES_EPISODE;
	string strEpisodeName      = ""                            ;
	int    iFlags              = EPG_TAG_FLAG_UNDEFINED        ;
	string strSeriesLink       = ""                            ;
	  
	// create container for epg days
	time_t startEPG = time(NULL);
	time_t endEPG   = time(NULL);

	// create container to look for channel
	SQLRecord sqlChannel;
	
	// start transaction
	sqlite3_exec(sqlDatabase, "BEGIN TRANSACTION;", NULL, NULL, NULL);

	// read in epg
	for (xml_node<> *pProgrammeNode = pRootNode->first_node(XMLTV_PROGRAM_MARKER); pProgrammeNode; pProgrammeNode = pProgrammeNode->next_sibling(XMLTV_PROGRAM_MARKER))
	{	
		// get attributes from xml node
		iUniqueBroadcastId  = ParseXMLDateTime  (GetXMLAttributeValue     (pProgrammeNode, XMLTV_STARTTM_MARKER    ,                                                                                       "0"));
		iUniqueChannelId++                                                                                                                                                                                      ;
		strTitle            =                    GetXMLNodeValue          (pProgrammeNode, XMLTV_TITLE_MARKER      , ""                                                    ,                                "") ;
		startTime           = ParseXMLDateTime  (GetXMLAttributeValue     (pProgrammeNode, XMLTV_STARTTM_MARKER    ,                                                                                       "0"));
		endTime             = ParseXMLDateTime  (GetXMLAttributeValue     (pProgrammeNode, XMLTV_STOPTM_MARKER     ,                                                                                       "0"));
		strPlot             =                    GetXMLNodeValue          (pProgrammeNode, XMLTV_PLOT_MARKER       , ""                                                    ,                                "") ;
		strCast             =                    GetXMLChildNodeValue     (pProgrammeNode, XMLTV_CREDITS_MARKER    , XMLTV_ACTOR_MARKER                                    , EPG_STRING_TOKEN_SEPARATOR,    "") ;
		strDirector         =                    GetXMLChildNodeValue     (pProgrammeNode, XMLTV_CREDITS_MARKER    , XMLTV_DIRECTOR_MARKER                                 , EPG_STRING_TOKEN_SEPARATOR,    "") ;
		strWriter           =                    GetXMLChildNodeValue     (pProgrammeNode, XMLTV_CREDITS_MARKER    , XMLTV_WRITER_MARKER                                   , EPG_STRING_TOKEN_SEPARATOR,    "") ;
		iYear               = ParseXMLYear      (GetXMLNodeValue          (pProgrammeNode, XMLTV_DATE_MARKER       , ""                                                    ,                               "0"));
		strIconPath         =                    GetXMLChildAttributeValue(pProgrammeNode, XMLTV_ICON_MARKER       , XMLTV_SOURCE_MARKER                                   ,                                "") ;
		strGenreDescription =                    GetXMLNodeValue          (pProgrammeNode, XMLTV_GENRE_MARKER      , EPG_STRING_TOKEN_SEPARATOR                            ,                                "") ;
		firstAired          = ParseXMLDate      (GetXMLNodeValue          (pProgrammeNode, XMLTV_DATE_MARKER       , ""                                                    ,                               "0"));
		iStarRating         = ParseXMLStarRating(GetXMLChildNodeValue     (pProgrammeNode, XMLTV_STAR_RATING_MARKER, XMLTV_STAR_RATING_VALUE                               , EPG_STRING_TOKEN_SEPARATOR,    ""));
		iSeriesNumber       = ParseXMLSeason    (GetXMLNodeValueAttr      (pProgrammeNode, XMLTV_SEASON_MARKER     , XMLTV_SEASON_ATTRIB_MARKER, XMLTV_SEASON_ATTRIB_VALUE , EPG_STRING_TOKEN_SEPARATOR,   "0"));
		iEpisodeNumber      = ParseXMLEpisode   (GetXMLNodeValueAttr      (pProgrammeNode, XMLTV_SEASON_MARKER     , XMLTV_SEASON_ATTRIB_MARKER, XMLTV_SEASON_ATTRIB_VALUE , EPG_STRING_TOKEN_SEPARATOR,   "0"));
		iEpisodePartNumber  = ParseXMLEpPart    (GetXMLNodeValueAttr      (pProgrammeNode, XMLTV_SEASON_MARKER     , XMLTV_SEASON_ATTRIB_MARKER, XMLTV_SEASON_ATTRIB_VALUE , EPG_STRING_TOKEN_SEPARATOR,   "0"));
		strEpisodeName      =                    GetXMLNodeValue          (pProgrammeNode, XMLTV_EPISODE_MARKER    , ""                                                    ,                                "") ;
		iFlags              =                                                              EPG_TAG_FLAG_UNDEFINED                                                                                               ;
		strSeriesLink       = ParseXMLSeries    (GetXMLNodeValueAttr      (pProgrammeNode, XMLTV_SEASON_MARKER     , XMLTV_SEASON_ATTRIB_MARKER, XMLTV_SERIES_ATTRIB_VALUE , EPG_STRING_TOKEN_SEPARATOR,    ""));
		strTvgId            =                    GetXMLAttributeValue     (pProgrammeNode, XMLTV_CHANNEL_MARKER    ,                                                                                        "") ;

		// look for channel in database (skip if not found)
		if (!FindRecord("EpgChannels", "strTvgId", strTvgId.c_str(), sqlChannel))
			continue;
		
		// check if series add flag
		if (strSeriesLink != "")
			iFlags |= EPG_TAG_FLAG_IS_SERIES;
		/*
		// check if new and add flag
		if (GetDate(startTime) = GetDate(firstAired))
			iFlags |= EPG_TAG_FLAG_IS_NEW;
		*/
		// create sql ontainer
		string sqlEpgEntry = string("(iUniqueBroadcastId, iUniqueChannelId   , strTitle        , startTime         , endTime       ,") + 
							 string(" strPlotOutline    , strPlot            , strOriginalTitle, strCast           , strDirector   ,") +
							 string(" strWriter         , iYear              , strIMDBNumber   , strIconPath       , iGenreType    ,") +
							 string(" iGenreSubType     , strGenreDescription, firstAired      , iParentalRating   , iStarRating   ,") +
							 string(" bNotify           , iSeriesNumber      , iEpisodeNumber  , iEpisodePartNumber, strEpisodeName,") +
							 string(" iFlags            , strSeriesLink      , strTvgId                                            )") +
							 string(" VALUES ") +  
							 string("( ") + StringUtils_Replace(itos(iUniqueBroadcastId ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iUniqueChannelId   ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strTitle           ),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(startTime          ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(endTime            ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strPlotOutline     ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strPlot            ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strOriginalTitle   ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strCast            ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strDirector        ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strWriter          ),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(iYear              ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strIMDBNumber      ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strIconPath        ),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(iGenreType         ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iGenreSubType      ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strGenreDescription),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(firstAired         ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iParentalRating    ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iStarRating        ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(btos(bNotify            ),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(iSeriesNumber      ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iEpisodeNumber     ),"'", "''") + string(" , ") +
							 string("  ") + StringUtils_Replace(itos(iEpisodePartNumber ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strEpisodeName     ),"'", "''") + string("', ") +
							 string("  ") + StringUtils_Replace(itos(iFlags             ),"'", "''") + string(" , ") +
							 string(" '") + StringUtils_Replace(    (strSeriesLink      ),"'", "''") + string("', ") +
							 string(" '") + StringUtils_Replace(    (strTvgId           ),"'", "''") + string("') ") ;
	
		// push to database
		AddRecord("EpgEntries", sqlEpgEntry);
	
		// log addition
		XBMC->Log(LOG_DEBUG, "C+: %s - Added '%s' to guide (%s) from [%i - %i]", __FUNCTION__, strTitle.c_str(), strTvgId.c_str(), startTime, endTime);
		
		// keep track of earliest and latest EPG imported
		startEPG = (startEPG < startTime) ? startEPG : startTime;
		endEPG   = (endEPG   > endTime  ) ? endEPG   : endTime  ;
	}

	// end transaction
	sqlite3_exec(sqlDatabase, "END TRANSACTION;", NULL, NULL, NULL);

	// log epg entries imported
	XBMC->Log(LOG_NOTICE, "C+: %s - %i guide entries imported", __FUNCTION__, GetTableSize("EpgEntries"));
	  
	// get read time of file
	tLastEPGRead = time(NULL);
	  
	// set number of epg days
	iNumEPGDays = ceil((endEPG - startEPG)/SECONDS_IN_DAY);
	
	// return no issue
	return;
}

void SQLConnection::ImportTimerTypes(void)
{
	// log function call
	CPPLog();
	  
	// create query containers
	string sqlResponse, sqlCommand, sqlTimerType;
	
	// create name container
	string TIMER_NAME;
	
	// start transaction
	sqlite3_exec(sqlDatabase, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	  
	// add once manual type
	             TIMER_NAME                = "One time";
	unsigned int TIMER_ONCE_MANUAL_ATTRIBS = PVR_TIMER_TYPE_IS_MANUAL           | PVR_TIMER_TYPE_SUPPORTS_CHANNELS |
	                                         PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_END_TIME |
	                                         PVR_TIMER_TYPE_SUPPORTS_PRIORITY   | PVR_TIMER_TYPE_SUPPORTS_LIFETIME ;
											 
	sqlResponse  = "";
	sqlTimerType = string("(iId, iAttributes, strDescription, iPrioritiesSize, iLifetimesSize)") +
	               string(" VALUES ") +  
	               string("( ") + StringUtils_Replace(itos(TIMER_ONCE_MANUAL        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(TIMER_ONCE_MANUAL_ATTRIBS),"'", "''") + string(" , ") +
	               string(" '") + StringUtils_Replace(    (TIMER_NAME               ),"'", "''") + string("', ") +
	               string("  ") + StringUtils_Replace(itos(0                        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(0                        ),"'", "''") + string(" ) ") ;

	// push to database
	AddRecord("TimerTypes", sqlTimerType);
	
	// log addition
	XBMC->Log(LOG_DEBUG, "C+: %s - Added timer type %s", __FUNCTION__, TIMER_NAME.c_str());
	  
	// add once epg type
	             TIMER_NAME             = "One-time (guide-based)";
	unsigned int TIMER_ONCE_EPG_ATTRIBS = PVR_TIMER_TYPE_SUPPORTS_CHANNELS         | PVR_TIMER_TYPE_SUPPORTS_START_TIME        |
	                                      PVR_TIMER_TYPE_SUPPORTS_END_TIME         | PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE |
	                                      PVR_TIMER_TYPE_SUPPORTS_START_END_MARGIN | PVR_TIMER_TYPE_SUPPORTS_PRIORITY          |
	                                      PVR_TIMER_TYPE_SUPPORTS_LIFETIME                                                     ;
							  
	sqlResponse  = "";
	sqlTimerType = string("(iId, iAttributes, strDescription, iPrioritiesSize, iLifetimesSize)") +
	               string(" VALUES ") +  
	               string("( ") + StringUtils_Replace(itos(TIMER_ONCE_EPG           ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(TIMER_ONCE_EPG_ATTRIBS   ),"'", "''") + string(" , ") +
	               string(" '") + StringUtils_Replace(    (TIMER_NAME               ),"'", "''") + string("', ") +
	               string("  ") + StringUtils_Replace(itos(0                        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(0                        ),"'", "''") + string(" ) ") ;

	// push to database
	AddRecord("TimerTypes", sqlTimerType);
	
	// log addition
	XBMC->Log(LOG_DEBUG, "C+: %s - Added timer type %s", __FUNCTION__, TIMER_NAME.c_str());
	  
	// add repeating manual type
	             TIMER_NAME                     = "Timer rule";      
	unsigned int TIMER_REPEATING_MANUAL_ATTRIBS = PVR_TIMER_TYPE_IS_MANUAL               | PVR_TIMER_TYPE_IS_REPEATING              |
	                                              PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE | PVR_TIMER_TYPE_SUPPORTS_CHANNELS         |
	                                              PVR_TIMER_TYPE_SUPPORTS_START_TIME     | PVR_TIMER_TYPE_SUPPORTS_END_TIME         |
	                                              PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS       | PVR_TIMER_TYPE_SUPPORTS_PRIORITY         |
	                                              PVR_TIMER_TYPE_SUPPORTS_LIFETIME       | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;

	sqlResponse  = "";
	sqlTimerType = string("(iId, iAttributes, strDescription, iPrioritiesSize, iLifetimesSize)") +
	               string(" VALUES ") +  
	               string("( ") + StringUtils_Replace(itos(TIMER_REPEATING_MANUAL        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(TIMER_REPEATING_MANUAL_ATTRIBS),"'", "''") + string(" , ") +
	               string(" '") + StringUtils_Replace(    (TIMER_NAME                    ),"'", "''") + string("', ") +
	               string("  ") + StringUtils_Replace(itos(0                             ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(0                             ),"'", "''") + string(" ) ") ;

	// push to database
	AddRecord("TimerTypes", sqlTimerType);
	
	// log addition
	XBMC->Log(LOG_DEBUG, "C+: %s - Added timer type %s", __FUNCTION__, TIMER_NAME.c_str());
	  
	// add repeating epg type
	             TIMER_NAME                  = "Timer rule (guide-base)";
	unsigned int TIMER_REPEATING_EPG_ATTRIBS = PVR_TIMER_TYPE_IS_REPEATING               | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE   |
	                                           PVR_TIMER_TYPE_SUPPORTS_TITLE_EPG_MATCH   | PVR_TIMER_TYPE_SUPPORTS_CHANNELS         |
	                                           PVR_TIMER_TYPE_SUPPORTS_START_TIME        | PVR_TIMER_TYPE_SUPPORTS_START_ANYTIME    |
	                                           PVR_TIMER_TYPE_SUPPORTS_END_TIME          | PVR_TIMER_TYPE_SUPPORTS_END_ANYTIME      |
	                                           PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS          | PVR_TIMER_TYPE_SUPPORTS_START_END_MARGIN |
	                                           PVR_TIMER_TYPE_SUPPORTS_PRIORITY          | PVR_TIMER_TYPE_SUPPORTS_LIFETIME         |
	                                           PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS | PVR_TIMER_TYPE_SUPPORTS_ANY_CHANNEL      ;
	sqlResponse  = "";
	sqlTimerType = string("(iId, iAttributes, strDescription, iPrioritiesSize, iLifetimesSize)") +
	               string(" VALUES ") +  
	               string("( ") + StringUtils_Replace(itos(TIMER_REPEATING_EPG        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(TIMER_REPEATING_EPG_ATTRIBS),"'", "''") + string(" , ") +
	               string(" '") + StringUtils_Replace(    (TIMER_NAME                 ),"'", "''") + string("', ") +
	               string("  ") + StringUtils_Replace(itos(0                          ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(0                          ),"'", "''") + string(" ) ") ;

	// push to database
	AddRecord("TimerTypes", sqlTimerType);
	
	// log addition
	XBMC->Log(LOG_DEBUG, "C+: %s - Added timer type %s", __FUNCTION__, TIMER_NAME.c_str());
	
	// add series link type
	             TIMER_NAME                         = "Timer rule (series)";
	unsigned int TIMER_REPEATING_SERIESLINK_ATTRIBS = PVR_TIMER_TYPE_IS_REPEATING              | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE          |
	                                                  PVR_TIMER_TYPE_SUPPORTS_CHANNELS         | PVR_TIMER_TYPE_SUPPORTS_START_TIME              |
	                                                  PVR_TIMER_TYPE_SUPPORTS_START_ANYTIME    | PVR_TIMER_TYPE_SUPPORTS_END_TIME                |
	                                                  PVR_TIMER_TYPE_SUPPORTS_END_ANYTIME      | PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS                |
	                                                  PVR_TIMER_TYPE_SUPPORTS_START_END_MARGIN | PVR_TIMER_TYPE_SUPPORTS_PRIORITY                |
	                                                  PVR_TIMER_TYPE_SUPPORTS_LIFETIME         | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS       |
	                                                  PVR_TIMER_TYPE_SUPPORTS_ANY_CHANNEL      | PVR_TIMER_TYPE_REQUIRES_EPG_SERIESLINK_ON_CREATE;
	  
	sqlResponse  = "";
	sqlCommand   = string("Insert '")+TIMER_NAME+("' into Timer Types");
	sqlTimerType = string("(iId, iAttributes, strDescription, iPrioritiesSize, iLifetimesSize)") +
	               string(" VALUES ") +  
	               string("( ") + StringUtils_Replace(itos(TIMER_REPEATING_SERIESLINK        ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(TIMER_REPEATING_SERIESLINK_ATTRIBS),"'", "''") + string(" , ") +
	               string(" '") + StringUtils_Replace(    (TIMER_NAME                        ),"'", "''") + string("', ") +
	               string("  ") + StringUtils_Replace(itos(0                                 ),"'", "''") + string(" , ") +
	               string("  ") + StringUtils_Replace(itos(0                                 ),"'", "''") + string(" ) ") ;

	// push to database
	AddRecord("TimerTypes", sqlTimerType);
	
	// log addition
	XBMC->Log(LOG_DEBUG, "C+: %s - Added timer type %s", __FUNCTION__, TIMER_NAME.c_str());
	
	// end transaction
	sqlite3_exec(sqlDatabase, "END TRANSACTION;", NULL, NULL, NULL);
	
	// log timer types imported
	XBMC->Log(LOG_NOTICE, "C+: %s - %i timer types imported", __FUNCTION__, GetTableSize("TimerTypes"));
	  
	// return no issue
	return;
}

/***********************************************************
 * Filter Definitions
 ***********************************************************/
void SQLConnection::FilterChannelsEPG(void)
{
	// log function call
	CPPLog(); 
	
	// only execute if enabled in settings
	if (settings->GetM3UFilter())
	{
		// get sql objects
		vector<SQLRecord> sqlChannels = GetRecords("Channels");
		
		// create container to look for channel
		SQLRecord sqlEpgChannel;
		
		// set counter for filters
		int iFilter = 0;
		
		// iterate through channels and hide those without epg data
		for (vector<SQLRecord>::iterator sqlChannel = sqlChannels.begin(); sqlChannel != sqlChannels.end(); sqlChannel++)
		{			
			// get id, and tvg data
			int    iUniqueId  = stoi(ParseSQLValue(sqlChannel->GetRecord(), "<iUniqueId>" ,  0));
			string strTvgId   =      ParseSQLValue(sqlChannel->GetRecord(), "<strTvgId>"  , "") ;
			string strTvgName =      ParseSQLValue(sqlChannel->GetRecord(), "<strTvgName>", "") ;
			
			// check if exists in epg channels (hide if not)
			if (!FindRecord("EpgChannels", "strTvgId", strTvgId.c_str(), sqlEpgChannel))
			{
				if (!FindRecord("EpgChannels", "strTvgName", strTvgName.c_str(), sqlEpgChannel))
				{
					// set to hidden
					string strChannel = " SET bIsHidden = 'true' WHERE iUniqueId = " + itos(iUniqueId);
					
					// send to database
					this->UpdateRecord("Channels", strChannel);	

					// increment counter for log
					iFilter++;
				}
			}
		}
		
		// log channels filtered
		XBMC->Log(LOG_NOTICE, "C+: %s - %i channels filtered", __FUNCTION__, GetTableSize("Channels")-iFilter);		
	}
}

/***********************************************************
 * Recording Definitions
 ***********************************************************/
PVR_ERROR SQLConnection::AddTimer(const PVR_TIMER &timer, int iSchedAdj /* = 0 */)
{
	// log function call
	CPPLog(); 
	
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
					  				  
	// send to database
	this->AddRecord("Timers", strTimer);
	
	// log addition of entry
	XBMC->Log(LOG_NOTICE, "C+: %s - Created %s timer (%i)", __FUNCTION__, timer.strTitle, iClientIndex);

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR SQLConnection::DeleteTimer(const PVR_TIMER &timer, bool bForceDelete)
{
	// log function call
	CPPLog();
	
	// this is only called on missed or error recordings (so set to cancelled, let stop timer handle active recordings)
	string strTimer = " SET state = " + itos(PVR_TIMER_STATE_CANCELLED) + " WHERE iClientIndex = " + itos(timer.iClientIndex) + " OR iParentClientIndex = " + itos(timer.iParentClientIndex) + ";";
	
	// send to database
	this->UpdateRecord("Timers", strTimer);

	// log deletion of entry
	XBMC->Log(LOG_NOTICE, "C+: %s - Deleted %s timer (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR SQLConnection::StartTimer(const PVR_TIMER &timer)
{
	// log function call
	CPPLog();
	
	// log attempt to start
	XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to start %s recording (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);
			
	// flag as recording
	string strTimer = " SET state = " + itos(PVR_TIMER_STATE_RECORDING) + " WHERE iClientIndex = " + itos(timer.iClientIndex) + ";";
	
	// update in database
	this->UpdateRecord("Timers", strTimer);

	// create process
	SQLTask sqlTask;
	
	sqlTask.iClientIndex      = timer.iClientIndex;
	sqlTask.iClientChannelUid = timer.iClientChannelUid;
	sqlTask.iEpgUid           = timer.iEpgUid;
	sqlTask.pProcess          = new PVRRecorder(timer.iClientIndex, timer.iClientChannelUid, timer.iEpgUid);
	
	// add to vector
	sqlTasks.push_back(sqlTask);
	
	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR SQLConnection::StopTimer(const PVR_TIMER &timer)
{
	// log function call
	CPPLog();
		
	// log attempt to stop
	XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to stop %s recording (%i)", __FUNCTION__, timer.strTitle, timer.iClientIndex);
	
	// flag as recording
	string strTimer = " SET state = " + itos(PVR_TIMER_STATE_COMPLETED) + " WHERE iClientIndex = " + itos(timer.iClientIndex) + ";";
	
	// update in database
	this->UpdateRecord("Timers", strTimer);
	
	// iterate through processes and stop the recording
	for (vector<SQLTask>::iterator sqlTask = sqlTasks.begin(); sqlTask != sqlTasks.end(); sqlTask++)
	{
		if (sqlTask->iClientIndex      == timer.iClientIndex      &&
		    sqlTask->iClientChannelUid == timer.iClientChannelUid &&
			sqlTask->iEpgUid           == timer.iEpgUid             )
			{
				// destruct timer
				sqlTask->iClientIndex      = PVR_TIMER_NO_CLIENT_INDEX;
				sqlTask->iClientChannelUid = PVR_CHANNEL_INVALID_UID;
				sqlTask->iEpgUid           = EPG_TAG_INVALID_UID;
				
				// delete timer if still working
				if (sqlTask->pProcess->IsWorking())
					SAFE_DELETE(sqlTask->pProcess);
				
				// remove vector
				sqlTasks.erase(sqlTask--);
	
				// exit loop
				break;
			}
	}

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR SQLConnection::ScheduleTimer(const PVR_TIMER &timer)
{
	// log function call
	CPPLog(); 
	
	// check that it is a timer rule
	if (timer.iTimerType == TIMER_ONCE_MANUAL || timer.iTimerType == TIMER_ONCE_EPG)
	{
		XBMC->Log(LOG_DEBUG, "C+: %s - Timer rule NOT detected, passing back to SQL database", __FUNCTION__);
		return PVR_ERROR_NO_ERROR;
	}
	
	// fetch current containers
	vector<SQLRecord> sqlChannels    = this->GetRecords("Channels"   );
	vector<SQLRecord> sqlEpgChannels = this->GetRecords("EpgChannels");
	vector<SQLRecord> sqlEpgEntries  = this->GetRecords("EpgEntries" );
	vector<SQLRecord> sqlTimers      = this->GetRecords("Timers"     );
  
	// create schedule container
	vector<DVRTimer> cSchedules;

	// initialize schecule container
	cSchedules.clear();
	   
	// get number of days stored in guide
	int iEPGDays = this->GetEPGDays();
  
	// time buffer
	time_t dtm;
	tm * ptm;
	char chr[64];
  
	// create containers for parsed text
	string strSummary;  
	
	// get list of possible EPG entries for guide based timers
	if (timer.iTimerType != TIMER_REPEATING_MANUAL)
	{
		// iterate through EPG entries
		for (vector<SQLRecord>::iterator sqlChannel = sqlChannels.begin(); sqlChannel != sqlChannels.end(); sqlChannel++)
		{
			// convert to c type
			IPTVChannel cChannel(sqlChannel->GetRecord());
			
			// look for tv guide id
			if (cChannel.GetUniqueId() == timer.iClientChannelUid)
			{
				// iterate through epg channels to get tvg id or display name
				for (vector<SQLRecord>::iterator sqlEpgChannel = sqlEpgChannels.begin(); sqlEpgChannel != sqlEpgChannels.end(); sqlEpgChannel++)
				{
					// convert to c type
					IPTVEpgChannel cEpgChannel(sqlEpgChannel->GetRecord());
			
					// look for tv guide id or display name 
					if (StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel.GetTvgId()), StringUtils_Trim(cChannel.GetTvgId())) || StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel.GetTvgName()), StringUtils_Trim(cChannel.GetTvgName())))
					{
						// iterate through EPG and pass back entries
						for (vector<SQLRecord>::iterator sqlEpgEntry = sqlEpgEntries.begin(); sqlEpgEntry != sqlEpgEntries.end(); sqlEpgEntry++)
						{
							// convert to c type
							IPTVEpgEntry cEpgEntry(sqlEpgEntry->GetRecord());
					
							// only add entries not missed
							if (time(NULL) <= cEpgEntry.GetEndTime())
							{
								// look for same broadcast channel, title, day, time
								switch (timer.iTimerType)
								{
									case TIMER_REPEATING_EPG:
										if ((            (cEpgEntry.GetTvgId()    ) ==          (cEpgChannel.GetTvgId())                         ) &&
										    (string      (cEpgEntry.GetTitle()    ) == string   (timer.strTitle        )                         ) &&
										    (ParseWeekDay(cEpgEntry.GetStartTime())  &          (timer.iWeekdays       )                         ) &&
										    (ParseTime   (cEpgEntry.GetStartTime()) == ParseTime(timer.startTime       ) || (timer.bStartAnyTime)) &&
										    (ParseTime   (cEpgEntry.GetEndTime()  ) == ParseTime(timer.endTime         ) || (timer.bEndAnyTime  ))   )
										{
											// clear out summary text
											strSummary = "";

											// create new summary text
											dtm = cEpgEntry.GetStartTime();
											ptm = localtime(&dtm);
											strftime(chr, 64, "%m/%d/%Y from %I:%M %p", ptm);
											strSummary += chr;
											StringUtils::Replace(strSummary, "from 0", "from ");

											dtm = cEpgEntry.GetEndTime();
											ptm = localtime(&dtm);
											strftime(chr, 64, " to %I:%M %p", ptm);
											strSummary += chr;
											StringUtils::Replace(strSummary, "to 0", "to ");

											// assign values to entry						
											string sqlTimer = "<iClientIndex>"              + itos(PVR_TIMER_NO_CLIENT_INDEX       ) + "</iClientIndex>"              +
															  "<iParentClientIndex>"        + itos(timer.iClientIndex              ) + "</iParentClientIndex>"        +
															  "<iClientChannelUid>"         + itos(timer.iClientChannelUid         ) + "</iClientChannelUid>"         +
															  "<startTime>"                 + itos(cEpgEntry.GetStartTime()        ) + "</startTime>"                 +
															  "<endTime>"                   + itos(cEpgEntry.GetEndTime()          ) + "</endTime>"                   +
															  "<bStartAnyTime>"             + btos(false                           ) + "</bStartAnyTime>"             +
															  "<bEndAnyTime>"               + btos(false                           ) + "</bEndAnyTime>"               +
															  "<state>"                     + itos(timer.state                     ) + "</state>"                     +
															  "<iTimerType>"                + itos(TIMER_ONCE_EPG                  ) + "</iTimerType>"                +
															  "<strTitle>"                  +      timer.strTitle                    + "</strTitle>"                  +
															  "<strEpgSearchString>"        +      timer.strEpgSearchString          + "</strEpgSearchString>"        +
															  "<bFullTextEpgSearch>"        + btos(timer.bFullTextEpgSearch        ) + "</bFullTextEpgSearch>"        +
															  "<strDirectory>"              +      timer.strDirectory                + "</strDirectory>"              +
															  "<strSummary>"                +            strSummary.c_str()          + "</strSummary>"                +
															  "<iPriority>"                 + itos(timer.iPriority                 ) + "</iPriority>"                 +
															  "<iLifetime>"                 + itos(timer.iLifetime                 ) + "</iLifetime>"                 +
															  "<iMaxRecordings>"            + itos(timer.iMaxRecordings            ) + "</iMaxRecordings>"            +
															  "<iRecordingGroup>"           + itos(timer.iRecordingGroup           ) + "</iRecordingGroup>"           +
															  "<firstDay>"                  + itos(timer.firstDay                  ) + "</firstDay>"                  +
															  "<iWeekdays>"                 + itos(PVR_WEEKDAY_NONE                ) + "</iWeekdays>"                 +
															  "<iPreventDuplicateEpisodes>" + itos(timer.iPreventDuplicateEpisodes ) + "</iPreventDuplicateEpisodes>" +
															  "<iEpgUid>"                   + itos(cEpgEntry.GetUniqueBroadcastId()) + "</iEpgUid>"                   +
															  "<iMarginStart>"              + itos(timer.iMarginStart              ) + "</iMarginStart>"              +
															  "<iMarginEnd>"                + itos(timer.iMarginEnd                ) + "</iMarginEnd>"                +
															  "<iGenreType>"                + itos(timer.iGenreType                ) + "</iGenreType>"                +
															  "<iGenreSubType>"             + itos(timer.iGenreSubType             ) + "</iGenreSubType>"             +
															  "<strSeriesLink>"             +      timer.strSeriesLink               + "</strSeriesLink>"             ;
															  
											// create container
											DVRTimer cTimer(sqlTimer);

											// add to schedule
											cSchedules.push_back(cTimer);
										}
										break;
									case TIMER_REPEATING_SERIESLINK:
										if ((            (cEpgEntry.GetTvgId()     ) ==          (cEpgChannel.GetTvgId())                         ) &&
										    (string      (cEpgEntry.GetSeriesLink()) == string   (timer.strSeriesLink   )                         ) &&
										    (ParseWeekDay(cEpgEntry.GetStartTime() )  &          (timer.iWeekdays       )                         ) &&
										    (ParseTime   (cEpgEntry.GetStartTime() ) == ParseTime(timer.startTime       ) || (timer.bStartAnyTime)) &&
										    (ParseTime   (cEpgEntry.GetEndTime()   ) == ParseTime(timer.endTime         ) || (timer.bEndAnyTime  ))   )
										{
											// clear out summary text
											strSummary = "";

											// create new summary text
											dtm = cEpgEntry.GetStartTime();
											ptm = localtime(&dtm);
											strftime(chr, 64, "%m/%d/%Y from %I:%M %p", ptm);
											strSummary += chr;
											StringUtils::Replace(strSummary, "from 0", "to ");

											dtm = cEpgEntry.GetEndTime();
											ptm = localtime(&dtm);
											strftime(chr, 64, " to %I:%M %p", ptm);
											strSummary += chr;
											StringUtils::Replace(strSummary, "to 0", "to ");

											// assign values to entry						
											string sqlTimer = "<iClientIndex>"              + itos(PVR_TIMER_NO_CLIENT_INDEX       ) + "</iClientIndex>"              +
															  "<iParentClientIndex>"        + itos(timer.iClientIndex              ) + "</iParentClientIndex>"        +
															  "<iClientChannelUid>"         + itos(timer.iClientChannelUid         ) + "</iClientChannelUid>"         +
															  "<startTime>"                 + itos(cEpgEntry.GetStartTime()        ) + "</startTime>"                 +
															  "<endTime>"                   + itos(cEpgEntry.GetEndTime()          ) + "</endTime>"                   +
															  "<bStartAnyTime>"             + btos(false                           ) + "</bStartAnyTime>"             +
															  "<bEndAnyTime>"               + btos(false                           ) + "</bEndAnyTime>"               +
															  "<state>"                     + itos(timer.state                     ) + "</state>"                     +
															  "<iTimerType>"                + itos(TIMER_ONCE_EPG                  ) + "</iTimerType>"                +
															  "<strTitle>"                  +      timer.strTitle                    + "</strTitle>"                  +
															  "<strEpgSearchString>"        +      timer.strEpgSearchString          + "</strEpgSearchString>"        +
															  "<bFullTextEpgSearch>"        + btos(timer.bFullTextEpgSearch        ) + "</bFullTextEpgSearch>"        +
															  "<strDirectory>"              +      timer.strDirectory                + "</strDirectory>"              +
															  "<strSummary>"                +            strSummary.c_str()          + "</strSummary>"                +
															  "<iPriority>"                 + itos(timer.iPriority                 ) + "</iPriority>"                 +
															  "<iLifetime>"                 + itos(timer.iLifetime                 ) + "</iLifetime>"                 +
															  "<iMaxRecordings>"            + itos(timer.iMaxRecordings            ) + "</iMaxRecordings>"            +
															  "<iRecordingGroup>"           + itos(timer.iRecordingGroup           ) + "</iRecordingGroup>"           +
															  "<firstDay>"                  + itos(timer.firstDay                  ) + "</firstDay>"                  +
															  "<iWeekdays>"                 + itos(PVR_WEEKDAY_NONE                ) + "</iWeekdays>"                 +
															  "<iPreventDuplicateEpisodes>" + itos(timer.iPreventDuplicateEpisodes ) + "</iPreventDuplicateEpisodes>" +
															  "<iEpgUid>"                   + itos(cEpgEntry.GetUniqueBroadcastId()) + "</iEpgUid>"                   +
															  "<iMarginStart>"              + itos(timer.iMarginStart              ) + "</iMarginStart>"              +
															  "<iMarginEnd>"                + itos(timer.iMarginEnd                ) + "</iMarginEnd>"                +
															  "<iGenreType>"                + itos(timer.iGenreType                ) + "</iGenreType>"                +
															  "<iGenreSubType>"             + itos(timer.iGenreSubType             ) + "</iGenreSubType>"             +
															  "<strSeriesLink>"             +      timer.strSeriesLink               + "</strSeriesLink>"             ;
															  
											// create container
											DVRTimer cTimer(sqlTimer);

											// add to schedule
											cSchedules.push_back(cTimer);
										}
										break;
								}
							}
						}
						
						// exit loop
						break;
					}
				}
				
				// exit loop
				break;
			}
		}
	}

	// create containers for parsed text
	int    tm_year;
	int    tm_mon;
	int    tm_mday;
	int    tm_hour;
	int    tm_min;
	int    tm_sec;
	time_t startTime;
	time_t endTime;
  
	// create manual timers
	if (timer.iTimerType == TIMER_REPEATING_MANUAL)
	{
		// iterate through EPG days
		for (int iDay = 0; iDay < iEPGDays; iDay++)
		{
			// create pointer of timer datetime
			dtm = timer.startTime;
			ptm = localtime(&dtm);

			tm_hour = ptm->tm_hour;
			tm_min  = ptm->tm_min ;
			tm_sec  = ptm->tm_sec; 

			// create pointer to today's datetime
			dtm = time(NULL) + iDay*SECONDS_IN_DAY;
			ptm = localtime(&dtm);

			// copy date over
			tm_year = ptm->tm_year;
			tm_mon  = ptm->tm_mon ;
			tm_mday = ptm->tm_mday;

			// adjust pointer
			ptm->tm_year = tm_year;
			ptm->tm_mon  = tm_mon ;
			ptm->tm_mday = tm_mday;
			ptm->tm_hour = tm_hour;
			ptm->tm_min  = tm_min ;
			ptm->tm_sec  = tm_sec ;

			// create start datetime for new entry
			startTime = mktime(ptm);

			// create pointer of timer datetime
			dtm = timer.endTime;
			ptm = localtime(&dtm);

			tm_hour = ptm->tm_hour;
			tm_min  = ptm->tm_min ;
			tm_sec  = ptm->tm_sec; 

			// create pointer to today's datetime
			dtm = time(NULL) + iDay*(SECONDS_IN_DAY);
			ptm = localtime(&dtm);

			// copy date over
			tm_year = ptm->tm_year;
			tm_mon  = ptm->tm_mon ;
			tm_mday = ptm->tm_mday;

			// adjust pointer
			ptm->tm_year = tm_year;
			ptm->tm_mon  = tm_mon ;
			ptm->tm_mday = tm_mday;
			ptm->tm_hour = tm_hour;
			ptm->tm_min  = tm_min ;
			ptm->tm_sec  = tm_sec ;

			// create end datetime for new entry
			endTime = mktime(ptm);

			// look for same day
			if (ParseWeekDay(startTime) & (timer.iWeekdays))
			{
				// clear out summary text
				strSummary = "";

				// create new summary text
				dtm = startTime;
				ptm = localtime(&dtm);
				strftime(chr, 64, "%m/%d/%Y from %I:%M %p", ptm);
				strSummary += chr;
				StringUtils::Replace(strSummary, "from 0", "to ");

				dtm = endTime;
				ptm = localtime(&dtm);
				strftime(chr, 64, " to %I:%M %p", ptm);
				strSummary += chr;
				StringUtils::Replace(strSummary, "to 0", "to ");

				// assign values to entry						
				string sqlTimer = "<iClientIndex>"              + itos(PVR_TIMER_NO_CLIENT_INDEX        ) + "</iClientIndex>"              +
								  "<iParentClientIndex>"        + itos(timer.iClientIndex               ) + "</iParentClientIndex>"        +
								  "<iClientChannelUid>"         + itos(timer.iClientChannelUid          ) + "</iClientChannelUid>"         +
								  "<startTime>"                 + itos(      startTime                  ) + "</startTime>"                 +
								  "<endTime>"                   + itos(      endTime                    ) + "</endTime>"                   +
								  "<bStartAnyTime>"             + btos(false                            ) + "</bStartAnyTime>"             +
								  "<bEndAnyTime>"               + btos(false                            ) + "</bEndAnyTime>"               +
								  "<state>"                     + itos(timer.state                      ) + "</state>"                     +
								  "<iTimerType>"                + itos(TIMER_ONCE_MANUAL                ) + "</iTimerType>"                +
								  "<strTitle>"                  +      timer.strTitle                     + "</strTitle>"                  +
								  "<strEpgSearchString>"        +      timer.strEpgSearchString           + "</strEpgSearchString>"        +
								  "<bFullTextEpgSearch>"        + btos(timer.bFullTextEpgSearch         ) + "</bFullTextEpgSearch>"        +
								  "<strDirectory>"              +      timer.strDirectory                 + "</strDirectory>"              +
								  "<strSummary>"                +            strSummary.c_str()           + "</strSummary>"                +
								  "<iPriority>"                 + itos(timer.iPriority                  ) + "</iPriority>"                 +
								  "<iLifetime>"                 + itos(timer.iLifetime                  ) + "</iLifetime>"                 +
								  "<iMaxRecordings>"            + itos(timer.iMaxRecordings             ) + "</iMaxRecordings>"            +
								  "<iRecordingGroup>"           + itos(timer.iRecordingGroup            ) + "</iRecordingGroup>"           +
								  "<firstDay>"                  + itos(timer.firstDay                   ) + "</firstDay>"                  +
								  "<iWeekdays>"                 + itos(PVR_WEEKDAY_NONE                 ) + "</iWeekdays>"                 +
								  "<iPreventDuplicateEpisodes>" + itos(timer.iPreventDuplicateEpisodes  ) + "</iPreventDuplicateEpisodes>" +
								  "<iEpgUid>"                   + itos(timer.iEpgUid                    ) + "</iEpgUid>"                   +
								  "<iMarginStart>"              + itos(timer.iMarginStart               ) + "</iMarginStart>"              +
								  "<iMarginEnd>"                + itos(timer.iMarginEnd                 ) + "</iMarginEnd>"                +
								  "<iGenreType>"                + itos(timer.iGenreType                 ) + "</iGenreType>"                +
								  "<iGenreSubType>"             + itos(timer.iGenreSubType              ) + "</iGenreSubType>"             +
								  "<strSeriesLink>"             +      timer.strSeriesLink                + "</strSeriesLink>"             ;
								  
				// create container
				DVRTimer cTimer(sqlTimer);

				// add to schedule
				cSchedules.push_back(cTimer);
			}
		}
	}
  
	// remove if already scheduled 
	for (vector<DVRTimer>::iterator cSchedule = cSchedules.begin(); cSchedule != cSchedules.end(); cSchedule++)
	{
		// check current timers
		for (vector<SQLRecord>::iterator sqlTimer = sqlTimers.begin(); sqlTimer != sqlTimers.end(); sqlTimer++)
		{
			// convert to c type
			DVRTimer cTimer(sqlTimer->GetRecord());
			
			// skip un-scheduled timers
			if (cTimer.GetState() == PVR_TIMER_STATE_SCHEDULED || cTimer.GetState() == PVR_TIMER_STATE_RECORDING || cTimer.GetState() == PVR_TIMER_STATE_COMPLETED || cTimer.GetState() == PVR_TIMER_STATE_ABORTED)
			{
				// only look for manual timers
				if (cTimer.GetTimerType() == TIMER_ONCE_MANUAL || cTimer.GetTimerType() == TIMER_ONCE_EPG)
				{
					// remove if found
					if (cSchedule->GetClientChannelUid() == cTimer.GetClientChannelUid() &&
					    cSchedule->GetStartTime()        == cTimer.GetStartTime()        &&
					    cSchedule->GetEndTime()          == cTimer.GetEndTime()            )
					{
						// remove
						cSchedules.erase(cSchedule--);
					}
				}
			}
		}
	}
	  
	// setup schedule adjuster
	int iIndex = 1;
	  
	// add timers from schedule, pass index to adjust id
	for (vector<DVRTimer>::iterator cSchedule = cSchedules.begin(); cSchedule != cSchedules.end(); cSchedule++)
		if (cSchedule->GetState() == PVR_TIMER_STATE_SCHEDULED)
			this->AddTimer(cSchedule->Timer(), iIndex++);
	  
	// clear containers
	sqlEpgEntries.clear();
	sqlEpgChannels.clear();
	sqlChannels.clear();
	sqlTimers.clear();
	cSchedules.clear();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}