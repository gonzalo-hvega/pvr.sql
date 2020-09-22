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
#include "Settings.h"

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
PVRSettings::PVRSettings(void)
{
	// set default values
	iM3UPathType       = 0                     ;
	strM3UPath         = ""                    ;
	bM3UCache          = false                 ;
	iM3UStartNum       = 0                     ;
	iM3URefresh        = REFRESH_INTERVAL_START;
	bM3UFilter         = false                 ;
	iEPGPathType       = 1                     ;
	strEPGPath         = ""                    ;
	bEPGCache          = false                 ;
	iEPGTimeShift      = 0.0                   ;
	iEPGRefresh        = REFRESH_INTERVAL_START;
	bEPGTSOverride     = false                 ;
	iLogoPathType      = 0                     ;
	strLogoPath        = ""                    ;
	iLogoFromEPG       = LOGO_PREFERENCE_IGNORE;
	strDVRPath         = ""                    ;
	iDVRPoll           = 10                    ;
	iDVRMode           = 0                     ;
	strServerIP        = SERVER_IP             ;
	iServerPort        = SERVER_PORT           ;
	strFFMPEGPath      = ""                    ;
	strAVParams        = ""                    ;
	strFileExt         = ""                    ;
	iStrmTimeout       = 60                    ;
	iStrmQuality       = 1                     ;
	strUserPath        = ""                    ;
	strClientPath      = ""                    ;
	  
	// load settings from xml
	LoadSettings();
}

PVRSettings::~PVRSettings(void)
{
	// no objects need deleted
}

/***********************************************************
 * M3U Definitions
 ***********************************************************/
string PVRSettings::GetM3UPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strM3UPath;
}

int PVRSettings::GetM3UStartNum(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iM3UStartNum;
}

int PVRSettings::GetM3URefresh(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iM3URefresh;
}

bool PVRSettings::GetM3UFilter(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return bM3UFilter;
}

/***********************************************************
 * EPG Definitions
 ***********************************************************/
string PVRSettings::GetEPGPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strEPGPath;
}

float PVRSettings::GetEPGTimeShift(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iEPGTimeShift;
}

int PVRSettings::GetEPGRefresh(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iEPGRefresh;
}

bool PVRSettings::GetEPGTSOverride(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return bEPGTSOverride;
}

/***********************************************************
 * Channel Logos Definitions
 ***********************************************************/
string PVRSettings::GetLogoPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strLogoPath;
}

int PVRSettings::GetLogoEPG(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iLogoFromEPG;
}

/***********************************************************
 * DVR Definitions
 ***********************************************************/
string PVRSettings::GetDVRPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strDVRPath;
}

int PVRSettings::GetSchedPoll(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iDVRPoll;
}

int PVRSettings::GetDVRMode(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iDVRMode;
}

string PVRSettings::GetServerIP(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strServerIP;
}

int PVRSettings::GetServerPort(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iServerPort;
}

string PVRSettings::GetFFMPEG(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strFFMPEGPath;
}

string PVRSettings::GetAVParams(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strAVParams;
}

string PVRSettings::GetFileExt(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return strFileExt;
}

int PVRSettings::GetStrmTimeout(void)
{
	// log function call
	CPPLog(); 
	  
	// return settings
	return iStrmTimeout;
}

/***********************************************************
 * Special Paths Definitions
 ***********************************************************/
void PVRSettings::SetClientPath(string strPath)
{
	// log function call
	CPPLog(); 
	  
	// set path for user directory
	strClientPath = strPath;
	
	// copy ffmpeg on android for server only
	#ifdef __ANDROID__    
	if (GetDVRMode() == SERVER_MODE)
	{
		// set value
		string strFFMPEGAndroid = strClientPath + ParseFolderSeparator(strClientPath) + GetFileName(strFFMPEGPath);
	  
		// look if ffmpeg exists in android data folder (e.g. has been copied before)
		if (!XBMC->FileExists(strFFMPEGAndroid.c_str(), false))
		{
			XBMC->Log(LOG_NOTICE, "C+: %s - Attempting to copy FFMPEG to Android PVR directory", __FUNCTION__);  

			if (XBMC_CopyFile(strFFMPEGPath.c_str(), strFFMPEGAndroid.c_str()))
				XBMC->Log(LOG_NOTICE, "C+: %s - Successfully copied FFMPEG to Android PVR directory (%s)", __FUNCTION__, strFFMPEGAndroid.c_str());
			else
				XBMC->Log(LOG_ERROR, "C+: %s - Error copying FFMPEG to Android PVR directory (%s)", __FUNCTION__, strFFMPEGAndroid.c_str());
		}

		// set value
		strFFMPEGPath = (XBMC->FileExists(strFFMPEGAndroid.c_str(), false)) ? strFFMPEGAndroid : "";
	}
	#endif
}

string PVRSettings::GetClientPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return setting
	return strClientPath;
}

