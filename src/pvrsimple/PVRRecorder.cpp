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
#include "PVRRecorder.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings   *settings;
extern SQLConnection *sqlite  ;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
PVRRecorder::PVRRecorder(const int iClientIndex, const int iClientChannelUid, const int iEpgUid)
{
	// log attempt to create object
	XBMC->Log(LOG_NOTICE, "C+: %s - Creating FFMPEG process", __FUNCTION__);
	
	// no establish connection at start
	bIsOpen      = false;
	bIsWorking   = false;
	bStop        = false;
	
	// assign timer
	iChannelId = iClientChannelUid;
	iTimerId   = iClientIndex;
	iEpgId     = iEpgUid;
	
	// add folder name to directory
	strLogPath += settings->GetUserPath() + FFMPEG_LOG_FOLDER + ParseFolderSeparator(settings->GetUserPath());
	
	// create directory if they don't exist  
	if (!XBMC->DirectoryExists(strLogPath.c_str()))
		XBMC->CreateDirectory(strLogPath.c_str());
	
	// add file name to directory
	strLogPath += FFMPEG_LOG_FILE;
	
	// log location of ffmpeg binary
	XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to locate FFMPEG binary (%s)", __FUNCTION__, settings->GetFFMPEG().c_str());
		
	// open ffmpeg
	Open();
	
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Created FFMPEG process", __FUNCTION__);
	
	// create thread
	if (IsOpen()) CreateThread();
}

PVRRecorder::~PVRRecorder(void)
{
	// mark as stopped
	bStop = true;
	
	// de-assign timer
	iChannelId = PVR_CHANNEL_INVALID_UID;
	iTimerId   = PVR_TIMER_NO_CLIENT_INDEX;
	iEpgId     = PVR_TIMER_NO_EPG_UID;
	 
	// attempt to disconnect
	Close();
}

/***********************************************************
 * Get Staus/Local Variable API Definitions
 ***********************************************************/
bool PVRRecorder::IsOpen(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsOpen;
}

bool PVRRecorder::IsWorking(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return bIsWorking;
}

/***********************************************************
 * Open/Close Definitions
 ***********************************************************/
void PVRRecorder::Open(void)
{
	// log function call
	CPPLog(); 
		
	// attempt to locate ffmpeg binary
	bIsOpen = (XBMC->FileExists(settings->GetFFMPEG().c_str(), false));
	
	// log failure to open
	if (!bIsOpen)
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to create a new FFMPEG process", __FUNCTION__);
}

void PVRRecorder::Close(bool bWait /* = true */)
{
	// log function call
	CPPLog(); 
	
	// log waiting for thread to close
	while (bIsWorking)
	{
	  // sleep to allow thread time to close
	  sleep(1);
	}
	
	// force close thread
	StopThread(bWait);
}

/***********************************************************
 * PVR Recorder Process Definitions
 ***********************************************************/
