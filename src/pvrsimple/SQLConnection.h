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
#include "../client.h"
#include "sqlite/sqlite3.h"
#include "p8-platform/threads/threads.h"

#include "PVRTypes.h"
#include "PVRRecorder.h"
#include "data/SQLRecord.h"
#include "utilities/FileHelpers.h"
#include "utilities/M3UHelpers.h"
#include "utilities/GZHelpers.h"
#include "utilities/XMLHelpers.h"
#include "utilities/SQLHelpers.h"
#include "utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Class Definitions
 ***********************************************************/
extern "C" {static int cCallback(void*, int   , char**, char**);}
 
class SQLConnection : P8PLATFORM::CThread
{	
	/* constructors/destrctors */
	public:
		         SQLConnection(void);
		virtual ~SQLConnection(void);
		
	/* status and variable api calls */
	public:
		bool   IsConnected(void);
		bool   IsWorking  (void);
		time_t LastM3URead(void);
		time_t LastEPGRead(void);
		int    GetEPGDays (void);
		string GetDBLog   (void);
		
	/* record api calls */
	public:
		void AddRecord   (const char*, const string, SQLRecord* = NULL        );
		void UpdateRecord(const char*, const string, SQLRecord* = NULL        );
		void DeleteRecord(const char*, const string                           );
		bool FindRecord  (const char*, const char* , const int    , SQLRecord&);
		bool FindRecord  (const char*, const char* , const char*  , SQLRecord&);
		bool FindRecord  (const char*, const string,                SQLRecord&);
		int  GetTableSize(const char*                                         );
		
	/* fetch table api calls */
	public:
		vector<SQLRecord> GetRecords(const char*);
		
	/* connect/disconnect */
	private:
		void Connect   (void       );
		void Disconnect(bool = true);
	
	/* query functions (mutex lock for callback vector */
	private:
	    friend int cCallback(      void*, int   , char**, char**);
		       int Callback (      int  , char**, char**        );
		       int SendQuery(const char*, void*                 );
				
	/* sql server */
	private:
		void *Process(void);
			
	/* create table functions */
	private:
		bool CreateChannels           (void);
		bool CreateChannelGroups      (void);
		bool CreateChannelGroupMembers(void);
		bool CreateGuideChannels      (void);
		bool CreateGuideEntries       (void);
		bool CreateTimerTypes         (void);
		bool CreateTimers             (void);
		bool CreateRecordings         (void);
			
	/* clear and clean functions */
	private:
		void ClearChannels           (void);
		void ClearChannelGroups      (void);
		void ClearChannelGroupMembers(void);
		void ClearGuideChannels      (void);
		void ClearGuideEntries       (void);
		void ClearTimerTypes         (void);
		void CleanTimers             (void);
		void CleanRecordings         (void);
		
	/* import functions */
	private:
		void ImportM3U       (void);
		void ImportXMLTV     (void);
		void ImportTimerTypes(void);
		
	/* filter channels */
	private:
		void FilterChannelsEPG(void);
		
	/* scheduler functions */
	private:
		PVR_ERROR AddTimer     (const PVR_TIMER&  , int  = 0    );
		PVR_ERROR DeleteTimer  (const PVR_TIMER&  , bool = false);
		PVR_ERROR StartTimer   (const PVR_TIMER&                );
		PVR_ERROR StopTimer    (const PVR_TIMER&                );
		PVR_ERROR ScheduleTimer(const PVR_TIMER&                );
		
	/* server variables */
	private:
		bool     bIsConnected;
		bool     bIsWorking  ;
		bool     bStop       ;
		time_t   tLastM3URead;
		time_t   tLastEPGRead;
		int      iNumEPGDays ;
		string   strDBPath   ;
		sqlite3 *sqlDatabase ;
		mutex    pMutex      ;
		
	/* callback & recorder variables */
	private:
		vector<SQLRecord> sqlCallback;
		vector<SQLMsg   > sqlLog     ;
		vector<SQLTask  > sqlTasks   ;
};