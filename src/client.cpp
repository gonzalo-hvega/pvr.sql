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
#include <kodi/xbmc_pvr_dll.h>

#include "client.h"
#include "p8-platform/util/util.h"

/***********************************************************
 * Global Object Pointer Definitions
 ***********************************************************/
CHelper_libXBMC_addon  *XBMC         = NULL;
CHelper_libXBMC_pvr    *PVR          = NULL;
CHelper_libKODI_guilib *GUI          = NULL;
PVRSettings            *settings     = NULL;
TCPServer              *server       = NULL;
TCPClient              *client       = NULL;
SQLConnection          *sqlite       = NULL;
IPTVClient             *iptv         = NULL;
DVRClient              *dvr          = NULL;
void                   *streamHandle = NULL;

/***********************************************************
 * Addon Creation Definitions
 ***********************************************************/
extern "C" {

void ADDON_ReadSettings(void)
{
	// log function call
	CLog();
  
	// create the settings object
	settings = new PVRSettings();
}

ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
	// check if proper arguments received
	if (!hdl || !props)
		return ADDON_STATUS_UNKNOWN;

	// initialize PVR properties
	PVR_PROPERTIES* pvrprops = (PVR_PROPERTIES*)props;

	// initialize XBMC addon helper
	XBMC = new CHelper_libXBMC_addon;
	
	if (!XBMC->RegisterMe(hdl))
	{
		SAFE_DELETE(XBMC);
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	// initialize PVR addon helper
	PVR = new CHelper_libXBMC_pvr;
	
	if (!PVR->RegisterMe(hdl))
	{
		SAFE_DELETE(PVR);
		SAFE_DELETE(XBMC);
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	// initialize GUI addon helper
	GUI = new CHelper_libKODI_guilib;
	if (!GUI->RegisterMe(hdl))
	{
		SAFE_DELETE(GUI);
		SAFE_DELETE(PVR);
		SAFE_DELETE(XBMC);
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	// BETA check
	if (!isBeta())
	{
		GUI->Dialog_OK_ShowAndGetInput("Information", "The BETA period has ended, thanks for your support.");
		SAFE_DELETE(GUI);
		SAFE_DELETE(PVR);
		SAFE_DELETE(XBMC);
		return ADDON_STATUS_PERMANENT_FAILURE;
	}

	// log function call
	CLog();

	// notify user addon is starting
	XBMC->QueueNotification(QUEUE_INFO, "DVR initializing");

	// fetch ADDON settings
	ADDON_ReadSettings();

	// assign paths
	string strUserPath   = pvrprops->strUserPath  ;
	string strClientPath = pvrprops->strClientPath;

	// push to settings
	settings->SetUserPath  (strUserPath  );
	settings->SetClientPath(strClientPath);

	// create user path directory
	if (!XBMC->DirectoryExists(strUserPath.c_str()))
		XBMC->CreateDirectory(strUserPath.c_str());

	// create the sql & iptv object
	server = new TCPServer();
	sqlite = new SQLConnection();
	client = new TCPClient();
	iptv   = new IPTVClient();
	dvr    = new DVRClient();
	
	// check objects if not call destroy
	if (!client || !iptv || !dvr)
	{
		SAFE_DELETE(GUI);
		SAFE_DELETE(PVR);
		SAFE_DELETE(XBMC);
		return ADDON_STATUS_PERMANENT_FAILURE;
	}
		
	// notify user addon is ready (check connection)
	if (client->IsConnected())
		XBMC->QueueNotification(QUEUE_INFO , "DVR connected"   );
	else
		XBMC->QueueNotification(QUEUE_ERROR, "DVR disconnected");
	
	// return addon status (global var)
	return ADDON_GetStatus();
}

ADDON_STATUS ADDON_GetStatus()
{
	// log function call
	CLog();

	// create return
	int iStatus = ADDON_STATUS_PERMANENT_FAILURE;
	
	// look at both iptv and dvr status
	if (iptv->GetStatus() == ADDON_STATUS_OK && dvr->GetStatus() == ADDON_STATUS_OK)
		iStatus = ADDON_STATUS_OK;

	// return addon status (global var)
	return ((ADDON_STATUS) iptv->GetStatus());
}

void ADDON_Destroy()
{
	// log function call
	CLog(); 

	// check if tcp client created, if so end tcp client
	if (client->IsWorking())
		SAFE_DELETE(client);

	// check if tcp server created, if so end tcp server
	if (server->IsWorking())
		SAFE_DELETE(server);
		
	// check if sqlite created, if so end sql object
	if (sqlite->IsWorking())
		SAFE_DELETE(sqlite);

	// check if addon created, if so end iptv object
	if (iptv->IsCreated())
		SAFE_DELETE(iptv);

	// check if addon created, if so end dvr object
	if (dvr->IsCreated())
		SAFE_DELETE(dvr);
  
	// end settings object
	SAFE_DELETE(settings);
}

bool ADDON_HasSettings()
{
	// log function not implemented
	CLogNYI();

	// return default
	return true;
}

ADDON_STATUS ADDON_SetSetting(const char *settingName, const void *settingValue)
{
	// log function call
	CLog(); 

	// return addon restart
	return ADDON_STATUS_NEED_RESTART;
}

void ADDON_Stop()
{
	// log function not implemented
	CLogNYI();
}

void ADDON_FreeSettings()
{
	// log function not implemented
	CLogNYI();
}

/***********************************************************
 * Addon Property Definitions
 ***********************************************************/
void OnSystemSleep()
{
	// log function not implemented
	CLogNYI();
}

void OnSystemWake()
{
	// log function not implemented
	CLogNYI();
}

void OnPowerSavingActivated()
{
	// log function not implemented
	CLogNYI();
}

void OnPowerSavingDeactivated()
{
	// log function not implemented
	CLogNYI();
}

const char* GetGUIAPIVersion(void)
{
	// return default, GUI API not used
	return "";
}

const char* GetMininumGUIAPIVersion(void)
{
	// return default, GUI API not used
	return "";
}

PVR_ERROR GetAddonCapabilities(PVR_ADDON_CAPABILITIES* pCapabilities)
{
	// log function call
	CLog();
  
	// set capabilities
	pCapabilities->bSupportsEPG                = true;
	pCapabilities->bSupportsTV                 = true;
	pCapabilities->bSupportsRadio              = true;
	pCapabilities->bSupportsRecordings         = true;
	pCapabilities->bSupportsTimers             = true;
	pCapabilities->bSupportsChannelGroups      = true;
	pCapabilities->bSupportsRecordingPlayCount = true;
	pCapabilities->bSupportsLastPlayedPosition = true;

	// return no error
	return PVR_ERROR_NO_ERROR;
}

const char *GetBackendName(void)
{
	// log function call
	CLog(); 

	// return backend name
	static const string strBackendName = "PVR Simple SQL Client";
	return strBackendName.c_str();
}

const char *GetBackendVersion(void)
{
	// log function call
	CLog(); 

	// return version
	static string strBackendVersion = STR(IPTV_VERSION);
	return strBackendVersion.c_str();
}

const char *GetBackendHostname(void)
{
	// log function call
	CLog(); 

	// return host
	return "";
}

const char *GetConnectionString(void)
{
	// log function call
	CLog(); 

	// return connection string
	static string strConnectionString = (dvr->GetConnectionString() == "Connected") ? dvr->GetConnectionString() : iptv->GetConnectionString();
	return strConnectionString.c_str();
}

PVR_ERROR GetDriveSpace(long long *iTotal, long long *iUsed)
{
	// log function call
	CLog(); 

	// pass drive space back to xbmc
	*iTotal = -1;
	*iUsed  = -1;

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

/***********************************************************
 * Timer Types Definitions
 ***********************************************************/
PVR_ERROR GetTimerTypes(PVR_TIMER_TYPE types[], int *size)
{
	// log function call
	CLog();

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetTimerTypes(types, size);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

/***********************************************************
 * Live Channels Definitions
 ***********************************************************/
PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetChannels(handle, bRadio);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetChannelGroups(handle, bRadio);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetChannelGroupMembers(handle, group);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

int GetChannelsAmount(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetChannelsAmount();

	// return error if pvr object not set
	return -1;
}

int GetChannelGroupsAmount(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetChannelGroupsAmount();

	// return error if pvr object not set
	return -1;
}

PVR_ERROR GetChannelStreamProperties(const PVR_CHANNEL* channel, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CLog();

	// call of pvr equivalent
	if (iptv)
		return iptv->GetChannelStreamProperties(channel, properties, iPropertiesCount);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

bool OpenLiveStream(const PVR_CHANNEL &channel)
{
	// log function call
	CLog(); 
	  
	// close live stream if open
	CloseLiveStream();

	// return the call of pvr equivalent
	if (iptv)
		return iptv->OpenLiveStream(channel);

	// return error if pvr object not set
	return false;
}

PVR_ERROR SignalStatus(PVR_SIGNAL_STATUS &signalStatus)
{
	// log function call
	CLog();

	// call of pvr equivalent
	if (iptv)
		return iptv->SignalStatus(signalStatus);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

bool CanPauseStream(void) 
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->CanPauseStream();

	// return error if pvr object not set
	return false;
}

bool CanSeekStream(void) // apparently the pause button only works if we can also seek.
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->CanSeekStream();

	// return error if pvr object not set
	return false;
}

int ReadLiveStream(unsigned char *pBuffer, unsigned int iBufferSize) 
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->ReadLiveStream(pBuffer, iBufferSize);

	// return error if pvr object not set
	return 0;
}

long long SeekLiveStream(long long iPosition, int iWhence /* = SEEK_SET */)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->SeekLiveStream(iPosition, iWhence);

	// return error if pvr object not set
	return -1;
}

long long PositionLiveStream(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->PositionLiveStream();

	// return error if pvr object not set
	return -1;
}

long long LengthLiveStream(void) 
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->LengthLiveStream();

	// return error if pvr object not set
	return -1;
}

void CloseLiveStream(void)
{
	// log function call
	CLog();

	// call of pvr equivalent
	if (iptv)
		iptv->CloseLiveStream();
}

bool SwitchChannel(const PVR_CHANNEL &channel)
{
	// log function call
	CLog(); 

	// call local close function
	CloseLiveStream();
	  
	// call local open function
	return OpenLiveStream(channel);
}

/***********************************************************
 * EPG Definitions
 ***********************************************************/
PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (iptv)
		return iptv->GetEPGForChannel(handle, channel, iStart, iEnd);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR IsEPGTagPlayable(const EPG_TAG *tag, bool* bIsPlayable)
{
	// log function call
	CLog();

	// return the call of pvr equivalent
	if (iptv)
		return iptv->IsEPGTagPlayable(tag, bIsPlayable);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR IsEPGTagRecordable(const EPG_TAG *tag, bool* bIsRecordable)
{
	// log function call
	CLog();

	// return the call of pvr equivalent
	if (iptv)
		return iptv->IsEPGTagRecordable(tag, bIsRecordable);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR GetEPGTagStreamProperties(const EPG_TAG *tag, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CLog();

	// call of pvr equivalent
	if (iptv)
		return iptv->GetEPGTagStreamProperties(tag, properties, iPropertiesCount);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

/***********************************************************
 * Timers Definitions
 ***********************************************************/
PVR_ERROR GetTimers(ADDON_HANDLE handle)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetTimers(handle);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR AddTimer(const PVR_TIMER &timer)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->AddTimer(timer);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR UpdateTimer(const PVR_TIMER &timer)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->UpdateTimer(timer);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR DeleteTimer(const PVR_TIMER &timer, bool bForceDelete)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->DeleteTimer(timer, bForceDelete);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

int GetTimersAmount(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetTimersAmount();

	// return error if pvr object not set
	return -1;
}

/***********************************************************
 * Recordings Definitions
 ***********************************************************/
PVR_ERROR GetRecordings(ADDON_HANDLE handle, bool deleted)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetRecordings(handle, deleted); 

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

PVR_ERROR DeleteRecording(const PVR_RECORDING &recording)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->DeleteRecording(recording);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR; 
}

PVR_ERROR GetRecordingStreamProperties(const PVR_RECORDING* recording, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CLog();

	// call of pvr equivalent
	if (dvr)
		return dvr->GetRecordingStreamProperties(recording, properties, iPropertiesCount);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;
}

bool OpenRecordedStream(const PVR_RECORDING &recording)
{
	// log function call
	CLog(); 
	
	// close recorded stream if open
	CloseRecordedStream();

	// return the call of pvr equivalent
	if (dvr)
		return dvr->OpenRecordedStream(recording);

	// return error if pvr object not set
	return false;
}

int ReadRecordedStream(unsigned char *pBuffer, unsigned int iBufferSize)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->ReadRecordedStream(pBuffer, iBufferSize);

	// return error if pvr object not set
	return 0; 
}

long long SeekRecordedStream(long long iPosition, int iWhence /* = SEEK_SET */)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->SeekRecordedStream(iPosition, iWhence);  

	// return error if pvr object not set
	return 0;
}

long long PositionRecordedStream(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->PositionRecordedStream();

	// return error if pvr object not set
	return -1; 
}

long long LengthRecordedStream(void)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->LengthRecordedStream();

	// return error if pvr object not set
	return 0;
}

void CloseRecordedStream(void)
{
	// log function call
	CLog(); 

	// call of pvr equivalent
	if (dvr)
		dvr->CloseRecordedStream();
}

PVR_ERROR SetRecordingPlayCount(const PVR_RECORDING &recording, int count)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->SetRecordingPlayCount(recording, count);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;  
}

PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING &recording, int lastplayedposition)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->SetRecordingLastPlayedPosition(recording, lastplayedposition);

	// return error if pvr object not set
	return PVR_ERROR_SERVER_ERROR;  
}

int GetRecordingLastPlayedPosition(const PVR_RECORDING &recording)
{ 
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetRecordingLastPlayedPosition(recording);

	// return error if pvr object not set
	return -1; 
}

int GetRecordingsAmount(bool deleted)
{
	// log function call
	CLog(); 

	// return the call of pvr equivalent
	if (dvr)
		return dvr->GetRecordingsAmount();

	// return error if pvr object not set
	return -1;
}

/***********************************************************
 * Unused PVR Client API Functions
 ***********************************************************/
PVR_ERROR    OpenDialogChannelScan         (void                                                       ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    CallMenuHook                  (const PVR_MENUHOOK &menuhook, const PVR_MENUHOOK_DATA &item) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    DeleteChannel                 (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    RenameChannel                 (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    MoveChannel                   (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    OpenDialogChannelSettings     (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    OpenDialogChannelAdd          (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
void         DemuxReset                    (void                                                       ) { CLogNYI();                                   }
void         DemuxFlush                    (void                                                       ) { CLogNYI();                                   }
const char * GetLiveStreamURL              (const PVR_CHANNEL &channel                                 ) { CLogNYI(); return ""                       ; }
PVR_ERROR    RenameRecording               (const PVR_RECORDING &recording                             ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    GetRecordingEdl               (const PVR_RECORDING &recording, PVR_EDL_ENTRY[], int*      ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
void         DemuxAbort                    (void                                                       ) { CLogNYI();                                   }
DemuxPacket* DemuxRead                     (void                                                       ) { CLogNYI(); return NULL                     ; }
unsigned int GetChannelSwitchDelay         (void                                                       ) { CLogNYI(); return 0                        ; }
void         PauseStream                   (bool bPaused                                               ) { CLogNYI();                                 ; } // This seemingly never actually gets called.
bool         SeekTime                      (double,bool,double*                                        ) { CLogNYI(); return false                    ; }
void         SetSpeed                      (int                                                        ) { CLogNYI();                                 ; }
bool         IsTimeshifting                (void                                                       ) { CLogNYI(); return false                    ; }
bool         IsRealTimeStream              (void                                                       ) { CLogNYI(); return true                     ; }
time_t       GetPlayingTime                (                                                           ) { CLogNYI(); return 0                        ; }
time_t       GetBufferTimeStart            (                                                           ) { CLogNYI(); return 0                        ; }
time_t       GetBufferTimeEnd              (                                                           ) { CLogNYI(); return 0                        ; }
PVR_ERROR    UndeleteRecording             (const PVR_RECORDING &recording                             ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    DeleteAllRecordingsFromTrash  (                                                           ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    SetEPGTimeFrame               (int                                                        ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }

PVR_ERROR    GetDescrambleInfo             (PVR_DESCRAMBLE_INFO*                                       ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    GetStreamTimes                (PVR_STREAM_TIMES*                                          ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    GetStreamProperties           (PVR_STREAM_PROPERTIES*                                     ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    GetStreamReadChunkSize        (int* chunksize                                             ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
void         FillBuffer                    (bool mode                                                  ) {                                              }
PVR_ERROR    GetEPGTagEdl                  (const EPG_TAG *tag, PVR_EDL_ENTRY edl[], int* size         ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }
PVR_ERROR    SetRecordingLifetime          (const PVR_RECORDING*                                       ) { CLogNYI(); return PVR_ERROR_NOT_IMPLEMENTED; }

} // extern "C"