void *PVRRecorder::Process(void)
{
	// log creation of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Started FFMPEG recorder", __FUNCTION__);
	
	// set thread working
	bIsWorking = true;
	
	// check if required params are set
	if (settings->GetDVRPath() == "")
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No DVR path to store files has been set, stop recorder", __FUNCTION__);
		goto exit;
	}

	if (settings->GetFileExt() == "")
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No file extension for recordings has been set, stop recorder", __FUNCTION__);
		goto exit;
	}

	if (settings->GetAVParams() == "")
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No recompression parameters for FFMPEG has been set, stop recorder", __FUNCTION__);
		goto exit;
	}
		
	{	
	// create channel, timer, and epg containers
	SQLRecord sqlChannel   ;
	SQLRecord sqlTimer     ;
	SQLRecord sqlEpgChannel;
	SQLRecord sqlEpgEntry  ;

	// fecth current channel timer data (required)
	if (!sqlite->FindRecord("Channels", "iUniqueId", iChannelId, sqlChannel))
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No channel associated with timer found, stop recorder", __FUNCTION__);
		goto exit;    
	}

	if (!sqlite->FindRecord("Timers", "iClientIndex", iTimerId, sqlTimer))
	{
		XBMC->Log(LOG_ERROR, "C+: %s - No timer data was found, stop recorder", __FUNCTION__);
		goto exit;    
	}
	
	// get the tvg data from the channel
	strTvgId   = ParseSQLValue(sqlChannel.GetRecord(), "<strTvgId>"  , "");
	strTvgName = ParseSQLValue(sqlChannel.GetRecord(), "<strTvgName>", "");
	
	// look for an EPG channel
	if      (sqlite->FindRecord("EpgChannels", "strTvgId"  , strTvgId.c_str()  , sqlEpgChannel))
	{
		XBMC->Log(LOG_NOTICE, "C+: %s - EPG channel was found, looking for guide entry", __FUNCTION__);
	}	
	else if (sqlite->FindRecord("EpgChannels", "strTvgName", strTvgName.c_str(), sqlEpgChannel))
	{
		XBMC->Log(LOG_NOTICE, "C+: %s - EPG channel was found, looking for guide entry", __FUNCTION__);
	}
	
	// get the tvg data from epg channel
	strTvgId   = ParseSQLValue(sqlEpgChannel.GetRecord(), "<strTvgId>"  , "");
	strTvgName = ParseSQLValue(sqlEpgChannel.GetRecord(), "<strTvgName>", "");

	// fetch current epg data (optional), look by tvg-id
	if      (sqlite->FindRecord("EpgEntries", string("strTvgId   = '")+(strTvgId  )+"' AND iUniqueBroadcastId = "+itos(iEpgId), sqlEpgEntry))
	{
		XBMC->Log(LOG_NOTICE, "C+: %s - EPG data for timer was found, proceed to import", __FUNCTION__);
	}

	// convert to c types
	IPTVChannel  cChannel (sqlChannel.GetRecord() );
	IPTVEpgEntry cEpgEntry(sqlEpgEntry.GetRecord());
	DVRTimer     cTimer   (sqlTimer.GetRecord()   );
	
	// create a kodi timer to update
	PVR_TIMER timer;
	memset(&timer, 0, sizeof(PVR_TIMER));
		   
		   timer.iClientIndex              =                   cTimer.GetClientIndex()             ;
		   timer.iParentClientIndex        =                   cTimer.GetParentClientIndex()       ;
		   timer.iClientChannelUid         =                   cTimer.GetClientChannelUid()        ;
		   timer.startTime                 =                   cTimer.GetStartTime()               ;
		   timer.endTime                   =                   cTimer.GetEndTime()                 ;
		   timer.bStartAnyTime             =                   cTimer.GetStartAnyTime()            ;
		   timer.bEndAnyTime               =                   cTimer.GetEndAnyTime()              ;
		   timer.state                     = (PVR_TIMER_STATE) cTimer.GetState()                   ;
		   timer.iTimerType                =                   cTimer.GetTimerType()               ;
	strcpy(timer.strTitle                  ,                   cTimer.GetTitle())                  ;
	strcpy(timer.strEpgSearchString        ,                   cTimer.GetEpgSearchString())        ;
		   timer.bFullTextEpgSearch        =                   cTimer.GetFullTextEpgSearch()       ;
	strcpy(timer.strDirectory              ,                   cTimer.GetDirectory())              ;
	strcpy(timer.strSummary                ,                   cTimer.GetSummary())                ;
		   timer.iPriority                 =                   cTimer.GetPriority()                ;
		   timer.iLifetime                 =                   cTimer.GetLifetime()                ;
		   timer.iMaxRecordings            =                   cTimer.GetMaxRecordings()           ;
		   timer.iRecordingGroup           =                   cTimer.GetRecordingGroup()          ;
		   timer.firstDay                  =                   cTimer.GetFirstDay()                ;
		   timer.iWeekdays                 =                   cTimer.GetWeekdays()                ;
		   timer.iPreventDuplicateEpisodes =                   cTimer.GetPreventDuplicateEpisodes();
		   timer.iEpgUid                   =                   cTimer.GetEpgUid()                  ;
		   timer.iMarginStart              =                   cTimer.GetMarginStart()             ;
		   timer.iMarginEnd                =                   cTimer.GetMarginEnd()               ;
		   timer.iGenreType                =                   cTimer.GetGenreType()               ;
		   timer.iGenreSubType             =                   cTimer.GetGenreSubType()            ;
	strcpy(timer.strSeriesLink             ,                   cTimer.GetSeriesLink())             ;
	
	// get the recording time
	time_t startTime = time(NULL);
	tm  *recTime     = localtime(&startTime); 
	char strRecTime[23];

	strftime(strRecTime, 23, " (%Y-%m-%dT%H-%M-%S)", recTime);
	  
	// create folder & file name
	string strFolderName = PrepFileName(string(cTimer.GetDirectory()));
	string strFileName   = PrepFileName(string(cTimer.GetTitle()    ) + ((string(cEpgEntry.GetEpisodeName()) == "") ? "" : " - ") + string(cEpgEntry.GetEpisodeName()) + string(strRecTime)) + "." + settings->GetFileExt();

	// derive separator
	string SEPARATOR = ParseFolderSeparator(settings->GetDVRPath()).c_str();
	   
	// create folder & file path
	string strFolderPath = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), strFolderName.c_str()                     );
	string strFilePath   = settings->GetDVRPath() + StringUtils_Join(SEPARATOR.c_str(), strFolderName.c_str(), strFileName.c_str());
	  
	// create dir if doesn't exist
	if (!XBMC->DirectoryExists(strFolderPath.c_str()))
		XBMC->CreateDirectory(strFolderPath.c_str());
	
	// add time to logfile name
	strLogPath = StringUtils_Replace(strLogPath, ".log", string(strRecTime) + ".log");

	// create ffmpeg commands
	string strParams = " -i \"" + string(cChannel.GetStreamURL()) + "\" " + settings->GetAVParams() + " -f " + settings->GetFileExt() + " pipe:1 2> \"" + strLogPath + "\"";

	// create sub-process
	subprocess ffmpeg;

	// log start of ffmpeg
	XBMC->Log(LOG_NOTICE, "C+: %s - Initializing FFMPEG command (%s %s)", __FUNCTION__, settings->GetFFMPEG().c_str(), strParams.c_str());
	
	// create containers for file, buffer, bytes read, and time of last read
	void*  fileHandle       ;
	char   readBuffer[4096] ;
	time_t recordingTime = 0;
	time_t lastRead      = 0;
	
	// open recording file for write
	fileHandle = XBMC->OpenFileForWrite(strFilePath.c_str(), true);
	
	// return error if could not get file handle, otherwise start recording
	if (!fileHandle)
	{
		// log error of write
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to get write handle for file, check connection to DVR path", __FUNCTION__);
	}
	else
	{
		// mark start time
		lastRead = time(NULL);

		// start command
		ffmpeg.pstart((settings->GetFFMPEG() + strParams).c_str(), "rb");
		
		XBMC->Log(LOG_NOTICE, "C+: %s - Started %s recording [%s]", __FUNCTION__, cTimer.GetTitle(), strFileName.c_str());

		// get raw binary video data from FFMPEG
		while(!bStop)
		{
			// read stdout stream
			try
			{ 
				// read binary ffmpeg pipe
				ffmpeg.pread(readBuffer, 4096);

				//if read proceed to write to file
				if (ffmpeg.gcount() > 0)
				{
					// write to file
					XBMC->WriteFile(fileHandle, readBuffer, ffmpeg.gcount());

					// mark time of read
					lastRead = time(NULL);
				}
			}
			catch (exception const &e)
			{
				// stub nothing read try to agaim
			}

			// fetch current timer state
			if (!sqlite->FindRecord("Timers", "iClientIndex", iTimerId, sqlTimer))
			{
				// log that timer was deleted externally
				XBMC->Log(LOG_NOTICE, "C+: %s - Externally deleted %s timer (%i),", __FUNCTION__, cTimer.GetTitle(), cTimer.GetClientIndex());

				// set state to terminated
				timer.state = PVR_TIMER_STATE_ABORTED;
			}
			else
			{
				// log that timer was deleted externally
				XBMC->Log(LOG_DEBUG, "C+: %s - Found current %s timer (%i),", __FUNCTION__, cTimer.GetTitle(), cTimer.GetClientIndex());

				// set state to terminated
				timer.state = sqlTimer.Timer().state;				
			}

			// check for termination of recording
			if (timer.state == PVR_TIMER_STATE_COMPLETED || timer.state == PVR_TIMER_STATE_ABORTED)
			{
				// mark completion on timer
				timer.state = PVR_TIMER_STATE_COMPLETED;

				// log failure due to timeout
				XBMC->Log(LOG_NOTICE, "C+: %s - Completed %s recording [%s]", __FUNCTION__, cTimer.GetTitle(), strFileName.c_str());

				// exit loop
				break;      
			}

			//check for timeout
			if (lastRead+settings->GetStrmTimeout() < time(NULL))
			{
				// mark error on timer
				timer.state = PVR_TIMER_STATE_ERROR;

				// log failure due to timeout
				XBMC->Log(LOG_ERROR, "C+: %s - Failed %s recording [%s]", __FUNCTION__, cTimer.GetTitle(), strFileName.c_str());

				// exit loop
				break;
			}
		}

		// stop ffmpeg
		if(ffmpeg.pterm() < 0)
		{
			// log failure to close thread
			XBMC->Log(LOG_ERROR, "C+: %s - Failed to close FFMPEG, killing process", __FUNCTION__);
		}
	}
	
	// close writing of file
	XBMC->CloseFile(fileHandle);

	// mark completed on timer if stop recording (called by scheduler)
	if (timer.state == PVR_TIMER_STATE_RECORDING)
		timer.state = PVR_TIMER_STATE_COMPLETED;
	
	// create update string
	string strTimer = " SET state = " + itos(timer.state) + " WHERE iClientIndex = " + itos(timer.iClientIndex) + ";";
	
	// update in database
	sqlite->UpdateRecord("Timers", strTimer);
	  
	// sleep allow pipe to clear out
	sleep(1);
	  
	// try to get duration from log otherwise fallback on time read
	string logContent   ;
	string logDuration  ;
	int    readDuration = lastRead - recordingTime;
	  
	fileHandle = XBMC->OpenFile(strLogPath.c_str(), 0);
	
	if (fileHandle)
	{
		while (int bytes = XBMC->ReadFile(fileHandle, readBuffer, 4096))
		{
			// read out buffer
			logContent = string(readBuffer).substr(0, bytes);

			// split into lines
			vector<string> lines = StringUtils::Split(logContent, "\n");

			// look for timestamp
			for (vector<string>::iterator line = lines.begin(); line != lines.end(); line++)
			{
				// convert line text to UTF8
				string decoded_line(XBMC->UnknownToUTF8(line->c_str()));

				// look for timestamp
				if ((int)decoded_line.find(FFMPEG_TIME_MARKER) > 0)
				logDuration = ReadLOGLineValue(decoded_line, FFMPEG_TIME_MARKER, "");
			}
		}
	}
	  
	if (!logDuration.empty())
	{
		// convert to seconds
		int h, m, s, ms = 0;

		if (sscanf(logDuration.c_str(), "%d:%d:%d.%d", &h, &m, &s, &ms) >= 2)
			readDuration = h *3600 + m*60 + s;

		// log pull of duration
		XBMC->Log(LOG_NOTICE, "C+: %s - Pulled duration from FFMPEG log (%s)", __FUNCTION__, logDuration.c_str());  
	}

	XBMC->CloseFile(fileHandle);
	
	// derive info for recording containers
	const string strRecordingId      = to_string(cTimer.GetClientIndex())                                                      ;
	const char*  strFanartPath       = ""                                                                                      ; // not supported
	const int    iDuration           = readDuration                                                                            ;
	const int    iPlayCount          = 0                                                                                       ;
	const int    iLastPlayedPosition = 0                                                                                       ;
	const bool   bIsDeleted          = false                                                                                   ;
	const int    channelType         = cChannel.GetIsRadio() ? PVR_RECORDING_CHANNEL_TYPE_RADIO : PVR_RECORDING_CHANNEL_TYPE_TV;
	
	// create a kodi recording to add
	PVR_RECORDING recording;
	memset(&recording, 0, sizeof(PVR_RECORDING));

	strncpy(recording.strRecordingId      ,                              strRecordingId.c_str()         , PVR_ADDON_NAME_STRING_LENGTH-1);
	strncpy(recording.strTitle            ,                              cTimer.GetTitle()              , PVR_ADDON_NAME_STRING_LENGTH-1);
	strncpy(recording.strEpisodeName      ,                              cEpgEntry.GetEpisodeName()     , PVR_ADDON_NAME_STRING_LENGTH-1);
	        recording.iSeriesNumber       =                              cEpgEntry.GetSeriesNumber()                                     ;
	        recording.iEpisodeNumber      =                              cEpgEntry.GetEpisodeNumber()                                    ;
	        recording.iYear               =                              cEpgEntry.GetYear()                                             ;
	strncpy(recording.strDirectory        ,                              cTimer.GetDirectory()          , PVR_ADDON_URL_STRING_LENGTH -1);
	strncpy(recording.strPlotOutline      ,                              cEpgEntry.GetPlotOutline()     , PVR_ADDON_DESC_STRING_LENGTH-1);
	strncpy(recording.strPlot             ,                              cEpgEntry.GetPlot()            , PVR_ADDON_DESC_STRING_LENGTH-1);
	strncpy(recording.strGenreDescription ,                              cEpgEntry.GetGenreDescription(), PVR_ADDON_DESC_STRING_LENGTH-1);
	strncpy(recording.strChannelName      ,                              cChannel.GetChannelName()      , PVR_ADDON_NAME_STRING_LENGTH-1);
	strncpy(recording.strIconPath         ,                              cChannel.GetIconPath()         , PVR_ADDON_URL_STRING_LENGTH -1);
	strncpy(recording.strThumbnailPath    ,                              cEpgEntry.GetIconPath()        , PVR_ADDON_URL_STRING_LENGTH -1);
	strncpy(recording.strFanartPath       ,                              strFanartPath                  , PVR_ADDON_URL_STRING_LENGTH -1);
	        recording.recordingTime       =                              cTimer.GetStartTime()                                           ;
	        recording.iDuration           =                              iDuration                                                       ;
	        recording.iPriority           =                              cTimer.GetPriority()                                            ;
	        recording.iLifetime           =                              cTimer.GetLifetime()                                            ;
	        recording.iGenreType          =                              cTimer.GetGenreType()                                           ;
	        recording.iGenreSubType       =                              cTimer.GetGenreSubType()                                        ;
	        recording.iPlayCount          =                              iPlayCount                                                      ;
	        recording.iLastPlayedPosition =                              iLastPlayedPosition                                             ;
	        recording.bIsDeleted          =                              bIsDeleted                                                      ;
	        recording.iEpgEventId         =                              cTimer.GetEpgUid()                                              ;
	        recording.iChannelUid         =                              cChannel.GetUniqueId()                                          ;
	        recording.channelType         = (PVR_RECORDING_CHANNEL_TYPE) channelType                                                     ;
			
	// create sql object
	string strRecording = string("(strRecordingId  , strTitle      , strEpisodeName, iSeriesNumber      , iEpisodeNumber, iYear      ,") +
						  string(" strDirectory    , strPlotOutline, strPlot       , strGenreDescription, strChannelName, strIconPath,") +
						  string(" strThumbnailPath, strFanartPath , recordingTime , iDuration          , iPriority     , iLifetime  ,") +
						  string(" iGenreType      , iGenreSubType , iPlayCount    , iLastPlayedPosition, bIsDeleted    , iEpgEventId,") +
						  string(" iChannelUid     , channelType   , strFileName                                                     )") +
						  string(" VALUES ") +  
						  string("('") + StringUtils_Replace(    (recording.strRecordingId     ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strTitle           ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strEpisodeName     ),"'", "''") + string("', ") +
						  string("  ") + StringUtils_Replace(itos(recording.iSeriesNumber      ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iEpisodeNumber     ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iYear              ),"'", "''") + string(" , ") +
						  string(" '") + StringUtils_Replace(    (recording.strDirectory       ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strPlotOutline     ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strPlot            ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strGenreDescription),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strChannelName     ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strIconPath        ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strThumbnailPath   ),"'", "''") + string("', ") +
						  string(" '") + StringUtils_Replace(    (recording.strFanartPath      ),"'", "''") + string("', ") +
						  string("  ") + StringUtils_Replace(itos(recording.recordingTime      ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iDuration          ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iPriority          ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iLifetime          ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iGenreType         ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iGenreSubType      ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iPlayCount         ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iLastPlayedPosition),"'", "''") + string(" , ") +
						  string(" '") + StringUtils_Replace(btos(recording.bIsDeleted         ),"'", "''") + string("', ") +
						  string("  ") + StringUtils_Replace(itos(recording.iEpgEventId        ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.iChannelUid        ),"'", "''") + string(" , ") +
						  string("  ") + StringUtils_Replace(itos(recording.channelType        ),"'", "''") + string(" , ") +
						  string(" '") + StringUtils_Replace(    (          strFileName        ),"'", "''") + string("');") ;

	// correct FLV duration
	if (iDuration >= 0)
		CorrectDurationFLV(strFilePath, iDuration);
	
	// init rec size
	int64_t iRecSize = XBMC_FileSize(strFilePath.c_str());
	
	// add recording if file is not empty (i.e not failed)
	if (iRecSize > 0)
	{
		// send to database and return our updated record
		sqlite->AddRecord("Recordings", strRecording);
	
		// log addition of entry
		XBMC->Log(LOG_NOTICE, "C+: %s - Added %s recording [%s]", __FUNCTION__, recording.strTitle, strFileName);	
	}
	else
	{
		// if not in a sub dir cleanup file
		if (string(recording.strDirectory) == "")
			XBMC->DeleteFile        (strFilePath.c_str()                                                                 );
		else
			XBMC_DeleteFileAndFolder(strFilePath.c_str(), string(settings->GetDVRPath() + recording.strDirectory).c_str());
		
		// log failure to record
		XBMC->Log(LOG_ERROR, "C+: %s - Failed to add recording, see above for more details", __FUNCTION__);
	}

	// remove ffmpeg log	
	if (timer.state != PVR_TIMER_STATE_ERROR)
		XBMC->DeleteFile(strLogPath.c_str());
	}