void PVRSettings::SetUserPath(string strPath)
{
	// log function call
	CPPLog(); 
	  
	// set path for user directory
	strUserPath = strPath;
}

string PVRSettings::GetUserPath(void)
{
	// log function call
	CPPLog(); 
	  
	// return setting
	return strUserPath;
}

/***********************************************************
 * Settings Loader
 ***********************************************************/
void PVRSettings::LoadSettings(void)
{
	// log function call
	CPPLog(); 

	// initialize local vars for reading from settings
	char  cBuffer [1024] ;
	int   iBuffer = 0    ;
	float fBuffer = 0.0  ;
	bool  bBuffer = false;
	  
	// read in m3u settings
	if (XBMC->GetSetting("m3u.path.type"      , &iBuffer)) { iM3UPathType   = iBuffer; }

	if (iM3UPathType == 0)
	{
		if (XBMC->GetSetting("m3u.path"       , &cBuffer)) { strM3UPath     = cBuffer; }
	}
	else
	{
		if (XBMC->GetSetting("m3u.url"        , &cBuffer)) { strM3UPath     = cBuffer; }
		if (XBMC->GetSetting("m3u.cache"      , &bBuffer)) { bM3UCache      = bBuffer; }
	}
	  
	if (XBMC->GetSetting("m3u.startnum"       , &iBuffer)) { iM3UStartNum   = iBuffer; }
	if (XBMC->GetSetting("m3u.refresh"        , &iBuffer)) { iM3URefresh    = iBuffer; }
	if (XBMC->GetSetting("m3u.filter"         , &bBuffer)) { bM3UFilter     = bBuffer; }

	// read in epg settings
	if (XBMC->GetSetting("epg.path.type"      , &iBuffer)) { iEPGPathType   = iBuffer; }

	if (iEPGPathType == 0)
	{
		if (XBMC->GetSetting("epg.path"       , &cBuffer)) { strEPGPath     = cBuffer; }
	}
	else
	{
		if (XBMC->GetSetting("epg.url"        , &cBuffer)) { strEPGPath     = cBuffer; }
		if (XBMC->GetSetting("epg.cache"      , &bBuffer)) { bEPGCache      = bBuffer; }
	}
	  
	if (XBMC->GetSetting("epg.time.shift"     , &fBuffer)) { iEPGTimeShift  = fBuffer; }
	if (XBMC->GetSetting("epg.refresh"        , &iBuffer)) { iEPGRefresh    = iBuffer; }
	if (XBMC->GetSetting("epg.ts.override"    , &bBuffer)) { bEPGTSOverride = bBuffer; }
	  
	// read in logo settings
	if (XBMC->GetSetting("logo.path.type"     , &iBuffer)) { iLogoPathType  = iBuffer; }

	if (iLogoPathType == 0)
	{
		if (XBMC->GetSetting("logo.path"      , &cBuffer)) { strLogoPath    = cBuffer; }
	}
	else
	{
		if (XBMC->GetSetting("logo.base.url"  , &cBuffer)) { strLogoPath    = cBuffer; }
	}

	if (XBMC->GetSetting("logo.from.epg"      , &iBuffer)) { iLogoFromEPG   = iBuffer; }

	// read in dvr settings
	if (XBMC->GetSetting("dvr.path"           , &cBuffer)) { strDVRPath     = cBuffer; }
	if (XBMC->GetSetting("dvr.poll"           , &iBuffer)) { iDVRPoll       = iBuffer; }
	if (XBMC->GetSetting("dvr.mode"           , &iBuffer)) { iDVRMode       = iBuffer; }
	   
	if (iDVRMode == SERVER_MODE)
	{
		if (XBMC->GetSetting("dvr.port"       , &iBuffer)) { iServerPort    = iBuffer; }
		if (XBMC->GetSetting("dvr.ffmpeg.path", &cBuffer)) { strFFMPEGPath  = cBuffer; }
	}
	else
	{
		if (XBMC->GetSetting("dvr.ip"         , &cBuffer)) { strServerIP    = cBuffer; }
		if (XBMC->GetSetting("dvr.port"       , &iBuffer)) { iServerPort    = iBuffer; }
		if (XBMC->GetSetting("dvr.ffmpeg.path", &cBuffer)) { strFFMPEGPath  = ""     ; }
	}
	  
	if (XBMC->GetSetting("dvr.ffmpeg.params"  , &cBuffer)) { strAVParams    = cBuffer; }
	if (XBMC->GetSetting("dvr.file.ext"       , &cBuffer)) { strFileExt     = cBuffer; }
	if (XBMC->GetSetting("dvr.stream.timeout" , &iBuffer)) { iStrmTimeout   = iBuffer; }
	if (XBMC->GetSetting("dvr.stream.quality" , &iBuffer)) { iStrmQuality   = iBuffer; }
	  
		 
	// log settings loaded
	XBMC->Log(LOG_NOTICE, "C+: %s - All settings loaded", __FUNCTION__);
}