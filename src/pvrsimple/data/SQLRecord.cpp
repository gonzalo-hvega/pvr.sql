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
#include "SQLRecord.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
SQLRecord::SQLRecord(string strData)
{
	// assign strRecord to structure
	strRecord = strData;
}

SQLRecord::SQLRecord(void)
{
	// no need to construct stubb for assignment operator
}

SQLRecord::~SQLRecord(void)
{
	// no need to destruct
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_CHANNEL SQLRecord::Channel(void)
{
	// create object
	PVR_CHANNEL xbmcChannel;
	memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));
			
	// assign data to structure
	       xbmcChannel.iUniqueId         = stoi(ParseSQLValue(strRecord, "<iUniqueId>"        ,     0)) ;
	       xbmcChannel.bIsRadio          = stob(ParseSQLValue(strRecord, "<bIsRadio>"         , false)) ;
	       xbmcChannel.iChannelNumber    = stoi(ParseSQLValue(strRecord, "<iChannelNumber>"   ,     0)) ;
	       xbmcChannel.iSubChannelNumber = stoi(ParseSQLValue(strRecord, "<iSubChannelNumber>",     0)) ;
	strcpy(xbmcChannel.strChannelName    , stoc(ParseSQLValue(strRecord, "<strChannelName>"   ,    "")));
	strcpy(xbmcChannel.strInputFormat    , stoc(ParseSQLValue(strRecord, "<strInputFormat>"   ,    "")));
	       xbmcChannel.iEncryptionSystem = stoi(ParseSQLValue(strRecord, "<iEncryptionSystem>",     0)) ;
	strcpy(xbmcChannel.strIconPath       , stoc(ParseSQLValue(strRecord, "<strIconPath>"      ,    "")));
	       xbmcChannel.bIsHidden         = stob(ParseSQLValue(strRecord, "<bIsHidden>"        , false)) ;
		   
	// return object
	return xbmcChannel;
}

PVR_CHANNEL_GROUP SQLRecord::ChannelGroup(void)
{
	// create object
	PVR_CHANNEL_GROUP xbmcChannelGroup;
	memset(&xbmcChannelGroup, 0, sizeof(PVR_CHANNEL_GROUP));
	
	// assign data to structure
	strcpy(xbmcChannelGroup.strGroupName , stoc(ParseSQLValue(strRecord, "<strGroupName>",    "")));
	       xbmcChannelGroup.bIsRadio     = stob(ParseSQLValue(strRecord, "<bIsRadio>"    , false)) ;
	       xbmcChannelGroup.iPosition    = stoi(ParseSQLValue(strRecord, "<iPosition>"   ,     0)) ;
		   
	// return object
	return xbmcChannelGroup;
}

PVR_CHANNEL_GROUP_MEMBER SQLRecord::ChannelGroupMember(void)
{
	// create object
	PVR_CHANNEL_GROUP_MEMBER xbmcChannelGroupMember;
	memset(&xbmcChannelGroupMember, 0, sizeof(PVR_CHANNEL_GROUP_MEMBER));
	
	// assign data to structure
	strcpy(xbmcChannelGroupMember.strGroupName      , stoc(ParseSQLValue(strRecord, "<strGroupName>"     ,    "")));
	       xbmcChannelGroupMember.iChannelUniqueId  = stoi(ParseSQLValue(strRecord, "<iChannelUniqueId>" ,     0)) ;
	       xbmcChannelGroupMember.iChannelNumber    = stoi(ParseSQLValue(strRecord, "<iChannelNumber>"   ,     0)) ;
	       xbmcChannelGroupMember.iSubChannelNumber = stoi(ParseSQLValue(strRecord, "<iSubChannelNumber>",     0)) ;
		   
	// return object
	return xbmcChannelGroupMember;
}

