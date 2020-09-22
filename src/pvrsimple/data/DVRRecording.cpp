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
#include "DVRRecording.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
DVRRecording::DVRRecording(string strData)
{
	// assign data to structure	
	strRecordingId      =      ParseSQLValue(strData, "<strRecordingId>"     ,    "") ;
	strTitle            =      ParseSQLValue(strData, "<strTitle>"           ,    "") ;
	strEpisodeName      =      ParseSQLValue(strData, "<strEpisodeName>"     ,    "") ;
	iSeriesNumber       = stoi(ParseSQLValue(strData, "<iSeriesNumber>"      ,     0));
	iEpisodeNumber      = stoi(ParseSQLValue(strData, "<iEpisodeNumber>"     ,     0));
	iYear               = stoi(ParseSQLValue(strData, "<iYear>"              ,     0));
	strDirectory        =      ParseSQLValue(strData, "<strDirectory>"       ,    "") ;
	strPlotOutline      =      ParseSQLValue(strData, "<strPlotOutline>"     ,    "") ;
	strPlot             =      ParseSQLValue(strData, "<strPlot>"            ,    "") ;
	strGenreDescription =      ParseSQLValue(strData, "<strGenreDescription>",    "") ;
	strChannelName      =      ParseSQLValue(strData, "<strChannelName>"     ,    "") ;
	strIconPath         =      ParseSQLValue(strData, "<strIconPath>"        ,    "") ;
	strThumbnailPath    =      ParseSQLValue(strData, "<strThumbnailPath>"   ,    "") ;
	strFanartPath       =      ParseSQLValue(strData, "<strFanartPath>"      ,    "") ;
	recordingTime       = stoi(ParseSQLValue(strData, "<recordingTime>"      ,     0));
	iDuration           = stoi(ParseSQLValue(strData, "<iDuration>"          ,     0));
	iPriority           = stoi(ParseSQLValue(strData, "<iPriority>"          ,     0));
	iLifetime           = stoi(ParseSQLValue(strData, "<iLifetime>"          ,     0));
	iGenreType          = stoi(ParseSQLValue(strData, "<iGenreType>"         ,     0));
	iGenreSubType       = stoi(ParseSQLValue(strData, "<iGenreSubType>"      ,     0));
	iPlayCount          = stoi(ParseSQLValue(strData, "<iPlayCount>"         ,     0));
	iLastPlayedPosition = stoi(ParseSQLValue(strData, "<iLastPlayedPosition>",     0));
	bIsDeleted          = stob(ParseSQLValue(strData, "<bIsDeleted>"         , false));
	iEpgEventId         = stoi(ParseSQLValue(strData, "<iEpgEventId>"        ,     0));
	iChannelUid         = stoi(ParseSQLValue(strData, "<iChannelUid>"        ,     0));
	channelType         = stoi(ParseSQLValue(strData, "<channelType>"        ,     0));
	strFilePath         =      ParseSQLValue(strData, "<strFilePath>"        ,    "") ;
	strFileName         =      ParseSQLValue(strData, "<strFileName>"        ,    "") ;
}

DVRRecording::DVRRecording(void)
{
	// no need to construct stubb for assignment operator
}

DVRRecording::~DVRRecording(void)
{
	// no need to destruct
}

/***********************************************************
 * Operator Definitions
 ***********************************************************/
