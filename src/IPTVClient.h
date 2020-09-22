#pragma once
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
#include "client.h"
#include "p8-platform/util/util.h"

#include "pvrsimple/PVRTypes.h"
#include "pvrsimple/TCPClient.h"
#include "pvrsimple/data/IPTVChannel.h"
#include "pvrsimple/data/IPTVChannelGroup.h"
#include "pvrsimple/data/IPTVChannelGroupMember.h"
#include "pvrsimple/data/IPTVEpgChannel.h"
#include "pvrsimple/data/IPTVEpgEntry.h"
#include "pvrsimple/data/DVRTimerType.h"
#include "pvrsimple/data/DVRTimer.h"
#include "pvrsimple/data/DVRRecording.h"
#include "pvrsimple/utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class IPTVClient
{	
	/* constructors/destrctors */
	public:
		         IPTVClient(void);
		virtual ~IPTVClient(void);
		
	/* status and variable api calls */
	public:
		long   GetStatus                  (void);
		bool   IsCreated                  (void);
		string GetBackendName             (void);
		string GetConnectionString        (void);
		time_t LastChannelsSync           (void);
		time_t LastChannelGroupsSync      (void);
		time_t LastChannelGroupMembersSync(void);
		time_t LastEpgChannelsSync        (void);
		time_t LastEpgEntriesSync         (void);
	
	/* channels api calls */
	public:
		PVR_ERROR GetChannels                 (      ADDON_HANDLE      , bool                                   );
		PVR_ERROR GetChannelGroups            (      ADDON_HANDLE      , bool                                   );
		PVR_ERROR GetChannelGroupMembers      (      ADDON_HANDLE      , const PVR_CHANNEL_GROUP&               );
		int       GetChannelsAmount           (void                                                             );
		int       GetChannelGroupsAmount      (void                                                             );
		int       GetChannelGroupMembersAmount(void                                                             );
		PVR_ERROR GetChannelStreamProperties  (const PVR_CHANNEL*      ,       PVR_NAMED_VALUE*  , unsigned int*);
		bool      OpenLiveStream              (const PVR_CHANNEL&                                               );
		PVR_ERROR SignalStatus                (      PVR_SIGNAL_STATUS&                                         );
		bool      CanPauseStream              (void                                                             );
		bool      CanSeekStream               (void                                                             );
		int       ReadLiveStream              (unsigned char*          , unsigned int                           ); 
		long long SeekLiveStream              (long long               , int                                    );
		long long PositionLiveStream          (void                                                             );
		long long LengthLiveStream            (void                                                             ); 
		void      CloseLiveStream             (void                                                             );
		
	/* epg api calls */
	public:
		PVR_ERROR GetEPGForChannel         (      ADDON_HANDLE, const PVR_CHANNEL&    , time_t       , time_t);
		PVR_ERROR IsEPGTagPlayable         (const EPG_TAG*    , bool*                                        );
		PVR_ERROR IsEPGTagRecordable       (const EPG_TAG*    , bool*                                        );
		PVR_ERROR GetEPGTagStreamProperties(const EPG_TAG*    ,       PVR_NAMED_VALUE*, unsigned int*        );
		int       GetEPGChannelsAmount     (void                                                             );
		int       GetEPGAmount             (void                                                             );
				
	/* fetch data api calls */
	public:
		vector<IPTVChannel           > GetChannels           (void);
		vector<IPTVChannelGroup      > GetChannelGroups      (void);
		vector<IPTVChannelGroupMember> GetChannelGroupMembers(void);
		vector<IPTVEpgChannel        > GetEpgChannels        (void);
		vector<IPTVEpgEntry          > GetEpgEntries         (void);
		
	/* trigger reload api calls */
	public:
		void TriggerChannelsUpdate(time_t);
		void TriggerEpgUpdate     (time_t);

	/* load functions */
	private:
		void LoadChannels           (time_t = time(NULL), bool = true);
		void LoadChannelGroups      (time_t = time(NULL), bool = true);
		void LoadChannelGroupMembers(time_t = time(NULL), bool = true);
		void LoadEpgChannels        (time_t = time(NULL), bool = true);
		void LoadEpgEntries         (time_t = time(NULL), bool = true);		
		
	/* client variables */
	private:
		int    iCurStatus                  ;
		bool   bCreated                    ;
		string strBackendName              ;
		time_t tLastChannelsSync           ;
		time_t tLastChannelGroupsSync      ;
		time_t tLastChannelGroupMembersSync;
		time_t tLastEpgChannelsSync        ;
		time_t tLastEpgEntriesSync         ;
		mutex  pMutex                      ;
		
	/* data variables */
	private:
		vector<IPTVChannel           > cChannels           ;
		vector<IPTVChannelGroup      > cChannelGroups      ;
		vector<IPTVChannelGroupMember> cChannelGroupMembers;
		vector<IPTVEpgChannel        > cEpgChannels        ;
		vector<IPTVEpgEntry          > cEpgEntries         ;
};