EPG_TAG SQLRecord::EpgTag(void)
{
	// create object
	EPG_TAG xbmcEpgEntry;
	memset(&xbmcEpgEntry, 0, sizeof(EPG_TAG));
	
	// assign data to structure
	xbmcEpgEntry.iUniqueBroadcastId  = stoi(ParseSQLValue(strRecord, "<iUniqueBroadcastId>" ,     0));
	xbmcEpgEntry.iUniqueChannelId    = stoi(ParseSQLValue(strRecord, "<iUniqueChannelId>"   ,     0)); 
	xbmcEpgEntry.strTitle            = stoc(ParseSQLValue(strRecord, "<strTitle>"           ,    ""));
	xbmcEpgEntry.startTime           = stoi(ParseSQLValue(strRecord, "<startTime>"          ,     0));
	xbmcEpgEntry.endTime             = stoi(ParseSQLValue(strRecord, "<endTime>"            ,     0));
	xbmcEpgEntry.strPlotOutline      = stoc(ParseSQLValue(strRecord, "<strPlotOutline>"     ,    ""));
	xbmcEpgEntry.strPlot             = stoc(ParseSQLValue(strRecord, "<strPlot>"            ,    ""));
	xbmcEpgEntry.strOriginalTitle    = stoc(ParseSQLValue(strRecord, "<strOriginalTitle>"   ,    ""));
	xbmcEpgEntry.strCast             = stoc(ParseSQLValue(strRecord, "<strCast>"            ,    ""));
	xbmcEpgEntry.strDirector         = stoc(ParseSQLValue(strRecord, "<strDirector>"        ,    ""));
	xbmcEpgEntry.strWriter           = stoc(ParseSQLValue(strRecord, "<strWriter>"          ,    ""));
	xbmcEpgEntry.iYear               = stoi(ParseSQLValue(strRecord, "<iYear>"              ,     0));
	xbmcEpgEntry.strIMDBNumber       = stoc(ParseSQLValue(strRecord, "<strIMDBNumber>"      ,    ""));
	xbmcEpgEntry.strIconPath         = stoc(ParseSQLValue(strRecord, "<strIconPath>"        ,    ""));
	xbmcEpgEntry.iGenreType          = stoi(ParseSQLValue(strRecord, "<iGenreType>"         ,     0));
	xbmcEpgEntry.iGenreSubType       = stoi(ParseSQLValue(strRecord, "<iGenreSubType>"      ,     0));
	xbmcEpgEntry.strGenreDescription = stoc(ParseSQLValue(strRecord, "<strGenreDescription>",    ""));
	xbmcEpgEntry.firstAired          = stoi(ParseSQLValue(strRecord, "<firstAired>"         ,     0));
	xbmcEpgEntry.iParentalRating     = stoi(ParseSQLValue(strRecord, "<iParentalRating>"    ,     0));
	xbmcEpgEntry.iStarRating         = stoi(ParseSQLValue(strRecord, "<iStarRating>"        ,     0));
	xbmcEpgEntry.bNotify             = stob(ParseSQLValue(strRecord, "<bNotify>"            , false));
	xbmcEpgEntry.iSeriesNumber       = stoi(ParseSQLValue(strRecord, "<iSeriesNumber>"      ,     0));
	xbmcEpgEntry.iEpisodeNumber      = stoi(ParseSQLValue(strRecord, "<iEpisodeNumber>"     ,     0));
	xbmcEpgEntry.iEpisodePartNumber  = stoi(ParseSQLValue(strRecord, "<iEpisodePartNumber>" ,     0));
	xbmcEpgEntry.strEpisodeName      = stoc(ParseSQLValue(strRecord, "<strEpisodeName>"     ,    ""));
	xbmcEpgEntry.iFlags              = stoi(ParseSQLValue(strRecord, "<iFlags>"             ,     0));
	xbmcEpgEntry.strSeriesLink       = stoc(ParseSQLValue(strRecord, "<strSeriesLink>"      ,    ""));
		   
	// return object
	return xbmcEpgEntry;
}

PVR_TIMER_TYPE SQLRecord::TimerType(void)
{
	// create object
	PVR_TIMER_TYPE xbmcTimerType;
	memset(&xbmcTimerType, 0, sizeof(PVR_TIMER_TYPE));
	
	// assign data to structure
	       xbmcTimerType.iId              = stoi(ParseSQLValue(strRecord, "<iId>"            ,  0)) ;
	       xbmcTimerType.iAttributes      = stoi(ParseSQLValue(strRecord, "<iAttributes>"    ,  0)) ;
	strcpy(xbmcTimerType.strDescription   , stoc(ParseSQLValue(strRecord, "<strDescription>" , "")));
	       xbmcTimerType.iPrioritiesSize  = stoi(ParseSQLValue(strRecord, "<iPrioritiesSize>",  0)) ;
	       xbmcTimerType.iLifetimesSize   = stoi(ParseSQLValue(strRecord, "<iLifetimesSize>" ,  0)) ;
		   
	// return object
	return xbmcTimerType;
}

