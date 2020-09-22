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
class TCPServer : P8PLATFORM::CThread
{	
	/* constructors/destrctors */
	public:
		         TCPServer(void);
		virtual ~TCPServer(void);
		
	/* status and variable api calls */
	public:
		bool IsConnected(void);
		bool IsWorking  (void);
		
	/* sqlite log api calls */
	private:
		string GetDBLog (void);
		
	/* record api calls */
	public:
		string AddRecord   (const string);
		string UpdateRecord(const string);
		
	/* fetch table api calls */
	public:
		string GetRecords(const string);

	/* connect/disconnect */
	private:
		void Connect   (void       );
		void Disconnect(bool = true);

	/* sql server */
	private:
		void *Process(void);
		
	/* server variables */
	private:
		bool  bIsConnected;
		bool  bIsWorking  ;
		bool  bStop       ;
		mutex pMutex      ;
};