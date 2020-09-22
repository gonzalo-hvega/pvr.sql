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
class DVRClient
{	
	/* constructors/destrctors */
	public:
		         DVRClient(void);
		virtual ~DVRClient(void);
		
	/* status and variable api calls */
	public:
		long   GetStatus          (void);
		bool   IsCreated          (void);
		string GetBackendName     (void);
		string GetConnectionString(void);
		time_t LastTimerTypesSync (void);
		time_t LastTimersSync     (void);
		time_t LastRecordingsSync (void);
		
	/* timer types api calls */
	public:
		PVR_ERROR GetTimerTypes      (     PVR_TIMER_TYPE [], int *);
		int       GetTimerTypesAmount(void                         );
	  
	/* timers api calls */
	public:
		PVR_ERROR GetTimers      (      ADDON_HANDLE              );
		PVR_ERROR AddTimer       (const PVR_TIMER&  , int = 0     );
		PVR_ERROR UpdateTimer    (const PVR_TIMER&                );
		PVR_ERROR DeleteTimer    (const PVR_TIMER&  , bool = false);
		int       GetTimersAmount(void                            );
		
	/* recordings api calls */
	public:
		PVR_ERROR GetRecordings                 (      ADDON_HANDLE  , bool                           );
		PVR_ERROR DeleteRecording               (const PVR_RECORDING&                                 );
		PVR_ERROR GetRecordingStreamProperties  (const PVR_RECORDING*, PVR_NAMED_VALUE*, unsigned int*);
		bool      OpenRecordedStream            (const PVR_RECORDING&                                 );
		int       ReadRecordedStream            (unsigned char*      , unsigned int                   );
		long long SeekRecordedStream            (long long           , int                            );
		long long PositionRecordedStream        (void                                                 );
		long long LengthRecordedStream          (void                                                 );
		void      CloseRecordedStream           (void                                                 );
		PVR_ERROR SetRecordingPlayCount         (const PVR_RECORDING&, int                            );
		PVR_ERROR SetRecordingLastPlayedPosition(const PVR_RECORDING&, int                            );
		int       GetRecordingLastPlayedPosition(const PVR_RECORDING&                                 );
		int       GetRecordingsAmount           (void                                                 );
			
	/* fetch data api calls */
	public:
		vector<DVRTimerType> GetTimerTypes(void);
		vector<DVRTimer    > GetTimers    (void);
		vector<DVRRecording> GetRecordings(void);
		
	/* trigger reload api calls */
	public:
		void TriggerTimerTypesUpdate(time_t);
		void TriggerTimersUpdate    (time_t);
		void TriggerRecordingsUpdate(time_t);
		
	/* load functions */
	private:
		void LoadTimerTypes(time_t = time(NULL), bool = true);
		void LoadTimers    (time_t = time(NULL), bool = true);
		void LoadRecordings(time_t = time(NULL), bool = true);
		
	/* server variables */
	private:
		int    iCurStatus         ;
		bool   bCreated           ;
		string strBackendName     ;
		time_t tLastTimerTypesSync;
		time_t tLastTimersSync    ;
		time_t tLastRecordingsSync;
		mutex  pMutex             ;
		
	/* data variables */
	private:
		vector<DVRTimerType> cTimerTypes;
		vector<DVRTimer    > cTimers    ;
		vector<DVRRecording> cRecordings;
};