PVR_TIMER SQLRecord::Timer(void)
{
	// create object
	PVR_TIMER xbmcTimer;
	memset(&xbmcTimer, 0, sizeof(PVR_TIMER));
	
	// assign data to structure
	       xbmcTimer.iClientIndex              =                   stoi(ParseSQLValue(strRecord, "<iClientIndex>"             ,     0)) ;
	       xbmcTimer.iParentClientIndex        =                   stoi(ParseSQLValue(strRecord, "<iParentClientIndex>"       ,     0)) ;
	       xbmcTimer.iClientChannelUid         =                   stoi(ParseSQLValue(strRecord, "<iClientChannelUid>"        ,     0)) ;
	       xbmcTimer.startTime                 =                   stoi(ParseSQLValue(strRecord, "<startTime>"                ,     0)) ;
	       xbmcTimer.endTime                   =                   stoi(ParseSQLValue(strRecord, "<endTime>"                  ,     0)) ;
	       xbmcTimer.bStartAnyTime             =                   stob(ParseSQLValue(strRecord, "<bStartAnyTime>"            , false)) ;
	       xbmcTimer.bEndAnyTime               =                   stob(ParseSQLValue(strRecord, "<bEndAnyTime>"              , false)) ;
	       xbmcTimer.state                     = (PVR_TIMER_STATE) stoi(ParseSQLValue(strRecord, "<state>"                    ,     0)) ;
	       xbmcTimer.iTimerType                =                   stoi(ParseSQLValue(strRecord, "<iTimerType>"               ,     0)) ;
	strcpy(xbmcTimer.strTitle                  ,                   stoc(ParseSQLValue(strRecord, "<strTitle>"                 ,    "")));
	strcpy(xbmcTimer.strEpgSearchString        ,                   stoc(ParseSQLValue(strRecord, "<strEpgSearchString>"       ,    "")));
	       xbmcTimer.bFullTextEpgSearch        =                   stob(ParseSQLValue(strRecord, "<bFullTextEpgSearch>"       , false)) ;
	strcpy(xbmcTimer.strDirectory              ,                   stoc(ParseSQLValue(strRecord, "<strDirectory>"             ,    "")));
	strcpy(xbmcTimer.strSummary                ,                   stoc(ParseSQLValue(strRecord, "<strSummary>"               ,    "")));
	       xbmcTimer.iPriority                 =                   stoi(ParseSQLValue(strRecord, "<iPriority>"                ,     0)) ;
	       xbmcTimer.iLifetime                 =                   stoi(ParseSQLValue(strRecord, "<iLifetime>"                ,     0)) ;
	       xbmcTimer.iMaxRecordings            =                   stoi(ParseSQLValue(strRecord, "<iMaxRecordings>"           ,     0)) ;
	       xbmcTimer.iRecordingGroup           =                   stoi(ParseSQLValue(strRecord, "<iRecordingGroup>"          ,     0)) ;
	       xbmcTimer.firstDay                  =                   stoi(ParseSQLValue(strRecord, "<firstDay>"                 ,     0)) ;
	       xbmcTimer.iWeekdays                 =                   stoi(ParseSQLValue(strRecord, "<iWeekdays>"                ,     0)) ;
	       xbmcTimer.iPreventDuplicateEpisodes =                   stoi(ParseSQLValue(strRecord, "<iPreventDuplicateEpisodes>",     0)) ;
	       xbmcTimer.iEpgUid                   =                   stoi(ParseSQLValue(strRecord, "<iEpgUid>"                  ,     0)) ;
	       xbmcTimer.iMarginStart              =                   stoi(ParseSQLValue(strRecord, "<iMarginStart>"             ,     0)) ;
	       xbmcTimer.iMarginEnd                =                   stoi(ParseSQLValue(strRecord, "<iMarginEnd>"               ,     0)) ;
	       xbmcTimer.iGenreType                =                   stoi(ParseSQLValue(strRecord, "<iGenreType>"               ,     0)) ;
	       xbmcTimer.iGenreSubType             =                   stoi(ParseSQLValue(strRecord, "<iGenreSubType>"            ,     0)) ;
	strcpy(xbmcTimer.strSeriesLink             ,                   stoc(ParseSQLValue(strRecord, "<strSeriesLink>"            ,    "")));
		   
	// return object
	return xbmcTimer;
}

