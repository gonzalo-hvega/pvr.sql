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
#include "netsockets/socket.hh"
#include "netsockets/http.hh"
#include "p8-platform/threads/threads.h"

#include "data/SQLRecord.h"
#include "utilities/HTTPHelpers.h"
#include "utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;
using namespace ADDON;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class TCPClient : P8PLATFORM::CThread
{	
	/* constructors/destrctors */
	public:
		         TCPClient(void);
		virtual ~TCPClient(void);
		
	/* status and variable api calls */
	public:
		bool IsConnected (void);
		bool IsWorking   (void);
	
	/* record api calls */
	public:
		void AddRecord   (const char*, const string, SQLRecord* = NULL);
		void UpdateRecord(const char*, const string, SQLRecord* = NULL);
		
	/* fetch table api calls */
	public:
		vector<SQLRecord> GetRecords(const char*);
		
	/* connect/disconnect */
	private:
		void Connect   (void       );
		void Disconnect(bool = true);

	/* sql client */
	private:
		void *Process(void);
	
	/* client variables */
	private:
		bool   bIsConnected   ;
		bool   bIsWorking     ;
		bool   bStop          ;
		mutex  pMutex         ;
};