exit:
	// end thread work
	bIsWorking = false;

	// log termination of thread
	XBMC->Log(LOG_NOTICE, "C+: %s - Stopped FFMPEG recorder", __FUNCTION__);

	// return completion
	return NULL;
}

/***********************************************************
 * Special File Handling Definitions
 ***********************************************************/
void PVRRecorder::CorrectDurationFLV(const string& strFilePath, const int iDur)
{
	// log function call
	CPPLog(); 

	// create container for buffer
	char  readBuffer[1024];
	int   bytesRead       ;
	void* fileHandle      ;

	// open flv file
	fileHandle = XBMC->OpenFile(strFilePath.c_str(), 0);

	// read 1st kilobyte
	bytesRead = XBMC->ReadFile(fileHandle, readBuffer, 1024);

	// close file
	XBMC->CloseFile(fileHandle);

	// create union for duration
	union
	{
		unsigned char cDur[8];
		double        dDur   ;
	} byte;

	byte.dDur = iDur;

	// if successful read of file, then correct FLV header
	if (bytesRead > 0)
	{
		// iterate through bytes
		int pos = -1;

		for (int i = 1; i < bytesRead-8-sizeof(double); i++)
		{
			// look for duration
			if (readBuffer[i] == 'd' && readBuffer[i+1] == 'u' && readBuffer[i+2] == 'r' && readBuffer[i+3] == 'a' && readBuffer[i+4] == 't' && readBuffer[i+5] == 'i' && readBuffer[i+6] == 'o' && readBuffer[i+7] == 'n')
			{
				pos = i+9;
				break;
			}
		}

		// if found duration
		if (pos >= 0)
		{
			// check for big indian and apply duration
			long one = 1;

			if(!(*((char *)(&one))))
			{
			readBuffer[pos+0] = byte.cDur[0];
			readBuffer[pos+1] = byte.cDur[1];
			readBuffer[pos+2] = byte.cDur[2];
			readBuffer[pos+3] = byte.cDur[3];
			readBuffer[pos+4] = byte.cDur[4];
			readBuffer[pos+5] = byte.cDur[5];
			readBuffer[pos+6] = byte.cDur[6];
			readBuffer[pos+7] = byte.cDur[7];
			}
			else
			{
			readBuffer[pos+0] = byte.cDur[7];
			readBuffer[pos+1] = byte.cDur[6];
			readBuffer[pos+2] = byte.cDur[5];
			readBuffer[pos+3] = byte.cDur[4];
			readBuffer[pos+4] = byte.cDur[3];
			readBuffer[pos+5] = byte.cDur[2];
			readBuffer[pos+6] = byte.cDur[1];
			readBuffer[pos+7] = byte.cDur[0];
			}

			// open new file
			fileHandle = XBMC->OpenFileForWrite(strFilePath.c_str(), false);
			/*
			// seek to beginning
			XBMC->SeekFile(fileHandle, 0, ios::beg);
			*/
			// write header to file
			if (XBMC->WriteFile(fileHandle, readBuffer, bytesRead) > 0)
			XBMC->Log(LOG_NOTICE, "C+: %s - Corrected duration of %s file (%i sec)", __FUNCTION__, strFilePath.c_str(), iDur);

			// close file
			XBMC->CloseFile(fileHandle);
		}
	}
}