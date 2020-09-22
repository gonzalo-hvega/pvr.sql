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
#include <kodi/xbmc_pvr_types.h>

#include "../utilities/SQLHelpers.h"
#include "../utilities/Utilities.h"

/***********************************************************
 * Namespace Definitions
 ***********************************************************/
using namespace std;

/***********************************************************
 * Class Definitions
 ***********************************************************/
class SQLRecord
{
	/* constructors/destrctors */
	public:
		 SQLRecord(string);
		 SQLRecord(void  );
		~SQLRecord(void  );
			
	/* fetch variable api calls */
	public:		
		string GetRecord(void) {return strRecord;}
		
	/* fetch structure api calls */
	public:
		PVR_CHANNEL              Channel           (void);
		PVR_CHANNEL_GROUP        ChannelGroup      (void);
		PVR_CHANNEL_GROUP_MEMBER ChannelGroupMember(void);
		EPG_TAG                  EpgTag            (void);
		PVR_TIMER_TYPE           TimerType         (void);
		PVR_TIMER                Timer             (void);
		PVR_RECORDING            Recording         (void);
		
	/* internal variables */
	private:
		string strRecord;
};