DVRRecording& DVRRecording::operator= (const DVRRecording &rhs)
{
	// assign new values if new
	if (this != &rhs)
	{
		strRecordingId      = rhs.strRecordingId     ;
		strTitle            = rhs.strTitle           ;
		strEpisodeName      = rhs.strEpisodeName     ;
		iSeriesNumber       = rhs.iSeriesNumber      ;
		iEpisodeNumber      = rhs.iEpisodeNumber     ;
		iYear               = rhs.iYear              ;
		strDirectory        = rhs.strDirectory       ;
		strPlotOutline      = rhs.strPlotOutline     ;
		strPlot             = rhs.strPlot            ;
		strGenreDescription = rhs.strGenreDescription;
		strChannelName      = rhs.strChannelName     ;
		strIconPath         = rhs.strIconPath        ;
		strThumbnailPath    = rhs.strThumbnailPath   ;
		strFanartPath       = rhs.strFanartPath      ;
		recordingTime       = rhs.recordingTime      ;
		iDuration           = rhs.iDuration          ;
		iPriority           = rhs.iPriority          ;
		iLifetime           = rhs.iLifetime          ;
		iGenreType          = rhs.iGenreType         ;
		iGenreSubType       = rhs.iGenreSubType      ;
		iPlayCount          = rhs.iPlayCount         ;
		iLastPlayedPosition = rhs.iLastPlayedPosition;
		bIsDeleted          = rhs.bIsDeleted         ;
		iEpgEventId         = rhs.iEpgEventId        ;
		iChannelUid         = rhs.iChannelUid        ;
		channelType         = rhs.channelType        ;
		strFilePath         = rhs.strFilePath        ;
		strFileName         = rhs.strFileName        ;
	}
	
	// return itself
	return *this;
}

/***********************************************************
 * Create PVR Object Definitions
 ***********************************************************/
PVR_RECORDING DVRRecording::Recording(void)
{
	// create object
	PVR_RECORDING xbmcRecording;
	memset(&xbmcRecording, 0, sizeof(PVR_RECORDING));
	
	// assign data to structure
	strcpy(xbmcRecording.strRecordingId      ,                              this->GetRecordingId())      ;
	strcpy(xbmcRecording.strTitle            ,                              this->GetTitle())            ;
	strcpy(xbmcRecording.strEpisodeName      ,                              this->GetEpisodeName())      ;
           xbmcRecording.iSeriesNumber       =                              this->GetSeriesNumber()      ;
           xbmcRecording.iEpisodeNumber      =                              this->GetEpisodeNumber()     ;
           xbmcRecording.iYear               =                              this->GetYear()              ;
	strcpy(xbmcRecording.strDirectory        ,                              this->GetDirectory())        ;
	strcpy(xbmcRecording.strPlotOutline      ,                              this->GetPlotOutline())      ;
	strcpy(xbmcRecording.strPlot             ,                              this->GetPlot())             ;
	strcpy(xbmcRecording.strGenreDescription ,                              this->GetGenreDescription()) ;
	strcpy(xbmcRecording.strChannelName      ,                              this->GetChannelName())      ;
	strcpy(xbmcRecording.strIconPath         ,                              this->GetIconPath())         ;
	strcpy(xbmcRecording.strThumbnailPath    ,                              this->GetThumbnailPath())    ;
	strcpy(xbmcRecording.strFanartPath       ,                              this->GetFanartPath())       ;
           xbmcRecording.recordingTime       =                              this->GetRecordingTime()     ;
           xbmcRecording.iDuration           =                              this->GetDuration()          ;
           xbmcRecording.iPriority           =                              this->GetPriority()          ;
           xbmcRecording.iLifetime           =                              this->GetLifetime()          ;
           xbmcRecording.iGenreType          =                              this->GetGenreType()         ;
           xbmcRecording.iGenreSubType       =                              this->GetGenreSubType()      ;
           xbmcRecording.iPlayCount          =                              this->GetPlayCount()         ;
           xbmcRecording.iLastPlayedPosition =                              this->GetLastPlayedPosition();
           xbmcRecording.bIsDeleted          =                              this->GetIsDeleted()         ;
           xbmcRecording.iEpgEventId         =                              this->GetEpgEventId()        ;
           xbmcRecording.iChannelUid         =                              this->GetChannelUid()        ;
           xbmcRecording.channelType         = (PVR_RECORDING_CHANNEL_TYPE) this->GetChannelType()       ;
		   
	// return object
	return xbmcRecording;
}