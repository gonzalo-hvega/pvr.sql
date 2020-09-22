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
#include "IPTVClient.h"

/***********************************************************
 * Global Definitions
 ***********************************************************/
extern PVRSettings   *settings    ;
extern TCPClient     *client      ;
extern DVRClient     *dvr         ;
extern void          *streamHandle;

/***********************************************************
 * Constructor/Destructor Definitions
 ***********************************************************/
IPTVClient::IPTVClient(void)
{ 
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Creating IPTV client", __FUNCTION__);
	
	// set status, creation, and backend info
	iCurStatus        = ADDON_STATUS_OK;
	bCreated          = true;
	strBackendName    = "IPTV";
	
	// clear containers
	cChannels.clear(); 
	cChannelGroups.clear(); 
	cChannelGroupMembers.clear(); 
	cEpgChannels.clear(); 
	cEpgEntries.clear(); 
	
	// load sql objects
	LoadChannels();
	LoadChannelGroups();
	LoadChannelGroupMembers();
	LoadEpgChannels();
	LoadEpgEntries();
	
	// log creation of object
	XBMC->Log(LOG_NOTICE, "C+: %s - Created IPTV client", __FUNCTION__);
}

IPTVClient::~IPTVClient(void)
{  
	// mark as disconnected and not created
	iCurStatus        = ADDON_STATUS_LOST_CONNECTION;
	bCreated          = false;
	
	// clear containers
	cChannels.clear(); 
	cChannelGroups.clear(); 
	cChannelGroupMembers.clear(); 
	cEpgChannels.clear(); 
	cEpgEntries.clear(); 
}

/***********************************************************
 * Addon Property API Definitions
 ***********************************************************/
long IPTVClient::GetStatus(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return iCurStatus;
}

bool IPTVClient::IsCreated(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return bCreated;
}

string IPTVClient::GetBackendName(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return strBackendName;
}

string IPTVClient::GetConnectionString(void)
{
	// log function call
	CPPLog(); 
  
	// return value
	return client->IsConnected() ? "Connected" : "Disconnected";
}

time_t IPTVClient::LastChannelsSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastChannelsSync;
}

time_t IPTVClient::LastChannelGroupsSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastChannelGroupsSync;
}

time_t IPTVClient::LastChannelGroupMembersSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastChannelGroupMembersSync;
}

time_t IPTVClient::LastEpgChannelsSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastEpgChannelsSync;
}

time_t IPTVClient::LastEpgEntriesSync(void)
{
	// log function call
	CPPLog(); 
	
	// return value
	return tLastEpgEntriesSync;
}

/***********************************************************
 * Live Channels API Definitions
 ***********************************************************/