PVR_RECORDING SQLRecord::Recording(void)
{
	// create object
	PVR_RECORDING xbmcRecording;
	memset(&xbmcRecording, 0, sizeof(PVR_RECORDING));
	
	// assign data to structure
	strcpy(xbmcRecording.strRecordingId      ,                              stoc(ParseSQLValue(strRecord, "<strRecordingId>"     ,    "")));
	strcpy(xbmcRecording.strTitle            ,                              stoc(ParseSQLValue(strRecord, "<strTitle>"           ,    "")));
	strcpy(xbmcRecording.strEpisodeName      ,                              stoc(ParseSQLValue(strRecord, "<strEpisodeName>"     ,    "")));
	       xbmcRecording.iSeriesNumber       =                              stoi(ParseSQLValue(strRecord, "<iSeriesNumber>"      ,     0)) ;
	       xbmcRecording.iEpisodeNumber      =                              stoi(ParseSQLValue(strRecord, "<iEpisodeNumber>"     ,     0)) ;
	       xbmcRecording.iYear               =                              stoi(ParseSQLValue(strRecord, "<iYear>"              ,     0)) ;
	strcpy(xbmcRecording.strDirectory        ,                              stoc(ParseSQLValue(strRecord, "<strDirectory>"       ,    "")));
	strcpy(xbmcRecording.strPlotOutline      ,                              stoc(ParseSQLValue(strRecord, "<strPlotOutline>"     ,    "")));
	strcpy(xbmcRecording.strPlot             ,                              stoc(ParseSQLValue(strRecord, "<strPlot>"            ,    "")));
	strcpy(xbmcRecording.strGenreDescription ,                              stoc(ParseSQLValue(strRecord, "<strGenreDescription>",    "")));
	strcpy(xbmcRecording.strChannelName      ,                              stoc(ParseSQLValue(strRecord, "<strChannelName>"     ,    "")));
	strcpy(xbmcRecording.strIconPath         ,                              stoc(ParseSQLValue(strRecord, "<strIconPath>"        ,    "")));
	strcpy(xbmcRecording.strThumbnailPath    ,                              stoc(ParseSQLValue(strRecord, "<strThumbnailPath>"   ,    "")));
	strcpy(xbmcRecording.strFanartPath       ,                              stoc(ParseSQLValue(strRecord, "<strFanartPath>"      ,    "")));
	       xbmcRecording.recordingTime       =                              stoi(ParseSQLValue(strRecord, "<recordingTime>"      ,     0)) ;
	       xbmcRecording.iDuration           =                              stoi(ParseSQLValue(strRecord, "<iDuration>"          ,     0)) ;
	       xbmcRecording.iPriority           =                              stoi(ParseSQLValue(strRecord, "<iPriority>"          ,     0)) ;
	       xbmcRecording.iLifetime           =                              stoi(ParseSQLValue(strRecord, "<iLifetime>"          ,     0)) ;
	       xbmcRecording.iGenreType          =                              stoi(ParseSQLValue(strRecord, "<iGenreType>"         ,     0)) ;
	       xbmcRecording.iGenreSubType       =                              stoi(ParseSQLValue(strRecord, "<iGenreSubType>"      ,     0)) ;
	       xbmcRecording.iPlayCount          =                              stoi(ParseSQLValue(strRecord, "<iPlayCount>"         ,     0)) ;
	       xbmcRecording.iLastPlayedPosition =                              stoi(ParseSQLValue(strRecord, "<iLastPlayedPosition>",     0)) ;
	       xbmcRecording.bIsDeleted          =                              stoi(ParseSQLValue(strRecord, "<bIsDeleted>"         , false)) ;
	       xbmcRecording.iEpgEventId         =                              stoi(ParseSQLValue(strRecord, "<iEpgEventId>"        ,     0)) ;
	       xbmcRecording.iChannelUid         =                              stoi(ParseSQLValue(strRecord, "<iChannelUid>"        ,     0)) ;
	       xbmcRecording.channelType         = (PVR_RECORDING_CHANNEL_TYPE) stoi(ParseSQLValue(strRecord, "<channelType>"        ,     0)) ;
		   
	// return object
	return xbmcRecording;
}