PVR_ERROR IPTVClient::GetChannels(ADDON_HANDLE handle, bool bRadio)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
  		
	// iterate through channels and pass back to kodi
	for (vector<IPTVChannel>::iterator cChannel = cChannels.begin(); cChannel != cChannels.end(); cChannel++)
	{
		// pass back appropriate entries
		if (cChannel->GetIsRadio() == bRadio)
		{			
			// create object
			PVR_CHANNEL xbmcChannel;
			memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));
	  
				   xbmcChannel.iUniqueId         = cChannel->GetUniqueId()        ;
				   xbmcChannel.bIsRadio          = cChannel->GetIsRadio()         ;
				   xbmcChannel.iChannelNumber    = cChannel->GetChannelNumber()   ;
				   xbmcChannel.iSubChannelNumber = cChannel->GetSubChannelNumber();
			strcpy(xbmcChannel.strChannelName    , cChannel->GetChannelName())    ;
			strcpy(xbmcChannel.strInputFormat    , cChannel->GetInputFormat())    ;
				   xbmcChannel.iEncryptionSystem = cChannel->GetEncryptionSystem();
			strcpy(xbmcChannel.strIconPath       , cChannel->GetIconPath())       ;
				   xbmcChannel.bIsHidden         = cChannel->GetIsHidden()        ;
				   
			// override logo
			if (settings->GetLogoEPG() > 0)
			{
				// iterate through epg channels to get tvg icon
				for (vector<IPTVEpgChannel>::iterator cEpgChannel = cEpgChannels.begin(); cEpgChannel != cEpgChannels.end(); cEpgChannel++)
				{
					// look for tv guide id or display name 
					if (StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgId()  ), StringUtils_Trim(cChannel->GetTvgId()      )) || 
						StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgName()), StringUtils_Trim(cChannel->GetTvgName()    )) ||
						StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgName()), StringUtils_Trim(cChannel->GetChannelName()))  )
					{	
						// replace logo
						if (settings->GetLogoEPG() == 1)
							strcpy(xbmcChannel.strIconPath, (string(cChannel->GetIconPath()  ) != "") ? cChannel->GetIconPath() : cEpgChannel->GetTvgLogo().c_str());
						if (settings->GetLogoEPG() == 2)
							strcpy(xbmcChannel.strIconPath, (string(cEpgChannel->GetTvgLogo()) != "") ? cEpgChannel->GetTvgLogo().c_str() : cChannel->GetIconPath());
						
						// break loop
						break;
					}
				}
			}
			
			// pass back to kodi
			PVR->TransferChannelEntry(handle, &xbmcChannel);				
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IPTVClient::GetChannelGroups(ADDON_HANDLE handle, bool bRadio)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
		
	// iterate through channel groups and pass back to kodi
	for (vector<IPTVChannelGroup>::iterator cChannelGroup = cChannelGroups.begin(); cChannelGroup != cChannelGroups.end(); cChannelGroup++)
	{
		// pass back appropriate entries
		if (cChannelGroup->GetIsRadio() == bRadio)
		{
			// create object
			PVR_CHANNEL_GROUP xbmcChannelGroup;
			memset(&xbmcChannelGroup, 0, sizeof(PVR_CHANNEL_GROUP));
			
			strcpy(xbmcChannelGroup.strGroupName , cChannelGroup->GetGroupName());
			       xbmcChannelGroup.bIsRadio     = cChannelGroup->GetIsRadio()   ;
			       xbmcChannelGroup.iPosition    = cChannelGroup->GetPosition()  ;

			// pass back to kodi
			PVR->TransferChannelGroup(handle, &xbmcChannelGroup);
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IPTVClient::GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
  			
	// iterate through channel group members and pass back to kodi
	for (vector<IPTVChannelGroupMember>::iterator cChannelGroupMember = cChannelGroupMembers.begin(); cChannelGroupMember != cChannelGroupMembers.end(); cChannelGroupMember++)
	{
		// pass back appropriate entries
		if (strcmp(cChannelGroupMember->GetGroupName(), group.strGroupName) == 0 && cChannelGroupMember->GetIsRadio() == group.bIsRadio)
		{
			// create object
			PVR_CHANNEL_GROUP_MEMBER xbmcChannelGroupMember;
			memset(&xbmcChannelGroupMember, 0, sizeof(PVR_CHANNEL_GROUP_MEMBER));
			
			strcpy(xbmcChannelGroupMember.strGroupName      , cChannelGroupMember->GetGroupName())      ;
			       xbmcChannelGroupMember.iChannelUniqueId  = cChannelGroupMember->GetChannelUniqueId() ;
			       xbmcChannelGroupMember.iChannelNumber    = cChannelGroupMember->GetChannelNumber()   ;
			       xbmcChannelGroupMember.iSubChannelNumber = cChannelGroupMember->GetSubChannelNumber();
			
			// pass back to kodi
			PVR->TransferChannelGroupMember(handle, &xbmcChannelGroupMember);
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

int IPTVClient::GetChannelsAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cChannels.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

int IPTVClient::GetChannelGroupsAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cChannelGroups.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

int IPTVClient::GetChannelGroupMembersAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cChannelGroupMembers.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

PVR_ERROR IPTVClient::GetChannelStreamProperties(const PVR_CHANNEL* channel, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
  		
	// iterate through channels and pass back to kodi
	for (vector<IPTVChannel>::iterator cChannel = cChannels.begin(); cChannel != cChannels.end(); cChannel++)
	{
		// look for channel id
		if (cChannel->GetUniqueId() == (*channel).iUniqueId)
		{	
			// initialize array size
			*iPropertiesCount = 0;

			// create property var
			string strProperty = PVR_STREAM_PROPERTY_STREAMURL;
			  
			strncpy(properties[*iPropertiesCount].strName ,           strProperty.c_str()   ,           strProperty.size()   );
			strncpy(properties[*iPropertiesCount].strValue, cChannel->GetStreamURL().c_str(), cChannel->GetStreamURL().size());

			(*iPropertiesCount)++;

			// break loop
			break;
		}
	}
	
	// unlock threads
	SetUnlock();
	
	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

bool IPTVClient::OpenLiveStream(const PVR_CHANNEL &channel)
{
	// log function call
	CPPLog(); 
	
	// stub, we do not open anything natively
	return true;
}

PVR_ERROR IPTVClient::SignalStatus(PVR_SIGNAL_STATUS &signalStatus)
{
	// log function call
	CPPLog(); 
	  
	// assign ok to signal status
	snprintf(signalStatus.strAdapterName  , sizeof(signalStatus.strAdapterName  ), "IPTV Simple Adapter 1");
	snprintf(signalStatus.strAdapterStatus, sizeof(signalStatus.strAdapterStatus), "OK"                   );

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

bool IPTVClient::CanPauseStream(void)
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return true;
}

bool IPTVClient::CanSeekStream(void)
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return true;
}

int IPTVClient::ReadLiveStream(unsigned char *pBuffer, unsigned int iBufferSize) 
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return 0;
}

long long IPTVClient::SeekLiveStream(long long iPosition, int iWhence /* = SEEK_SET */)
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return -1;
}

long long IPTVClient::PositionLiveStream(void)
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return -1;
}

long long IPTVClient::LengthLiveStream(void) 
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
	return -1;
}

void IPTVClient::CloseLiveStream(void)
{
	// log function call
	CPPLog(); 

	// stub, we do not open anything natively
}

/***********************************************************
 * EPG API Definitions
 ***********************************************************/
PVR_ERROR IPTVClient::GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
				
	// iterate through channels to find in database
	for (vector<IPTVChannel>::iterator cChannel = cChannels.begin(); cChannel != cChannels.end(); cChannel++)
	{
		// look for tv guide id
		if (cChannel->GetUniqueId() == channel.iUniqueId)
		{
			// iterate through epg channels to get tvg id or display name
			for (vector<IPTVEpgChannel>::iterator cEpgChannel = cEpgChannels.begin(); cEpgChannel != cEpgChannels.end(); cEpgChannel++)
			{
				// look for tv guide id or display name 
				if (StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgId()  ), StringUtils_Trim(cChannel->GetTvgId()      )) || 
				    StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgName()), StringUtils_Trim(cChannel->GetTvgName()    )) ||
					StringUtils::EqualsNoCase(StringUtils_Trim(cEpgChannel->GetTvgName()), StringUtils_Trim(cChannel->GetChannelName()))  )
				{
					// iterate through EPG and pass back entries
					for (vector<IPTVEpgEntry>::iterator cEpgEntry = cEpgEntries.begin(); cEpgEntry != cEpgEntries.end(); cEpgEntry++)
					{
						// if matches channel guide and in time frame then pass back to kodi
						if (cEpgEntry->GetTvgId() == cEpgChannel->GetTvgId())
						{
							// only pass back those in window
							if (cEpgEntry->GetEndTime() >= iStart && cEpgEntry->GetStartTime() <= iEnd)
							{	
								// create object
								EPG_TAG xbmcEpgEntry;
								memset(&xbmcEpgEntry, 0, sizeof(EPG_TAG));
											
								xbmcEpgEntry.iUniqueBroadcastId  = cEpgEntry->GetUniqueBroadcastId()                                                                                                       ;
								xbmcEpgEntry.iUniqueChannelId    = cChannel->GetUniqueId()                                                                                                                 ;
								xbmcEpgEntry.strTitle            = cEpgEntry->GetTitle()                                                                                                                   ;
								xbmcEpgEntry.startTime           = cEpgEntry->GetStartTime() + (settings->GetEPGTimeShift() + (settings->GetEPGTSOverride() ? 0 : cChannel->GetTvgShift()))*SECONDS_IN_HOUR;
								xbmcEpgEntry.endTime             = cEpgEntry->GetEndTime()   + (settings->GetEPGTimeShift() + (settings->GetEPGTSOverride() ? 0 : cChannel->GetTvgShift()))*SECONDS_IN_HOUR;
								xbmcEpgEntry.strPlotOutline      = cEpgEntry->GetPlotOutline()                                                                                                             ; 
								xbmcEpgEntry.strPlot             = cEpgEntry->GetPlot()                                                                                                                    ; 
								xbmcEpgEntry.strOriginalTitle    = cEpgEntry->GetOriginalTitle()                                                                                                           ;
								xbmcEpgEntry.strCast             = cEpgEntry->GetCast()                                                                                                                    ;
								xbmcEpgEntry.strDirector         = cEpgEntry->GetDirector()                                                                                                                ;
								xbmcEpgEntry.strWriter           = cEpgEntry->GetWriter()                                                                                                                  ;
								xbmcEpgEntry.iYear               = cEpgEntry->GetYear()                                                                                                                    ;
								xbmcEpgEntry.strIMDBNumber       = cEpgEntry->GetIMDBNumber()                                                                                                              ;
								xbmcEpgEntry.strIconPath         = cEpgEntry->GetIconPath()                                                                                                                ;
								xbmcEpgEntry.iGenreType          = cEpgEntry->GetGenreType()                                                                                                               ;
								xbmcEpgEntry.iGenreSubType       = cEpgEntry->GetGenreSubType()                                                                                                            ;
								xbmcEpgEntry.strGenreDescription = cEpgEntry->GetGenreDescription()                                                                                                        ;
								xbmcEpgEntry.firstAired          = cEpgEntry->GetFirstAired()                                                                                                              ;
								xbmcEpgEntry.iParentalRating     = cEpgEntry->GetParentalRating()                                                                                                          ;
								xbmcEpgEntry.iStarRating         = cEpgEntry->GetStarRating()                                                                                                              ;
								xbmcEpgEntry.bNotify             = cEpgEntry->GetNotify()                                                                                                                  ;
								xbmcEpgEntry.iSeriesNumber       = cEpgEntry->GetSeriesNumber()                                                                                                            ;
								xbmcEpgEntry.iEpisodeNumber      = cEpgEntry->GetEpisodeNumber()                                                                                                           ;
								xbmcEpgEntry.iEpisodePartNumber  = cEpgEntry->GetEpisodePartNumber()                                                                                                       ;
								xbmcEpgEntry.strEpisodeName      = cEpgEntry->GetEpisodeName()                                                                                                             ;
								xbmcEpgEntry.iFlags              = cEpgEntry->GetFlags()                                                                                                                   ;
								xbmcEpgEntry.strSeriesLink       = cEpgEntry->GetSeriesLink()                                                                                                              ;

								// pass back to kodi
								PVR->TransferEpgEntry(handle, &xbmcEpgEntry);
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
	
	// unlock threads
	SetUnlock();

	// return sucess of query
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IPTVClient::IsEPGTagPlayable(const EPG_TAG *tag, bool* bIsPlayable)
{
	// log function call
	CPPLog();

	// TO-DO: implement catchup
	  
	// set playable
	*bIsPlayable = ((*tag).startTime <= time(NULL) && (*tag).endTime > time(NULL)) ? true : false;

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IPTVClient::IsEPGTagRecordable(const EPG_TAG *tag, bool* bIsRecordable)
{
	// log function call
	CPPLog();

	// TO-DO: implement catchup
	  
	// set playable
	*bIsRecordable = ((*tag).endTime > time(NULL)) ? true : false;

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

PVR_ERROR IPTVClient::GetEPGTagStreamProperties(const EPG_TAG *tag, PVR_NAMED_VALUE* properties, unsigned int* iPropertiesCount)
{
	// log function call
	CPPLog();
	
	// lock threads
	SetLock();
				
	// iterate through channels to find in database
	for (vector<IPTVChannel>::iterator cChannel = cChannels.begin(); cChannel != cChannels.end(); cChannel++)
	{
		// look for channel id
		if (cChannel->GetUniqueId() == (*tag).iUniqueChannelId)
		{
			// initialize array size
			*iPropertiesCount = 0;		  
			  
			// create property var
			string strProperty;

			// assign stream property
			strProperty = PVR_STREAM_PROPERTY_STREAMURL;
			  
			strncpy(properties[*iPropertiesCount].strName ,           strProperty.c_str()   ,           strProperty.size()   );
			strncpy(properties[*iPropertiesCount].strValue, cChannel->GetStreamURL().c_str(), cChannel->GetStreamURL().size());
			(*iPropertiesCount)++;
			  
			// assign live property
			strProperty = PVR_STREAM_PROPERTY_EPGPLAYBACKASLIVE;
			  
			memcpy(properties[*iPropertiesCount].strName ,    strProperty.c_str(),    strProperty.size());
			memcpy(properties[*iPropertiesCount].strValue, string("true").c_str(), string("true").size());
			(*iPropertiesCount)++;

			// break loop
			break;
		}	
	}
	
	// unlock threads
	SetUnlock();

	// return no issue
	return PVR_ERROR_NO_ERROR;
}

int IPTVClient::GetEPGChannelsAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cEpgChannels.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

int IPTVClient::GetEPGAmount(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// get size
	int size = cEpgEntries.size();
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return size;
}

/***********************************************************
 * Fetch Copy of Data API Definitions
 ***********************************************************/
vector<IPTVChannel> IPTVClient::GetChannels(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<IPTVChannel> ret = cChannels;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<IPTVChannelGroup> IPTVClient::GetChannelGroups(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<IPTVChannelGroup> ret = cChannelGroups;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<IPTVChannelGroupMember> IPTVClient::GetChannelGroupMembers(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<IPTVChannelGroupMember> ret = cChannelGroupMembers;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<IPTVEpgChannel> IPTVClient::GetEpgChannels(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<IPTVEpgChannel> ret = cEpgChannels;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

vector<IPTVEpgEntry> IPTVClient::GetEpgEntries(void)
{
	// log function call
	CPPLog(); 
	
	// lock threads
	SetLock();

	// read current vectors
	vector<IPTVEpgEntry> ret = cEpgEntries;
	
	// unlock threads
	SetUnlock();

	// return size of vector
	return ret;
}

/***********************************************************
 * Trigger Reload API Definitions
 ***********************************************************/
void IPTVClient::TriggerChannelsUpdate(time_t tSync)
{
	// log function call
	CPPLog();	
	
	// lock threads
	SetLock();
	
	// reload sql objects
	LoadChannels(tSync, false);
	LoadChannelGroups(tSync, false);
	LoadChannelGroupMembers(tSync, false);
	
	// unlock threads
	SetUnlock();
	
	// notify user of reload
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channels loaded", __FUNCTION__, GetChannelsAmount());
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channel groups loaded", __FUNCTION__, GetChannelGroupsAmount());
	XBMC->Log(LOG_NOTICE, "C+: %s - %i channel groups members loaded", __FUNCTION__, GetChannelGroupMembersAmount());
	
	// trigger client update
	PVR->TriggerChannelUpdate();
	PVR->TriggerChannelGroupsUpdate();	
}

void IPTVClient::TriggerEpgUpdate(time_t tSync)
{
	// log function call
	CPPLog();	
	
	// lock threads
	SetLock();
	
	// reload sql objects
	LoadEpgChannels(tSync, false);
	LoadEpgEntries(tSync, false);
	
	// unlock threads
	SetUnlock();
	
	// notify user of reload
	XBMC->Log(LOG_NOTICE, "C+: %s - %i guide entries loaded", __FUNCTION__, GetEPGAmount());
	
	// get channels snapshot
	vector<IPTVChannel> xChannels = GetChannels();
	
	// trigger client update
	for (vector<IPTVChannel>::iterator cChannel = xChannels.begin(); cChannel != xChannels.end(); cChannel++)
		PVR->TriggerEpgUpdate(cChannel->GetUniqueId());	
	
	// clear local container
	xChannels.clear();
}

/***********************************************************
 * Load Data Definitions
 ***********************************************************/
void IPTVClient::LoadChannels(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cChannels.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlChannels = client->GetRecords("Channels");
	
	// iterate through channels and add to dvr cache
	for (vector<SQLRecord>::iterator sqlChannel = sqlChannels.begin(); sqlChannel != sqlChannels.end(); sqlChannel++)
	{	
		// initialize record
		IPTVChannel cChannel(sqlChannel->GetRecord());
		
		// push back to timer types container
		cChannels.push_back(cChannel);	
	}
	
	// notify user of channels loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i channel(s) loaded", GetChannelsAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i channel(s) loaded", __FUNCTION__, GetChannelsAmount());
	}
	
	// set last sync to now
	tLastChannelsSync = tSync;
}

void IPTVClient::LoadChannelGroups(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cChannelGroups.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlChannelGroups = client->GetRecords("ChannelGroups");
	
	// iterate through channels and add to dvr cache
	for (vector<SQLRecord>::iterator sqlChannelGroup = sqlChannelGroups.begin(); sqlChannelGroup != sqlChannelGroups.end(); sqlChannelGroup++)
	{	
		// initialize record
		IPTVChannelGroup cChannelGroup(sqlChannelGroup->GetRecord());
		
		// push back to timer types container
		cChannelGroups.push_back(cChannelGroup);	
	}
	
	// notify user of channel groups loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i channel group(s) loaded", GetChannelGroupsAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i channel group(s) loaded", __FUNCTION__, GetChannelGroupsAmount());
	}
	
	// set last sync to now
	tLastChannelGroupsSync = tSync;
}

void IPTVClient::LoadChannelGroupMembers(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cChannelGroupMembers.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlChannelGroupMembers = client->GetRecords("ChannelGroupMembers");
	
	// iterate through channels and add to dvr cache
	for (vector<SQLRecord>::iterator sqlChannelGroupMember = sqlChannelGroupMembers.begin(); sqlChannelGroupMember != sqlChannelGroupMembers.end(); sqlChannelGroupMember++)
	{	
		// initialize record
		IPTVChannelGroupMember cChannelGroupMember(sqlChannelGroupMember->GetRecord());
		
		// push back to timer types container
		cChannelGroupMembers.push_back(cChannelGroupMember);	
	}
	
	// notify user of channel group members loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i channel(s) grouped", GetChannelGroupMembersAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i channel(s) grouped", __FUNCTION__, GetChannelGroupMembersAmount());
	}
	
	// set last sync to now
	tLastChannelGroupMembersSync = tSync;
}

void IPTVClient::LoadEpgChannels(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cEpgChannels.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlEpgChannels = client->GetRecords("EpgChannels");
	
	// iterate through epg channels and add to dvr cache
	for (vector<SQLRecord>::iterator sqlEpgChannel = sqlEpgChannels.begin(); sqlEpgChannel != sqlEpgChannels.end(); sqlEpgChannel++)
	{	
		// initialize record
		IPTVEpgChannel cEpgChannel(sqlEpgChannel->GetRecord());
		
		// push back to timer types container
		cEpgChannels.push_back(cEpgChannel);	
	}
	
	// notify user of epg channels loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i guide channel(s) loaded", GetEPGChannelsAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i guide channel(s) loaded", __FUNCTION__, GetEPGChannelsAmount());
	}
	
	// notify user of channels filtered
	if (settings->GetM3UFilter())
	{
		if (bNotify)
		{
			XBMC->QueueNotification(QUEUE_INFO, "EPG channel(s) filtered");
			XBMC->Log(LOG_NOTICE, "C+: %s - EPG channel(s) filtered", __FUNCTION__);
		}
	}
	
	// set last sync to now
	tLastEpgChannelsSync = tSync;
}

void IPTVClient::LoadEpgEntries(time_t tSync/* = time(NULL) */, bool bNotify /* = true */)
{
	// log function call
	CPPLog();
	
	// clear container
	cEpgEntries.clear();
  	  
	// get sql objects
	vector<SQLRecord> sqlEpgEntries = client->GetRecords("EpgEntries");
	
	// iterate through epg and add to dvr cache
	for (vector<SQLRecord>::iterator sqlEpgEntry = sqlEpgEntries.begin(); sqlEpgEntry != sqlEpgEntries.end(); sqlEpgEntry++)
	{	
		// initialize record
		IPTVEpgEntry cEpgEntry(sqlEpgEntry->GetRecord());
		
		// push back to timer types container
		cEpgEntries.push_back(cEpgEntry);	
	}
	
	// notify user of epg loaded
	if (bNotify)
	{
		XBMC->QueueNotification(QUEUE_INFO, "%i guide entries loaded", GetEPGAmount());
		XBMC->Log(LOG_NOTICE, "C+: %s - %i guide entries loaded", __FUNCTION__, GetEPGAmount());
	}
	
	// set last sync to now
	tLastEpgEntriesSync = tSync;
}